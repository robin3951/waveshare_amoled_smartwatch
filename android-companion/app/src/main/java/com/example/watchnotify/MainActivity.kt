package com.example.watchnotify

import android.Manifest
import android.content.ComponentName
import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.provider.Settings
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat

/**
 * Single-screen UI for the companion app.
 *
 * Responsibilities:
 * - Starts [WatchForegroundService] so the BLE connection survives once this
 *   activity is closed/backgrounded.
 * - Wires [BleManager] status/log callbacks to on-screen [TextView]s.
 * - Lets the user trigger a BLE scan/connect ([BleManager.startScan]) and
 *   send a test notification ([BleManager.sendNotification]).
 * - Checks and, if missing, prompts the user to grant the
 *   [NotificationService] and [NavigationAccessibilityService] system
 *   permissions, plus runtime Bluetooth/location permissions.
 */
class MainActivity : AppCompatActivity() {

    private lateinit var statusText: TextView
    private lateinit var listenerStatus: TextView
    private lateinit var logText: TextView
    private lateinit var connectButton: Button
    private lateinit var testButton: Button

    /** Action to run once pending runtime permissions are granted, or `null` if none pending. */
    private var pendingAction: (() -> Unit)? = null

    /**
     * Initializes the views, starts the foreground service, subscribes to
     * [BleManager] callbacks, and wires up the connect/test buttons.
     */
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        statusText     = findViewById(R.id.statusText)
        listenerStatus = findViewById(R.id.listenerStatus)
        logText        = findViewById(R.id.logText)
        connectButton  = findViewById(R.id.connectButton)
        testButton     = findViewById(R.id.testButton)

        // Ask for POST_NOTIFICATIONS before the foreground service tries to post
        // its status notification (Android 13+ requires this runtime grant; the
        // call doesn't crash without it, but the notification is silently
        // dropped, so the user would never see the connection status).
        requestNotificationPermissionIfNeeded()

        // Start foreground service to keep process alive in background
        try {
            startForegroundService(Intent(this, WatchForegroundService::class.java))
        } catch (e: Exception) {
            android.util.Log.e("MainActivity", "Foreground service start failed: ${e.message}")
        }

        BleManager.instance.onStatusChange = { msg ->
            runOnUiThread { statusText.text = msg }
        }
        BleManager.instance.onLogLine = { line ->
            runOnUiThread {
                val prev = logText.text.toString()
                val lines = prev.lines().takeLast(7)
                logText.text = (lines + line).joinToString("\n")
            }
        }

        connectButton.setOnClickListener {
            requestBlePermissions {
                BleManager.instance.startScan(this)
            }
        }

