# 🔢 قواعد الأنواع ذات الحجم الثابت / Fixed-Size Types Rules

**التاريخ:** 2 ديسمبر 2025  
**الإصدار:** 1.0 - Stage 1  
**الحالة:** مرجع للأنواع الصحيحة ذات الحجم المحدد

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **الأنواع الصحيحة ذات الحجم الثابت** للغة "ص"، بما في ذلك الأنواع الموقعة (int8-64) وغير الموقعة (uint8-64) مع أحجامها ونطاقاتها الدقيقة، للاستخدام في البرمجة منخفضة المستوى وبروتوكولات الاتصال.

### (EN) Description
This file documents the **fixed-size integer types** لكل Sad language, including signed (int8-64) and unsigned (uint8-64) types with precise sizes and ranges, لكل use في low-level programming and communication protocols.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/01_types.md` - Type system base
- ✅ `include/data/types/fixed_size_types.h` - Type definitions
- ✅ `docs/system_build_plan/01_stage1_low_level.md` - Fixed-size types spec
- ✅ `src/interpreter/types/fixed_size_handler.cpp` - Implementation

---

## 📋 قائمة الأنواع / Types List

### Signed Types (أنواع موقعة)

| النوع | Type | الحجم | النطاق | الاستخدام |
|------|------|------|--------|----------|
| `int8` | Signed 8-bit | 1 بايت | -128 إلى 127 | بايت موقع، offset صغير |
| `int16` | Signed 16-bit | 2 بايت | -32,768 إلى 32,767 | كلمة موقعة، audio samples |
| `int32` | Signed 32-bit | 4 بايت | -2,147,483,648 إلى 2,147,483,647 | عدد صحيح قياسي |
| `int64` | Signed 64-bit | 8 بايت | -9,223,372,036,854,775,808 إلى 9,223,372,036,854,775,807 | أرقام كبيرة، timestamps |

### Unsigned Types (أنواع غير موقعة)

| النوع | Type | الحجم | النطاق | الاستخدام |
|------|------|------|--------|----------|
| `uint8` | Unsigned 8-bit | 1 بايت | 0 إلى 255 | بايت خام، بكسل، رمز حرف |
| `uint16` | Unsigned 16-bit | 2 بايت | 0 إلى 65,535 | منفذ، Unicode، sensor data |
| `uint32` | Unsigned 32-bit | 4 بايت | 0 إلى 4,294,967,295 | عنوان IPv4، رمز لون، CRC |
| `uint64` | Unsigned 64-bit | 8 بايت | 0 إلى 18,446,744,073,709,551,615 | MAC address، UUID، hashes |

---

## 1️⃣ Declaration Syntax / صيغة الإعلان

### Syntax / البناء النحوي (EBNF)

```ebnf
fixed_size_decl ::= fixed_size_type IDENTIFIER ['=' expression]

fixed_size_type ::= 'int8' | 'int16' | 'int32' | 'int64'
                  | 'uint8' | 'uint16' | 'uint32' | 'uint64'
```

**الشرح بالعربية:**
- `fixed_size_decl`: إعلان متغير بنوع حجم ثابت
- `fixed_size_type`: أحد الأنواع الثمانية ذات الحجم المحدد
- الحجم مضمون على جميع المنصات (بعكس `int` و `long`)

### Semantics / الدلالات

- **Size Guarantee / ضمان الحجم:** الحجم ثابت على جميع الأنظمة
- **Alignment / المحاذاة:** عادة محاذاة طبيعية (1, 2, 4, 8 بايت)
- **Overflow / الفيض:** عمليات signed overflow غير محددة، unsigned overflow محددة (modulo 2^n)
- **Portability / القابلية للنقل:** مثالية للبروتوكولات وتنسيقات الملفات

### Examples / الأمثلة

```s
# أنواع موقعة / Signed types
int8 temperature = -40    # درجة الحرارة / Temperature
int16 altitude = 8848     # الارتفاع بالأمتار / Altitude in meters
int32 balance = -1500000  # رصيد بنكي / Bank balance
int64 timestamp = 1701475200000  # Unix timestamp (ms)

