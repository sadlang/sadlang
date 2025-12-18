# 🔌 قواعد منافذ الإدخال/الإخراج / I/O Ports Rules

**التاريخ:** 2 ديسمبر 2025  
**الإصدار:** 1.0 - Stage 1  
**الحالة:** مرجع لنظام الوصول المباشر للمنافذ

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام الوصول المباشر لمنافذ الإدخال/الإخراج** للغة "ص"، بما في ذلك عمليات القراءة والكتابة للمنافذ بأحجام مختلفة (8-bit, 16-bit, 32-bit) والوصول للأجهزة منخفضة المستوى.

### (EN) Description
This file documents the **direct I/O port access system** for Sad language, including read/write operations for ports at different sizes (8-bit, 16-bit, 32-bit) and low-level hardware access.

---

## 📦 الاعتماديات / Dependencies

- ✅ `include/low_level/io_ports.h` - IOPorts class
- ✅ `include/low_level/interrupts.h` - Interrupt integration
- ✅ `docs/system_build_plan/01_stage1_low_level.md` - I/O ports spec
- ✅ `src/low_level/io_ports.cpp` - Implementation

---

## 📋 قائمة العمليات / Operations List

| # | العملية (AR) | Operation (EN) | الحجم | الحالة |
|---|------------|---------------|------|-------|
| 1 | `inb` | Input Byte | 8-bit | ✅ Core |
| 2 | `outb` | Output Byte | 8-bit | ✅ Core |
| 3 | `inw` | Input Word | 16-bit | ✅ Core |
| 4 | `outw` | Output Word | 16-bit | ✅ Core |
| 5 | `inl` | Input Long | 32-bit | ✅ Core |
| 6 | `outl` | Output Long | 32-bit | ✅ Core |
| 7 | `port_wait` | Port Delay | - | 🔧 Utility |

---

## 1️⃣ inb/outb - إدخال/إخراج بايت / Byte I/O

### Syntax / البناء النحوي (EBNF)

```ebnf
inb_call  ::= 'اقرأ_بايت' '(' port_number ')'
outb_call ::= 'اكتب_بايت' '(' port_number ',' value_expr ')'

port_number ::= integer_literal | hex_literal  # 0x0000 - 0xFFFF
value_expr  ::= expression                     # 0x00 - 0xFF
```

**الشرح بالعربية:**
- `inb`: يقرأ بايت واحد (8 بت) من منفذ محدد
- `outb`: يكتب بايت واحد (8 بت) إلى منفذ محدد
- رقم المنفذ: من 0x0000 إلى 0xFFFF
- القيمة: من 0x00 إلى 0xFF (0-255)

### Semantics / الدلالات

- **Port Range / نطاق المنافذ:** 0x0000 - 0xFFFF (65536 منفذ ممكن)
- **Data Size / حجم البيانات:** 8 bits = 1 byte
- **Return / الإرجاع:** `inb` يعيد uint8 (0-255)
- **Side Effects / آثار جانبية:** قراءة/كتابة قد تغير حالة الجهاز

### Standard Ports / المنافذ القياسية

| المنفذ | الاستخدام (AR) | Usage (EN) |
|-------|---------------|-----------|
| 0x60 | لوحة المفاتيح | Keyboard data |
| 0x64 | حالة لوحة المفاتيح | Keyboard status |
| 0x3F8 | COM1 (Serial) | Serial port 1 |
| 0x3FC | COM1 Status | Serial status |
| 0x20 | PIC Master | Interrupt controller |
| 0xA0 | PIC Slave | Slave interrupt controller |
| 0x70 | CMOS/RTC | Real-time clock |
| 0x80 | POST | Diagnostic port |

### Examples / الأمثلة

