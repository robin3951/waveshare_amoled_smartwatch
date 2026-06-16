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

class MainActivity : AppCompatActivity() {

    private lateinit var statusText: TextView
    private lateinit var listenerStatus: TextView
    private lateinit var logText: TextView
    private lateinit var connectButton: Button
    private lateinit var testButton: Button
    private var pendingAction: (() -> Unit)? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        statusText     = findViewById(R.id.statusText)
        listenerStatus = findViewById(R.id.listenerStatus)
        logText        = findViewById(R.id.logText)
        connectButton  = findViewById(R.id.connectButton)
        testButton     = findViewById(R.id.testButton)

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

    override fun onResume() {
        super.onResume()
        updateListenerStatus()
    }

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

    private fun isNotificationListenerEnabled(): Boolean {
        val flat = Settings.Secure.getString(
            contentResolver, "enabled_notification_listeners"
        ) ?: return false
        return flat.contains(
            ComponentName(this, NotificationService::class.java).flattenToString()
        )
    }

    private fun isAccessibilityEnabled(): Boolean {
        val flat = Settings.Secure.getString(
            contentResolver, "enabled_accessibility_services"
        ) ?: return false
        return flat.contains(
            ComponentName(this, NavigationAccessibilityService::class.java).flattenToString()
        )
    }

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

    private fun hasPermission(p: String) =
        ContextCompat.checkSelfPermission(this, p) == PackageManager.PERMISSION_GRANTED

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
        private const val REQ_PERMS = 1001
    }
}