# أنواع غير موقعة / Unsigned types
uint8 red = 255           # قناة لون / Color channel
uint16 port = 8080        # منفذ شبكة / Network port
uint32 ip_addr = 0xC0A80001  # عنوان IP / IP address (192.168.0.1)
uint64 mac = 0x001122334455  # عنوان MAC / MAC address

# إعلان بدون تهيئة / Declaration without initialization
int8 byte_value
uint16 sensor_reading

# تهيئة بقيم hex / Hex initialization
uint8 flags = 0xFF
uint32 color = 0xFF0000  # أحمر / Red

# تهيئة بقيم binary / Binary initialization
uint8 mask = 0b11110000
```

---

## 2️⃣ Type Conversion / تحويل الأنواع

### Explicit Casting / التحويل الصريح

```s
# تحويل من نوع أكبر لأصغر / Downcast (larger to smaller)
int32 big = 300
int8 small = int8(big)  # يقطع إلى -44 (300 % 256 = 44, signed = -44)

# تحويل من نوع أصغر لأكبر / Upcast (smaller to larger)
int8 tiny = -50
int32 larger = int32(tiny)  # -50 (مع امتداد الإشارة / sign extension)

# تحويل signed إلى unsigned / Signed to unsigned
int8 signed_val = -1
uint8 unsigned_val = uint8(signed_val)  # 255 (0xFF)

# تحويل unsigned إلى signed / Unsigned to signed
uint8 u_val = 200
int8 s_val = int8(u_val)  # -56 (wrap around)

# تحويل آمن مع فحص / Safe conversion with check
int32 value = 1000
إذا (value >= -128 && value <= 127)
    int8 safe = int8(value)
وإلا
    اطبع("خطأ: القيمة خارج النطاق!")
نهاية
```

### Implicit Conversion Rules / قواعد التحويل الضمني

```s
# ✅ Upcast آمن / Safe upcast
int8 a = 10
int16 b = a    # ✅ ضمني، آمن / Implicit, safe

# ❌ Downcast يتطلب صريح / Downcast requires explicit
int32 c = 1000
# int8 d = c   # ❌ خطأ: يتطلب تحويل صريح
int8 d = int8(c)  # ✅ صريح

# ✅ unsigned إلى signed أكبر / Unsigned to larger signed
uint8 u = 255
int16 s = u    # ✅ 255 (آمن، لا فقدان)

# ⚠️ signed إلى unsigned / Signed to unsigned
int8 neg = -50
# uint8 pos = neg  # ❌ يتطلب صريح
uint8 pos = uint8(neg)  # ⚠️ 206 (wrap around)
```

---

## 3️⃣ Arithmetic Operations / العمليات الحسابية

### Basic Operations / العمليات الأساسية

```s
# جمع / Addition
uint8 a = 200
uint8 b = 100
uint8 c = a + b  # 44 (300 % 256, overflow!)

int8 x = 100
int8 y = 50
int8 z = x + y   # -106 (overflow في signed غير محدد!)

# طرح / Subtraction
uint8 d = 50
uint8 e = 100
uint8 f = d - e  # 206 (wrap around: 50 - 100 = -50 → 206)

# ضرب / Multiplication
uint16 width = 1920
uint16 height = 1080
uint32 pixels = uint32(width) * uint32(height)  # 2,073,600

# قسمة / Division
int16 dividend = 1000
int16 divisor = 3
int16 quotient = dividend / divisor  # 333
int16 remainder = dividend % divisor # 1
```

### Overflow Handling / معالجة الفيض

```s
# كشف overflow في unsigned / Detect overflow in unsigned
دالة bool add_would_overflow_uint8(uint8 a، uint8 b)
    إرجاع (a > (255 - b))
نهاية

uint8 x = 200
uint8 y = 100
إذا (add_would_overflow_uint8(x, y))
    اطبع("تحذير: سيحدث overflow!")
    # استخدم نوع أكبر / Use larger type
    uint16 result = uint16(x) + uint16(y)
وإلا
    uint8 result = x + y
نهاية

