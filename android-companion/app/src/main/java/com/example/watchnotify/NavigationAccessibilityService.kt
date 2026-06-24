package com.example.watchnotify

import android.accessibilityservice.AccessibilityService
import android.util.Log
import android.view.accessibility.AccessibilityEvent
import android.view.accessibility.AccessibilityNodeInfo

private const val TAG      = "NavAccess"
private const val MAPS_PKG = "com.google.android.apps.maps"

// Minimum time between two identical navigation updates sent to the watch
private const val DEBOUNCE_MS = 4_000L

/**
 * [AccessibilityService] dedicated to extracting live Google Maps navigation
 * instructions and forwarding them to the watch via [BleManager].
 *
 * [NotificationService] alone is not sufficient for navigation: Maps often
 * updates its ongoing notification or screen content without re-posting a
 * notification the listener would see. This service instead reads Maps'
 * accessibility events/view tree directly, so turn-by-turn info (direction,
 * distance, street, ETA) reaches the watch even while Maps is in the
 * foreground and not posting fresh notifications. Must be enabled by the
 * user under Settings → Accessibility (checked in
 * [MainActivity.isAccessibilityEnabled]).
 */
class NavigationAccessibilityService : AccessibilityService() {

    /** Last payload sent, used to suppress duplicate sends. */
    private var lastPayload  = ""

    /** Timestamp (epoch ms) of the last successful send, used for [DEBOUNCE_MS]. */
    private var lastSentTime = 0L

    // ── AccessibilityService callbacks ────────────────────────────────────

    /** Logs that the service is up and running once Android binds it. */
    override fun onServiceConnected() {
        Log.i(TAG, "NavigationAccessibilityService connected")
        BleManager.instance.log("🗺 AccessibilityService aktiv")
    }

    /**
     * Entry point for every accessibility event system-wide.
     *
     * Immediately discards events from anything other than Google Maps, then
     * dispatches to [handleNotificationEvent] (for announced/heads-up turn
     * instructions) or [handleWindowChange] (for live on-screen navigation
     * UI) depending on the event type.
     *
     * @param event The accessibility event delivered by the system.
     */
    override fun onAccessibilityEvent(event: AccessibilityEvent) {
        val pkg = event.packageName?.toString() ?: return
        if (!pkg.contains("maps")) return

        when (event.eventType) {
            // Fires for heads-up / posted notifications — catches turn announcements
            AccessibilityEvent.TYPE_NOTIFICATION_STATE_CHANGED -> handleNotificationEvent(event)
            // Fires when Maps UI content changes — catches live navigation updates
            AccessibilityEvent.TYPE_WINDOW_CONTENT_CHANGED     -> handleWindowChange()
        }
    }

    /** Required override; navigation extraction doesn't need interrupt handling. */
    override fun onInterrupt() {}

    // ── Notification event (heads-up, announced turns) ────────────────────

    /**
     * Handles a [AccessibilityEvent.TYPE_NOTIFICATION_STATE_CHANGED] event
     * from Maps by concatenating its announced text segments and forwarding
     * them as a single payload via [send].
     *
     * @param event The notification-state-changed accessibility event.
     */
    private fun handleNotificationEvent(event: AccessibilityEvent) {
        val texts = event.text
            .map { it.toString().trim() }
            .filter { it.isNotBlank() }
        if (texts.isEmpty()) return

        val payload = texts.joinToString(" · ")
        send("Maps", payload)
    }

    // ── Window traversal (Maps in foreground) ─────────────────────────────

    /**
     * Handles a [AccessibilityEvent.TYPE_WINDOW_CONTENT_CHANGED] event by
     * walking Maps' current view hierarchy and building a navigation payload.
     *
     * Debounced via [DEBOUNCE_MS] because window-content-changed events fire
     * very frequently (effectively on every pixel of UI animation), and bails
     * out early if Maps is not the currently active window.
     */
    private fun handleWindowChange() {
        // Debounce: window content changes fire very frequently
        val now = System.currentTimeMillis()
        if (now - lastSentTime < DEBOUNCE_MS) return

        val root = rootInActiveWindow ?: return
        if (root.packageName?.toString()?.contains("maps") != true) {
            root.recycle()
            return
        }

        val texts = mutableListOf<String>()
        collectTexts(root, texts)
        root.recycle()

        val payload = buildNavPayload(texts)
        if (payload.isNotBlank()) send("Maps Navigation", payload)
    }

