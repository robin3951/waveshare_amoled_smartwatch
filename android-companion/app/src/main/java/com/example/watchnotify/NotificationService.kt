package com.example.watchnotify

import android.app.Notification
import android.os.Bundle
import android.service.notification.NotificationListenerService
import android.service.notification.StatusBarNotification
import android.util.Log

private const val TAG = "WatchNotif"

private val BLOCKED_PACKAGES = setOf(
    "android",
    "com.android.systemui",
    "com.android.settings",
    "com.google.android.gms",
    "com.google.android.gsf",
)

private val BLOCKED_CATEGORIES = setOf(
    Notification.CATEGORY_PROGRESS,
    Notification.CATEGORY_PROMO,
    Notification.CATEGORY_TRANSPORT,
    Notification.CATEGORY_SERVICE,
    Notification.CATEGORY_SYSTEM,
)

private val ONGOING_ALLOWED_CATEGORIES = setOf(
    Notification.CATEGORY_NAVIGATION,
    Notification.CATEGORY_CALL,
)

class NotificationService : NotificationListenerService() {

    // Suppress repeated identical notifications from the same app (e.g. Maps ETA updates)
    private val lastSent = mutableMapOf<String, String>()

    override fun onNotificationPosted(sbn: StatusBarNotification) {
        val pkg   = sbn.packageName
        val notif = sbn.notification
        val flags = notif.flags

        if (flags and Notification.FLAG_GROUP_SUMMARY != 0) return
        if (BLOCKED_PACKAGES.any { pkg.startsWith(it) }) return
        if (notif.category in BLOCKED_CATEGORIES) return

        val isOngoing = flags and Notification.FLAG_ONGOING_EVENT != 0
        if (isOngoing && notif.category !in ONGOING_ALLOWED_CATEGORIES) return

        val extras: Bundle = notif.extras

        // Debug-dump for Maps so we can see which fields contain the turn info
        if (pkg.contains("maps") || notif.category == Notification.CATEGORY_NAVIGATION) {
            dumpExtras(pkg, extras, notif)
        }

        val title   = extras.getString(Notification.EXTRA_TITLE)?.trim() ?: ""
        val text    = extras.getCharSequence(Notification.EXTRA_TEXT)?.toString()?.trim() ?: ""
        val bigText = extras.getCharSequence(Notification.EXTRA_BIG_TEXT)?.toString()?.trim() ?: ""
        val subText = extras.getCharSequence(Notification.EXTRA_SUB_TEXT)?.toString()?.trim() ?: ""
        val infoText= extras.getString(Notification.EXTRA_INFO_TEXT)?.trim() ?: ""
        val ticker  = notif.tickerText?.toString()?.trim() ?: ""
        val lines   = extras.getCharSequenceArray(Notification.EXTRA_TEXT_LINES)
            ?.mapNotNull { it?.toString()?.trim() }
            ?.filter { it.isNotBlank() }
            ?.joinToString(" | ") ?: ""

        // Best body candidate: prefer bigText, then text, then lines, then subText, then ticker
        val bodyCandidate = listOf(bigText, text, lines, subText, infoText, ticker)
            .firstOrNull { it.isNotBlank() } ?: ""

        val sender: String
        val body: String

        when {
            // Both title and body available → combine as "Title | Body" for navigation,
            // or show title as sender and body as message for normal notifications
            title.isNotBlank() && bodyCandidate.isNotBlank() -> {
                if (notif.category == Notification.CATEGORY_NAVIGATION) {
                    // Show full instruction: "In 200 m links – Musterstraße"
                    sender = appLabel(pkg)
                    body   = "$title – $bodyCandidate"
                } else {
                    sender = title
                    body   = bodyCandidate
                }
            }
            // Only title (Maps ongoing notification with instruction in title)
            title.isNotBlank() -> {
                sender = appLabel(pkg)
                body   = title
            }
            // Only body
            bodyCandidate.isNotBlank() -> {
                sender = appLabel(pkg)
                body   = bodyCandidate
            }
            else -> return
        }

        // Trim stray newlines (Google Maps leaves leading \n when title is empty)
        val cleanSender = sender.trim()
        val cleanBody   = body.trim()

        if (cleanBody.isBlank()) return

        // Suppress repeated identical updates from the same app (Maps ETA fires every ~3 s)
        val dedupeKey = "$cleanSender\n$cleanBody"
        if (lastSent[pkg] == dedupeKey) return
        lastSent[pkg] = dedupeKey

        Log.i(TAG, "pkg=$pkg  sender='$cleanSender'  body='${cleanBody.take(80)}'")
        BleManager.instance.log("📬 $cleanSender: ${cleanBody.take(40)}")
        BleManager.instance.sendNotification(pkg, cleanSender, cleanBody)
    }

    private fun dumpExtras(pkg: String, extras: Bundle, notif: Notification) {
        val sb = StringBuilder()
        sb.append("category=${notif.category} ticker=${notif.tickerText}\n")
        for (key in extras.keySet()) {
            val v = extras.get(key)
            if (v != null) sb.append("  $key=$v\n")
        }
        Log.d(TAG, "MAPS DUMP $pkg:\n$sb")
        // Show first 3 interesting lines in UI log
        val preview = sb.lines().filter { it.contains("TITLE") || it.contains("TEXT") || it.contains("ticker") }
            .take(3).joinToString(" | ")
        BleManager.instance.log("🗺 $preview")
    }

    private fun appLabel(pkg: String): String = when {
        pkg.contains("maps")      -> "Maps"
        pkg.contains("db")        -> "DB Navigator"
        pkg.contains("calendar")  -> "Kalender"
        pkg.contains("clock")     -> "Uhr"
        pkg.contains("gmail")     -> "Gmail"
        else -> pkg.substringAfterLast('.').replaceFirstChar { it.uppercase() }
    }
}
