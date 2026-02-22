// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: hover_provider.cpp
// الوصف: مزود التلميحات الثورية - يعرض معلومات شاملة وغنية عند التحويم
// ══════════════════════════════════════════════════════════════════════════════
//
// عند تحويم المستخدم فوق أي عنصر في الكود، نعرض:
//   ✦ للكلمات المفتاحية: شرح مفصل + مثال + رابط للتوثيق
//   ✦ للدوال: التوقيع الكامل + وصف المعلمات + نوع الإرجاع + أمثلة
//   ✦ للمتغيرات: النوع + القيمة + معلومات النطاق
//   ✦ للأصناف: الوصف + الأصناف الأب + الأعضاء
//   ✦ للدوال المدمجة: توثيق كامل مع أمثلة عملية
//   ✦ تنسيق Markdown غني مع ألوان وجداول
//
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "lsp_engine.h"
#include "arabic_utils.h"
#include <unordered_map>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  توثيق الكلمات المفتاحية العربية
// ══════════════════════════════════════════════════════════════════════════════

struct KeywordDoc {
    std::string description;  // وصف مفصل
    std::string example;      // مثال عملي
    std::string category;     // التصنيف
};

static const std::unordered_map<std::string, KeywordDoc>& get_keyword_docs() {
    static std::unordered_map<std::string, KeywordDoc> docs = {
        // ──── تعريفات ────
        {"\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9", { // دالة
            "تعريف دالة جديدة. الدالة هي كتلة من الكود يمكن استدعاؤها بالاسم.",
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9(\xd8\xa3, \xd8\xa8) {\n    \xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9 \xd8\xa3 + \xd8\xa8\n}",
            "تعريفات"
        }},
        {"\xd8\xb5\xd9\x86\xd9\x81", { // صنف
            "تعريف صنف (class). الصنف هو قالب لإنشاء كائنات تحمل بيانات وسلوكيات.",
            "\xd8\xb5\xd9\x86\xd9\x81 \xd8\xb4\xd8\xae\xd8\xb5 {\n    \xd9\x85\xd9\x86\xd8\xb4\xd8\xa6(\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85, \xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd8\xb1) {\n        \xd9\x87\xd8\xb0\xd8\xa7.\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85 = \xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85\n    }\n}",
            "تعريفات"
        }},
        {"\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1", { // متغير
            "تعريف متغير قابل للتعديل. يمكن تغيير قيمته لاحقاً.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf = 0\n\xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf = \xd8\xa7\xd9\x84\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf + 1",
            "تعريفات"
        }},
        {"\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa", { // ثابت
            "تعريف ثابت لا يمكن تعديل قيمته بعد التعيين الأول.",
            "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa \xd8\xa7\xd9\x84\xd8\xad\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89 = 100",
            "تعريفات"
        }},
        // ──── بنى التحكم ────
        {"\xd8\xa5\xd8\xb0\xd8\xa7", { // إذا
            "جملة شرطية. يُنفَّذ الكود داخلها فقط إذا كان الشرط صحيحاً.",
            "\xd8\xa5\xd8\xb0\xd8\xa7 (\xd8\xa7\xd9\x84\xd8\xb9\xd9\x85\xd8\xb1 >= 18) {\n    \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd8\xb1\xd8\xa7\xd8\xb4\xd8\xaf\")\n} \xd9\x88\xd8\xa5\xd9\x84\xd8\xa7 {\n    \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd9\x82\xd8\xa7\xd8\xb5\xd8\xb1\")\n}",
            "بنى التحكم"
        }},
        {"\xd9\x88\xd8\xa5\xd9\x84\xd8\xa7", { // وإلا
            "الفرع البديل في جملة شرطية. يُنفَّذ عندما يكون شرط 'إذا' خاطئاً.",
            "\xd8\xa5\xd8\xb0\xd8\xa7 (\xd8\xa7\xd9\x84\xd8\xb4\xd8\xb1\xd8\xb7) {\n    // ...\n} \xd9\x88\xd8\xa5\xd9\x84\xd8\xa7 {\n    // \xd9\x8a\xd9\x86\xd9\x81\xd8\xb0 \xd9\x87\xd9\x86\xd8\xa7\n}",
            "بنى التحكم"
        }},
        {"\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7", { // بينما
            "حلقة تكرارية تستمر طالما الشرط صحيح.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb9 = 0\n\xd8\xa8\xd9\x8a\xd9\x86\xd9\x85\xd8\xa7 (\xd8\xb9 < 10) {\n    \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\xd8\xb9)\n    \xd8\xb9 = \xd8\xb9 + 1\n}",
            "الحلقات"
        }},
        {"\xd9\x84\xd9\x83\xd9\x84", { // لكل
            "حلقة تمر على كل عنصر في مجموعة (مصفوفة، نطاق، إلخ).",
            "\xd9\x84\xd9\x83\xd9\x84 \xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1 \xd9\x81\xd9\x8a [1, 2, 3] {\n    \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1)\n}",
            "الحلقات"
        }},
        // ──── معالجة الأخطاء ────
        {"\xd8\xad\xd8\xa7\xd9\x88\xd9\x84", { // حاول
            "كتلة معالجة الأخطاء. يُنفَّذ الكود داخلها، وإذا حدث خطأ يُنتقل لكتلة 'امسك'.",
            "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84 {\n    \xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9 = 10 / 0\n} \xd8\xa7\xd9\x85\xd8\xb3\xd9\x83 (\xd8\xae) {\n    \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd8\xad\xd8\xaf\xd8\xab \xd8\xae\xd8\xb7\xd8\xa3: \" + \xd8\xae)\n}",
            "معالجة الأخطاء"
        }},
        {"\xd8\xa7\xd9\x85\xd8\xb3\xd9\x83", { // امسك
            "كتلة التقاط الخطأ. تستقبل الخطأ الذي حدث في كتلة 'حاول'.",
            "\xd8\xad\xd8\xa7\xd9\x88\xd9\x84 {\n    // \xd9\x83\xd9\x88\xd8\xaf \xd9\x82\xd8\xaf \xd9\x8a\xd8\xae\xd8\xb7\xd8\xa6\n} \xd8\xa7\xd9\x85\xd8\xb3\xd9\x83 (\xd8\xae\xd8\xb7\xd8\xa3) {\n    \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\xd8\xae\xd8\xb7\xd8\xa3)\n}",
            "معالجة الأخطاء"
        }},
        // ──── كلمات مفتاحية أخرى ────
        {"\xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9", { // إرجاع
            "إرجاع قيمة من دالة وإنهاء تنفيذها.",
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9(\xd9\x86) {\n    \xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9 \xd9\x86 * \xd9\x86\n}",
            "تحكم"
        }},
        {"\xd8\xa7\xd8\xae\xd8\xb1\xd8\xac", { // اخرج
            "الخروج من حلقة (بينما/لكل) فوراً.",
            "\xd9\x84\xd9\x83\xd9\x84 \xd8\xb9 \xd9\x81\xd9\x8a \xd8\xa7\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9 {\n    \xd8\xa5\xd8\xb0\xd8\xa7 (\xd8\xb9 == 0) {\n        \xd8\xa7\xd8\xae\xd8\xb1\xd8\xac\n    }\n}",
            "تحكم"
        }},
        {"\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x85\xd8\xb1", { // استمر
            "الانتقال للدورة التالية مباشرة في الحلقة.",
            "\xd9\x84\xd9\x83\xd9\x84 \xd8\xb9 \xd9\x81\xd9\x8a [1, 2, 3, 4, 5] {\n    \xd8\xa5\xd8\xb0\xd8\xa7 (\xd8\xb9 % 2 == 0) {\n        \xd8\xa7\xd8\xb3\xd8\xaa\xd9\x85\xd8\xb1\n    }\n    \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\xd8\xb9)\n}",
            "تحكم"
        }},
        {"\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf", { // استورد
            "استيراد وحدة أو مكتبة خارجية.",
            "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x88\xd8\xb1\xd8\xaf \"\xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6\xd9\x8a\xd8\xa7\xd8\xaa\"\n\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\xd8\xac\xd8\xb0\xd8\xb1(16))",
            "وحدات"
        }},
        // ──── القيم ────
        {"\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", { // صحيح
            "القيمة المنطقية 'صحيح' (true). تمثل الحالة الإيجابية.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86\xd8\xb4\xd8\xb7 = \xd8\xb5\xd8\xad\xd9\x8a\xd8\xad",
            "قيم"
        }},
        {"\xd8\xae\xd8\xb7\xd8\xa3", { // خطأ
            "القيمة المنطقية 'خطأ' (false). تمثل الحالة السلبية.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x85\xd9\x86\xd8\xaa\xd9\x87\xd9\x8a = \xd8\xae\xd8\xb7\xd8\xa3",
            "قيم"
        }},
        {"\xd8\xb9\xd8\xaf\xd9\x85", { // عدم
            "القيمة الفارغة (null/none). تمثل غياب قيمة.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9 = \xd8\xb9\xd8\xaf\xd9\x85",
            "قيم"
        }},
        {"\xd9\x87\xd8\xb0\xd8\xa7", { // هذا
            "مرجع للكائن الحالي داخل صنف. يشير إلى العنصر الذي تم استدعاء الطريقة عليه.",
            "\xd8\xb5\xd9\x86\xd9\x81 \xd8\xb4\xd8\xae\xd8\xb5 {\n    \xd9\x85\xd9\x86\xd8\xb4\xd8\xa6(\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85) {\n        \xd9\x87\xd8\xb0\xd8\xa7.\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85 = \xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85\n    }\n}",
            "كائنات"
        }},
        // ──── بنى البيانات المتقدمة ────
        {"\xd8\xa8\xd9\x86\xd9\x8a\xd8\xa9", { // بنية
            "تعريف بنية بيانات (struct). مشابهة للصنف لكن أبسط وأخف، مناسبة لتجميع بيانات مرتبطة.",
            "\xd8\xa8\xd9\x86\xd9\x8a\xd8\xa9 \xd9\x86\xd9\x82\xd8\xb7\xd8\xa9 {\n    \xd8\xb3: \xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\n    \xd8\xb5: \xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a\n}",
            "بنى البيانات"
        }},
        {"\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf", { // تعداد
            "تعريف تعداد (enum). يحدد مجموعة من القيم المسماة.",
            "\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf \xd8\xa7\xd9\x84\xd9\x84\xd9\x88\xd9\x86 {\n    \xd8\xa3\xd8\xad\xd9\x85\xd8\xb1\n    \xd8\xa3\xd8\xae\xd8\xb6\xd8\xb1\n    \xd8\xa3\xd8\xb2\xd8\xb1\xd9\x82\n}",
            "بنى البيانات"
        }},
        {"\xd8\xb3\xd9\x85\xd8\xa9", { // سمة
            "تعريف سمة (trait/interface). تحدد مجموعة من الطرق التي يجب على الأصناف تنفيذها.",
            "\xd8\xb3\xd9\x85\xd8\xa9 \xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd9\x84\xd9\x84\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9 {\n    \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9()\n}",
            "بنى البيانات"
        }},
        {"\xd8\xaa\xd9\x86\xd9\x81\xd9\x8a\xd8\xb0", { // تنفيذ
            "تنفيذ سمة لصنف أو بنية (impl). يربط الصنف بالسمة بتقديم تطبيقات فعلية للطرق.",
            "\xd8\xaa\xd9\x86\xd9\x81\xd9\x8a\xd8\xb0 \xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd9\x84\xd9\x84\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9 \xd9\x84 \xd8\xb4\xd8\xae\xd8\xb5 {\n    \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9() {\n        \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\xd9\x87\xd8\xb0\xd8\xa7.\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85)\n    }\n}",
            "بنى البيانات"
        }},
        {"\xd9\x81\xd8\xb6\xd8\xa7\xd8\xa1", { // فضاء
            "فضاء أسماء (namespace). يُستخدم لتنظيم الكود ومنع تعارض الأسماء بين الوحدات.",
            "\xd9\x81\xd8\xb6\xd8\xa7\xd8\xa1 \xd8\xb1\xd9\x8a\xd8\xa7\xd8\xb6\xd9\x8a\xd8\xa7\xd8\xaa {\n    \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xac\xd8\xb0\xd8\xb1(\xd9\x86) { ... }\n}",
            "تنظيم الكود"
        }},
        // ──── غير متزامن ────
        {"\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86", { // غير_متزامن
            "تعريف دالة غير متزامنة (async). تسمح بتنفيذ عمليات طويلة بدون حجب البرنامج.",
            "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86 \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xac\xd9\x84\xd8\xa8_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa() {\n    \xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9 = \xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1 \xd8\xb7\xd9\x84\xd8\xa8_\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9()\n    \xd8\xa5\xd8\xb1\xd8\xac\xd8\xa7\xd8\xb9 \xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9\n}",
            "تزامن"
        }},
        {"\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1", { // انتظر
            "انتظار نتيجة عملية غير متزامنة (await). يُستخدم داخل دالة غير_متزامن.",
            "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86 \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd9\x85\xd8\xab\xd8\xa7\xd9\x84() {\n    \xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86 = \xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1 \xd8\xb9\xd9\x85\xd9\x84\xd9\x8a\xd8\xa9()\n}",
            "تزامن"
        }},
        // ──── اختبارات ────
        {"\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1", { // اختبار
            "تعريف كتلة اختبار وحدوية (unit test). يتم تشغيلها تلقائياً عند تشغيل الاختبارات.",
            "\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb1 \"\xd8\xac\xd9\x85\xd8\xb9 \xd8\xb1\xd9\x82\xd9\x85\xd9\x8a\xd9\x86\" {\n    \xd8\xa3\xd9\x83\xd8\xaf(\xd8\xac\xd9\x85\xd8\xb9(2, 3) == 5)\n}",
            "اختبارات"
        }},
        {"\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82", { // تطابق
            "مطابقة الأنماط (pattern matching). مشابهة لـ switch لكن أقوى، تدعم أنماطاً معقدة.",
            "\xd8\xaa\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82 (\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9) {\n    \xd8\xad\xd8\xa7\xd9\x84\xd8\xa9 1 { \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd9\x88\xd8\xa7\xd8\xad\xd8\xaf\") }\n    \xd8\xad\xd8\xa7\xd9\x84\xd8\xa9 2 { \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd8\xa7\xd8\xab\xd9\x86\xd8\xa7\xd9\x86\") }\n    \xd8\xa7\xd9\x81\xd8\xaa\xd8\xb1\xd8\xa7\xd8\xb6\xd9\x8a { \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd8\xa3\xd8\xae\xd8\xb1\xd9\x89\") }\n}",
            "بنى التحكم"
        }},
        // ──── أنواع البيانات ────
        {"\xd8\xb1\xd9\x82\xd9\x85", { // رقم
            "نوع عدد صحيح (integer). يمثل الأعداد الصحيحة بدون فاصلة عشرية.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb9\xd9\x85\xd8\xb1: \xd8\xb1\xd9\x82\xd9\x85 = 25",
            "أنواع البيانات"
        }},
        {"\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a", { // عشري
            "نوع عدد عشري (float/double). يمثل الأعداد ذات الفاصلة العشرية.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb3\xd8\xb9\xd8\xb1: \xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a = 19.99",
            "أنواع البيانات"
        }},
        {"\xd9\x86\xd8\xb5", { // نص
            "نوع نصي (string). يمثل سلسلة من الحروف.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd8\xa7\xd8\xb3\xd9\x85: \xd9\x86\xd8\xb5 = \"\xd8\xa3\xd8\xad\xd9\x85\xd8\xaf\"",
            "أنواع البيانات"
        }},
        {"\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", { // منطقي
            "نوع منطقي (boolean). يقبل فقط: صحيح أو خطأ.",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86\xd8\xb4\xd8\xb7: \xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a = \xd8\xb5\xd8\xad\xd9\x8a\xd8\xad",
            "أنواع البيانات"
        }},
    };
    return docs;
}

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ══════════════════════════════════════════════════════════════════════════════

