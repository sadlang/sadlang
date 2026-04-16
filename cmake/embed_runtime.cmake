# ============================================================================
# بسم الله الرحمن الرحيم
# ============================================================================
# سكريبت CMake لتضمين ملف Runtime كبيانات في C++
# CMake script to embed runtime C file as data in C++
# ============================================================================
# الوصف:
#   يقرأ ملف sad_embedded_runtime.c ويحوّله إلى مصفوفة بايتات C++
#   (char array) يمكن تضمينها مباشرة في الكود المترجم sadc.
#   هذا يلغي الحاجة لنسخ runtime مكررة كنصوص خام (raw strings).
#
# المدخلات:
#   RUNTIME_INPUT_FILE  — مسار ملف .c المراد تضمينه
#   RUNTIME_OUTPUT_FILE — مسار ملف .h الناتج
#
# المخرجات:
#   ملف header يحتوي على:
#     - const char sad_embedded_runtime_data[] = "...";
#     - const unsigned long long sad_embedded_runtime_size = ...;
# ============================================================================

# (AR) التحقق من وجود المتغيرات المطلوبة
# (EN) Verify required variables are set
if(NOT DEFINED RUNTIME_INPUT_FILE)
    message(FATAL_ERROR "RUNTIME_INPUT_FILE غير محدد / RUNTIME_INPUT_FILE not defined")
endif()

if(NOT DEFINED RUNTIME_OUTPUT_FILE)
    message(FATAL_ERROR "RUNTIME_OUTPUT_FILE غير محدد / RUNTIME_OUTPUT_FILE not defined")
endif()

# (AR) قراءة الملف المصدري
# (EN) Read the source file
file(READ "${RUNTIME_INPUT_FILE}" RUNTIME_CONTENT)

# (AR) حساب حجم المحتوى
# (EN) Calculate content size
string(LENGTH "${RUNTIME_CONTENT}" RUNTIME_SIZE)

# (AR) تحويل الأحرف الخاصة لتكون متوافقة مع نص C++
# (EN) Escape special characters for C++ string compatibility
# ترتيب الاستبدال مهم: يجب تهريب \ أولاً ثم الباقي
string(REPLACE "\\" "\\\\" RUNTIME_CONTENT "${RUNTIME_CONTENT}")
string(REPLACE "\"" "\\\"" RUNTIME_CONTENT "${RUNTIME_CONTENT}")
string(REPLACE "\n" "\\n\"\n\"" RUNTIME_CONTENT "${RUNTIME_CONTENT}")

# (AR) كتابة ملف الهيدر
# (EN) Write the header file
file(WRITE "${RUNTIME_OUTPUT_FILE}"
"// ============================================================================\n\
// ملف مُنشأ تلقائياً — لا تعدّل يدوياً!\n\
// Auto-generated file — DO NOT EDIT manually!\n\
// ============================================================================\n\
// المصدر: ${RUNTIME_INPUT_FILE}\n\
// ============================================================================\n\
#ifndef SAD_EMBEDDED_RUNTIME_DATA_H\n\
#define SAD_EMBEDDED_RUNTIME_DATA_H\n\
\n\
// (AR) بيانات ملف runtime المضمّنة كنص C++\n\
// (EN) Embedded runtime file data as C++ string\n\
static const char sad_embedded_runtime_data[] =\n\
\"${RUNTIME_CONTENT}\";\n\
\n\
// (AR) حجم البيانات بالبايتات\n\
// (EN) Data size in bytes\n\
static const unsigned long long sad_embedded_runtime_size = ${RUNTIME_SIZE};\n\
\n\
#endif // SAD_EMBEDDED_RUNTIME_DATA_H\n"
)

message(STATUS "  (AR) تم تضمين runtime: ${RUNTIME_INPUT_FILE} -> ${RUNTIME_OUTPUT_FILE} (${RUNTIME_SIZE} bytes)")
