package com.example.watchnotify

import android.app.*
import android.content.Intent
import android.os.IBinder
import androidx.core.app.NotificationCompat

private const val CHANNEL_ID = "watch_fg"
private const val NOTIF_ID   = 42

class WatchForegroundService : Service() {

    private val statusListener: (String) -> Unit = { status ->
        notificationManager().notify(NOTIF_ID, buildNotification(status))
    }

    override fun onCreate() {
        super.onCreate()
        createChannel()
        BleManager.instance.addStatusListener(statusListener)
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        startForeground(NOTIF_ID, buildNotification("Bereit…"))
        return START_STICKY   // Android restarts this service if killed
    }

    override fun onDestroy() {
        BleManager.instance.removeStatusListener(statusListener)
        super.onDestroy()
    }

    override fun onBind(intent: Intent?): IBinder? = null

    private fun createChannel() {
        val ch = NotificationChannel(
            CHANNEL_ID,
            "Smartwatch Verbindung",
            NotificationManager.IMPORTANCE_LOW    // silent, no pop-up
        ).apply { description = "Hält BLE-Verbindung zur Smartwatch aufrecht" }
        notificationManager().createNotificationChannel(ch)
    }

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

    private fun notificationManager() =
        getSystemService(NOTIFICATION_SERVICE) as NotificationManager
}
