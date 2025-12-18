# ⚡ قواعد المقاطعات / Interrupts Rules

**التاريخ:** 2 ديسمبر 2025  
**الإصدار:** 1.0 - Stage 1  
**الحالة:** مرجع لنظام معالجة المقاطعات

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام معالجة المقاطعات** للغة "ص"، بما في ذلك تسجيل معالجات المقاطعات (ISR)، إدارة IDT، تفعيل/تعطيل المقاطعات، والكلمة المفتاحية `volatile` للمتغيرات المشتركة.

### (EN) Description
This file documents the **interrupt handling system** لكل Sad language, including Interrupt Service Routine (ISR) registration, IDT management, enabling/disabling interrupts, and the `volatile` keyword لكل shared variables.

---

## 📦 الاعتماديات / Dependencies

- ✅ `include/low_level/interrupts.h` - InterruptManager class
- ✅ `include/low_level/io_ports.h` - Port access integration
- ✅ `docs/system_build_plan/01_stage1_low_level.md` - Interrupts spec
- ✅ `src/low_level/interrupt_handler.cpp` - Implementation

---

## 📋 قائمة العمليات / Operations List

| # | العملية (AR) | Operation (EN) | النوع | الحالة |
|---|------------|---------------|------|-------|
| 1 | `register_handler` | Register ISR | تسجيل | ✅ Core |
| 2 | `enable_interrupts` | Enable IRQs | تفعيل | ✅ Core |
| 3 | `disable_interrupts` | Disable IRQs | تعطيل | ✅ Core |
| 4 | `ack_interrupt` | Acknowledge IRQ | إقرار | ✅ Core |
| 5 | `volatile` | Volatile Variable | متغير | ✅ Core |
| 6 | `get_irq_mask` | Get IRQ Mask | قراءة | 🔧 Utility |
| 7 | `set_irq_mask` | Set IRQ Mask | كتابة | 🔧 Utility |

---

## 1️⃣ register_handler - تسجيل معالج مقاطعة / Register Interrupt Handler

### Syntax / البناء النحوي (EBNF)

```ebnf
register_handler_call ::= 'سجل_معالج' '(' irq_number ',' handler_func ')'

irq_number   ::= integer_literal  # 0-255 (0x00-0xFF)
handler_func ::= function_reference | lambda_expr
```

**الشرح بالعربية:**
- `register_handler`: يسجل دالة لمعالجة مقاطعة محددة
- `irq_number`: رقم المقاطعة من 0 إلى 255
- `handler_func`: دالة تُستدعى عند حدوث المقاطعة

### Semantics / الدلالات

- **IRQ Numbers / أرقام المقاطعات:**
  - 0-15: Hardware IRQs (Timer, Keyboard, etc.)
  - 32-47: Mapped hardware IRQs in protected mode
  - 48-255: Software interrupts and exceptions
- **Handler Signature / توقيع المعالج:** `دالة handler()`
- **Context / السياق:** المعالج يعمل في سياق المقاطعة (محدودية الوصول)
- **Return / الإرجاع:** المعالج يجب أن يعيد التحكم بسرعة

### IRQ Table / جدول المقاطعات

| IRQ | الوصف (AR) | Description (EN) |
|-----|-----------|-----------------|
| 0 | مؤقت النظام | System Timer (PIT) |
| 1 | لوحة المفاتيح | Keyboard |
| 2 | مسلسل من PIC Slave | Cascade from slave PIC |
| 3 | COM2 | Serial port 2 |
| 4 | COM1 | Serial port 1 |
| 5 | LPT2 | Parallel port 2 |
| 6 | Floppy Disk | Floppy controller |
| 7 | LPT1 | Parallel port 1 |
| 8 | RTC | Real-time clock |
| 9-11 | متاح | Available |
| 12 | PS/2 Mouse | PS/2 mouse |
| 13 | FPU | Math coprocessor |
| 14 | IDE Primary | Primary IDE |
| 15 | IDE Secondary | Secondary IDE |

### Examples / الأمثلة

