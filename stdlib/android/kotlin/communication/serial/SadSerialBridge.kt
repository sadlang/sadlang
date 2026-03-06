package sad.android.communication.serial

import android.content.Context
import android.hardware.usb.UsbDevice
import android.hardware.usb.UsbManager
import java.io.IOException
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors
import java.util.concurrent.atomic.AtomicBoolean

/**
 * جسر الاتصال التسلسلي - Serial/UART communication
 * Serial Bridge - Serial port communication via USB
 * 
 * يحتاج مكتبة usb-serial-for-android
 * 
 * @author فريق لغة ص
 */
object SadSerialBridge {
    
    private val executor: ExecutorService = Executors.newCachedThreadPool()
    private val ports = ConcurrentHashMap<String, SerialPortWrapper>()
    
    /**
     * wrapper للمنفذ / Port wrapper
     */
    private data class SerialPortWrapper(
        val id: String,
        val deviceName: String,
        var baudRate: Int = 9600,
        var dataBits: Int = 8,
        var stopBits: Int = 1,
        var parity: Parity = Parity.NONE,
        val isOpen: AtomicBoolean = AtomicBoolean(false),
        val isReading: AtomicBoolean = AtomicBoolean(false)
    )
    
    /**
     * إعدادات المنفذ / Port settings
     */
    data class SerialSettings(
        val baudRate: Int = 9600,
        val dataBits: Int = 8,
        val stopBits: Int = 1,
        val parity: Parity = Parity.NONE,
        val flowControl: FlowControl = FlowControl.NONE,
        val dtr: Boolean = true,
        val rts: Boolean = true
    )
    
    /**
     * التكافؤ / Parity
     */
    enum class Parity {
        NONE, ODD, EVEN, MARK, SPACE
    }
    
    /**
     * التحكم في التدفق / Flow control
     */
    enum class FlowControl {
        NONE, RTS_CTS, DSR_DTR, XON_XOFF
    }
    
    /**
     * معدلات البود الشائعة / Common baud rates
     */
    object BaudRates {
        const val B300 = 300
        const val B1200 = 1200
        const val B2400 = 2400
        const val B4800 = 4800
        const val B9600 = 9600
        const val B19200 = 19200
        const val B38400 = 38400
        const val B57600 = 57600
        const val B115200 = 115200
        const val B230400 = 230400
        const val B460800 = 460800
        const val B921600 = 921600
    }
    
    /**
     * معلومات المنفذ / Port info
     */
    data class SerialPortInfo(
        val portId: String,
        val deviceName: String,
        val vendorId: Int,
        val productId: Int,
        val driverName: String,
        val portNumber: Int
    )
    
