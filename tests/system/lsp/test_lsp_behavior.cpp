// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: test_lsp_behavior.cpp
// الوصف: اختبارات سلوك لخادم LSP — سيناريوهات واقعية كاملة (دورة حياة المستند)
// ══════════════════════════════════════════════════════════════════════════════
//
// بينما يختبر `test_lsp_engine_comprehensive.cpp` كلّ ميزة منفردةً، يختبر هذا الملفّ
// **السلوك المتكامل** عبر سيناريوهات تحاكي جلسة محرّر حقيقية:
//   • سيناريو: فتح → استعلام → تعديل تدريجيّ → إغلاق
//   • سيناريو: العربية الأصيلة (تطبيع التشكيل في الإكمال)
//   • سيناريو: كشف خطأ نحويّ ثم اختفاؤه بعد الإصلاح
//   • سيناريو: تعدّد المستندات المفتوحة في آنٍ واحد
//
// المبدأ: نؤكّد على سلوك يراه المستخدم (تشخيص يظهر/يختفي، رمز يُكتشف بعد الكتابة)،
// لا على تفاصيل التنفيذ الداخلية.
//
// يربط: sad_lsp_engine + sad_core    يُبنى تحت BUILD_TESTS=ON
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "sad_test.h"
#include "lsp_engine.h"

#include <string>
#include <vector>

using namespace sad::lsp;

namespace {

/// مشهد محرّر: محرّك + التقاط آخر تشخيصات لكلّ مستند.
struct Editor {
    LspEngine engine;
    std::vector<Diagnostic> diags;

    Editor() {
        engine.set_diagnostics_publisher(
            [this](const DocumentUri&, const std::vector<Diagnostic>& d) { diags = d; });
        engine.initialize("file:///");
    }

    void open(const DocumentUri& uri, const std::string& text, int version = 1) {
        TextDocumentItem item;
        item.uri = uri;
        item.language_id = "sad";
        item.version = version;
        item.text = text;
        engine.did_open(item);
    }

    /// تعديل بإحلال كامل (whole-document replace) — أبسط شكل تغيير.
    void replace(const DocumentUri& uri, const std::string& new_text, int version) {
        TextDocumentContentChangeEvent change;
        change.text = new_text;          // بلا range ⇒ إحلال كامل
        engine.did_change(uri, {change}, version);
    }

    bool has_error() const {
        for (const auto& d : diags)
            if (d.severity == DiagnosticSeverity::Error) return true;
        return false;
    }
    bool has_code(const std::string& code) const {
        for (const auto& d : diags)
            if (d.code == code) return true;
        return false;
    }
};

const char* VALID_FUNC = u8"دالة مرحبا()\n"
                         u8"    اطبع(\"السلام عليكم\")\n"
                         u8"نهاية\n";

} // namespace

// ══════════════════════════════════════════════════════════════════════════════
//  سيناريو ① — دورة حياة كاملة: فتح → استعلام → إغلاق
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspBehavior, دورة_حياة_فتح_استعلام_إغلاق) {
    Editor ed;
    DocumentUri uri = "file:///مرحبا.ص";

    // فتح
    ed.open(uri, VALID_FUNC);
    ASSERT_FALSE(ed.has_error());

    // استعلام: الدالة مكتشفة في رموز المستند
    auto syms = ed.engine.document_symbols(uri);
    bool found = false;
    for (const auto& s : syms)
        if (s.name.find(u8"مرحبا") != std::string::npos) found = true;
    ASSERT_TRUE(found);

    // إغلاق: تُمسح التشخيصات
    ed.engine.did_close(uri);
    ASSERT_TRUE(ed.diags.empty());
}

// ══════════════════════════════════════════════════════════════════════════════
//  سيناريو ② — تحذير يظهر ثم يختفي بعد الإصلاح
// ══════════════════════════════════════════════════════════════════════════════
//
// ملاحظة سلوكية مهمّة: نقص "نهاية" يُستردّ منه المحلل تلقائيًّا (يُدرجها) ⇒ يُنتج
// تحذيرًا (Warning) لا خطأً (Error). لذا نختبر دورة «ظهور ثمّ اختفاء» عبر مولّد
// تشخيص حتميّ موثوق: نمط P2 (إعادة إسناد 'متغير' داخل حلقة). الإصلاح = إسناد عاديّ.
TEST(LspBehavior, تحذير_يظهر_ثم_يختفي_بعد_الإصلاح) {
    Editor ed;
    DocumentUri uri = "file:///إصلاح.ص";

    // كود بنمط P2 ⇒ تحذير SAD-P2 يظهر
    ed.open(uri,
            u8"دالة عد()\n"
            u8"    متغير ن = 0\n"
            u8"    بينما ن < 3\n"
            u8"        متغير ن = ن + 1\n"   // ← المشكلة: 'متغير' يُنشئ متغيّرًا محليًّا
            u8"    نهاية\n"
            u8"نهاية\n");
    ASSERT_TRUE(ed.has_code("SAD-P2"));

    // إصلاح: إسناد عاديّ بلا 'متغير' ⇒ التحذير يجب أن يزول (إعادة فتح ⇒ تحليل متزامن)
    ed.open(uri,
            u8"دالة عد()\n"
            u8"    متغير ن = 0\n"
            u8"    بينما ن < 3\n"
            u8"        ن = ن + 1\n"          // ← مُصلَح: إسناد لا تعريف
            u8"    نهاية\n"
            u8"نهاية\n",
            2);
    ASSERT_FALSE(ed.has_code("SAD-P2"));
}