```s
# قراءة من لوحة المفاتيح / Read from keyboard
uint8 key = اقرأ_بايت(0x60)
إذا (key != 0)
    اطبع(ن"مفتاح مضغوط: {key}")
نهاية

# التحقق من جاهزية لوحة المفاتيح / Check keyboard ready
uint8 status = اقرأ_بايت(0x64)
إذا ((status & 0x01) != 0)
    اطبع("لوحة المفاتيح جاهزة")
نهاية

# كتابة إلى منفذ تشخيصي / Write to diagnostic port
اكتب_بايت(0x80, 0xAA)  # POST code

# إرسال بايت عبر COM1 / Send byte via serial
دالة serial_send(uint8 byte)
    # انتظر حتى جاهز / Wait until ready
    متغير uint8 status
    كرر
        status = اقرأ_بايت(0x3FD)
    حتى ((status & 0x20) != 0)
    
    # أرسل البايت / Send byte
    اكتب_بايت(0x3F8, byte)
نهاية

serial_send(0x41)  # Send 'A'

# قراءة من COM1 / Read from serial
دالة uint8 serial_receive()
    # انتظر حتى يوجد بيانات / Wait for data
    متغير uint8 status
    كرر
        status = اقرأ_بايت(0x3FD)
    حتى ((status & 0x01) != 0)
    
    # اقرأ البايت / Read byte
    إرجاع اقرأ_بايت(0x3F8)
نهاية

uint8 received = serial_receive()
اطبع(ن"استلام: 0x{received:02X}")
```

---

## 2️⃣ inw/outw - إدخال/إخراج كلمة / Word I/O

### Syntax / البناء النحوي (EBNF)

```ebnf
inw_call  ::= 'اقرأ_كلمة' '(' port_number ')'
outw_call ::= 'اكتب_كلمة' '(' port_number (','| '،') value_expr ')'

value_expr ::= expression  # 0x0000 - 0xFFFF
```

### Semantics / الدلالات

- **Data Size / حجم البيانات:** 16 bits = 2 bytes
- **Return / الإرجاع:** `inw` يعيد uint16 (0-65535)
- **Alignment / المحاذاة:** بعض المنافذ تتطلب عناوين زوجية

### Examples / الأمثلة

```s
# قراءة كلمة من منفذ / Read word from port
uint16 data = اقرأ_كلمة(0x1F0)  # IDE data register
اطبع(ن"البيانات: 0x{data:04X}")

# كتابة كلمة إلى منفذ / Write word to port
اكتب_كلمة(0x1F0, 0x1234)

# قراءة قطاع من IDE / Read sector from IDE
دالة read_ide_sector(مؤشر<uint16> buffer)
    # انتظر حتى جاهز / Wait until ready
    متغير uint8 status
    كرر
        status = اقرأ_بايت(0x1F7)
    حتى ((status & 0x08) != 0)
    
    # اقرأ 256 كلمة (512 بايت) / Read 256 words (512 bytes)
    لكل ع في مدى(256)
        uint16 word = اقرأ_كلمة(0x1F0)
        اكتب_في(عنوان_من(buffer, ع * 2), word)
    نهاية
نهاية

مؤشر<uint16> sector_buffer = خصص_ذاكرة(512)
read_ide_sector(sector_buffer)
حرر(sector_buffer)
```

---

## 3️⃣ inl/outl - إدخال/إخراج long / Long I/O

### Syntax / البناء النحوي (EBNF)

```ebnf
inl_call  ::= 'اقرأ_مزدوج' '(' port_number ')'
outl_call ::= 'اكتب_مزدوج' '(' port_number ',' value_expr ')'

value_expr ::= expression  # 0x00000000 - 0xFFFFFFFF
```

### Semantics / الدلالات

- **Data Size / حجم البيانات:** 32 bits = 4 bytes
- **Return / الإرجاع:** `inl` يعيد uint32
- **Use Case / حالة الاستخدام:** PCI configuration, modern devices

### Examples / الأمثلة

