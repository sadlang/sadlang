# ============================================================================
# أمثلة على استخدام المؤشرات في لغة ص
# Pointer Usage Examples in Sad Language
# ============================================================================
#
# الوصف بالعربية:
# ----------------
# هذا الملف يحتوي على أمثلة عملية لاستخدام المؤشرات في لغة ص
# يغطي الحالات الشائعة والأنماط البرمجية المهمة
#
# English Description:
# -------------------
# This file contains practical examples of using pointers in Sad Language
# Covers common use cases and important programming patterns
#
# ============================================================================

# ============================================================================
# مثال 1: المؤشرات الأساسية
# Example 1: Basic Pointers
# ============================================================================

اطبع("=== مثال 1: المؤشرات الأساسية ===") ;
اطبع("=== Example 1: Basic Pointers ===") ;

# تعريف متغير عادي
# Define a regular variable
رقم x = 42 ;

# إنشاء مؤشر للمتغير (الحصول على عنوانه)
# Create pointer to variable (get its address)
مؤشر<رقم> ptr = &x ;

# إلغاء المرجعية للقراءة
# Dereference for reading
رقم value = *ptr ;
اطبع("القيمة عبر المؤشر / Value via pointer: ", value) ;

# إلغاء المرجعية للكتابة
# Dereference for writing
*ptr = 100 ;
اطبع("القيمة الجديدة / New value: ", x) ;

# ============================================================================
# مثال 2: المؤشرات والمصفوفات
# Example 2: Pointers and Arrays
# ============================================================================

اطبع("\n=== مثال 2: المؤشرات والمصفوفات ===") ;
اطبع("=== Example 2: Pointers and Arrays ===") ;

# تعريف مصفوفة
# Define an array
مصفوفة<رقم> numbers = [10, 20, 30, 40, 50] ;

# مؤشر لأول عنصر
# Pointer to first element
مؤشر<رقم> arrayPtr = &numbers[0] ;

# التنقل في المصفوفة باستخدام حساب المؤشر
# Traverse array using pointer arithmetic
اطبع("عناصر المصفوفة / Array elements:") ;
لكل (رقم i = 0 ; i < 5 ; i++) {
    اطبع("  [", i, "] = ", *(arrayPtr + i)) ;
}

# ============================================================================
# مثال 3: تمرير المؤشرات للدوال (Pass by Reference)
# Example 3: Passing Pointers to Functions (Pass by Reference)
# ============================================================================

اطبع("\n=== مثال 3: تمرير المؤشرات للدوال ===") ;
اطبع("=== Example 3: Passing Pointers to Functions ===") ;

# دالة تضاعف قيمة عبر مؤشر
# Function that doubles a value via pointer
دالة double_value(مؤشر<رقم> ptr) -> فارغ {
    *ptr = *ptr * 2 ;
}

رقم num = 15 ;
اطبع("قبل / Before: ", num) ;
double_value(&num) ;
اطبع("بعد / After: ", num) ;

# ============================================================================
# مثال 4: تبديل قيمتين باستخدام المؤشرات
# Example 4: Swapping Values Using Pointers
# ============================================================================

اطبع("\n=== مثال 4: تبديل القيم ===") ;
اطبع("=== Example 4: Swapping Values ===") ;

دالة swap(مؤشر<رقم> a, مؤشر<رقم> b) -> فارغ {
    رقم temp = *a ;
    *a = *b ;
    *b = temp ;
}

رقم first = 10 ;
رقم second = 20 ;
اطبع("قبل التبديل / Before swap: first=", first, ", second=", second) ;
swap(&first, &second) ;
اطبع("بعد التبديل / After swap: first=", first, ", second=", second) ;

# ============================================================================
# مثال 5: المؤشرات والبنى (Structures)
# Example 5: Pointers and Structures
# ============================================================================

اطبع("\n=== مثال 5: المؤشرات والبنى ===") ;
اطبع("=== Example 5: Pointers and Structures ===") ;

# تعريف صنف (بنية بيانات)
# Define a class (data structure)
صنف Point {
    رقم x ;
    رقم y ;
    
    دالة init(رقم x_val, رقم y_val) {
        x = x_val ;
        y = y_val ;
    }
    
    دالة print() {
        اطبع("نقطة / Point (", x, ", ", y, ")") ;
    }
}

Point p = جديد Point(5, 10) ;
مؤشر<Point> ptrToPoint = &p ;

# الوصول للأعضاء عبر المؤشر
# Access members via pointer
(*ptrToPoint).x = 15 ;
(*ptrToPoint).print() ;