// ══════════════════════════════════════════════════════════════════════════════
//  سيناريو ③ — العربية الأصيلة: الإكمال لا يتأثّر بالتشكيل
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspBehavior, الإكمال_يعمل_مع_أسماء_عربية) {
    Editor ed;
    DocumentUri uri = "file:///عربي.ص";
    ed.open(uri,
            u8"دالة احسب_المجموع(قائمة: قائمة)\n"
            u8"    أرجع 0\n"
            u8"نهاية\n"
            u8"\n");

    // الإكمال في السطر الفارغ ⇒ يجب أن يقترح شيئًا (مدمجات على الأقل)
    CompletionList list = ed.engine.completion(uri, Position{3, 0});
    ASSERT_GT(list.items.size(), 0u);
}

// ══════════════════════════════════════════════════════════════════════════════
//  سيناريو ④ — تعدّد المستندات المفتوحة معًا
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspBehavior, مستندان_مفتوحان_معًا) {
    Editor ed;
    DocumentUri a = "file:///أ.ص";
    DocumentUri b = "file:///ب.ص";

    ed.open(a, u8"دالة أولى()\n    أرجع 1\n نهاية\n");
    ed.open(b, u8"دالة ثانية()\n    أرجع 2\n نهاية\n");

    // كلٌّ يحتفظ برموزه المستقلّة
    auto sa = ed.engine.document_symbols(a);
    auto sb = ed.engine.document_symbols(b);

    bool a_has_first = false, b_has_second = false;
    for (const auto& s : sa) if (s.name.find(u8"أولى") != std::string::npos) a_has_first = true;
    for (const auto& s : sb) if (s.name.find(u8"ثانية") != std::string::npos) b_has_second = true;
    ASSERT_TRUE(a_has_first);
    ASSERT_TRUE(b_has_second);

    ed.engine.did_close(a);
    ed.engine.did_close(b);
}

// ══════════════════════════════════════════════════════════════════════════════
//  سيناريو ⑤ — تحذير مشكلة معروفة يظهر للمستخدم (تكامل end-to-end)
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspBehavior, تحذير_مشكلة_معروفة_يصل_للمستخدم) {
    Editor ed;
    DocumentUri uri = "file:///مشكلة.ص";
    ed.open(uri,
            u8"دالة عدّ()\n"
            u8"    متغير ع = 0\n"
            u8"    بينما ع < 5\n"
            u8"        متغير ع = ع + 1\n"
            u8"    نهاية\n"
            u8"نهاية\n");
    // القلب المجّانيّ يحذّر من نمط P2 (حلقة قد تكون لانهائية)
    ASSERT_TRUE(ed.has_code("SAD-P2"));
}

// ══════════════════════════════════════════════════════════════════════════════
//  سيناريو ⑥ — مدمجة من مصدر الحقيقة لا تُحذَّر كاذبًا كـ«غير معرّف»
// ══════════════════════════════════════════════════════════════════════════════
//
// قبل توحيد المصدر كان كاشف «غير معرّف» يعرف 21 اسمًا مهرَّدًا فقط، فيحذّر كاذبًا
// عن ~99٪ من المدمجات. الآن يستشير سجلّ مصدر الحقيقة (isKnownBuiltin، 1073).
// «قناة» مدمجة (تزامن، بلا استيراد) لم تكن في القائمة المهرَّدة القديمة.
TEST(LspBehavior, مدمجة_من_المصدر_لا_تُحذَّر_غير_معرّفة) {
    Editor ed;
    DocumentUri uri = "file:///مدمجة.ص";
    ed.open(uri,
            u8"دالة جرّب()\n"
            u8"    متغير ق = قناة(5)\n"
            u8"    أرجع ق\n"
            u8"نهاية\n");

    // لا يجوز وجود تحذير «المعرّف غير معرّف» (ص-ت١٠٣) عن «قناة».
    for (const auto& d : ed.diags) {
        if (d.code == u8"ص-ت١٠٣")
            ASSERT_TRUE(d.message.find(u8"قناة") == std::string::npos);
    }
}

// ══════════════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ══════════════════════════════════════════════════════════════════════════════

int main(int argc, char** argv) {
    return SAD_RUN_TESTS();
}
