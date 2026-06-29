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
// (AR) المدخل الموحَّد الوحيد لنظام الأنواع — يضمّن الترويسة المولَّدة ضمنًا
//      (sadTypeKindArabicName + SURFACE_TYPE_NAMES + surfaceTypeDescriptionAr).
//      الترويسة نفسها توثّق: «include هذا فقط — لا حاجة لغيره» (CW-04/CW-06).
// (EN) The single unified entry point for the type system; it transitively
//      includes the generated header (no extra include needed by design).
#include "sad_type_system.h"
// (AR) أوصاف hover للكلمات المفتاحية — نظام داخليّ مملوك لأداة LSP، يُولَّد من
//      tools/lsp/data/keyword_docs.yaml ويُتحقَّق منه ضدّ المعجم (language-truth)
//      وقت التوليد. الأداة تستهلك مصدر الحقيقة ولا توسّعه (CW-19/CW-10).
// (EN) Keyword hover descriptions — an LSP-owned internal system, generated from
//      tools/lsp/data/keyword_docs.yaml and validated against the lexicon at
//      build time. The tool consumes the SoT, it does not expand it.
#include "keyword_docs_generated.h"
#include <unordered_map>

namespace sad {
namespace lsp {

// ══════════════════════════════════════════════════════════════════════════════
//  أسماء أنواع لا تُعرض في سهم/سطر الإرجاع — من مصدر الحقيقة لا مهرَّدة
//  (AR) «فراغ» (Void) = لا قيمة، و«غير_محدد» = استنتاج عاجز. نشتقّ «فراغ» من
//        sadTypeKindArabicName(Void) (CW-19/CW-10) كي لا ينحرف الحارس بعد توحيد
//        الأسماء (كان «عدم» المهرَّد فلم يعد يطابق Void بعد صيرورته «فراغ»).
//  (EN) Return-type names hidden in hover, sourced from the SoT (no magic literal).
// ══════════════════════════════════════════════════════════════════════════════
static const std::string HOVER_RET_VOID =
    Sad::Types::sadTypeKindArabicName(Sad::Types::SadTypeKind::Void); // فراغ
static const std::string HOVER_RET_UNRESOLVED =
    "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf"; // غير_محدد

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
            // (AR) مثال «دالة»: استُعمل «ارجع» (KEYWORD_RETURN من المعجم) بدل «إرجاع»
            //      المهرَّدة خطأً — تطابقًا لتصحيح مفتاح ارجع نفسه.
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd9\x85\xd8\xac\xd9\x85\xd9\x88\xd8\xb9(\xd8\xa3, \xd8\xa8) {\n    \xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9 \xd8\xa3 + \xd8\xa8\n}",
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
        {"\xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9", { // ارجع (SoT: KEYWORD_RETURN — كان مهرَّدًا خطأً «إرجاع»)
            "إرجاع قيمة من دالة وإنهاء تنفيذها.",
            "\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd9\x85\xd8\xb1\xd8\xa8\xd8\xb9(\xd9\x86) {\n    \xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9 \xd9\x86 * \xd9\x86\n}",
            "تحكم"
        }},
        {"\xd8\xaa\xd9\x88\xd9\x82\xd9\x81", { // توقف (SoT: KEYWORD_BREAK — كان مهرَّدًا خطأً «اخرج»)
            "الخروج من حلقة (بينما/لكل) فوراً.",
            "\xd9\x84\xd9\x83\xd9\x84 \xd8\xb9 \xd9\x81\xd9\x8a \xd8\xa7\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9 {\n    \xd8\xa5\xd8\xb0\xd8\xa7 (\xd8\xb9 == 0) {\n        \xd8\xaa\xd9\x88\xd9\x82\xd9\x81\n    }\n}",
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
        {"\xd8\xb9\xd8\xaf\xd9\x85", { // عدم (نوع/قيمة سطحيّ: الوصف من المصدر — انظر أدناه)
            // (AR) «عدم» سطحيّ (surface:true)، فوصفه يأتي من مصدر الحقيقة كبقيّة
            //      الأنواع السطحية ⇒ يُترك فارغًا هنا (لا نصّ ميّت). المثال يبقى.
            // (EN) «عدم» is surface:true; its description comes from the SoT like the
            //      other surface types, so it's left empty here (no dead text).
            "",
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
        {"\xd9\x86\xd9\x81\xd9\x91\xd8\xb0", { // نفّذ (SoT: block_opener impl — كان مهرَّدًا خطأً «تنفيذ»)
            "تنفيذ سمة لصنف أو بنية (impl). يربط الصنف بالسمة بتقديم تطبيقات فعلية للطرق.",
            "\xd9\x86\xd9\x81\xd9\x91\xd8\xb0 \xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd9\x84\xd9\x84\xd8\xb7\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xa9 \xd9\x84 \xd8\xb4\xd8\xae\xd8\xb5 {\n    \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9() {\n        \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\xd9\x87\xd8\xb0\xd8\xa7.\xd8\xa7\xd9\x84\xd8\xa7\xd8\xb3\xd9\x85)\n    }\n}",
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
            // (AR) مثال «غير_متزامن»: «إرجاع» المهرَّدة خطأً ⇐ «ارجع» المعجمية.
            "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86 \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd8\xac\xd9\x84\xd8\xa8_\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa() {\n    \xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9 = \xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1 \xd8\xb7\xd9\x84\xd8\xa8_\xd8\xb4\xd8\xa8\xd9\x83\xd8\xa9()\n    \xd8\xa7\xd8\xb1\xd8\xac\xd8\xb9 \xd9\x86\xd8\xaa\xd9\x8a\xd8\xac\xd8\xa9\n}",
            "تزامن"
        }},
        {"\xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1", { // انتظر
            "انتظار نتيجة عملية غير متزامنة (await). يُستخدم داخل دالة غير_متزامن.",
            "\xd8\xba\xd9\x8a\xd8\xb1_\xd9\x85\xd8\xaa\xd8\xb2\xd8\xa7\xd9\x85\xd9\x86 \xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9 \xd9\x85\xd8\xab\xd8\xa7\xd9\x84() {\n    \xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd9\x86 = \xd8\xa7\xd9\x86\xd8\xaa\xd8\xb8\xd8\xb1 \xd8\xb9\xd9\x85\xd9\x84\xd9\x8a\xd8\xa9()\n}",
            "تزامن"
        }},
        // ──── اختبارات ────
        {"\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xb1", { // اختبر (SoT: block_opener test — كان مهرَّدًا خطأً «اختبار»)
            "تعريف كتلة اختبار وحدوية (unit test). يتم تشغيلها تلقائياً عند تشغيل الاختبارات.",
            "\xd8\xa7\xd8\xae\xd8\xaa\xd8\xa8\xd8\xb1 \"\xd8\xac\xd9\x85\xd8\xb9 \xd8\xb1\xd9\x82\xd9\x85\xd9\x8a\xd9\x86\" {\n    \xd8\xa3\xd9\x83\xd8\xaf(\xd8\xac\xd9\x85\xd8\xb9(2, 3) == 5)\n}",
            "اختبارات"
        }},
        {"\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82", { // طابق (SoT: KEYWORD_MATCH — كان مهرَّدًا خطأً «تطابق»)
            "مطابقة الأنماط (pattern matching). مشابهة لـ switch لكن أقوى، تدعم أنماطاً معقدة.",
            "\xd8\xb7\xd8\xa7\xd8\xa8\xd9\x82 (\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9) {\n    \xd8\xb9\xd9\x86\xd8\xaf\xd9\x85\xd8\xa7 1 { \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd9\x88\xd8\xa7\xd8\xad\xd8\xaf\") }\n    \xd8\xb9\xd9\x86\xd8\xaf\xd9\x85\xd8\xa7 2 { \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd8\xa7\xd8\xab\xd9\x86\xd8\xa7\xd9\x86\") }\n    \xd8\xa7\xd9\x81\xd8\xaa\xd8\xb1\xd8\xa7\xd8\xb6\xd9\x8a { \xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9(\"\xd8\xa3\xd8\xae\xd8\xb1\xd9\x89\") }\n}",
            "بنى التحكم"
        }},
        // ──── أنواع البيانات ────
        // (AR) الأنواع السطحية: الوصف يأتي حصرًا من مصدر الحقيقة (types.yaml ⇒
        //      resolve_surface_type_desc)، فحقل description هنا يُترك فارغًا عمدًا
        //      تجنّبًا لنصّ ميّت/مكرَّر (CW-10/CW-19/DRY). يبقى المثال والتصنيف.
        // (EN) Surface types: the description is sourced solely from the SoT, so the
        //      description field is intentionally empty (no dead/duplicated text).
        //      The example and category remain (the SoT carries no example yet).
        {"\xd8\xb1\xd9\x82\xd9\x85", { // رقم
            "",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb9\xd9\x85\xd8\xb1: \xd8\xb1\xd9\x82\xd9\x85 = 25",
            "أنواع البيانات"
        }},
        {"\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a", { // عشري
            "",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb3\xd8\xb9\xd8\xb1: \xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a = 19.99",
            "أنواع البيانات"
        }},
        {"\xd9\x86\xd8\xb5", { // نص
            "",
            "\xd9\x85\xd8\xaa\xd8\xba\xd9\x8a\xd8\xb1 \xd8\xa7\xd8\xb3\xd9\x85: \xd9\x86\xd8\xb5 = \"\xd8\xa3\xd8\xad\xd9\x85\xd8\xaf\"",
            "أنواع البيانات"
        }},
        {"\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a", { // منطقي
            "",
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
        sym.func_info->return_type.name != HOVER_RET_VOID) { // فراغ (Void: لا قيمة)
        sig += " -> " + sym.func_info->return_type.name;
    }

    return sig;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الأنواع السطحية — اشتقاق من مصدر الحقيقة (types.yaml ⇒ الترويسة المولَّدة)
// ──────────────────────────────────────────────────────────────────────────────
//  (AR) تصنيف التلميح للأنواع السطحية. ثابت لا رقم/نصّ سحريّ (CW-09/CW-10).
//  (EN) Hover category label for surface types. A named constant (no magic text).
// ══════════════════════════════════════════════════════════════════════════════
static const std::string SURFACE_TYPE_CATEGORY =
    "\xd8\xa3\xd9\x86\xd9\x88\xd8\xa7\xd8\xb9 \xd8\xa7\xd9\x84\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa"; // أنواع البيانات

/// (AR) وصف النوع السطحيّ من مصدر الحقيقة، مع تطبيع عربيّ احتياطيّ (همزة/ألف/ياء/تاء).
///      يجرّب المطابقة الحرفيّة أولًا (الأسرع)، ثمّ يطابق صورة المستخدم المُطبَّعة
///      على أسماء الأنواع السطحية المُطبَّعة (كي يطابق «اي» ما يكتبه المستخدم لـ«أي»).
///      يُرجِع "" إن لم تكن الكلمة نوعًا سطحيًّا.
/// (EN) Surface-type description from the SoT, with Arabic-normalization fallback
///      (hamza/alef/yaa/taa). Tries the literal match first, then matches the user's
///      normalized form against normalized surface names. Returns "" if not a surface type.
static std::string resolve_surface_type_desc(const std::string& word) {
    // (AR) ١) مطابقة حرفيّة مباشرة من المصدر / (EN) 1) direct literal SoT match
    const char* direct = Sad::Types::surfaceTypeDescriptionAr(word);
    if (direct[0] != '\0') return direct;

    // (AR) ٢) مطابقة مُطبَّعة (تتسامح مع همزة «أي»←«اي»، إلخ) / (EN) 2) normalized match
    const std::string norm = arabic::normalize_arabic(word);
    for (const auto& surface_name : Sad::Types::SURFACE_TYPE_NAMES) {
        const std::string name_str{surface_name};
        if (arabic::normalize_arabic(name_str) == norm) {
            const char* desc = Sad::Types::surfaceTypeDescriptionAr(name_str);
            if (desc[0] != '\0') return desc;
        }
    }
    return "";
}

/// (AR) سطر عنوان تلميح الكلمة المفتاحية «### 🔑 كلمة مفتاحية: `word`» — مشترك بين
///      مسارات التلميح الثلاثة (المعجم/النوع السطحيّ/المعجم بلا توثيق محرَّر) إزالةً
///      للتكرار (CW-19: DRY). نقطةُ تغييرٍ واحدة لو تغيّر التنسيق مستقبلًا.
/// (EN) Keyword-hover title line, shared by all three hover paths (DRY, single point
///      of change for the format).
static std::string keyword_hover_title(const std::string& word) {
    return "### 🔑 كلمة مفتاحية: `" + word + "`\n\n";
}

/// (AR) يبني تلميح نوع سطحيّ (وصفه من المصدر) لكلمةٍ ليست في معجم الكلمات المفتاحية،
///      كي لا يبقى النوع السطحيّ (مصفوفة/خريطة/أي/فراغ) بلا تلميح أصلًا.
/// (EN) Builds a surface-type hover (SoT-sourced) for a word absent from the keyword
///      lexicon, so surface types (array/map/any/void) are never left without a hover.
static std::string build_surface_type_hover_body(const std::string& word,
                                                 const std::string& sot_desc) {
    std::string content;
    content += keyword_hover_title(word);
    content += "**\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb5\xd9\x86\xd9\x8a\xd9\x81:** " + SURFACE_TYPE_CATEGORY + "\n\n"; // التصنيف
    content += "---\n\n";
    content += sot_desc + "\n\n";
    return content;
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

    // (AR) حساب نطاق الكلمة — مشترك لكلّ مسارات التلميح أدناه (DRY، CW-19).
    // (EN) Word range — shared by every hover path below (DRY).
    auto compute_word_range = [&]() -> Range {
        std::string line = doc_store_->get_line(uri, pos.line);
        auto id_range = arabic::get_identifier_range(line, pos.character);
        return Range{{pos.line, id_range.first}, {pos.line, id_range.second}};
    };

    // ──── ١. فحص الكلمات المفتاحية أولاً ────
    {
        const auto& kw_docs = get_keyword_docs();
        auto it = kw_docs.find(word);
        if (it != kw_docs.end()) {
            Hover hover_result;
            std::string content;

            content += keyword_hover_title(word);
            content += "**التصنيف:** " + it->second.category + "\n\n";
            content += "---\n\n";
            // (AR) أولويّة الوصف: نوع سطحيّ من مصدر الحقيقة (types.yaml) ثمّ وصف
            //      الأداة المولَّد (keyword_docs.yaml، مُتحقَّق ضدّ المعجم) ثمّ الوصف
            //      المحرَّر احتياطًا. الأداة تستهلك SoT ولا توسّعه (CW-19/CW-10).
            // (EN) Description priority: surface type from the SoT (types.yaml), then
            //      the tool's generated doc (keyword_docs.yaml, validated against the
            //      lexicon), then the authored fallback. The tool consumes the SoT.
            const std::string sot_type_desc = resolve_surface_type_desc(word);
            const std::string& tool_kw_desc =
                sad::lsp::docs::keywordDocDescriptionAr(word);
            content += (!sot_type_desc.empty()    ? sot_type_desc
                        : !tool_kw_desc.empty()   ? tool_kw_desc
                                                  : it->second.description) + "\n\n";
            content += "**مثال:**\n```sad\n" + it->second.example + "\n```\n";

            hover_result.contents = {"markdown", content};
            hover_result.range = compute_word_range();
            return hover_result;
        }
    }

    // ──── ١-ب. نوع سطحيّ غير مُسجَّل في المعجم (مصفوفة/خريطة/أي/فراغ) ────
    // (AR) بعض الأنواع السطحية ليست كلماتٍ مفتاحية في المعجم، فكانت بلا تلميح
    //      إطلاقًا. نعرض وصفها من مصدر الحقيقة كي يكون الاشتقاق مكتمِلًا لكلّ
    //      الأنواع السطحية التسعة لا أربعةٍ منها فقط (BF-22: لا ثغرة تغطية).
    // (EN) Some surface types aren't keywords in the lexicon, so they had NO hover.
    //      Show their SoT description so derivation covers ALL nine surface types.
    {
        const std::string sot_type_desc = resolve_surface_type_desc(word);
        if (!sot_type_desc.empty()) {
            Hover hover_result;
            hover_result.contents =
                {"markdown", build_surface_type_hover_body(word, sot_type_desc)};
            hover_result.range = compute_word_range();
            return hover_result;
        }
    }

    // ──── ١-ج. كلمة معجمية بلا توثيق محرَّر (ليس/أطلق/سمة/عقد…) ────
    // (AR) كثير من كلمات المعجم ليست في get_keyword_docs (مثل العامل «ليس»)، فكانت
    //      بلا تلميح إطلاقًا. نظام أوصاف الأداة (keyword_docs.yaml) يغطّيها فنعرضه
    //      مباشرة — تغطية كاملة لا جزئية (BF-22). لا «التصنيف» هنا لأنّ get_keyword_docs
    //      (مصدره) غائب؛ نكتفي بوصف الأداة المُتحقَّق ضدّ المعجم.
    // (EN) Many lexicon words (e.g. «ليس») are absent from get_keyword_docs and had
    //      NO hover. The tool's docs system (keyword_docs.yaml) covers them, so we
    //      show it — full, not partial (BF-22). No category line here.
    {
        const std::string& tool_kw_desc =
            sad::lsp::docs::keywordDocDescriptionAr(word);
        if (!tool_kw_desc.empty()) {
            Hover hover_result;
            std::string content;
            content += keyword_hover_title(word);
            content += "---\n\n";
            content += tool_kw_desc + "\n\n";
            hover_result.contents = {"markdown", content};
            hover_result.range = compute_word_range();
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
            def->func_info->return_type.name != HOVER_RET_VOID &&       // فراغ (Void)
            def->func_info->return_type.name != HOVER_RET_UNRESOLVED) { // غير_محدد
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
    hover_result.range = compute_word_range();

    return hover_result;
}

} // namespace lsp
} // namespace sad

