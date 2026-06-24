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

/**
 * System-wide [NotificationListenerService] that mirrors relevant phone
 * notifications to the smartwatch via [BleManager].
 *
 * Must be enabled by the user under Settings → Notification access (checked
 * in [MainActivity.isNotificationListenerEnabled]). Filters out noise (system
 * UI, ongoing progress notifications, grouped summaries), picks the most
 * informative text field out of the several Android offers, and deduplicates
 * per-package so rapidly-updating notifications (e.g. Maps ETA) don't flood
 * the BLE link.
 */
class NotificationService : NotificationListenerService() {

    // Suppress repeated identical notifications from the same app (e.g. Maps ETA updates)
    private val lastSent = mutableMapOf<String, String>()

    /**
     * Called by the system whenever any app posts or updates a notification.
     *
     * Applies several filters (package blocklist, category blocklist, ongoing
     * events outside [ONGOING_ALLOWED_CATEGORIES]), extracts the best
     * available title/body combination, deduplicates against the last
     * notification sent for that package, and forwards the result to
     * [BleManager.sendNotification].
     *
     * @param sbn The status bar notification posted by the system.
     */
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

    /**
     * Logs every extra field of a Google Maps notification for debugging.
     *
     * Maps' navigation-notification layout varies by version/locale, so this
     * helper makes it possible to see at runtime (via [BleManager.log] / Logcat)
     * which extras actually carry the turn instruction, distance, etc.
     *
     * @param pkg Source package name (used only for the log line).
     * @param extras The notification's extras [Bundle].
     * @param notif The full [Notification], used for category/ticker info.
     */
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

    /**
     * Maps a package name to a short, human-readable sender label.
     *
     * Used as a fallback "sender" when a notification has no usable title,
     * e.g. Maps' ongoing navigation notification.
     *
     * @param pkg Android package name of the notifying app.
     * @return A short display name, falling back to the capitalized last
     *   segment of the package name if no special case matches.
     */
    private fun appLabel(pkg: String): String = when {
        pkg.contains("maps")      -> "Maps"
        pkg.contains("db")        -> "DB Navigator"
        pkg.contains("calendar")  -> "Kalender"
        pkg.contains("clock")     -> "Uhr"
        pkg.contains("gmail")     -> "Gmail"
        else -> pkg.substringAfterLast('.').replaceFirstChar { it.uppercase() }
    }
}