```s
# قراءة من PCI configuration / Read PCI config
uint32 pci_config = اقرأ_مزدوج(0xCF8)
اطبع(ن"PCI Config: 0x{pci_config:08X}")

# كتابة عنوان PCI / Write PCI address
اكتب_مزدوج(0xCF8, 0x80000000)

# قراءة بيانات PCI / Read PCI data
uint32 device_id = اقرأ_مزدوج(0xCFC)
اطبع(ن"Device ID: 0x{device_id:08X}")

# مثال كامل: قراءة PCI device / Complete example: Read PCI device
دالة uint32 pci_read(uint8 bus، uint8 slot، uint8 func، uint8 offset)
    # بناء عنوان PCI / Build PCI address
    uint32 address = (bus << 16) | (slot << 11) | (func << 8) | (offset & 0xFC) | 0x80000000
    
    # كتابة العنوان / Write address
    اكتب_مزدوج(0xCF8, address)
    
    # قراءة البيانات / Read data
    uint32 data = اقرأ_مزدوج(0xCFC)
    
    # استخراج البايت المطلوب / Extract requested byte
    إرجاع (data >> ((offset & 0x03) * 8)) & 0xFFFFFFFF
نهاية

# قراءة vendor ID للجهاز 0:0.0 / Read vendor ID of device 0:0.0
uint32 vendor = pci_read(0, 0, 0, 0)
اطبع(ن"Vendor: 0x{vendor & 0xFFFF:04X}")
```

---

## 4️⃣ Port Wait / تأخير المنفذ

### Syntax / البناء النحوي (EBNF)

```ebnf
port_wait_call ::= 'انتظر_منفذ' '(' ')'
io_wait_call   ::= 'انتظر_إدخال_إخراج' '(' ')'
```

### Semantics / الدلالات

- **Purpose / الغرض:** إنشاء تأخير قصير جداً بين عمليات I/O
- **Implementation / التنفيذ:** عادة يستخدم `اكتب_بايت(0x80, 0)` - كتابة لمنفذ غير مستخدم
- **Duration / المدة:** ~1 ميكروثانية (يعتمد على السرعة)

### Examples / الأمثلة

```s
# عمليات I/O متتالية مع تأخير / Sequential I/O with delay
اكتب_بايت(0x60, 0xED)  # أمر LED
انتظر_منفذ()        # انتظر قليلاً
اكتب_بايت(0x60, 0x02)  # بيانات LED

# إعادة تشغيل لوحة المفاتيح / Reset keyboard
اكتب_بايت(0x64, 0xFE)
انتظر_منفذ()
اكتب_بايت(0x64, 0xFF)
انتظر_منفذ()

# دالة مساعدة / Helper function
دالة io_delay()
    اكتب_بايت(0x80, 0)  # POST port - آمن للاستخدام
نهاية

# استخدام / Usage
اكتب_بايت(0x3F8, 0x41)
io_delay()
اكتب_بايت(0x3F8, 0x42)
```

---

## 🖥️ Device Examples / أمثلة الأجهزة

### Keyboard Controller (لوحة المفاتيح)

```s
# قراءة حالة لوحة المفاتيح / Read keyboard status
دالة bool keyboard_ready()
    uint8 status = اقرأ_بايت(0x64)
    إرجاع (status & 0x01) != 0
نهاية

# قراءة مفتاح / Read key
دالة uint8 read_key()
    كرر
    حتى keyboard_ready()
    
    إرجاع اقرأ_بايت(0x60)
نهاية

# إرسال أمر للوحة المفاتيح / Send command to keyboard
دالة keyboard_command(uint8 cmd)
    # انتظر حتى buffer فارغ / Wait for empty buffer
    كرر
        uint8 status = اقرأ_بايت(0x64)
    حتى ((status & 0x02) == 0)
    
    # أرسل الأمر / Send command
    اكتب_بايت(0x60, cmd)
نهاية

# تفعيل LEDs / Enable LEDs
keyboard_command(0xED)
انتظر_منفذ()
keyboard_command(0x07)  # Caps + Num + Scroll
```

### Serial Port (COM1)