    // ── View-tree traversal ───────────────────────────────────────────────

    /**
     * Recursively collects displayable text from a view node and its children.
     *
     * Prefers [AccessibilityNodeInfo.getText]; falls back to
     * [AccessibilityNodeInfo.getContentDescription] for non-text elements like
     * the turn-arrow icon, which exposes its meaning only as a description.
     * Single-character strings are skipped as noise.
     *
     * @param node Current node to inspect (recycled by the caller after the
     *   top-level call returns; child nodes are recycled here).
     * @param out Mutable list accumulating collected text values, in tree order.
     */
    private fun collectTexts(node: AccessibilityNodeInfo, out: MutableList<String>) {
        val text = node.text?.toString()?.trim()
        val desc = node.contentDescription?.toString()?.trim()

        // Prefer text; fall back to content description (used for turn-arrow images)
        val value = when {
            !text.isNullOrBlank() -> text
            !desc.isNullOrBlank() -> desc
            else -> null
        }
        if (value != null && value.length > 1) out.add(value)

        for (i in 0 until node.childCount) {
            node.getChild(i)?.let { child ->
                collectTexts(child, out)
                child.recycle()
            }
        }
    }

    // ── Payload builder ───────────────────────────────────────────────────

    /**
     * Reconstructs a coherent navigation message from the unordered, raw
     * texts scraped out of Maps' view tree by [collectTexts].
     *
     * Maps' UI scatters direction, distance, street name, arrival time and
     * remaining time across many small, unlabeled views, so this uses regex
     * heuristics to classify each text fragment by shape (e.g. `"200 m"`
     * matches a distance pattern) rather than by position, then reassembles
     * them in a fixed, readable order.
     *
     * @param texts All text/content-description fragments found in the
     *   current Maps window, in arbitrary view-tree order.
     * @return A single human-readable payload like
     *   `"links abbiegen · 200 m · Musterstraße"`, or an empty string if no
     *   recognizable fragments were found.
     */
    private fun buildNavPayload(texts: List<String>): String {
        val distRx    = Regex("""^\d+[.,]?\d*\s*(m|km|mi|ft)$""",         RegexOption.IGNORE_CASE)
        val timeRx    = Regex("""^\d+\s*(min|h|Std\.?|Sek\.?)$""",        RegexOption.IGNORE_CASE)
        val arrivalRx = Regex("""(Ankunft|Arrive|ETA|um)\s+\d{1,2}:\d{2}""", RegexOption.IGNORE_CASE)
        val dirRx     = Regex("""(abbiegen|links|rechts|geradeaus|wenden|auffahrt|ausfahrt|
                                 |turn|left|right|straight|u.turn|merge|exit|ramp)""".trimIndent(),
                              RegexOption.IGNORE_CASE)

        val distance  = texts.find { it.matches(distRx) }
        val time      = texts.find { it.matches(timeRx) }
        val arrival   = texts.find { arrivalRx.containsMatchIn(it) }
        val direction = texts.find { dirRx.containsMatchIn(it) && it.length < 60 }

        // Street name: longest text that isn't one of the above, plausible length
        val used = setOfNotNull(distance, time, arrival, direction)
        val street = texts
            .filter { it !in used && !it.matches(distRx) && !it.matches(timeRx) }
            .filter { it.length in 3..60 }
            .maxByOrNull { it.length }

        val parts = listOfNotNull(direction, distance, street, arrival, time)
        return parts.distinct().joinToString(" · ")
    }

    // ── Dedup + forward ───────────────────────────────────────────────────

    /**
     * Sends a navigation payload to the watch unless it is identical to the
     * last one sent (regardless of which handler produced it).
     *
     * @param sender Display name shown as the notification sender on the watch.
     * @param payload The navigation text to send; deduplicated against [lastPayload].
     */
    private fun send(sender: String, payload: String) {
        if (payload == lastPayload) return
        lastPayload  = payload
        lastSentTime = System.currentTimeMillis()

        Log.i(TAG, "Nav → $sender: $payload")
        BleManager.instance.log("🗺 ${payload.take(50)}")
        BleManager.instance.sendNotification(MAPS_PKG, sender, payload)
    }
}
