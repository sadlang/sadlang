# ============================================================================
# أمثلة على العمليات الثنائية في لغة ص
# Bitwise Operations Examples in Sad Language
# ============================================================================
#
# الوصف بالعربية:
# ----------------
# هذا الملف يحتوي على أمثلة عملية للعمليات الثنائية في لغة ص
# العمليات الثنائية ضرورية لبرمجة النظام والتعامل مع الأجهزة
#
# English Description:
# -------------------
# This file contains practical examples of bitwise operations in Sad Language
# Bitwise operations are essential for system programming and hardware control
#
# ============================================================================

# ============================================================================
# مثال 1: العمليات الثنائية الأساسية
# Example 1: Basic Bitwise Operations
# ============================================================================

اطبع("=== مثال 1: العمليات الثنائية الأساسية ===") ;
اطبع("=== Example 1: Basic Bitwise Operations ===") ;

رقم a = 0b1100 ;  # 12 في النظام العشري / 12 in decimal
رقم b = 0b1010 ;  # 10 في النظام العشري / 10 in decimal

# AND ثنائي - Bitwise AND
رقم and_result = a & b ;  # 0b1000 = 8
اطبع("AND: ", a, " & ", b, " = ", and_result) ;

# OR ثنائي - Bitwise OR
رقم or_result = a | b ;  # 0b1110 = 14
اطبع("OR: ", a, " | ", b, " = ", or_result) ;

# XOR ثنائي - Bitwise XOR
رقم xor_result = a ^ b ;  # 0b0110 = 6
اطبع("XOR: ", a, " ^ ", b, " = ", xor_result) ;

# NOT ثنائي - Bitwise NOT
رقم not_result = ~a ;
اطبع("NOT: ~", a, " = ", not_result) ;

# ============================================================================
# مثال 2: عمليات الإزاحة
# Example 2: Shift Operations
# ============================================================================

اطبع("\n=== مثال 2: عمليات الإزاحة ===") ;
اطبع("=== Example 2: Shift Operations ===") ;

رقم value = 8 ;  # 0b1000

# إزاحة لليسار (الضرب في 2)
# Left shift (multiply by 2)
رقم left_shift = value << 1 ;  # 16
اطبع("إزاحة لليسار / Left shift: ", value, " << 1 = ", left_shift) ;

# إزاحة لليسار بمقدار 3 (الضرب في 8)
# Left shift by 3 (multiply by 8)
left_shift = value << 3 ;  # 64
اطبع("إزاحة لليسار / Left shift: ", value, " << 3 = ", left_shift) ;

# إزاحة لليمين (القسمة على 2)
# Right shift (divide by 2)
رقم right_shift = value >> 1 ;  # 4
اطبع("إزاحة لليمين / Right shift: ", value, " >> 1 = ", right_shift) ;

# إزاحة لليمين بمقدار 2 (القسمة على 4)
# Right shift by 2 (divide by 4)
right_shift = value >> 2 ;  # 2
اطبع("إزاحة لليمين / Right shift: ", value, " >> 2 = ", right_shift) ;

# ============================================================================
# مثال 3: فحص وتعيين البتات
# Example 3: Testing and Setting Bits
# ============================================================================

اطبع("\n=== مثال 3: فحص وتعيين البتات ===") ;
اطبع("=== Example 3: Testing and Setting Bits ===") ;

رقم flags = 0b00000000 ;

# تعيين بت معين (set bit)
دالة set_bit(رقم value, رقم bit) -> رقم {
    أرجع value | (1 << bit) ;
}

# مسح بت معين (clear bit)
دالة clear_bit(رقم value, رقم bit) -> رقم {
    أرجع value & ~(1 << bit) ;
}

# فحص بت معين (test bit)
دالة test_bit(رقم value, رقم bit) -> منطقي {
    أرجع (value & (1 << bit)) != 0 ;
}

# تعيين البتات 0, 2, 4
# Set bits 0, 2, 4
flags = set_bit(flags, 0) ;
flags = set_bit(flags, 2) ;
flags = set_bit(flags, 4) ;
اطبع("بعد تعيين البتات 0, 2, 4 / After setting bits 0, 2, 4: ", flags) ;

# فحص البتات
# Test bits
اطبع("البت 0 معين؟ / Bit 0 set? ", test_bit(flags, 0)) ;
اطبع("البت 1 معين؟ / Bit 1 set? ", test_bit(flags, 1)) ;
اطبع("البت 2 معين؟ / Bit 2 set? ", test_bit(flags, 2)) ;