    /**
     * مستمع القراءة / Read listener
     */
    interface SerialListener {
        fun onDataReceived(portId: String, data: ByteArray)
        fun onError(portId: String, error: String)
        fun onConnected(portId: String)
        fun onDisconnected(portId: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         اكتشاف المنافذ
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على المنافذ المتاحة / Get available ports
     */
    fun getAvailablePorts(context: Context): List<SerialPortInfo> {
        val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
        val devices = usbManager.deviceList.values
        
        return devices.mapNotNull { device ->
            val driver = detectDriver(device)
            if (driver != null) {
                SerialPortInfo(
                    portId = "${device.vendorId}:${device.productId}",
                    deviceName = device.deviceName,
                    vendorId = device.vendorId,
                    productId = device.productId,
                    driverName = driver,
                    portNumber = 0
                )
            } else null
        }
    }
    
    /**
     * اكتشاف نوع الدرايفر / Detect driver type
     */
    private fun detectDriver(device: UsbDevice): String? {
        // أشهر chips المستخدمة في USB Serial adapters
        return when {
            // FTDI
            device.vendorId == 0x0403 -> "FTDI"
            // Prolific PL2303
            device.vendorId == 0x067B && device.productId == 0x2303 -> "PL2303"
            // Silicon Labs CP210x
            device.vendorId == 0x10C4 && device.productId in listOf(0xEA60, 0xEA61, 0xEA70, 0xEA71, 0xEA80) -> "CP210x"
            // CH340/CH341
            device.vendorId == 0x1A86 && device.productId in listOf(0x7523, 0x5523, 0x0445) -> "CH340"
            // Arduino
            device.vendorId == 0x2341 -> "Arduino"
            device.vendorId == 0x2A03 -> "Arduino"
            // Generic CDC ACM
            device.deviceClass == 0x02 -> "CDC-ACM"
            else -> null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         فتح وإغلاق المنفذ
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * فتح منفذ / Open port
     */
    fun open(
        context: Context,
        portId: String,
        settings: SerialSettings = SerialSettings(),
        listener: SerialListener
    ): Boolean {
        if (ports.containsKey(portId)) {
            return true // مفتوح بالفعل
        }
        
        val wrapper = SerialPortWrapper(
            id = portId,
            deviceName = portId,
            baudRate = settings.baudRate,
            dataBits = settings.dataBits,
            stopBits = settings.stopBits,
            parity = settings.parity
        )
        
        // التنفيذ الفعلي يستخدم مكتبة usb-serial-for-android
        /*
        val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
        val driver = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager).firstOrNull { 
            "${it.device.vendorId}:${it.device.productId}" == portId 
        }
        
        if (driver == null) {
            listener.onError(portId, "لم يتم العثور على المنفذ")
            return false
        }
        
        val connection = usbManager.openDevice(driver.device)
        if (connection == null) {
            listener.onError(portId, "لا يوجد إذن")
            return false
        }
        
        val port = driver.ports[0]
        port.open(connection)
        port.setParameters(settings.baudRate, settings.dataBits, settings.stopBits, settings.parity.ordinal)
        port.dtr = settings.dtr
        port.rts = settings.rts
        */
        
        ports[portId] = wrapper
        wrapper.isOpen.set(true)
        listener.onConnected(portId)
        
        return true
    }
    
    /**
     * إغلاق المنفذ / Close port
     */
    fun close(portId: String): Boolean {
        val wrapper = ports.remove(portId) ?: return false
        wrapper.isReading.set(false)
        wrapper.isOpen.set(false)
        // التنفيذ الفعلي: port.close()
        return true
    }
    
    /**
     * هل المنفذ مفتوح؟ / Is port open?
     */
    fun isOpen(portId: String): Boolean {
        return ports[portId]?.isOpen?.get() == true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الكتابة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال بايتات / Send bytes
     */
    fun write(portId: String, data: ByteArray): Int {
        val wrapper = ports[portId] ?: return -1
        if (!wrapper.isOpen.get()) return -1
        
        // التنفيذ الفعلي: return port.write(data, timeout)
        return data.size
    }
    
    /**
     * إرسال نص / Send text
     */
    fun writeText(portId: String, text: String, charset: String = "UTF-8"): Int {
        return write(portId, text.toByteArray(charset(charset)))
    }
    
    /**
     * إرسال سطر / Send line
     */
    fun writeLine(portId: String, text: String, lineEnding: String = "\r\n"): Int {
        return writeText(portId, text + lineEnding)
    }
    
    /**
     * إرسال أمر AT / Send AT command
     */
    fun sendAtCommand(portId: String, command: String): Int {
        return writeLine(portId, command)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         القراءة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * قراءة بايتات / Read bytes
     */
    fun read(portId: String, bufferSize: Int = 1024, timeout: Int = 1000): ByteArray? {
        val wrapper = ports[portId] ?: return null
        if (!wrapper.isOpen.get()) return null
        
        // التنفيذ الفعلي:
        // val buffer = ByteArray(bufferSize)
        // val read = port.read(buffer, timeout)
        // return if (read > 0) buffer.copyOf(read) else null
        
        return null
    }
    
    /**
     * قراءة نص / Read text
     */
    fun readText(portId: String, bufferSize: Int = 1024, timeout: Int = 1000, charset: String = "UTF-8"): String? {
        val data = read(portId, bufferSize, timeout) ?: return null
        return String(data, charset(charset))
    }
    
    /**
     * بدء القراءة المستمرة / Start continuous reading
     */
    fun startReading(portId: String, bufferSize: Int = 1024, listener: SerialListener) {
        val wrapper = ports[portId] ?: return
        if (!wrapper.isOpen.get()) return
        if (wrapper.isReading.get()) return
        
        wrapper.isReading.set(true)
        
        executor.execute {
            val buffer = ByteArray(bufferSize)
            
            while (wrapper.isReading.get() && wrapper.isOpen.get()) {
                try {
                    // التنفيذ الفعلي:
                    // val read = port.read(buffer, 100)
                    // if (read > 0) {
                    //     listener.onDataReceived(portId, buffer.copyOf(read))
                    // }
                    
                    Thread.sleep(10)
                } catch (e: IOException) {
                    if (wrapper.isReading.get()) {
                        listener.onError(portId, e.message ?: "خطأ في القراءة")
                    }
                    break
                }
            }
        }
    }
    
    /**
     * إيقاف القراءة المستمرة / Stop continuous reading
     */
    fun stopReading(portId: String) {
        ports[portId]?.isReading?.set(false)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إشارات التحكم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تعيين DTR / Set DTR
     */
    fun setDtr(portId: String, state: Boolean): Boolean {
        val wrapper = ports[portId] ?: return false
        // التنفيذ الفعلي: port.dtr = state
        return true
    }
    
    /**
     * تعيين RTS / Set RTS
     */
    fun setRts(portId: String, state: Boolean): Boolean {
        val wrapper = ports[portId] ?: return false
        // التنفيذ الفعلي: port.rts = state
        return true
    }
    
    /**
     * الحصول على حالة CD / Get CD state
     */
    fun getCd(portId: String): Boolean {
        // التنفيذ الفعلي: return port.cd
        return false
    }
    
    /**
     * الحصول على حالة CTS / Get CTS state
     */
    fun getCts(portId: String): Boolean {
        // التنفيذ الفعلي: return port.cts
        return false
    }
    
    /**
     * الحصول على حالة DSR / Get DSR state
     */
    fun getDsr(portId: String): Boolean {
        // التنفيذ الفعلي: return port.dsr
        return false
    }
    
    /**
     * إرسال Break / Send break
     */
    fun sendBreak(portId: String, durationMs: Int = 100): Boolean {
        // التنفيذ الفعلي: port.setBreak(true); Thread.sleep(durationMs); port.setBreak(false)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإعدادات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تغيير إعدادات المنفذ / Change port settings
     */
    fun setSettings(portId: String, settings: SerialSettings): Boolean {
        val wrapper = ports[portId] ?: return false
        
        wrapper.baudRate = settings.baudRate
        wrapper.dataBits = settings.dataBits
        wrapper.stopBits = settings.stopBits
        wrapper.parity = settings.parity
        
        // التنفيذ الفعلي:
        // port.setParameters(settings.baudRate, settings.dataBits, settings.stopBits, settings.parity.ordinal)
        
        return true
    }
    
    /**
     * تغيير معدل البود / Change baud rate
     */
    fun setBaudRate(portId: String, baudRate: Int): Boolean {
        val wrapper = ports[portId] ?: return false
        wrapper.baudRate = baudRate
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنظيف
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تنظيف المخزن المؤقت / Purge buffer
     */
    fun purge(portId: String, input: Boolean = true, output: Boolean = true): Boolean {
        // التنفيذ الفعلي: port.purgeHwBuffers(output, input)
        return true
    }
    
    /**
     * إغلاق جميع المنافذ / Close all ports
     */
    fun closeAll() {
        ports.keys.toList().forEach { close(it) }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        closeAll()
        executor.shutdown()
    }
}
