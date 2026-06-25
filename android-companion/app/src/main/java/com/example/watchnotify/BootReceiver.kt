package com.example.watchnotify

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent

/**
 * Restarts the [WatchForegroundService] after the device has finished booting.
 *
 * Without this receiver, the app would only start its BLE/notification-bridge
 * once the user manually opens it after a reboot. Requires the app to declare
 * a listener for [Intent.ACTION_BOOT_COMPLETED] (and the corresponding
 * `RECEIVE_BOOT_COMPLETED` permission) in the manifest.
 */
class BootReceiver : BroadcastReceiver() {

    /**
     * Called by the system when a registered broadcast is delivered.
     *
     * Ignores every action except [Intent.ACTION_BOOT_COMPLETED]; on boot it
     * starts [WatchForegroundService] so the BLE connection to the watch is
     * re-established without user interaction.
     *
     * @param context Context supplied by the system, used to start the service.
     * @param intent The received broadcast intent; only its `action` is inspected.
     */
    override fun onReceive(context: Context, intent: Intent) {
        if (intent.action != Intent.ACTION_BOOT_COMPLETED) return
        context.startForegroundService(Intent(context, WatchForegroundService::class.java))
    }
}
