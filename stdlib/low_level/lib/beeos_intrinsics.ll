; =============================================================================
; ملف: beeos_intrinsics.ll
; الوصف: تنفيذ الدوال المدمجة منخفضة المستوى لنظام النحلة
; =============================================================================
; 
; هذا الملف يوفر تنفيذ LLVM IR للدوال المدمجة المطلوبة لكتابة
; نظام التشغيل بلغة ص. يستخدم inline assembly للوصول للتعليمات
; التي لا يمكن التعبير عنها بـ LLVM IR وحده.
;
; =============================================================================

target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-none"

; =============================================================================
; القسم 1: الذاكرة المباشرة
; =============================================================================

; قراءة بايت من عنوان ذاكرة مباشر
define i8 @اقرأ_بايت(i64 %addr) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    %val = load volatile i8, ptr %ptr, align 1
    ret i8 %val
}

; قراءة كلمة (16 بت)
define i16 @اقرأ_كلمة(i64 %addr) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    %val = load volatile i16, ptr %ptr, align 2
    ret i16 %val
}

; قراءة كلمة مزدوجة (32 بت)
define i32 @اقرأ_مزدوج(i64 %addr) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    %val = load volatile i32, ptr %ptr, align 4
    ret i32 %val
}

; قراءة كلمة رباعية (64 بت)
define i64 @اقرأ_رباعي(i64 %addr) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    %val = load volatile i64, ptr %ptr, align 8
    ret i64 %val
}

; كتابة بايت
define void @اكتب_بايت(i64 %addr, i8 %val) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    store volatile i8 %val, ptr %ptr, align 1
    ret void
}

; كتابة كلمة
define void @اكتب_كلمة(i64 %addr, i16 %val) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    store volatile i16 %val, ptr %ptr, align 2
    ret void
}

; كتابة كلمة مزدوجة
define void @اكتب_مزدوج(i64 %addr, i32 %val) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    store volatile i32 %val, ptr %ptr, align 4
    ret void
}

; كتابة كلمة رباعية
define void @اكتب_رباعي(i64 %addr, i64 %val) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    store volatile i64 %val, ptr %ptr, align 8
    ret void
}

; =============================================================================
; القسم 2: منافذ الإدخال/الإخراج
; =============================================================================

; قراءة بايت من منفذ I/O
define i8 @اقرأ_منفذ8(i16 %port) #0 {
entry:
    %port32 = zext i16 %port to i32
    %result = call i8 asm sideeffect "inb %dx, %al", "={al},{dx}"(i32 %port32)
    ret i8 %result
}

; قراءة كلمة من منفذ I/O
define i16 @اقرأ_منفذ16(i16 %port) #0 {
entry:
    %port32 = zext i16 %port to i32
    %result = call i16 asm sideeffect "inw %dx, %ax", "={ax},{dx}"(i32 %port32)
    ret i16 %result
}

; قراءة كلمة مزدوجة من منفذ I/O
define i32 @اقرأ_منفذ32(i16 %port) #0 {
entry:
    %port32 = zext i16 %port to i32
    %result = call i32 asm sideeffect "inl %dx, %eax", "={eax},{dx}"(i32 %port32)
    ret i32 %result
}

; كتابة بايت إلى منفذ I/O
define void @اكتب_منفذ8(i16 %port, i8 %val) #0 {
entry:
    %port32 = zext i16 %port to i32
    call void asm sideeffect "outb %al, %dx", "{al},{dx}"(i8 %val, i32 %port32)
    ret void
}

; كتابة كلمة إلى منفذ I/O
define void @اكتب_منفذ16(i16 %port, i16 %val) #0 {
entry:
    %port32 = zext i16 %port to i32
    call void asm sideeffect "outw %ax, %dx", "{ax},{dx}"(i16 %val, i32 %port32)
    ret void
}

; كتابة كلمة مزدوجة إلى منفذ I/O
define void @اكتب_منفذ32(i16 %port, i32 %val) #0 {
entry:
    %port32 = zext i16 %port to i32
    call void asm sideeffect "outl %eax, %dx", "{eax},{dx}"(i32 %val, i32 %port32)
    ret void
}

; =============================================================================
; القسم 3: التحكم بالمعالج
; =============================================================================

; تعطيل المقاطعات
define void @عطل_مقاطعات() #0 {
entry:
    call void asm sideeffect "cli", ""()
    ret void
}

; تفعيل المقاطعات
define void @فعل_مقاطعات() #0 {
entry:
    call void asm sideeffect "sti", ""()
    ret void
}

; إيقاف المعالج
define void @توقف() #0 {
entry:
    call void asm sideeffect "hlt", ""()
    ret void
}