```s
# معالج بسيط للمؤقت / Simple timer handler
متطاير رقم timer_ticks = 0

دالة timer_handler()
    timer_ticks = timer_ticks + 1
    
    # إقرار المقاطعة / Acknowledge interrupt
    أقر_مقاطعة(0)
نهاية

# تسجيل معالج المؤقت / Register timer handler
سجل_معالج(0, timer_handler)

# تفعيل المقاطعات / Enable interrupts
فعل_المقاطعات()

# انتظر 100 ticks / Wait لكل 100 ticks
كرر
حتى (timer_ticks >= 100)

اطبع(ن"مر {timer_ticks} ticks")

# معالج لوحة المفاتيح / Keyboard handler
متطاير uint8 last_key = 0

دالة keyboard_handler()
    last_key = اقرأ_بايت(0x60)  # قراءة scancode
    اطبع(ن"مفتاح: 0x{last_key:02X}")
    
    أقر_مقاطعة(1)
نهاية

سجل_معالج(1, keyboard_handler)

# معالج مع lambda / Handler with lambda
سجل_معالج(8, () => {
    اطبع("RTC tick!")
    أقر_مقاطعة(8)
})

# معالج متقدم / Advanced handler
متطاير منطقي network_packet_ready = false

دالة network_handler()
    # معالجة البيانات الواردة / Process incoming data
    network_packet_ready = true
    
    # قراءة من بطاقة الشبكة / Read from network card
    uint16 status = اقرأ_كلمة(0xC100)
    
    إذا ((status & 0x01) != 0)
        # بيانات متاحة / Data available
        process_network_packet()
    نهاية
    
    أقر_مقاطعة(11)
نهاية

سجل_معالج(11, network_handler)
```

---

## 2️⃣ enable_interrupts / disable_interrupts - التحكم في المقاطعات

### Syntax / البناء النحوي (EBNF)

```ebnf
enable_interrupts_call  ::= 'فعل_المقاطعات' '(' ')'
disable_interrupts_call ::= 'عطل_المقاطعات' '(' ')'

# تفعيل/تعطيل مقاطعة محددة
enable_irq_call  ::= 'فعل_مقاطعة' '(' irq_number ')'
disable_irq_call ::= 'عطل_مقاطعة' '(' irq_number ')'
```

### Semantics / الدلالات

- **enable_interrupts:** يفعل جميع المقاطعات عالمياً (تعليمة `sti`)
- **disable_interrupts:** يعطل جميع المقاطعات عالمياً (تعليمة `cli`)
- **فعل_مقاطعة(n):** يفعل IRQ محدد (يزيل القناع في PIC)
- **عطل_مقاطعة(n):** يعطل IRQ محدد (يضع قناع في PIC)

### Examples / الأمثلة

```s
# تعطيل المقاطعات للقسم الحرج / Disable لكل critical section
عطل_المقاطعات()

# القسم الحرج - لا مقاطعات / Critical section - no interrupts
متغير رقم important_data = 0
important_data = calculate_critical()

فعل_المقاطعات()

# نمط آمن مع حفظ الحالة / Safe pattern with state save
دالة critical_operation()
    bool old_state = are_interrupts_enabled()
    عطل_المقاطعات()
    
    # عملية حرجة / Critical operation
    modify_shared_data()
    
    إذا (old_state)
        فعل_المقاطعات()
    نهاية
نهاية

# تعطيل IRQ محدد / Disable specific IRQ
عطل_مقاطعة(1)  # تعطيل لوحة المفاتيح
# ... عمل بدون مقاطعات لوحة المفاتيح ...
فعل_مقاطعة(1)   # إعادة تفعيل

# نمط متقدم / Advanced pattern
دالة atomic_increment(متطاير مؤشر<رقم> counter)
    عطل_المقاطعات()
    
    رقم value = قيمة_من(counter)
    value = value + 1
    اكتب_في(counter, value)
    
    فعل_المقاطعات()
نهاية

متطاير رقم global_counter = 0
atomic_increment(عنوان_من(global_counter))
```

---

## 3️⃣ ack_interrupt - إقرار المقاطعة / Acknowledge Interrupt

### Syntax / البناء النحوي (EBNF)

```ebnf
ack_interrupt_call ::= 'أقر_مقاطعة' '(' irq_number ')'
```

### Semantics / الدلالات

- **Purpose / الغرض:** إخبار PIC بأن المقاطعة تمت معالجتها
- **Required / مطلوب:** يجب استدعاؤه في نهاية كل معالج مقاطعة
- **EOI Command / أمر EOI:** يرسل End-Of-Interrupt للـ PIC
- **Master/Slave:** IRQ 0-7 (master)، IRQ 8-15 (slave + master)