# مسح البت 2
# Clear bit 2
flags = clear_bit(flags, 2) ;
اطبع("بعد مسح البت 2 / After clearing bit 2: ", flags) ;
اطبع("البت 2 معين؟ / Bit 2 set? ", test_bit(flags, 2)) ;

# ============================================================================
# مثال 4: الأقنعة (Bit Masks)
# Example 4: Bit Masks
# ============================================================================

اطبع("\n=== مثال 4: الأقنعة (Bit Masks) ===") ;
اطبع("=== Example 4: Bit Masks ===") ;

# استخراج البايت الأدنى
# Extract lower byte
رقم data = 0x12345678 ;
رقم lower_byte = data & 0xFF ;
اطبع("البايت الأدنى / Lower byte: 0x", lower_byte) ;

# استخراج البايت الأعلى
# Extract upper byte (from lower word)
رقم upper_byte = (data >> 8) & 0xFF ;
اطبع("البايت الأعلى / Upper byte: 0x", upper_byte) ;

# استخراج الكلمة الأدنى (16 bit)
# Extract lower word (16 bits)
رقم lower_word = data & 0xFFFF ;
اطبع("الكلمة الأدنى / Lower word: 0x", lower_word) ;

# استخراج الكلمة الأعلى (16 bit)
# Extract upper word (16 bits)
رقم upper_word = (data >> 16) & 0xFFFF ;
اطبع("الكلمة الأعلى / Upper word: 0x", upper_word) ;

# ============================================================================
# مثال 5: تبديل البتات (Bit Swapping)
# Example 5: Bit Swapping
# ============================================================================

اطبع("\n=== مثال 5: تبديل البتات ===") ;
اطبع("=== Example 5: Bit Swapping ===") ;

# تبديل بايتين في كلمة
# Swap bytes in a word
دالة swap_bytes(رقم value) -> رقم {
    رقم low = value & 0xFF ;
    رقم high = (value >> 8) & 0xFF ;
    أرجع (low << 8) | high ;
}

رقم word = 0x1234 ;
رقم swapped = swap_bytes(word) ;
اطبع("الأصلي / Original: 0x", word) ;
اطبع("المبدل / Swapped: 0x", swapped) ;

# ============================================================================
# مثال 6: عد البتات المعينة (Population Count)
# Example 6: Population Count
# ============================================================================

اطبع("\n=== مثال 6: عد البتات المعينة ===") ;
اطبع("=== Example 6: Population Count ===") ;

# عد عدد البتات المعينة (1s)
# Count number of set bits (1s)
دالة popcount(رقم value) -> رقم {
    رقم count = 0 ;
    بينما (value != 0) {
        count = count + (value & 1) ;
        value = value >> 1 ;
    }
    أرجع count ;
}

رقم num1 = 0b10101010 ;  # 4 بتات معينة / 4 bits set
رقم num2 = 0b11111111 ;  # 8 بتات معينة / 8 bits set
رقم num3 = 0b10000001 ;  # 2 بتات معينة / 2 bits set

اطبع("عدد البتات المعينة في / Bits set in ", num1, ": ", popcount(num1)) ;
اطبع("عدد البتات المعينة في / Bits set in ", num2, ": ", popcount(num2)) ;
اطبع("عدد البتات المعينة في / Bits set in ", num3, ": ", popcount(num3)) ;

# ============================================================================
# مثال 7: فحص التماثل (Parity Check)
# Example 7: Parity Check
# ============================================================================

اطبع("\n=== مثال 7: فحص التماثل ===") ;
اطبع("=== Example 7: Parity Check ===") ;

# فحص التماثل الزوجي/الفردي
# Check even/odd parity
دالة is_even_parity(رقم value) -> منطقي {
    رقم count = popcount(value) ;
    أرجع (count % 2) == 0 ;
}

رقم test1 = 0b11100000 ;  # 3 بتات (فردي / odd)
رقم test2 = 0b11110000 ;  # 4 بتات (زوجي / even)

اطبع("التماثل الزوجي؟ / Even parity? ", test1, ": ", is_even_parity(test1)) ;
اطبع("التماثل الزوجي؟ / Even parity? ", test2, ": ", is_even_parity(test2)) ;

# ============================================================================
# مثال 8: عكس البتات (Bit Reversal)
# Example 8: Bit Reversal
# ============================================================================

اطبع("\n=== مثال 8: عكس البتات ===") ;
اطبع("=== Example 8: Bit Reversal ===") ;

# عكس ترتيب البتات في بايت
# Reverse bit order in a byte
دالة reverse_byte(رقم value) -> رقم {
    رقم result = 0 ;
    لكل (رقم i = 0 ; i < 8 ; i++) {
        result = (result << 1) | (value & 1) ;
        value = value >> 1 ;
    }
    أرجع result ;
}