/// بناء وصف نوع الرمز بالعربية
static std::string symbol_kind_to_arabic(AnalyzedSymbolKind kind) {
    switch (kind) {
        case AnalyzedSymbolKind::Function:    return "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9"; // دالة
        case AnalyzedSymbolKind::Method:      return "\xd8\xb7\xd8\xb1\xd9\x8a\xd9\x82\xd8\xa9"; // طريقة
        case AnalyzedSymbolKind::Variable:    return "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1"; // متغير
        case AnalyzedSymbolKind::Constant:    return "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa"; // ثابت
        case AnalyzedSymbolKind::Class:       return "\xd8\xb5\xd9\x86\xd9\x81"; // صنف
        case AnalyzedSymbolKind::Property:    return "\xd8\xae\xd8\xa7\xd8\xb5\xd9\x8a\xd8\xa9"; // خاصية
        case AnalyzedSymbolKind::Parameter:   return "\xd9\x85\xd8\xb9\xd9\x84\xd9\x85\xd8\xa9"; // معلمة
        case AnalyzedSymbolKind::Enum:        return "\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf"; // تعداد
        case AnalyzedSymbolKind::Module:      return "\xd9\x88\xd8\xad\xd8\xaf\xd8\xa9"; // وحدة
        case AnalyzedSymbolKind::Import:      return "\xd8\xa7\xd8\xb3\xd8\xaa\xd9\x8a\xd8\xb1\xd8\xa7\xd8\xaf"; // استيراد
        case AnalyzedSymbolKind::Constructor: return "\xd9\x85\xd9\x86\xd8\xb4\xd8\xa6"; // منشئ
        case AnalyzedSymbolKind::Interface:   return "\xd9\x88\xd8\xa7\xd8\xac\xd9\x87\xd8\xa9"; // واجهة
        case AnalyzedSymbolKind::Struct:      return "\xd8\xa8\xd9\x86\xd9\x8a\xd8\xa9"; // بنية
    }
    return "رمز";
}