### Examples / الأمثلة

```s
# معالج بسيط / Simple handler
دالة irq_handler()
    # معالجة المقاطعة / Handle interrupt
    do_work()
    
    # إقرار - مهم! / Acknowledge - important!
    أقر_مقاطعة(5)
نهاية

# معالج مع slave PIC / Handler with slave PIC
دالة rtc_handler()
    # قراءة RTC / Read RTC
    اكتب_بايت(0x70, 0x0C)
    uint8 status = اقرأ_بايت(0x71)
    
    # معالجة / Process
    handle_rtc(status)
    
    # إقرار (IRQ 8 = slave) / Acknowledge (IRQ 8 = slave)
    أقر_مقاطعة(8)  # يرسل EOI للـ master و slave
نهاية

# نمط خطأ شائع / Common error pattern
دالة bad_handler()
    do_work()
    # ❌ نسيت أقر_مقاطعة() - المقاطعة ستتوقف!
نهاية

# النمط الصحيح / Correct pattern
دالة good_handler()
    try {
        do_work()
    } finally {
        أقر_مقاطعة(3)  # ✅ دائماً نفذ حتى لو حدث خطأ
    نهاية
نهاية
```

### PIC Programming

```s
# إرسال EOI للـ master PIC / Send EOI to master PIC
دالة pic_send_eoi(uint8 irq)
    إذا (irq >= 8)
        # slave PIC
        اكتب_بايت(0xA0, 0x20)
    نهاية
    
    # master PIC دائماً / always master PIC
    اكتب_بايت(0x20, 0x20)
نهاية

# تهيئة PIC / Initialize PIC
دالة pic_init()
    # ICW1: بدء التهيئة / Start initialization
    اكتب_بايت(0x20, 0x11)  # Master
    اكتب_بايت(0xA0, 0x11)  # Slave
    
    # ICW2: تعيين offsets / Set vector offsets
    اكتب_بايت(0x21, 0x20)  # Master starts at 32
    اكتب_بايت(0xA1, 0x28)  # Slave starts at 40
    
    # ICW3: إعداد master/slave / Setup cascading
    اكتب_بايت(0x21, 0x04)  # Master: slave at IRQ2
    اكتب_بايت(0xA1, 0x02)  # Slave: cascade identity
    
    # ICW4: وضع 8086 / 8086 mode
    اكتب_بايت(0x21, 0x01)
    اكتب_بايت(0xA1, 0x01)
    
    # تعيين الأقنعة / Set masks
    اكتب_بايت(0x21, 0x00)  # Enable all IRQs
    اكتب_بايت(0xA1, 0x00)
نهاية

pic_init()
```

---

## 4️⃣ volatile - المتغيرات المتطايرة / Volatile Variables

### Syntax / البناء النحوي (EBNF)

```ebnf
volatile_decl ::= 'متطاير' type IDENTIFIER ['=' expression]
```

**الشرح بالعربية:**
- `volatile`: كلمة مفتاحية تخبر المترجم أن المتغير قد يتغير خارج التحكم الطبيعي
- تمنع المترجم من تحسين الوصول للمتغير
- ضرورية للمتغيرات المشتركة بين الكود الرئيسي ومعالجات المقاطعات

### Semantics / الدلالات

- **No Optimization / بدون تحسين:** المترجم يقرأ دائماً من الذاكرة
- **Memory Barrier / حاجز الذاكرة:** يضمن ترتيب العمليات
- **Use Cases / حالات الاستخدام:**
  - متغيرات يعدلها ISR
  - متغيرات hardware-mapped
  - أعلام مشتركة بين threads

### Examples / الأمثلة

