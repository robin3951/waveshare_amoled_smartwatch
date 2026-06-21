package com.example.watchnotify

import android.annotation.SuppressLint
import android.bluetooth.*
import android.bluetooth.le.*
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
private val NUS_RX      = UUID.fromString("6e400002-b5a3-f393-e0a9-e50e24dcca9e") // phone → watch
private val NUS_TX      = UUID.fromString("6e400003-b5a3-f393-e0a9-e50e24dcca9e") // watch → phone
private val CCCD_UUID   = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb")

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
        val instance = BleManager()
    }

    // Multiple listeners so both MainActivity and WatchForegroundService can subscribe
    private val statusListeners = mutableListOf<(String) -> Unit>()
    var onStatusChange: ((String) -> Unit)? = null   // kept for MainActivity compatibility
    var onLogLine: ((String) -> Unit)? = null

    fun addStatusListener(l: (String) -> Unit)    { statusListeners.add(l) }
    fun removeStatusListener(l: (String) -> Unit) { statusListeners.remove(l) }

    private var appContext: Context? = null
    private var gatt: BluetoothGatt? = null
    private var rxChar: BluetoothGattCharacteristic? = null
    private var negotiatedMtu = 23
    private val writeQueue = LinkedBlockingQueue<ByteArray>()
    private var writePending = false
    private var lastDevice: BluetoothDevice? = null
    private val handler = Handler(Looper.getMainLooper())

    private fun setStatus(msg: String) {
        Log.i(TAG, msg)
        handler.post {
            onStatusChange?.invoke(msg)
            statusListeners.forEach { it(msg) }
        }
    }

    fun log(msg: String) {
        Log.i(TAG, msg)
        handler.post { onLogLine?.invoke(msg) }
    }

    // ── Scan & connect ────────────────────────────────────────────────────
    /**
     * Starts scanning for the watch device.
     * 
     * @param context Any valid Context, used for BluetoothManager and GATT connection. Application context is used internally to avoid leaks.
     */
    fun startScan(context: Context) {
        appContext = context.applicationContext
        val btManager = context.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        val scanner = btManager.adapter?.bluetoothLeScanner ?: run {
            setStatus("BLE nicht verfügbar"); return
        }
        setStatus("Suche nach \"$DEVICE_NAME\"…")

        val filter   = ScanFilter.Builder().setDeviceName(DEVICE_NAME).build()
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

    private fun connect(device: BluetoothDevice) {
        val ctx = appContext ?: return
        setStatus("Verbinde mit ${device.address}…")
        gatt?.close()
        gatt = device.connectGatt(ctx, false, gattCallback, BluetoothDevice.TRANSPORT_LE)
    }

    private fun scheduleReconnect() {
        val dev = lastDevice ?: return
        setStatus("Verbindung getrennt — Neuversuch in 5 s…")
        handler.postDelayed({ connect(dev) }, 5_000)
    }

    // ── GATT callbacks ────────────────────────────────────────────────────

    private val gattCallback = object : BluetoothGattCallback() {

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

        override fun onMtuChanged(g: BluetoothGatt, mtu: Int, status: Int) {
            negotiatedMtu = mtu
            Log.i(TAG, "MTU=$mtu")
            g.discoverServices()
        }

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

        override fun onCharacteristicWrite(
            g: BluetoothGatt, char: BluetoothGattCharacteristic, status: Int
        ) {
            writePending = false
            drainQueue(g)
        }
    }

    // ── Packet building ───────────────────────────────────────────────────

    /**
     * Build and queue a notification packet for the watch.
     * Protocol: AB [len_hi] [len_low] FF 72 80 [icon] 02 [sender\nmessage\0]
     */
    fun sendNotification(packageName: String, sender: String, text: String) {
        val g  = gatt ?: run { log("✗ nicht verbunden — Nachricht verworfen"); return }
        val rc = rxChar ?: run { log("✗ kein RX-Handle — Nachricht verworfen"); return }
        log("→ $sender: ${text.take(40)}")

        val icon      = packageToIcon(packageName)
        val rawText   = "$sender\n$text"
        val textBytes = rawText.toByteArray(Charsets.UTF_8) + byteArrayOf(0x00)

        // payload = FF 72 80 icon 02 text...
        val payload   = byteArrayOf(0xFF.toByte(), 0x72, 0x80.toByte(), icon, 0x02) + textBytes
        val len       = payload.size
        val packet    = byteArrayOf(0xAB.toByte(), (len shr 8).toByte(), (len and 0xFF).toByte()) + payload

        // Split into chunks the BLE link can carry (MTU - 3 bytes ATT overhead)
        val chunkSize = (negotiatedMtu - 3).coerceAtLeast(20)
        var offset = 0
        while (offset < packet.size) {
            writeQueue.add(packet.copyOfRange(offset, minOf(offset + chunkSize, packet.size)))
            offset += chunkSize
        }
        drainQueue(g)
    }

    private fun drainQueue(g: BluetoothGatt) {
        if (writePending) return
        val chunk = writeQueue.poll() ?: return
        val rc = rxChar ?: return
        writePending = true
        writeCharacteristicCompat(g, rc, chunk)
    }

    // ── Compat helpers for deprecated API changes in Android 13 ──────────

    @Suppress("DEPRECATION")
    private fun writeCharacteristicCompat(
        g: BluetoothGatt, char: BluetoothGattCharacteristic, value: ByteArray
    ) {
        if (Build.VERSION.SDK_INT >= 33) {
            g.writeCharacteristic(char, value, BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT)
        } else {
            char.value     = value
            char.writeType = BluetoothGattCharacteristic.WRITE_TYPE_DEFAULT
            g.writeCharacteristic(char)
        }
    }

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

    private fun packageToIcon(pkg: String): Byte {
        val code = when {
            pkg.contains("whatsapp")                              -> 0x0A
            pkg.contains("telegram")                             -> 0x17
            pkg.contains("viber")                               -> 0x13
            pkg.contains("skype")                               -> 0x14
            pkg.contains("wechat") || pkg.contains("weixin")    -> 0x09
            pkg.contains("gmail")                               -> 0x0B
            pkg.contains("instagram")                           -> 0x12
            pkg.contains("snapchat")                            -> 0x20
            pkg.contains("linkedin")                            -> 0x11
            pkg.contains("twitter") || pkg.contains("x.android") -> 0x0E
            pkg.contains("facebook") || pkg.contains("orca")   -> 0x0F
            pkg.contains("mms") || pkg.contains("messaging") ||
                pkg.contains(".sms")                            -> 0x03
            pkg.contains("maps") || pkg.contains("navigation") -> 0x0D  // "Google"
            else                                                -> 0x00
        }
        return code.toByte()
    }
}