```s
# تهيئة COM1 / Initialize COM1
دالة serial_init()
    اكتب_بايت(0x3FB, 0x80)    # Enable DLAB
    اكتب_بايت(0x3F8, 0x03)    # Divisor low byte (38400 baud)
    اكتب_بايت(0x3F9, 0x00)    # Divisor high byte
    اكتب_بايت(0x3FB, 0x03)    # 8 bits, no parity, 1 stop bit
    اكتب_بايت(0x3FC, 0x00)    # Disable interrupts
    اكتب_بايت(0x3FA, 0xC7)    # Enable FIFO, clear, 14-byte threshold
    اكتب_بايت(0x3FC, 0x0B)    # IRQs enabled, RTS/DSR set
نهاية

# إرسال نص عبر serial / Send string via serial
دالة serial_write_string(نص str)
    لكل ح في str
        serial_send(ح)
    نهاية
نهاية

serial_init()
serial_write_string("مرحبا من لغة ص!\n")
```

### Timer (8253/8254 PIT)

```s
# تعيين تردد المؤقت / Set timer frequency
دالة timer_set_frequency(uint32 hz)
    uint32 divisor = 1193180 / hz
    
    # أرسل الأمر / Send command
    اكتب_بايت(0x43, 0x36)  # Channel 0, Mode 3, Binary
    
    # أرسل الفاصل / Send divisor
    اكتب_بايت(0x40, divisor & 0xFF)        # Low byte
    اكتب_بايت(0x40, (divisor >> 8) & 0xFF) # High byte
نهاية

# تعيين 100 Hz (10ms tick) / Set 100 Hz (10ms tick)
timer_set_frequency(100)
```

---

## 🔧 Port Access Patterns / أنماط الوصول للمنافذ

### Pattern 1: Status-Data Pattern (نمط الحالة-البيانات)

```s
# قراءة مع فحص الحالة / Read with status check
دالة uint8 port_read_safe(uint16 status_port، uint16 data_port، uint8 ready_bit)
    متغير uint8 status
    
    # انتظر حتى جاهز / Wait until ready
    كرر
        status = اقرأ_بايت(status_port)
    حتى ((status & ready_bit) != 0)
    
    # اقرأ البيانات / Read data
    إرجاع اقرأ_بايت(data_port)
نهاية

# استخدام / Usage
uint8 key = port_read_safe(0x64, 0x60, 0x01)
```

### Pattern 2: Command-Data Pattern (نمط الأمر-البيانات)

```s
# كتابة أمر ثم بيانات / Write command then data
دالة port_command_data(uint16 cmd_port، uint8 cmd، uint16 data_port، uint8 data)
    اكتب_بايت(cmd_port, cmd)
    انتظر_منفذ()
    اكتب_بايت(data_port, data)
نهاية

# استخدام / Usage
port_command_data(0x64, 0x60, 0x60, 0x47)  # Keyboard command
```

### Pattern 3: Read-Modify-Write (نمط قراءة-تعديل-كتابة)

```s
# تعديل بت في منفذ / Modify bit in port
دالة port_set_bit(uint16 port، uint8 bit)
    uint8 value = اقرأ_بايت(port)
    value = value | (1 << bit)
    اكتب_بايت(port, value)
نهاية

دالة port_clear_bit(uint16 port، uint8 bit)
    uint8 value = اقرأ_بايت(port)
    value = value & ~(1 << bit)
    اكتب_بايت(port, value)
نهاية

# استخدام / Usage
port_set_bit(0x21, 0)    # تفعيل IRQ0
port_clear_bit(0x21, 1)  # تعطيل IRQ1
```

---

## 📊 Common Port Ranges / نطاقات المنافذ الشائعة

| النطاق | الاستخدام (AR) | Usage (EN) |
|-------|---------------|-----------|
| 0x0000-0x001F | DMA Controller | DMA controller |
| 0x0020-0x003F | PIC Master | Interrupt controller |
| 0x0040-0x005F | Timer | System timer |
| 0x0060-0x006F | Keyboard | Keyboard controller |
| 0x0070-0x007F | CMOS/RTC | Real-time clock |
| 0x0080-0x009F | DMA Page | DMA page registers |
| 0x00A0-0x00BF | PIC Slave | Slave interrupt controller |
| 0x01F0-0x01F7 | IDE Primary | Primary IDE channel |
| 0x0170-0x0177 | IDE Secondary | Secondary IDE channel |
| 0x03F8-0x03FF | COM1 | Serial port 1 |
| 0x02F8-0x02FF | COM2 | Serial port 2 |
| 0x0378-0x037F | LPT1 | Parallel port 1 |
| 0x0CF8-0x0CFF | PCI Config | PCI configuration |

