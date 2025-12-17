# ======================================================================
# test_memory_04.s - اختبار كشف تسرب الذاكرة
# Stage 1 Test: Memory Leak Detection
# ======================================================================
# الوصف / Description:
#   اختبار كشف تسرب الذاكرة (تخصيص بدون تحرير)
#   Test memory leak detection (allocation without deallocation)
#
# Expected Output:
#   ALLOCATED
#   WARNING: Memory leak detected
# ======================================================================

دالة leak_memory()
    مؤشر<رقم> ptr = خصص(100)
    اطبع("ALLOCATED")
    # عدم تحرير الذاكرة يسبب تسرب!
    # Not freeing memory causes leak!
نهاية

دالة main()
    leak_memory()
    
    # عند انتهاء البرنامج، يجب اكتشاف التسرب
    # On program exit, leak should be detected
نهاية