رقم original = 0b10000001 ;
رقم reversed = reverse_byte(original) ;
اطبع("الأصلي / Original: ", original) ;
اطبع("المعكوس / Reversed: ", reversed) ;

# ============================================================================
# مثال 9: استخدامات عملية - أعلام الأذونات
# Example 9: Practical Use - Permission Flags
# ============================================================================

اطبع("\n=== مثال 9: أعلام الأذونات ===") ;
اطبع("=== Example 9: Permission Flags ===") ;

# تعريف الأذونات
# Define permissions
ثابت READ = 1 << 0 ;    # 0b001
ثابت WRITE = 1 << 1 ;   # 0b010
ثابت EXECUTE = 1 << 2 ; # 0b100

# منح أذونات
# Grant permissions
رقم permissions = 0 ;
permissions = permissions | READ ;
permissions = permissions | WRITE ;

اطبع("الأذونات الحالية / Current permissions: ", permissions) ;
اطبع("يمكن القراءة؟ / Can read? ", test_bit(permissions, 0)) ;
اطبع("يمكن الكتابة؟ / Can write? ", test_bit(permissions, 1)) ;
اطبع("يمكن التنفيذ؟ / Can execute? ", test_bit(permissions, 2)) ;

# إضافة إذن التنفيذ
# Add execute permission
permissions = permissions | EXECUTE ;
اطبع("بعد إضافة التنفيذ / After adding execute: ", permissions) ;
اطبع("يمكن التنفيذ؟ / Can execute? ", test_bit(permissions, 2)) ;

# إلغاء إذن الكتابة
# Revoke write permission
permissions = permissions & ~WRITE ;
اطبع("بعد إلغاء الكتابة / After revoking write: ", permissions) ;
اطبع("يمكن الكتابة؟ / Can write? ", test_bit(permissions, 1)) ;

# ============================================================================
# مثال 10: استخدامات عملية - التحكم في الأجهزة
# Example 10: Practical Use - Hardware Control
# ============================================================================

اطبع("\n=== مثال 10: التحكم في الأجهزة ===") ;
اطبع("=== Example 10: Hardware Control ===") ;

# محاكاة سجل تحكم جهاز
# Simulate device control register
صنف DeviceControl {
    رقم register ;
    
    # البتات / Bits:
    # 0: ENABLE - تمكين الجهاز / Enable device
    # 1: INTERRUPT - تمكين المقاطعات / Enable interrupts
    # 2: DMA - تمكين DMA / Enable DMA
    # 3: RESET - إعادة تعيين / Reset
    
    دالة init() {
        register = 0 ;
    }
    
    دالة enable_device() {
        register = set_bit(register, 0) ;
        اطبع("✓ تم تمكين الجهاز / Device enabled") ;
    }
    
    دالة disable_device() {
        register = clear_bit(register, 0) ;
        اطبع("✓ تم تعطيل الجهاز / Device disabled") ;
    }
    
    دالة enable_interrupts() {
        register = set_bit(register, 1) ;
        اطبع("✓ تم تمكين المقاطعات / Interrupts enabled") ;
    }
    
    دالة enable_dma() {
        register = set_bit(register, 2) ;
        اطبع("✓ تم تمكين DMA / DMA enabled") ;
    }
    
    دالة reset_device() {
        register = set_bit(register, 3) ;
        اطبع("✓ إعادة تعيين الجهاز / Device reset") ;
        # في التطبيق الحقيقي، سيتم مسح البت تلقائياً
        # In real implementation, bit would be cleared automatically
        register = clear_bit(register, 3) ;
    }
    
    دالة is_enabled() -> منطقي {
        أرجع test_bit(register, 0) ;
    }
    
    دالة print_status() {
        اطبع("حالة الجهاز / Device status:") ;
        اطبع("  ممكّن / Enabled: ", test_bit(register, 0)) ;
        اطبع("  المقاطعات / Interrupts: ", test_bit(register, 1)) ;
        اطبع("  DMA: ", test_bit(register, 2)) ;
        اطبع("  السجل / Register: 0x", register) ;
    }
}

# استخدام سجل التحكم
# Use control register
DeviceControl device = جديد DeviceControl() ;
device.init() ;

device.print_status() ;

device.enable_device() ;
device.enable_interrupts() ;
device.enable_dma() ;

اطبع("") ;
device.print_status() ;

اطبع("\n=== انتهت الأمثلة ===") ;
اطبع("=== Examples Complete ===") ;