/// بناء إيموجي لنوع الرمز
static std::string symbol_kind_emoji(AnalyzedSymbolKind kind) {
    switch (kind) {
        case AnalyzedSymbolKind::Function:    return "⚡";
        case AnalyzedSymbolKind::Method:      return "🔧";
        case AnalyzedSymbolKind::Variable:    return "📦";
        case AnalyzedSymbolKind::Constant:    return "🔒";
        case AnalyzedSymbolKind::Class:       return "🏗️";
        case AnalyzedSymbolKind::Property:    return "📋";
        case AnalyzedSymbolKind::Parameter:   return "📥";
        case AnalyzedSymbolKind::Enum:        return "📊";
        case AnalyzedSymbolKind::Module:      return "📁";
        case AnalyzedSymbolKind::Constructor: return "🔨";
        default: return "🔹";
    }
}

/// بناء توقيع دالة بتنسيق Markdown
static std::string build_function_signature(const AnalyzedSymbol& sym) {
    if (!sym.func_info) return sym.name;

    std::string sig = "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 " + sym.name + "("; // "دالة "
    bool first = true;
    for (const auto& [pname, ptype] : sym.func_info->parameters) {
        if (!first) sig += ", ";
        first = false;
        sig += pname;
        if (!ptype.name.empty() && ptype.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf") {
            sig += ": " + ptype.name;
        }
    }
    sig += ")";

    if (!sym.func_info->return_type.name.empty() &&
        sym.func_info->return_type.name != "\xd8\xb9\xd8\xaf\xd9\x85") {
        sig += " -> " + sym.func_info->return_type.name;
    }

    return sig;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تنفيذ التلميحات الثورية
// ══════════════════════════════════════════════════════════════════════════════

std::optional<Hover> LspEngine::hover(const DocumentUri& uri, const Position& pos) {
    auto doc = doc_store_->get(uri);
    if (!doc) return std::nullopt;

    // الحصول على الكلمة تحت المؤشر
    std::string word = doc_store_->get_word_at(uri, pos);
    if (word.empty()) return std::nullopt;

    // ──── ١. فحص الكلمات المفتاحية أولاً ────
    {
        const auto& kw_docs = get_keyword_docs();
        auto it = kw_docs.find(word);
        if (it != kw_docs.end()) {
            Hover hover_result;
            std::string content;

            content += "### 🔑 كلمة مفتاحية: `" + word + "`\n\n";
            content += "**التصنيف:** " + it->second.category + "\n\n";
            content += "---\n\n";
            content += it->second.description + "\n\n";
            content += "**مثال:**\n```sad\n" + it->second.example + "\n```\n";

            hover_result.contents = {"markdown", content};

            // حساب نطاق الكلمة
            std::string line = doc_store_->get_line(uri, pos.line);
            auto id_range = arabic::get_identifier_range(line, pos.character);
            hover_result.range = Range{
                {pos.line, id_range.first},
                {pos.line, id_range.second}
            };
            return hover_result;
        }
    }

    // ──── ٢. البحث في الفهرس ────
    auto def = index_->find_definition(word, uri, pos);
    if (!def) return std::nullopt;

    // بناء محتوى التلميح الغني
    Hover hover_result;
    std::string content;

    // عنوان: إيموجي + نوع الرمز
    content += "### " + symbol_kind_emoji(def->kind) + " "
             + symbol_kind_to_arabic(def->kind);

    // إذا مدمج
    if (def->is_builtin) {
        content += " (مدمجة)";
    }
    // إذا مصدّر
    if (def->is_exported) {
        content += " 📤";
    }
    content += "\n\n";

    // التوقيع (للدوال والطرق)
    if (def->func_info) {
        content += "```sad\n" + build_function_signature(*def) + "\n```\n\n";

        // معلومات إضافية عن الدالة
        if (def->func_info->is_async) content += "⏳ **غير متزامنة** (async)\n\n";
        if (def->func_info->is_generator) content += "🔄 **مولّدة** (generator)\n\n";

        // جدول المعلمات
        if (!def->func_info->parameters.empty()) {
            content += "| المعلمة | النوع |\n|---------|-------|\n";
            for (const auto& [pname, ptype] : def->func_info->parameters) {
                content += "| `" + pname + "` | ";
                if (!ptype.name.empty() && ptype.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf") {
                    content += "`" + ptype.name + "`";
                } else {
                    content += "أي";
                }
                content += " |\n";
            }
            content += "\n";
        }

        // نوع الإرجاع
        if (!def->func_info->return_type.name.empty() &&
            def->func_info->return_type.name != "\xd8\xb9\xd8\xaf\xd9\x85" &&
            def->func_info->return_type.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf") {
            content += "**الإرجاع:** `" + def->func_info->return_type.name + "`\n\n";
        }
    } else {
        // عرض المتغير/الثابت/الصنف
        content += "```sad\n";
        if (def->kind == AnalyzedSymbolKind::Constant) {
            content += "\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa "; // "ثابت "
        } else if (def->kind == AnalyzedSymbolKind::Variable) {
            content += "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 "; // "متغير "
        } else if (def->kind == AnalyzedSymbolKind::Class) {
            content += "\xd8\xb5\xd9\x86\xd9\x81 "; // "صنف "
        } else if (def->kind == AnalyzedSymbolKind::Enum) {
            content += "\xd8\xaa\xd8\xb9\xd8\xaf\xd8\xa7\xd8\xaf "; // "تعداد "
        }
        content += def->name;
        if (!def->type.name.empty() && def->type.name != "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf") {
            content += ": " + def->type.name;
        }
        content += "\n```\n\n";

        // معلومات الصنف
        if (def->class_info) {
            if (!def->class_info->parent_class.empty()) {
                content += "**يرث من:** `" + def->class_info->parent_class + "`\n\n";
            }
        }
    }

    // معلومات النطاق
    if (!def->scope_owner.empty()) {
        content += "📍 **داخل:** `" + def->scope_owner + "`\n\n";
    }

    // التوثيق
    if (!def->documentation.empty()) {
        content += "---\n\n" + def->documentation + "\n";
    }

    hover_result.contents = {"markdown", content};

    // حساب نطاق الكلمة
    std::string line = doc_store_->get_line(uri, pos.line);
    auto id_range = arabic::get_identifier_range(line, pos.character);
    hover_result.range = Range{
        {pos.line, id_range.first},
        {pos.line, id_range.second}
    };

    return hover_result;
}

} // namespace lsp
} // namespace sad