        testButton.setOnClickListener {
            BleManager.instance.sendNotification(
                packageName = "com.whatsapp",
                sender      = "Test",
                text        = "Hallo von der WatchNotify App! Dies ist eine längere Testnachricht."
            )
            logText.append("\n→ Test gesendet")
        }
    }

    /** Re-checks listener/accessibility permission status every time the screen becomes visible. */
    override fun onResume() {
        super.onResume()
        updateListenerStatus()
    }

    /**
     * Updates the on-screen permission status text and, if either the
     * notification-listener or accessibility-service permission is missing,
     * shows a dialog guiding the user to the relevant system settings screen.
     *
     * The accessibility dialog is only shown once the notification-listener
     * permission is already granted, to avoid stacking two dialogs at once.
     */
    private fun updateListenerStatus() {
        val notifOk = isNotificationListenerEnabled()
        val accessOk = isAccessibilityEnabled()

        listenerStatus.text = buildString {
            append(if (notifOk)  "Benachrichtigungen: ✓\n" else "Benachrichtigungen: ✗\n")
            append(if (accessOk) "Maps Navigation:    ✓"   else "Maps Navigation:    ✗")
        }

        if (!notifOk) {
            AlertDialog.Builder(this)
                .setTitle("Benachrichtigungszugriff erforderlich")
                .setMessage("WatchNotify muss als Benachrichtigungslistener aktiviert werden.")
                .setPositiveButton("Einstellungen") { _, _ ->
                    startActivity(Intent(Settings.ACTION_NOTIFICATION_LISTENER_SETTINGS))
                }
                .setNegativeButton("Abbrechen", null)
                .show()
        } else if (!accessOk) {
            AlertDialog.Builder(this)
                .setTitle("Eingabehilfen-Zugriff erforderlich")
                .setMessage(
                    "Für Google Maps Navigationshinweise (Abbiegehinweise, Distanz, Straße) " +
                    "muss \"WatchNotify Navigation\" unter Einstellungen → Eingabehilfen aktiviert werden."
                )
                .setPositiveButton("Einstellungen") { _, _ ->
                    startActivity(Intent(Settings.ACTION_ACCESSIBILITY_SETTINGS))
                }
                .setNegativeButton("Später", null)
                .show()
        }
    }

    /**
     * Checks whether the user has enabled [NotificationService] as a
     * notification listener, by inspecting the system's
     * `enabled_notification_listeners` secure setting (there is no direct
     * API for this check).
     *
     * @return `true` if this app's [NotificationService] is in the enabled list.
     */
    private fun isNotificationListenerEnabled(): Boolean {
        val flat = Settings.Secure.getString(
            contentResolver, "enabled_notification_listeners"
        ) ?: return false
        return flat.contains(
            ComponentName(this, NotificationService::class.java).flattenToString()
        )
    }

    /**
     * Checks whether the user has enabled [NavigationAccessibilityService],
     * by inspecting the system's `enabled_accessibility_services` secure setting.
     *
     * @return `true` if this app's [NavigationAccessibilityService] is in the enabled list.
     */
    private fun isAccessibilityEnabled(): Boolean {
        val flat = Settings.Secure.getString(
            contentResolver, "enabled_accessibility_services"
        ) ?: return false
        return flat.contains(
            ComponentName(this, NavigationAccessibilityService::class.java).flattenToString()
        )
    }

    /**
     * Requests whichever Bluetooth/location runtime permissions are missing
     * for BLE scanning, then runs [onGranted] — immediately if nothing is
     * missing, or once [onRequestPermissionsResult] confirms the grant.
     *
     * Uses [Manifest.permission.BLUETOOTH_SCAN]/`BLUETOOTH_CONNECT` on
     * Android 12+ (API 31), and falls back to
     * [Manifest.permission.ACCESS_FINE_LOCATION] on older versions where BLE
     * scanning required location permission instead.
     *
     * @param onGranted Callback invoked once all required permissions are granted.
     */
    private fun requestBlePermissions(onGranted: () -> Unit) {
        val needed = mutableListOf<String>()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (!hasPermission(Manifest.permission.BLUETOOTH_SCAN))
                needed += Manifest.permission.BLUETOOTH_SCAN
            if (!hasPermission(Manifest.permission.BLUETOOTH_CONNECT))
                needed += Manifest.permission.BLUETOOTH_CONNECT
        } else {
            if (!hasPermission(Manifest.permission.ACCESS_FINE_LOCATION))
                needed += Manifest.permission.ACCESS_FINE_LOCATION
        }
        if (needed.isEmpty()) { onGranted(); return }
        pendingAction = onGranted
        ActivityCompat.requestPermissions(this, needed.toTypedArray(), REQ_PERMS)
    }

    /**
     * Requests [Manifest.permission.POST_NOTIFICATIONS] on Android 13+ (API 33)
     * if not already granted. No-op on older versions, where the permission
     * doesn't exist and notifications need no runtime grant.
     *
     * Fire-and-forget: unlike [requestBlePermissions], nothing needs to wait
     * on the result here, so this doesn't go through [pendingAction]/
     * [onRequestPermissionsResult] — the system dialog shows on its own
     * request code and the user's answer just determines whether the status
     * notification becomes visible.
     */
    private fun requestNotificationPermissionIfNeeded() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU) return
        if (hasPermission(Manifest.permission.POST_NOTIFICATIONS)) return
        ActivityCompat.requestPermissions(
            this, arrayOf(Manifest.permission.POST_NOTIFICATIONS), REQ_NOTIF_PERMS
        )
    }

    /** @return `true` if permission [p] is currently granted to this app. */
    private fun hasPermission(p: String) =
        ContextCompat.checkSelfPermission(this, p) == PackageManager.PERMISSION_GRANTED

    /**
     * Receives the result of the runtime permission dialog triggered by
     * [requestBlePermissions], and invokes [pendingAction] if and only if
     * every requested permission was granted.
     */
    override fun onRequestPermissionsResult(
        requestCode: Int, permissions: Array<out String>, grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == REQ_PERMS &&
            grantResults.isNotEmpty() &&
            grantResults.all { it == PackageManager.PERMISSION_GRANTED }
        ) {
            pendingAction?.invoke()
        }
        pendingAction = null
    }

    companion object {
        /** Request code used for the runtime BLE/location permission dialog. */
        private const val REQ_PERMS = 1001

        /** Request code used for the runtime POST_NOTIFICATIONS dialog. */
        private const val REQ_NOTIF_PERMS = 1002
    }
}
