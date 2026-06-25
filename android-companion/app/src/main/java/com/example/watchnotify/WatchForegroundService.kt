package com.example.watchnotify

import android.app.*
import android.content.Intent
import android.os.IBinder
import androidx.core.app.NotificationCompat

private const val CHANNEL_ID = "watch_fg"
private const val NOTIF_ID   = 42

/**
 * Foreground service whose only job is to keep the app process (and therefore
 * the [BleManager] singleton's GATT connection) alive while the app is not in
 * the foreground.
 *
 * Android aggressively kills background processes; a foreground service with
 * an active, low-importance notification is the standard way to prevent that
 * without requiring the user to keep the app open. Started initially from
 * [MainActivity] and again after reboot via [BootReceiver].
 */
class WatchForegroundService : Service() {

    /** Forwards [BleManager] status text into the persistent notification. */
    private val statusListener: (String) -> Unit = { status ->
        notificationManager().notify(NOTIF_ID, buildNotification(status))
    }

    /**
     * Creates the notification channel and subscribes to BLE status updates
     * so the notification text reflects the current connection state.
     */
    override fun onCreate() {
        super.onCreate()
        createChannel()
        BleManager.instance.addStatusListener(statusListener)
    }

    /**
     * Promotes the service to the foreground with an initial "Bereit…" status.
     *
     * @return [Service.START_STICKY] so the system recreates the service (and
     *   calls this again with a `null` intent) if it is killed under memory
     *   pressure, keeping the watch connection as persistent as possible.
     */
    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        startForeground(NOTIF_ID, buildNotification("Bereit…"))
        return START_STICKY   // Android restarts this service if killed
    }

    /** Unsubscribes from [BleManager] status updates to avoid leaking this service. */
    override fun onDestroy() {
        BleManager.instance.removeStatusListener(statusListener)
        super.onDestroy()
    }

    /** Not a bound service — always returns `null`. */
    override fun onBind(intent: Intent?): IBinder? = null

    /**
     * Registers the notification channel used for the persistent status
     * notification, with [NotificationManager.IMPORTANCE_LOW] so it never
     * shows as a heads-up popup or makes sound.
     */
    private fun createChannel() {
        val ch = NotificationChannel(
            CHANNEL_ID,
            "Smartwatch Verbindung",
            NotificationManager.IMPORTANCE_LOW    // silent, no pop-up
        ).apply { description = "Hält BLE-Verbindung zur Smartwatch aufrecht" }
        notificationManager().createNotificationChannel(ch)
    }

    /**
     * Builds the ongoing, silent status notification shown while the service runs.
     *
     * @param status Human-readable connection status (e.g. "Verbunden — Smartwatch verbunden ✓").
     * @return A non-dismissible [Notification] that opens [MainActivity] when tapped.
     */
    private fun buildNotification(status: String): Notification {
        val tap = PendingIntent.getActivity(
            this, 0,
            Intent(this, MainActivity::class.java),
            PendingIntent.FLAG_IMMUTABLE
        )
        return NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle("WatchNotify")
            .setContentText(status)
            .setSmallIcon(android.R.drawable.ic_dialog_info)
            .setContentIntent(tap)
            .setOngoing(true)
            .setSilent(true)
            .build()
    }

    /** Shorthand for the system [NotificationManager]. */
    private fun notificationManager() =
        getSystemService(NOTIFICATION_SERVICE) as NotificationManager
}