; انتظار قصير (للـ spinlock)
define void @انتظر_قصير() #0 {
entry:
    call void asm sideeffect "pause", ""()
    ret void
}

; لا عملية
define void @لا_عملية() #0 {
entry:
    call void asm sideeffect "nop", ""()
    ret void
}

; =============================================================================
; القسم 4: سجلات النظام
; =============================================================================

; قراءة CR0
define i64 @اقرأ_سجل_تحكم0() #0 {
entry:
    %result = call i64 asm sideeffect "mov %cr0, $0", "=r"()
    ret i64 %result
}

; قراءة CR2 (عنوان خطأ الصفحة)
define i64 @اقرأ_سجل_تحكم2() #0 {
entry:
    %result = call i64 asm sideeffect "mov %cr2, $0", "=r"()
    ret i64 %result
}

; قراءة CR3 (قاعدة جدول الصفحات)
define i64 @اقرأ_سجل_تحكم3() #0 {
entry:
    %result = call i64 asm sideeffect "mov %cr3, $0", "=r"()
    ret i64 %result
}

; قراءة CR4
define i64 @اقرأ_سجل_تحكم4() #0 {
entry:
    %result = call i64 asm sideeffect "mov %cr4, $0", "=r"()
    ret i64 %result
}

; كتابة CR0
define void @اكتب_سجل_تحكم0(i64 %val) #0 {
entry:
    call void asm sideeffect "mov $0, %cr0", "r"(i64 %val)
    ret void
}

; كتابة CR3
define void @اكتب_سجل_تحكم3(i64 %val) #0 {
entry:
    call void asm sideeffect "mov $0, %cr3", "r"(i64 %val)
    ret void
}

; كتابة CR4
define void @اكتب_سجل_تحكم4(i64 %val) #0 {
entry:
    call void asm sideeffect "mov $0, %cr4", "r"(i64 %val)
    ret void
}

; قراءة MSR
define i64 @اقرأ_سجل_نموذج(i32 %msr) #0 {
entry:
    %result = call {i32, i32} asm sideeffect "rdmsr", "={eax},={edx},{ecx}"(i32 %msr)
    %lo = extractvalue {i32, i32} %result, 0
    %hi = extractvalue {i32, i32} %result, 1
    %lo64 = zext i32 %lo to i64
    %hi64 = zext i32 %hi to i64
    %hi_shifted = shl i64 %hi64, 32
    %combined = or i64 %hi_shifted, %lo64
    ret i64 %combined
}

; كتابة MSR
define void @اكتب_سجل_نموذج(i32 %msr, i64 %val) #0 {
entry:
    %lo64 = and i64 %val, 4294967295
    %hi64 = lshr i64 %val, 32
    %lo = trunc i64 %lo64 to i32
    %hi = trunc i64 %hi64 to i32
    call void asm sideeffect "wrmsr", "{eax},{edx},{ecx}"(i32 %lo, i32 %hi, i32 %msr)
    ret void
}

; =============================================================================
; القسم 5: جداول الواصفات
; =============================================================================

; هيكل GDTR/IDTR
%DescriptorTablePtr = type { i16, i64 }

; تحميل GDT
define void @حمل_جدول_واصفات_عام(i64 %base, i16 %limit) #0 {
entry:
    %ptr = alloca %DescriptorTablePtr, align 8
    %limit_ptr = getelementptr %DescriptorTablePtr, ptr %ptr, i32 0, i32 0
    store i16 %limit, ptr %limit_ptr, align 2
    %base_ptr = getelementptr %DescriptorTablePtr, ptr %ptr, i32 0, i32 1
    store i64 %base, ptr %base_ptr, align 8
    call void asm sideeffect "lgdt ($0)", "r"(ptr %ptr)
    ret void
}

; تحميل IDT
define void @حمل_جدول_مقاطعات(i64 %base, i16 %limit) #0 {
entry:
    %ptr = alloca %DescriptorTablePtr, align 8
    %limit_ptr = getelementptr %DescriptorTablePtr, ptr %ptr, i32 0, i32 0
    store i16 %limit, ptr %limit_ptr, align 2
    %base_ptr = getelementptr %DescriptorTablePtr, ptr %ptr, i32 0, i32 1
    store i64 %base, ptr %base_ptr, align 8
    call void asm sideeffect "lidt ($0)", "r"(ptr %ptr)
    ret void
}

; تحميل TR
define void @حمل_سجل_مهمة(i16 %selector) #0 {
entry:
    call void asm sideeffect "ltr $0", "r"(i16 %selector)
    ret void
}