# جمع آمن مع saturate / Safe saturating add
دالة uint8 saturating_add_uint8(uint8 a، uint8 b)
    uint16 sum = uint16(a) + uint16(b)
    إذا (sum > 255)
        إرجاع 255
    وإلا
        إرجاع uint8(sum)
    نهاية
نهاية

uint8 result = saturating_add_uint8(200, 100)  # 255 (saturated)

# كشف overflow في signed / Detect overflow in signed
دالة bool add_would_overflow_int8(int8 a، int8 b)
    إذا (b > 0 && a > (127 - b))
        إرجاع true  # overflow موجب
    نهاية
    إذا (b < 0 && a < (-128 - b))
        إرجاع true  # overflow سالب
    نهاية
    إرجاع false
نهاية
```

---

## 4️⃣ Bitwise Operations / العمليات البتية

```s
# العمليات البتية تعمل بشكل مثالي على الأنواع الثابتة
# Bitwise operations work perfectly with fixed-size types

# استخراج بايت من uint32 / Extract byte from uint32
uint32 value = 0x12345678
uint8 byte0 = uint8(value & 0xFF)         # 0x78
uint8 byte1 = uint8((value >> 8) & 0xFF)  # 0x56
uint8 byte2 = uint8((value >> 16) & 0xFF) # 0x34
uint8 byte3 = uint8((value >> 24) & 0xFF) # 0x12

# بناء uint32 من بايتات / Build uint32 from bytes
uint8 b0 = 0x78
uint8 b1 = 0x56
uint8 b2 = 0x34
uint8 b3 = 0x12
uint32 combined = uint32(b0) | (uint32(b1) << 8) | (uint32(b2) << 16) | (uint32(b3) << 24)
# combined = 0x12345678

# قناع بت / Bit masking
uint8 flags = 0b11010110
bool bit_0 = (flags & 0b00000001) != 0  # false
bool bit_1 = (flags & 0b00000010) != 0  # true
bool bit_7 = (flags & 0b10000000) != 0  # true

# تعيين/مسح بت / Set/clear bit
uint8 value = 0b00000000
value = value | (1 << 3)   # تعيين بت 3 / Set bit 3: 0b00001000
value = value & ~(1 << 1)  # مسح بت 1 / Clear bit 1