# ============================================================================
# مثال 6: مؤشرات متعددة المستويات
# Example 6: Multi-level Pointers
# ============================================================================

اطبع("\n=== مثال 6: مؤشرات متعددة المستويات ===") ;
اطبع("=== Example 6: Multi-level Pointers ===") ;

رقم original = 99 ;
مؤشر<رقم> ptr1 = &original ;
مؤشر<مؤشر<رقم>> ptr2 = &ptr1 ;

اطبع("القيمة الأصلية / Original value: ", original) ;
اطبع("عبر مؤشر واحد / Via single pointer: ", *ptr1) ;
اطبع("عبر مؤشر مزدوج / Via double pointer: ", **ptr2) ;

# تعديل القيمة عبر مؤشر مزدوج
# Modify value via double pointer
**ptr2 = 200 ;
اطبع("القيمة المعدلة / Modified value: ", original) ;

# ============================================================================
# مثال 7: المؤشرات الثابتة (Const Pointers)
# Example 7: Const Pointers
# ============================================================================

اطبع("\n=== مثال 7: المؤشرات الثابتة ===") ;
اطبع("=== Example 7: Const Pointers ===") ;

رقم value1 = 100 ;
رقم value2 = 200 ;

# مؤشر لقيمة ثابتة (لا يمكن تعديل القيمة عبر المؤشر)
# Pointer to const value (cannot modify value via pointer)
ثابت مؤشر<رقم> constPtr = &value1 ;
# *constPtr = 50 ;  # خطأ / Error!

# مؤشر ثابت (لا يمكن تغيير المؤشر نفسه)
# Const pointer (cannot change pointer itself)
مؤشر<رقم> ثابت ptrConst = &value1 ;
*ptrConst = 150 ;  # مسموح / Allowed
# ptrConst = &value2 ;  # خطأ / Error!

# ============================================================================
# مثال 8: المؤشرات ووظائف تخصيص الذاكرة
# Example 8: Pointers and Memory Allocation
# ============================================================================

اطبع("\n=== مثال 8: تخصيص الذاكرة ===") ;
اطبع("=== Example 8: Memory Allocation ===") ;

# تخصيص ذاكرة ديناميكية
# Allocate dynamic memory
مؤشر<رقم> dynamicArray = خصص<رقم>(5) ;

# ملء المصفوفة
# Fill the array
لكل (رقم i = 0 ; i < 5 ; i++) {
    *(dynamicArray + i) = i * i ;
}

# طباعة المحتوى
# Print contents
اطبع("المصفوفة الديناميكية / Dynamic array:") ;
لكل (رقم i = 0 ; i < 5 ; i++) {
    اطبع("  [", i, "] = ", *(dynamicArray + i)) ;
}

# تحرير الذاكرة
# Free memory
حرر(dynamicArray) ;

# ============================================================================
# مثال 9: المؤشرات والنصوص (Strings)
# Example 9: Pointers and Strings
# ============================================================================

اطبع("\n=== مثال 9: المؤشرات والنصوص ===") ;
اطبع("=== Example 9: Pointers and Strings ===") ;

نص message = "مرحباً بالعالم" ;
مؤشر<نص> strPtr = &message ;

اطبع("النص عبر المؤشر / String via pointer: ", *strPtr) ;

# تعديل النص عبر المؤشر
# Modify string via pointer
*strPtr = "Hello World" ;
اطبع("النص المعدل / Modified string: ", message) ;

# ============================================================================
# مثال 10: أنماط برمجية متقدمة
# Example 10: Advanced Programming Patterns
# ============================================================================

اطبع("\n=== مثال 10: أنماط برمجية متقدمة ===") ;
اطبع("=== Example 10: Advanced Programming Patterns ===") ;

# Linked List Node (عقدة قائمة مترابطة)
صنف Node {
    رقم data ;
    مؤشر<Node> next ;
    
    دالة init(رقم value) {
        data = value ;
        next = فارغ ;
    }
}

# إنشاء قائمة مترابطة بسيطة
# Create simple linked list
Node node1 = جديد Node(10) ;
Node node2 = جديد Node(20) ;
Node node3 = جديد Node(30) ;

node1.next = &node2 ;
node2.next = &node3 ;

# التنقل في القائمة
# Traverse the list
اطبع("عناصر القائمة المترابطة / Linked list elements:") ;
مؤشر<Node> current = &node1 ;
بينما (current != فارغ) {
    اطبع("  ", (*current).data) ;
    current = (*current).next ;
}

اطبع("\n=== انتهت الأمثلة ===") ;
اطبع("=== Examples Complete ===") ;
