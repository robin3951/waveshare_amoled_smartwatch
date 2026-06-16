package com.example.watchnotify

import android.accessibilityservice.AccessibilityService
import android.util.Log
import android.view.accessibility.AccessibilityEvent
import android.view.accessibility.AccessibilityNodeInfo

private const val TAG      = "NavAccess"
private const val MAPS_PKG = "com.google.android.apps.maps"

// Minimum time between two identical navigation updates sent to the watch
private const val DEBOUNCE_MS = 4_000L

class NavigationAccessibilityService : AccessibilityService() {

    private var lastPayload  = ""
    private var lastSentTime = 0L

    // ── AccessibilityService callbacks ────────────────────────────────────

    override fun onServiceConnected() {
        Log.i(TAG, "NavigationAccessibilityService connected")
        BleManager.instance.log("🗺 AccessibilityService aktiv")
    }

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

    override fun onInterrupt() {}

    // ── Notification event (heads-up, announced turns) ────────────────────

    private fun handleNotificationEvent(event: AccessibilityEvent) {
        val texts = event.text
            .map { it.toString().trim() }
            .filter { it.isNotBlank() }
        if (texts.isEmpty()) return

        val payload = texts.joinToString(" · ")
        send("Maps", payload)
    }

    // ── Window traversal (Maps in foreground) ─────────────────────────────

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

    private fun send(sender: String, payload: String) {
        if (payload == lastPayload) return
        lastPayload  = payload
        lastSentTime = System.currentTimeMillis()

        Log.i(TAG, "Nav → $sender: $payload")
        BleManager.instance.log("🗺 ${payload.take(50)}")
        BleManager.instance.sendNotification(MAPS_PKG, sender, payload)
    }
}
