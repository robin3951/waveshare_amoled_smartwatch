package com.example.watchnotify

import android.annotation.SuppressLint
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothGatt
import android.bluetooth.BluetoothGattCallback
import android.bluetooth.BluetoothGattCharacteristic
import android.bluetooth.BluetoothGattDescriptor
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothProfile
import android.bluetooth.le.ScanCallback
import android.bluetooth.le.ScanFilter
import android.bluetooth.le.ScanResult
import android.bluetooth.le.ScanSettings
import android.content.Context
import android.os.Build
import android.os.Handler
import android.os.Looper
import android.util.Log
import java.util.UUID
import java.util.concurrent.LinkedBlockingQueue

private const val TAG = "WatchBle"

// Nordic UART Service UUIDs
private val NUS_SERVICE = UUID.fromString("6e400001-b5a3-f393-e0a9-e50e24dcca9e")
private val NUS_RX = UUID.fromString("6e400002-b5a3-f393-e0a9-e50e24dcca9e") // phone → watch
private val NUS_TX = UUID.fromString("6e400003-b5a3-f393-e0a9-e50e24dcca9e") // watch → phone
private val CCCD_UUID = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")

private const val DEVICE_NAME = "Chronos"

/**
 * Manages BLE connection to the watch and sending notification packets.
 * Uses Nordic UART Service (NUS) for communication.
 * MainActivity and WatchForegroundService can both subscribe to status updates.
 * Reconnects automatically if connection is lost.
 */
@SuppressLint("MissingPermission")
class BleManager private constructor() {

    companion object {
        /** The single, process-wide [BleManager] instance shared by every component. */
        val instance = BleManager()
    }

    // Multiple listeners so both MainActivity and WatchForegroundService can subscribe
    /** Callbacks invoked on the main thread whenever the connection status changes. */
    private val statusListeners = mutableListOf<(String) -> Unit>()

    /** Single-slot status callback kept for [MainActivity]'s original API; prefer [addStatusListener]. */
    var onStatusChange: ((String) -> Unit)? = null   // kept for MainActivity compatibility

    /** Callback invoked on the main thread for each new debug/UI log line (see [log]). */
    var onLogLine: ((String) -> Unit)? = null

    /**
     * Subscribes [l] to connection-status updates (see [setStatus]).
     *
     * Unlike [onStatusChange], multiple listeners can be registered at once,
     * which is what allows both [MainActivity] and [WatchForegroundService]
     * to observe status changes simultaneously.
     *
     * @param l Callback invoked on the main thread with a human-readable status string.
     */
    fun addStatusListener(l: (String) -> Unit) {
        statusListeners.add(l)
    }

    /**
     * Unsubscribes a listener previously added with [addStatusListener].
     *
     * @param l The exact callback instance to remove.
     */
    fun removeStatusListener(l: (String) -> Unit) {
        statusListeners.remove(l)
    }

    /** Application context captured in [startScan], used for GATT operations without leaking an Activity. */
    private var appContext: Context? = null

    /** Active GATT connection to the watch, or `null` if not connected. */
    private var gatt: BluetoothGatt? = null

    /** The NUS RX characteristic (phone → watch) used to write outgoing packets. */
    private var rxChar: BluetoothGattCharacteristic? = null

    /** ATT MTU negotiated with the watch; defaults to the BLE minimum of 23 bytes until negotiated. */
    private var negotiatedMtu = 23

    /** Pending packet chunks awaiting a write, since only one GATT write can be in flight at a time. */
    private val writeQueue = LinkedBlockingQueue<ByteArray>()

    /** `true` while a characteristic write is in flight and awaiting [BluetoothGattCallback.onCharacteristicWrite]. */
    private var writePending = false

    /** The most recently scanned/connected device, kept so [scheduleReconnect] can retry it. */
    private var lastDevice: BluetoothDevice? = null

    /** Main-thread handler used to dispatch callbacks and schedule the reconnect delay. */
    private val handler = Handler(Looper.getMainLooper())

    /**
     * Logs and broadcasts a connection-status update to [onStatusChange] and
     * every listener registered via [addStatusListener], always on the main thread.
     *
     * @param msg Human-readable status string (German, shown directly in the UI).
     */
    private fun setStatus(msg: String) {
        Log.i(TAG, msg)
        handler.post {
            onStatusChange?.invoke(msg)
            statusListeners.forEach { it(msg) }
        }
    }

    /**
     * Logs and forwards a debug/activity line (e.g. a notification that was
     * sent) to [onLogLine], on the main thread.
     *
     * @param msg Free-form log line, typically prefixed with an emoji marker
     *   (e.g. `"📬 ..."` for sent notifications, `"✗ ..."` for failures).
     */
    fun log(msg: String) {
        Log.i(TAG, msg)
        handler.post { onLogLine?.invoke(msg) }
    }