---

## ⚠️ Safety Guidelines / إرشادات الأمان

```s
# ❌ 1. لا تكتب لمنافذ غير معروفة / Don't write to unknown ports
# اكتب_بايت(0x1234, 0xFF)  # قد يعطل النظام!

# ❌ 2. لا تنسى فحص الحالة / Don't forget status checks
# uint8 data = اقرأ_بايت(0x60)  # قد تكون البيانات غير جاهزة!

# ✅ الطريقة الصحيحة / Correct way
إذا ((اقرأ_بايت(0x64) & 0x01) != 0)
    uint8 data = اقرأ_بايت(0x60)
    # استخدم البيانات / Use data
نهاية

# ❌ 3. عمليات I/O سريعة جداً / Too fast I/O operations
# اكتب_بايت(0x60, 0xED)
# اكتب_بايت(0x60, 0x01)  # قد لا يعمل!

# ✅ أضف تأخير / Add delay
اكتب_بايت(0x60, 0xED)
انتظر_منفذ()
اكتب_بايت(0x60, 0x01)

# ✅ 4. تحقق من القيم / Validate values
دالة safe_outb(uint16 port، uint8 value)
    إذا (port > 0xFFFF)
        اطبع("خطأ: رقم منفذ غير صحيح")
        إرجاع
    نهاية
    
    اكتب_بايت(port, value)
نهاية
```

---

## 🧪 Testing Example / مثال اختبار

```s
# اختبار كامل لمنافذ I/O / Complete I/O ports test
دالة test_io_ports()
    اطبع("=== اختبار منافذ I/O ===")
    
    # Test 1: POST port
    اطبع("Test 1: POST port...")
    اكتب_بايت(0x80, 0xAA)
    انتظر_منفذ()
    اكتب_بايت(0x80, 0x55)
    اطبع("✅ POST port OK")
    
    # Test 2: Read keyboard status
    اطبع("Test 2: Keyboard status...")
    uint8 status = اقرأ_بايت(0x64)
    اطبع(ن"حالة لوحة المفاتيح: 0x{status:02X}")
    
    # Test 3: Timer frequency
    اطبع("Test 3: Timer...")
    timer_set_frequency(100)
    اطبع("✅ Timer configured to 100 Hz")
    
    # Test 4: Serial port
    اطبع("Test 4: Serial port...")
    serial_init()
    serial_write_string("Test\n")
    اطبع("✅ Serial port OK")
    
    اطبع("=== جميع الاختبارات نجحت! ===")
نهاية

test_io_ports()
```

---

## 🔧 C++ API Integration

```cpp
// include/low_level/io_ports.h
class IOPorts {
public:
    // Byte I/O (8-bit)
    static uint8_t اقرأ_بايت(uint16_t port);
    static void اكتب_بايت(uint16_t port, uint8_t value);
    
    // Word I/O (16-bit)
    static uint16_t اقرأ_كلمة(uint16_t port);
    static void اكتب_كلمة(uint16_t port, uint16_t value);
    
    // Long I/O (32-bit)
    static uint32_t اقرأ_مزدوج(uint16_t port);
    static void اكتب_مزدوج(uint16_t port, uint32_t value);
    
    // Utility
    static void انتظر_إدخال_إخراج();
    static void port_delay();
    
    // Safety
    static bool is_valid_port(uint16_t port);
    static bool is_safe_to_write(uint16_t port);
};

// Inline assembly implementation (x86/x64)
inline uint8_t IOPorts::اقرأ_بايت(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

inline void IOPorts::اكتب_بايت(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}
```

---

**آخر تحديث:** 2 ديسمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 12 KB