; =============================================================================
; القسم 6: العمليات الذرية
; =============================================================================

; تحميل ذري
define i64 @تحميل_ذري(ptr %ptr) #0 {
entry:
    %val = load atomic i64, ptr %ptr seq_cst, align 8
    ret i64 %val
}

; تخزين ذري
define void @تخزين_ذري(ptr %ptr, i64 %val) #0 {
entry:
    store atomic i64 %val, ptr %ptr seq_cst, align 8
    ret void
}

; إضافة ذرية
define i64 @إضافة_ذرية(ptr %ptr, i64 %val) #0 {
entry:
    %old = atomicrmw add ptr %ptr, i64 %val seq_cst
    ret i64 %old
}

; طرح ذري
define i64 @طرح_ذري(ptr %ptr, i64 %val) #0 {
entry:
    %old = atomicrmw sub ptr %ptr, i64 %val seq_cst
    ret i64 %old
}

; تبادل ذري
define i64 @تبادل_ذري(ptr %ptr, i64 %val) #0 {
entry:
    %old = atomicrmw xchg ptr %ptr, i64 %val seq_cst
    ret i64 %old
}

; مقارنة وتبديل
define i64 @قارن_وبدل_ذري(ptr %ptr, i64 %expected, i64 %desired) #0 {
entry:
    %result = cmpxchg ptr %ptr, i64 %expected, i64 %desired seq_cst seq_cst
    %old = extractvalue { i64, i1 } %result, 0
    ret i64 %old
}

; =============================================================================
; القسم 7: حواجز الذاكرة
; =============================================================================

define void @حاجز_ذاكرة() #0 {
entry:
    fence seq_cst
    ret void
}

define void @حاجز_قراءة() #0 {
entry:
    fence acquire
    ret void
}

define void @حاجز_كتابة() #0 {
entry:
    fence release
    ret void
}

; =============================================================================
; القسم 8: الذاكرة الافتراضية
; =============================================================================

; إبطال صفحة TLB
define void @أبطل_صفحة(i64 %addr) #0 {
entry:
    %ptr = inttoptr i64 %addr to ptr
    call void asm sideeffect "invlpg ($0)", "r"(ptr %ptr)
    ret void
}

; تنظيف TLB بالكامل (إعادة تحميل CR3)
define void @نظف_ذاكرة_مؤقتة_صفحات() #0 {
entry:
    %cr3 = call i64 @اقرأ_سجل_تحكم3()
    call void @اكتب_سجل_تحكم3(i64 %cr3)
    ret void
}

; =============================================================================
; القسم 9: دوال VGA للشاشة النصية
; =============================================================================

@VGA_BUFFER = constant i64 753664  ; 0xB8000

; كتابة حرف إلى الشاشة
define void @اكتب_حرف_شاشة(i32 %x, i32 %y, i8 %char, i8 %color) #0 {
entry:
    ; حساب الموقع: (y * 80 + x) * 2
    %y64 = zext i32 %y to i64
    %x64 = zext i32 %x to i64
    %row_offset = mul i64 %y64, 160
    %col_offset = mul i64 %x64, 2
    %offset = add i64 %row_offset, %col_offset
    %base = load i64, ptr @VGA_BUFFER, align 8
    %addr = add i64 %base, %offset
    
    ; كتابة الحرف
    %ptr = inttoptr i64 %addr to ptr
    store volatile i8 %char, ptr %ptr, align 1
    
    ; كتابة اللون
    %color_addr = add i64 %addr, 1
    %color_ptr = inttoptr i64 %color_addr to ptr
    store volatile i8 %color, ptr %color_ptr, align 1
    
    ret void
}

; مسح الشاشة
define void @امسح_شاشة(i8 %color) #0 {
entry:
    br label %loop

loop:
    %i = phi i64 [ 0, %entry ], [ %next_i, %loop ]
    %base = load i64, ptr @VGA_BUFFER, align 8
    %char_addr = add i64 %base, %i
    %char_ptr = inttoptr i64 %char_addr to ptr
    store volatile i8 32, ptr %char_ptr, align 1  ; مسافة
    
    %color_offset = add i64 %i, 1
    %color_addr = add i64 %base, %color_offset
    %color_ptr = inttoptr i64 %color_addr to ptr
    store volatile i8 %color, ptr %color_ptr, align 1
    
    %next_i = add i64 %i, 2
    %done = icmp uge i64 %next_i, 4000  ; 80 * 25 * 2
    br i1 %done, label %exit, label %loop

exit:
    ret void
}

; =============================================================================
; السمات
; =============================================================================

attributes #0 = { nounwind "frame-pointer"="none" "no-builtins" }