    // ── Scan & connect ────────────────────────────────────────────────────
    /**
     * Starts scanning for the watch device.
     * 
     * @param context Any valid Context, used for BluetoothManager and GATT
     *   connection. Application context is used internally to avoid leaks.
     */
    fun startScan(context: Context) {
        appContext = context.applicationContext
        val btManager = context.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        val scanner = btManager.adapter?.bluetoothLeScanner ?: run {
            setStatus("BLE nicht verfügbar"); return
        }
        setStatus("Suche nach \"$DEVICE_NAME\"…")

        val filter = ScanFilter.Builder().setDeviceName(DEVICE_NAME).build()
        val settings = ScanSettings.Builder()
            .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY).build()

        scanner.startScan(listOf(filter), settings, object : ScanCallback() {
            override fun onScanResult(type: Int, result: ScanResult) {
                scanner.stopScan(this)
                lastDevice = result.device
                connect(result.device)
            }

            override fun onScanFailed(errorCode: Int) {
                setStatus("Scan-Fehler: $errorCode")
            }
        })
    }

    /**
     * Opens (or re-opens) a GATT connection to [device], closing any previous
     * connection first to avoid leaking GATT client resources.
     *
     * @param device The watch's Bluetooth device, obtained from a scan result
     *   or from [lastDevice] on reconnect.
     */
    private fun connect(device: BluetoothDevice) {
        val ctx = appContext ?: return
        setStatus("Verbinde mit ${device.address}…")
        gatt?.close()
        gatt = device.connectGatt(ctx, false, gattCallback, BluetoothDevice.TRANSPORT_LE)
    }

    /**
     * Schedules a reconnect attempt to [lastDevice] 5 seconds from now.
     *
     * Called whenever the GATT connection drops unexpectedly, so the watch
     * link recovers automatically (e.g. after walking out of BLE range)
     * without requiring the user to tap "connect" again.
     */
    private fun scheduleReconnect() {
        val dev = lastDevice ?: return
        setStatus("Verbindung getrennt — Neuversuch in 5 s…")
        handler.postDelayed({ connect(dev) }, 5_000)
    }

    // ── GATT callbacks ────────────────────────────────────────────────────

    /**
     * Drives the full connect → MTU-negotiate → discover-services → ready
     * sequence, and handles disconnects by cleaning up state and triggering
     * [scheduleReconnect].
     */
    private val gattCallback = object : BluetoothGattCallback() {

        /**
         * On connect, kicks off MTU negotiation (512 bytes requested, to
         * allow larger notification payloads per write). On disconnect,
         * clears all connection-scoped state and schedules a reconnect.
         */
        override fun onConnectionStateChange(g: BluetoothGatt, status: Int, newState: Int) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                setStatus("Verbunden — MTU-Verhandlung…")
                g.requestMtu(512)
            } else {
                rxChar = null
                writeQueue.clear()
                writePending = false
                gatt?.close()
                gatt = null
                scheduleReconnect()
            }
        }

        /** Records the actually-negotiated MTU (used to size write chunks) and starts service discovery. */
        override fun onMtuChanged(g: BluetoothGatt, mtu: Int, status: Int) {
            negotiatedMtu = mtu
            Log.i(TAG, "MTU=$mtu")
            g.discoverServices()
        }

        /**
         * Locates the Nordic UART Service, caches the RX characteristic for
         * outgoing writes, and subscribes to TX notifications so the watch's
         * own handshake/battery messages can be received. Marks the
         * connection ready once done.
         */
        override fun onServicesDiscovered(g: BluetoothGatt, status: Int) {
            val svc = g.getService(NUS_SERVICE) ?: run {
                setStatus("NUS-Dienst nicht gefunden"); return
            }
            rxChar = svc.getCharacteristic(NUS_RX)

            // Subscribe to TX so watch sends its info/battery handshake
            val txChar = svc.getCharacteristic(NUS_TX)
            if (txChar != null) {
                g.setCharacteristicNotification(txChar, true)
                val desc = txChar.getDescriptor(CCCD_UUID)
                if (desc != null) {
                    writeDescriptorCompat(g, desc, BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE)
                }
            }
            setStatus("Bereit — Smartwatch verbunden ✓")
        }

        /** Frees up the write slot and immediately attempts to send the next queued chunk. */
        override fun onCharacteristicWrite(
            g: BluetoothGatt, char: BluetoothGattCharacteristic, status: Int
        ) {
            writePending = false
            drainQueue(g)
        }
    }

    // ── Packet building ───────────────────────────────────────────────────

    /**
     * Builds a Chronos-protocol notification packet and queues it for
     * transmission to the watch.
     *
     * Wire format: `AB [len_hi] [len_low] FF 72 80 [icon] 02 [sender\nmessage\0]`
     * — `AB` is the packet marker, the two length bytes cover everything
     * after them, `FF 72 80` is the fixed Chronos notification-packet header,
     * `[icon]` is resolved via [packageToIcon], `02` marks this as a text
     * payload, followed by `"sender\nmessage"` UTF-8 encoded and
     * NUL-terminated. The packet is then split into MTU-sized chunks (see
     * [negotiatedMtu]) and handed to [drainQueue].
     *
     * No-ops (and logs via [log]) if there is no active connection or RX
     * characteristic.
     *
     * @param packageName Android package name of the notifying app, used by [packageToIcon].
     * @param sender Display name shown as the message sender on the watch.
     * @param text Message body shown on the watch.
     */
    fun sendNotification(packageName: String, sender: String, text: String) {
        val g = gatt ?: run { log("✗ nicht verbunden — Nachricht verworfen"); return }
        val rc = rxChar ?: run { log("✗ kein RX-Handle — Nachricht verworfen"); return }
        log("→ $sender: ${text.take(40)}")

        val icon = packageToIcon(packageName)
        val rawText = "$sender\n$text"
        val textBytes = rawText.toByteArray(Charsets.UTF_8) + byteArrayOf(0x00)

        // payload = FF 72 80 icon 02 text...
        val payload = byteArrayOf(0xFF.toByte(), 0x72, 0x80.toByte(), icon, 0x02) + textBytes
        val len = payload.size
        val packet = byteArrayOf(0xAB.toByte(), (len shr 8).toByte(), (len and 0xFF).toByte()) + payload

        // Split into chunks the BLE link can carry (MTU - 3 bytes ATT overhead)
        val chunkSize = (negotiatedMtu - 3).coerceAtLeast(20)
        var offset = 0
        while (offset < packet.size) {
            writeQueue.add(packet.copyOfRange(offset, minOf(offset + chunkSize, packet.size)))
            offset += chunkSize
        }
        drainQueue(g)
    }

    /**
     * Sends the next queued chunk, if any, provided no write is already in
     * flight. Safe to call repeatedly/redundantly — it is a no-op if a write
     * is pending or the queue is empty.
     *
     * @param g The active [BluetoothGatt] connection to write on.
     */
    private fun drainQueue(g: BluetoothGatt) {
        if (writePending) return
        val chunk = writeQueue.poll() ?: return
        val rc = rxChar ?: return
        writePending = true
        writeCharacteristicCompat(g, rc, chunk)
    }

    // ── Compat helpers for deprecated API changes in Android 13 ──────────

    /**
     * Writes [value] to [char], using the non-deprecated API on Android 13+
     * (API 33) and the legacy `char.value =`/`writeCharacteristic()` pattern
     * on older versions, since the old setter-based API was removed/deprecated.
     *
     * @param g The active GATT connection.
     * @param char The characteristic to write to (the NUS RX characteristic).
     * @param value The raw bytes to write.
     */
    @Suppress("DEPRECATION")
    private fun writeCharacteristicCompat(
        g: BluetoothGatt, char: BluetoothGattCharacteristic, value: ByteArray
    ) {
        if (Build.VERSION.SDK_INT >= 33) {
            g.writeCharacteristic(char, value, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        } else {
            char.value = value
            char.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
            g.writeCharacteristic(char)
        }
    }

    /**
     * Writes [value] to [desc] using the same API-level compatibility
     * pattern as [writeCharacteristicCompat], used here to enable
     * notifications on the NUS TX characteristic's CCCD descriptor.
     *
     * @param g The active GATT connection.
     * @param desc The descriptor to write to (typically the CCCD, [CCCD_UUID]).
     * @param value The raw bytes to write (e.g. [BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE]).
     */
    @Suppress("DEPRECATION")
    private fun writeDescriptorCompat(
        g: BluetoothGatt, desc: BluetoothGattDescriptor, value: ByteArray
    ) {
        if (Build.VERSION.SDK_INT >= 33) {
            g.writeDescriptor(desc, value)
        } else {
            desc.value = value
            g.writeDescriptor(desc)
        }
    }

    // ── Icon mapping: Android package name → Chronos icon byte ───────────

    /**
     * Maps an Android package name to the icon byte expected by the Chronos
     * BLE protocol, so the watch can render an app-specific icon next to the
     * notification.
     *
     * @param pkg Android package name of the notifying app.
     * @return The Chronos icon code, or `0x00` ("none"/generic) if no
     *   specific app is recognized.
     */
    @Suppress("CyclomaticComplexMethod") // flat package-name -> icon-byte lookup table, not genuine logic complexity
    private fun packageToIcon(pkg: String): Byte {
        val code = when {
            pkg.contains("whatsapp") -> 0x0A
            pkg.contains("telegram") -> 0x17
            pkg.contains("viber") -> 0x13
            pkg.contains("skype") -> 0x14
            pkg.contains("wechat") || pkg.contains("weixin") -> 0x09
            pkg.contains("gmail") -> 0x0B
            pkg.contains("instagram") -> 0x12
            pkg.contains("snapchat") -> 0x20
            pkg.contains("linkedin") -> 0x11
            pkg.contains("twitter") || pkg.contains("x.android") -> 0x0E
            pkg.contains("facebook") || pkg.contains("orca") -> 0x0F
            pkg.contains("mms") || pkg.contains("messaging") ||
                    pkg.contains(".sms") -> 0x03

            pkg.contains("maps") || pkg.contains("navigation") -> 0x0D  // "Google"
            else -> 0x00
        }
        return code.toByte()
    }
}