```s
# مثال خطأ بدون volatile / Wrong example without volatile
رقم counter = 0  # ❌ بدون volatile

دالة timer_isr()
    counter = counter + 1
    أقر_مقاطعة(0)
نهاية

دالة wait_100_ticks()
    counter = 0
    كرر
        # المترجم قد يحسن هذا ويقرأ counter مرة واحدة فقط!
    حتى (counter >= 100)
نهاية

# المثال الصحيح مع volatile / Correct example with volatile
متطاير رقم counter = 0  # ✅ مع volatile

دالة timer_isr()
    counter = counter + 1
    أقر_مقاطعة(0)
نهاية

دالة wait_100_ticks()
    counter = 0
    كرر
        # المترجم يقرأ counter من الذاكرة في كل تكرار
    حتى (counter >= 100)
نهاية

# أمثلة متقدمة / Advanced examples

# علم جاهزية البيانات / Data ready flag
متطاير منطقي data_ready = false

دالة data_isr()
    process_data()
    data_ready = true
    أقر_مقاطعة(5)
نهاية

دالة wait_for_data()
    data_ready = false
    كرر
    حتى (data_ready)
    
    اطبع("البيانات جاهزة!")
نهاية

# عداد مشترك / Shared counter
متطاير رقم packets_received = 0

دالة network_isr()
    packets_received = packets_received + 1
    أقر_مقاطعة(11)
نهاية

# buffer دائري / Circular buffer
متطاير uint8[256] keyboard_buffer
متطاير رقم buffer_head = 0
متطاير رقم buffer_tail = 0

دالة keyboard_isr()
    uint8 scancode = اقرأ_بايت(0x60)
    
    # إضافة للـ buffer / Add to buffer
    keyboard_buffer[buffer_head] = scancode
    buffer_head = (buffer_head + 1) % 256
    
    أقر_مقاطعة(1)
نهاية

دالة uint8 read_keyboard()
    كرر
    حتى (buffer_tail != buffer_head)
    
    uint8 key = keyboard_buffer[buffer_tail]
    buffer_tail = (buffer_tail + 1) % 256
    إرجاع key
نهاية

# متغير hardware-mapped / Hardware-mapped variable
متطاير مؤشر<uint32> video_memory = 0xB8000

دالة write_char(رقم x، رقم y، uint8 c)
    رقم offset = (y * 80 + x) * 2
    اكتب_في(عنوان_من(video_memory, offset), c)
نهاية
```

### Volatile vs Non-Volatile Comparison

```s
# بدون volatile - خطأ محتمل / Without volatile - potential bug
رقم flag = 0

دالة isr()
    flag = 1
نهاية

دالة wait()
    # المترجم قد يحسن هذا إلى حلقة لا نهائية!
    # Compiler may optimize to infinite loop!
    كرر حتى (flag == 1)
    
    # في assembly قد يصبح:
    # MOV EAX, [flag]  ; قراءة مرة واحدة
    # TEST EAX, EAX
    # JZ loop          ; حلقة لا نهائية
نهاية

# مع volatile - صحيح / With volatile - correct
متطاير رقم flag = 0

دالة isr()
    flag = 1
نهاية

دالة wait()
    # المترجم يقرأ flag في كل تكرار
    # Compiler reads flag every iteration
    كرر حتى (flag == 1)
    
    # في assembly:
    # loop:
    # MOV EAX, [flag]  ; قراءة في كل مرة
    # TEST EAX, EAX
    # JZ loop
نهاية
```

---

## 🔧 Complete Interrupt System Example / مثال كامل لنظام المقاطعات

```s
# نظام مقاطعات كامل / Complete interrupt system

# تهيئة النظام / System initialization
دالة init_interrupts()
    # تهيئة PIC / Initialize PIC
    pic_init()
    
    # تسجيل المعالجات / Register handlers
    سجل_معالج(0, timer_handler)
    سجل_معالج(1, keyboard_handler)
    سجل_معالج(8, rtc_handler)
    
    # تفعيل IRQs / Enable IRQs
    فعل_مقاطعة(0)   # Timer
    فعل_مقاطعة(1)   # Keyboard
    فعل_مقاطعة(8)   # RTC
    
    # تفعيل المقاطعات عالمياً / Enable interrupts globally
    فعل_المقاطعات()
    
    اطبع("✅ نظام المقاطعات جاهز")
نهاية

# المتغيرات المشتركة / Shared variables
متطاير رقم system_ticks = 0
متطاير uint8 last_key = 0
متطاير منطقي key_pressed = false

# معالج المؤقت / Timer handler
دالة timer_handler()
    system_ticks = system_ticks + 1
    
    # كل 100 ticks / Every 100 ticks
    إذا ((system_ticks % 100) == 0)
        اطبع(ن"مر {system_ticks / 100} ثانية")
    نهاية
    
    أقر_مقاطعة(0)
نهاية

# معالج لوحة المفاتيح / Keyboard handler
دالة keyboard_handler()
    last_key = اقرأ_بايت(0x60)
    key_pressed = true
    
    أقر_مقاطعة(1)
نهاية

# معالج RTC / RTC handler
دالة rtc_handler()
    # قراءة register C لإزالة المقاطعة / Read register C to clear
    اكتب_بايت(0x70, 0x0C)
    uint8 status = اقرأ_بايت(0x71)
    
    اطبع("RTC tick")
    
    أقر_مقاطعة(8)
نهاية

# البرنامج الرئيسي / Main program
دالة main()
    init_interrupts()
    
    اطبع("انتظار المقاطعات... اضغط ESC للخروج")
    
    # حلقة رئيسية / Main loop
    كرر
        إذا (key_pressed)
            اطبع(ن"مفتاح: 0x{last_key:02X}")
            key_pressed = false
            
            # ESC = 0x01
            إذا (last_key == 0x01)
                إيقاف
            نهاية
        نهاية
        
        # انتظار مقاطعة / Wait لكل interrupt
        hlt()  # توقف CPU حتى مقاطعة / Halt until interrupt
    نهاية
    
    عطل_المقاطعات()
    اطبع(ن"الخروج بعد {system_ticks} ticks")
نهاية

main()
```