# تبديل بت / Toggle bit
value = value ^ (1 << 5)   # تبديل بت 5 / Toggle bit 5
```

---

## 5️⃣ Common Use Cases / حالات استخدام شائعة

### Network Protocols (بروتوكولات الشبكة)

```s
# IPv4 Packet Header (مبسط) / IPv4 Packet Header (simplified)
صنف IPv4Header {
    uint8 version_ihl      # Version (4 bits) + IHL (4 bits)
    uint8 dscp_ecn         # DSCP (6 bits) + ECN (2 bits)
    uint16 total_length    # طول الحزمة / Packet length
    uint16 identification  # معرف فريد / Unique ID
    uint16 flags_fragment  # Flags (3 bits) + Fragment offset (13 bits)
    uint8 ttl              # Time to live
    uint8 protocol         # البروتوكول / Protocol (TCP=6, UDP=17)
    uint16 checksum        # Checksum
    uint32 source_ip       # عنوان المصدر / Source IP
    uint32 dest_ip         # عنوان الوجهة / Destination IP
نهاية

# إنشاء رأس / Create header
IPv4Header header
header.version_ihl = 0x45      # IPv4, IHL=5
header.total_length = 60       # 60 bytes
header.ttl = 64                # 64 hops
header.protocol = 6            # TCP
header.source_ip = 0xC0A80001  # 192.168.0.1
header.dest_ip = 0xC0A800FE    # 192.168.0.254

# قراءة رأس من buffer / Read header from buffer
مؤشر<uint8> packet_buffer = receive_packet()
uint8 version = (قيمة_من(packet_buffer) >> 4) & 0x0F
uint8 ihl = قيمة_من(packet_buffer) & 0x0F
```

### Color Representation (تمثيل الألوان)

```s
# RGBA Color (32-bit) / لون RGBA
صنف Color {
    uint8 red
    uint8 green
    uint8 blue
    uint8 alpha
نهاية

# إنشاء لون / Create color
Color red_color
red_color.red = 255
red_color.green = 0
red_color.blue = 0
red_color.alpha = 255

# تحويل إلى uint32 / Convert to uint32
دالة uint32 color_to_uint32(Color c)
    إرجاع uint32(c.red) | (uint32(c.green) << 8) | 
           (uint32(c.blue) << 16) | (uint32(c.alpha) << 24)
نهاية

# تحويل من uint32 / Convert from uint32
دالة Color uint32_to_color(uint32 value)
    Color c
    c.red = uint8(value & 0xFF)
    c.green = uint8((value >> 8) & 0xFF)
    c.blue = uint8((value >> 16) & 0xFF)
    c.alpha = uint8((value >> 24) & 0xFF)
    إرجاع c
نهاية

uint32 packed_color = 0xFF0000FF  # أحمر بشفافية كاملة / Red, full alpha
Color unpacked = uint32_to_color(packed_color)
```

### File Format Parsing (تحليل تنسيقات الملفات)

```s
# BMP File Header / رأس ملف BMP
صنف BMPHeader {
    uint16 signature       # 'BM' = 0x4D42
    uint32 file_size       # حجم الملف بالبايت / File size in bytes
    uint16 reserved1
    uint16 reserved2
    uint32 data_offset     # إزاحة بيانات الصورة / Image data offset
نهاية

# قراءة رأس BMP / Read BMP header
دالة BMPHeader read_bmp_header(مؤشر<uint8> file_data)
    BMPHeader header
    
    # قراءة signature (little-endian) / Read signature
    header.signature = uint16(قيمة_من(file_data)) | 
                      (uint16(قيمة_من(عنوان_من(file_data, 1))) << 8)
    
    # قراءة file_size / Read file size
    header.file_size = uint32(قيمة_من(عنوان_من(file_data, 2))) |
                      (uint32(قيمة_من(عنوان_من(file_data, 3))) << 8) |
                      (uint32(قيمة_من(عنوان_من(file_data, 4))) << 16) |
                      (uint32(قيمة_من(عنوان_من(file_data, 5))) << 24)
    
    # ... باقي الحقول / rest of fields
    
    إرجاع header
نهاية

# التحقق من signature / Verify signature
إذا (header.signature == 0x4D42)
    اطبع("ملف BMP صحيح!")
وإلا
    اطبع("خطأ: ليس ملف BMP!")
نهاية
```

### Serial Communication (الاتصال التسلسلي)

```s
# إرسال بايت عبر serial / Send byte via serial
دالة serial_write_uint8(uint8 byte)
    # انتظر حتى جاهز / Wait until ready
    كرر
        uint8 status = اقرأ_بايت(0x3FD)
    حتى ((status & 0x20) != 0)
    
    اكتب_بايت(0x3F8, byte)
نهاية

# إرسال uint16 (little-endian) / Send uint16
دالة serial_write_uint16(uint16 value)
    serial_write_uint8(uint8(value & 0xFF))        # low byte
    serial_write_uint8(uint8((value >> 8) & 0xFF)) # high byte
نهاية

# إرسال uint32 (little-endian) / Send uint32
دالة serial_write_uint32(uint32 value)
    serial_write_uint8(uint8(value & 0xFF))
    serial_write_uint8(uint8((value >> 8) & 0xFF))
    serial_write_uint8(uint8((value >> 16) & 0xFF))
    serial_write_uint8(uint8((value >> 24) & 0xFF))
نهاية

# قراءة uint16 / Read uint16
دالة uint16 serial_read_uint16()
    uint8 low = serial_read_uint8()
    uint8 high = serial_read_uint8()
    إرجاع uint16(low) | (uint16(high) << 8)
نهاية
```

---

## 6️⃣ Type Limits / حدود الأنواع

### Constants / الثوابت

```s
# حدود الأنواع الموقعة / Signed type limits
const int8 INT8_MIN = -128
const int8 INT8_MAX = 127
const int16 INT16_MIN = -32768
const int16 INT16_MAX = 32767
const int32 INT32_MIN = -2147483648
const int32 INT32_MAX = 2147483647
const int64 INT64_MIN = -9223372036854775808
const int64 INT64_MAX = 9223372036854775807

# حدود الأنواع غير الموقعة / Unsigned type limits
const uint8 UINT8_MAX = 255
const uint16 UINT16_MAX = 65535
const uint32 UINT32_MAX = 4294967295
const uint64 UINT64_MAX = 18446744073709551615

# استخدام / Usage
دالة validate_int8(int32 value)
    إذا (value < INT8_MIN || value > INT8_MAX)
        اطبع("خطأ: القيمة خارج نطاق int8!")
        إرجاع false
    نهاية
    إرجاع true
نهاية
```

---

## 7️⃣ Endianness Handling / معالجة ترتيب البايتات

```s
# تحويل من big-endian إلى little-endian / Big to little endian
دالة uint16 swap_endian_16(uint16 value)
    إرجاع ((value & 0xFF) << 8) | ((value >> 8) & 0xFF)
نهاية

دالة uint32 swap_endian_32(uint32 value)
    إرجاع ((value & 0xFF) << 24) |
           (((value >> 8) & 0xFF) << 16) |
           (((value >> 16) & 0xFF) << 8) |
           ((value >> 24) & 0xFF)
نهاية

# قراءة uint16 big-endian / Read uint16 big-endian
دالة uint16 read_be16(مؤشر<uint8> ptr)
    uint16 high = uint16(قيمة_من(ptr))
    uint16 low = uint16(قيمة_من(عنوان_من(ptr, 1)))
    إرجاع (high << 8) | low
نهاية

# قراءة uint32 little-endian / Read uint32 little-endian
دالة uint32 read_le32(مؤشر<uint8> ptr)
    uint32 b0 = uint32(قيمة_من(عنوان_من(ptr, 0)))
    uint32 b1 = uint32(قيمة_من(عنوان_من(ptr, 1)))
    uint32 b2 = uint32(قيمة_من(عنوان_من(ptr, 2)))
    uint32 b3 = uint32(قيمة_من(عنوان_من(ptr, 3)))
    إرجاع b0 | (b1 << 8) | (b2 << 16) | (b3 << 24)
نهاية

# مثال: قراءة TCP port (big-endian) / Example: Read TCP port
مؤشر<uint8> tcp_header = get_tcp_packet()
uint16 source_port = read_be16(tcp_header)
uint16 dest_port = read_be16(عنوان_من(tcp_header, 2))
```

---

## 🧪 Complete Example / مثال كامل

```s
# نظام معالجة حزم الشبكة / Network packet processing system

# تعريف البروتوكولات / Protocol definitions
const uint8 PROTO_ICMP = 1
const uint8 PROTO_TCP = 6
const uint8 PROTO_UDP = 17

# معالجة حزمة IPv4 / Process IPv4 packet
دالة process_ipv4_packet(مؤشر<uint8> packet، uint16 length)
    # قراءة الرأس / Read header
    uint8 version_ihl = قيمة_من(packet)
    uint8 version = (version_ihl >> 4) & 0x0F
    uint8 ihl = version_ihl & 0x0F
    
    إذا (version != 4)
        اطبع("خطأ: ليس IPv4!")
        إرجاع
    نهاية
    
    # قراءة الطول الكلي / Read total length
    uint16 total_length = read_be16(عنوان_من(packet, 2))
    
    # قراءة البروتوكول / Read protocol
    uint8 protocol = قيمة_من(عنوان_من(packet, 9))
    
    # قراءة عناوين IP / Read IP addresses
    uint32 src_ip = read_be32(عنوان_من(packet, 12))
    uint32 dst_ip = read_be32(عنوان_من(packet, 16))
    
    # طباعة المعلومات / Print info
    اطبع(ن"IPv4 Packet:")
    اطبع(ن"  Length: {total_length} bytes")
    اطبع(ن"  Protocol: {protocol}")
    اطبع(ن"  Source: {format_ip(src_ip)}")
    اطبع(ن"  Dest: {format_ip(dst_ip)}")
    
    # معالجة حسب البروتوكول / Process by protocol
    uint16 header_len = uint16(ihl) * 4
    مؤشر<uint8> payload = عنوان_من(packet, header_len)
    
    إذا (protocol == PROTO_TCP)
        process_tcp(payload)
    وإلا إذا (protocol == PROTO_UDP)
        process_udp(payload)
    نهاية
نهاية

# تنسيق عنوان IP / Format IP address
دالة نص format_ip(uint32 ip)
    uint8 b1 = uint8((ip >> 24) & 0xFF)
    uint8 b2 = uint8((ip >> 16) & 0xFF)
    uint8 b3 = uint8((ip >> 8) & 0xFF)
    uint8 b4 = uint8(ip & 0xFF)
    إرجاع ن"{b1}.{b2}.{b3}.{b4}"
نهاية

# اختبار / Test
مؤشر<uint8> test_packet = خصص_ذاكرة(20)
اكتب_في(عنوان_من(test_packet, 0), 0x45)  # Version=4, IHL=5
اكتب_في(عنوان_من(test_packet, 9), 6)     # Protocol=TCP

process_ipv4_packet(test_packet, 20)
حرر(test_packet)
```

---

## 📊 Size and Alignment Summary / ملخص الحجم والمحاذاة

| النوع | الحجم (بايت) | المحاذاة | Printf Format |
|------|------------|---------|---------------|
| int8 | 1 | 1 | %d |
| int16 | 2 | 2 | %d |
| int32 | 4 | 4 | %d |
| int64 | 8 | 8 | %lld |
| uint8 | 1 | 1 | %u |
| uint16 | 2 | 2 | %u |
| uint32 | 4 | 4 | %u |
| uint64 | 8 | 8 | %llu |

---

## ⚠️ Common Pitfalls / مزالق شائعة

```s
# ❌ 1. Overflow غير متوقع / Unexpected overflow
uint8 a = 200
uint8 b = 100
uint8 c = a + b  # ❌ 44 (overflow!)

# ✅ استخدم نوع أكبر / Use larger type
uint16 c = uint16(a) + uint16(b)  # ✅ 300

# ❌ 2. تحويل signed/unsigned خاطئ / Wrong signed/unsigned conversion
int8 neg = -1
uint8 pos = uint8(neg)  # ❌ 255 (قد لا يكون متوقع / may be unexpected)

# ✅ تحقق من القيمة / Check value
إذا (neg >= 0)
    uint8 pos = uint8(neg)
وإلا
    اطبع("خطأ: لا يمكن تحويل قيمة سالبة!")
نهاية

# ❌ 3. نسيان endianness / Forgetting endianness
uint16 port = 0x1234
اكتب_بايت(0x3F8, port)  # ❌ يرسل فقط 0x34!

# ✅ أرسل بايتين / Send both bytes
اكتب_بايت(0x3F8, uint8(port >> 8))   # high byte
اكتب_بايت(0x3F8, uint8(port & 0xFF)) # low byte

# ❌ 4. تحويل من نوع أكبر بدون فحص / Downcast without check
int32 big = 1000
int8 small = int8(big)  # ❌ -24 (overflow!)

# ✅ تحقق أولاً / Check first
إذا (big >= INT8_MIN && big <= INT8_MAX)
    int8 small = int8(big)
وإلا
    اطبع("خطأ: القيمة لا تناسب int8!")
نهاية
```

---

## 🔧 C++ Integration

```cpp
# include/data/types/fixed_size_types.h
#include <cstdint>

# Type aliases
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

# Endianness utilities
inline uint16_t swap_endian_16(uint16_t value) {
    إرجاع ((value & 0xFF) << 8) | ((value >> 8) & 0xFF);
نهاية

inline uint32_t swap_endian_32(uint32_t value) {
    إرجاع ((value & 0xFF) << 24) |
           (((value >> 8) & 0xFF) << 16) |
           (((value >> 16) & 0xFF) << 8) |
           ((value >> 24) & 0xFF);
نهاية

# Type limits
constexpr int8_t INT8_MIN = -128;
constexpr int8_t INT8_MAX = 127;
constexpr uint8_t UINT8_MAX = 255;
# ... etc
```

---

**آخر تحديث:** 2 ديسمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 16 KB