---

## 📊 Interrupt Priority / أولوية المقاطعات

| الأولوية | IRQ | الوصف (AR) | Description (EN) |
|---------|-----|-----------|-----------------|
| أعلى | 0 | مؤقت | System timer |
| ↑ | 1 | لوحة المفاتيح | Keyboard |
| | 8-15 | Slave PIC | Slave controller IRQs |
| | 3-7 | متفرقة | Miscellaneous |
| أدنى | 2 | Cascade | (Reserved) |

---

## ⚠️ Best Practices / أفضل الممارسات

```s
# ✅ 1. معالجات سريعة / Fast handlers
دالة good_isr()
    # عمل قليل فقط / Minimal work only
    set_flag()
    أقر_مقاطعة(0)
نهاية

# ❌ عمل كثير في ISR / Too much work in ISR
دالة bad_isr()
    # ❌ لا تفعل هذا! / Don't do this!
    لكل ع في مدى(1000000)
        complex_calculation()
    نهاية
    أقر_مقاطعة(0)
نهاية

# ✅ 2. استخدم volatile / Use volatile
متطاير منطقي flag = false  # ✅ صحيح
# bool flag = false         # ❌ خطأ محتمل

# ✅ 3. دائماً ack / Always acknowledge
دالة isr()
    do_work()
    أقر_مقاطعة(5)  # ✅ لا تنسى!
نهاية

# ✅ 4. احم الأقسام الحرجة / Protect critical sections
دالة modify_shared()
    عطل_المقاطعات()
    # قسم حرج / Critical section
    shared_data = new_value
    فعل_المقاطعات()
نهاية

# ✅ 5. نمط two-stage / Two-stage pattern
متطاير منطقي work_pending = false

دالة isr()
    work_pending = true  # سريع / Fast
    أقر_مقاطعة(0)
نهاية

دالة main_loop()
    كرر
        إذا (work_pending)
            do_heavy_work()  # بطيء، خارج ISR / Slow, outside ISR
            work_pending = false
        نهاية
    نهاية
نهاية
```

---

## 🔧 C++ API Integration

```cpp
# include/low_level/interrupts.h
class InterruptManager {
public:
    # Handler type
    using InterruptHandler = std::function<void()>;
    
    # Register handler
    static void registerHandler(uint8_t irq, InterruptHandler handler);
    
    # Control interrupts
    static void enable();
    static void disable();
    static bool areEnabled();
    
    # IRQ control
    static void enableIRQ(uint8_t irq);
    static void disableIRQ(uint8_t irq);
    
    # Acknowledge
    static void acknowledge(uint8_t irq);
    
    # PIC control
    static void initPIC();
    static uint16_t getIRQMask();
    static void setIRQMask(uint16_t mask);
};

# Inline assembly (x86/x64)
inline void InterruptManager::enable() {
    __asm__ volatile("sti");
نهاية

inline void InterruptManager::disable() {
    __asm__ volatile("cli");
نهاية

inline bool InterruptManager::areEnabled() {
    uint64_t flags;
    __asm__ volatile("pushf; pop %0" : "=r"(flags));
    إرجاع (flags & 0x200) != 0;
نهاية
```

---

**آخر تحديث:** 2 ديسمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 14 KB
