// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: test_lsp_engine_comprehensive.cpp
// الوصف: اختبارات C++ شاملة لمحرّك خادم LSP (القلب المجّانيّ المفتوح)
// ══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يختبر صنف `LspEngine` مباشرةً عبر واجهته العامّة (لا عبر JSON-RPC)،
// مغطّيًا الميزات الـ22 الموصولة في القلب المجّانيّ بعد إزالة المزوّدات المتقدّمة:
//   • دورة الحياة والقدرات (initialize/capabilities)
//   • مزامنة المستندات والتشخيصات (did_open/did_change/did_close)
//   • ذكاء اللغة (completion/hover/definition/references/document_symbols)
//   • التحرير (semantic_tokens/folding/rename/code_actions/inlay_hints)
//   • كاشف المشاكل المعروفة (SAD-P2 … جزء من القلب المجّانيّ)
//
// كلّ اختبار يؤكّد ثوابت سليمة (لا انهيار + بنية صحيحة + سلوك حتميّ حيث يُمكن)،
// متجنّبًا التوكيد المفرط على تفاصيل قد تتغيّر بتغيّر المحلل.
//
// يربط: sad_lsp_engine + sad_core    يُبنى تحت BUILD_TESTS=ON
// الحمد لله رب العالمين
// ══════════════════════════════════════════════════════════════════════════════

#include "sad_test.h"
#include "lsp_engine.h"

#include <string>
#include <vector>

using namespace sad::lsp;

// ══════════════════════════════════════════════════════════════════════════════
//  مساعدات الاختبار
// ══════════════════════════════════════════════════════════════════════════════

namespace {

/// تُنشئ محرّكًا مهيّأً (يسجّل المدمجات عبر initialize) وتفتح مستندًا واحدًا.
/// تُعيد المحرّك عبر مؤشّر فريد، وتلتقط آخر تشخيصات مُنشورة في `out_diags`.
struct EngineFixture {
    LspEngine engine;
    std::vector<Diagnostic> last_diags;
    DocumentUri uri = "file:///اختبار.ص";

    EngineFixture() {
        engine.set_diagnostics_publisher(
            [this](const DocumentUri&, const std::vector<Diagnostic>& d) {
                last_diags = d;
            });
        engine.initialize("file:///");
    }

    /// يفتح مستندًا بالمحتوى المعطى (النشر متزامن في did_open).
    void open(const std::string& text) {
        TextDocumentItem item;
        item.uri = uri;
        item.language_id = "sad";
        item.version = 1;
        item.text = text;
        engine.did_open(item);
    }
};

/// نموذج كود ص صالح يحوي دالة ومتغيّرات — يُستعمل في عدّة اختبارات.
const char* SAMPLE = u8"دالة اجمع(أ: رقم، ب: رقم)\n"
                     u8"    متغير الناتج = أ + ب\n"
                     u8"    أرجع الناتج\n"
                     u8"نهاية\n";

} // namespace

// ══════════════════════════════════════════════════════════════════════════════
//  ① دورة الحياة والقدرات
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspCapabilities, تُعلن_قدرات_القلب_المجّانيّ) {
    LspEngine engine;
    ServerCapabilities caps = engine.initialize("file:///");

    // الميزات الموصولة يجب أن تكون مُعلَنة
    ASSERT_TRUE(caps.hover_provider);
    ASSERT_TRUE(caps.definition_provider);
    ASSERT_TRUE(caps.references_provider);
    ASSERT_TRUE(caps.document_symbol_provider);
    ASSERT_TRUE(caps.workspace_symbol_provider);
    ASSERT_TRUE(caps.code_action_provider);
    ASSERT_TRUE(caps.document_formatting_provider);
    ASSERT_TRUE(caps.rename_provider);
    ASSERT_TRUE(caps.inlay_hint_provider);
    ASSERT_TRUE(caps.folding_range_provider);
    ASSERT_TRUE(caps.code_lens_provider);
    ASSERT_TRUE(caps.selection_range_provider);
    ASSERT_TRUE(caps.call_hierarchy_provider);
    ASSERT_TRUE(caps.type_hierarchy_provider);
    ASSERT_TRUE(caps.document_link_provider);
    ASSERT_TRUE(caps.semantic_tokens_provider.full);
}

TEST(LspCapabilities, مزامنة_تدريجية_ومحفّزات_عربية) {
    LspEngine engine;
    ServerCapabilities caps = engine.initialize();
    ASSERT_TRUE(caps.text_document_sync.open_close);
    ASSERT_EQ(caps.text_document_sync.change, 2); // 2 = Incremental

    // الفاصلة العربية يجب أن تكون محفّزًا للإكمال
    bool has_arabic_comma = false;
    for (const auto& c : caps.completion_provider.trigger_characters)
        if (c == "\xd8\x8c") has_arabic_comma = true;
    ASSERT_TRUE(has_arabic_comma);
}

TEST(LspCapabilities, معلومات_الخادم) {
    LspEngine engine;
    engine.initialize();
    ServerInfo info = engine.get_server_info();
    ASSERT_FALSE(info.name.empty());
    ASSERT_FALSE(info.version.empty());
}

// ══════════════════════════════════════════════════════════════════════════════
//  ② مزامنة المستندات والتشخيصات
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspDiagnostics, كود_صالح_ينشر_تشخيصات_بلا_أخطاء) {
    EngineFixture fx;
    fx.open(SAMPLE);
    // قد تكون فارغة (لا أخطاء) — المهم ألّا تحوي خطأً (Error) في كود صالح
    for (const auto& d : fx.last_diags)
        ASSERT_TRUE(d.severity != DiagnosticSeverity::Error);
}

TEST(LspDiagnostics, كاشف_المشاكل_المعروفة_P2_حلقة_لانهائية) {
    EngineFixture fx;
    // متغير داخل بينما يُعيد إسناد نفسه ⇒ تحذير SAD-P2 (جزء من القلب المجّانيّ)
    fx.open(u8"دالة عد()\n"
            u8"    متغير س = 0\n"
            u8"    بينما س < 10\n"
            u8"        متغير س = س + 1\n"
            u8"    نهاية\n"
            u8"نهاية\n");
    bool found_p2 = false;
    for (const auto& d : fx.last_diags)
        if (d.code == "SAD-P2") found_p2 = true;
    ASSERT_TRUE(found_p2);
}

TEST(LspDiagnostics, إغلاق_المستند_يمسح_التشخيصات) {
    EngineFixture fx;
    fx.open(SAMPLE);
    fx.engine.did_close(fx.uri);
    // عند الإغلاق يُنشر مسحٌ (قائمة فارغة)
    ASSERT_TRUE(fx.last_diags.empty());
}

// ══════════════════════════════════════════════════════════════════════════════
//  ③ ذكاء اللغة
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspCompletion, يُكمل_بالمدمجات_والرموز_المحلّية) {
    EngineFixture fx;
    fx.open(SAMPLE);
    // الإكمال في بداية سطر جديد داخل النطاق العامّ
    CompletionList list = fx.engine.completion(fx.uri, Position{4, 0});
    ASSERT_GT(list.items.size(), 0u); // المدمجات وحدها تكفي لعدم الفراغ
}

// كتالوج المدمجات نابع من مصدر الحقيقة (language-truth/builtins/*.yaml عبر
// ALL_BUILTINS المُولَّد)، لا من قائمة مهرَّدة. حارس انحدار: عند كتابة البادئة
// «اطبع» يقترح الإكمالُ «اطبع_سطر» — مدمجة موجودة في مصدر الحقيقة وكانت غائبة
// عن التهريد اليدويّ القديم (16 دالة فقط).
TEST(LspCompletion, الكتالوج_من_مصدر_الحقيقة_لا_تهريد) {
    EngineFixture fx;
    // مطابقة الفهرس بالاسم الكامل (fuzzy_match_arabic = تطابق تامّ بعد التطبيع).
    // «اطبع_سطر» اسم مدمجة من مصدر الحقيقة لم تكن في التهريد القديم.
    fx.open(u8"دالة س()\n"
            u8"    اطبع_سطر\n"
            u8"نهاية\n");
    // المؤشّر بعد «اطبع_سطر» (4 مسافات + 8 أحرف = العمود 12).
    CompletionList list = fx.engine.completion(fx.uri, Position{1, 12});

    bool has_sot_only = false; // مدمجة من مصدر الحقيقة لم تكن في القائمة المهرَّدة
    for (const auto& item : list.items) {
        if (item.label.find(u8"اطبع_سطر") != std::string::npos) has_sot_only = true;
    }
    ASSERT_TRUE(has_sot_only);
}

// حارس انحدار: ALL_BUILTINS يحوي أسماء مكرّرة عبر فضاءات («استبدل» ×3 مثلًا)؛
// يجب ألّا يظهر الاسم نفسه مرّتين في الإكمال (إزالة التكرار في register_builtins).
TEST(LspCompletion, لا_تكرار_لاسم_مدمجة_واحدة) {
    EngineFixture fx;
    fx.open(u8"دالة س()\n"
            u8"    استبدل\n" // مدمجة مكرّرة في المصدر
            u8"نهاية\n");
    CompletionList list = fx.engine.completion(fx.uri, Position{1, 10});

    int count = 0;
    for (const auto& item : list.items) {
        if (item.label == u8"استبدل") ++count;
    }
    // إن ظهرت أصلًا فمرّة واحدة فقط (لا 3 كما في ALL_BUILTINS الخام).
    ASSERT_LE(count, 1);
}

// حارس انحدار: أنواع معاملات المدمجات غير متوفّرة في مصدر الحقيقة، فلا يجوز
// اختلاق النوع «أيّ». التوثيق التلقائيّ لمدمجة يجب ألّا يحوي «: أيّ».
TEST(LspCompletion, لا_يختلق_نوع_معامل_للمدمجة) {
    EngineFixture fx;
    fx.open(u8"دالة س()\n"
            u8"    اطبع\n" // مدمجة لها معامل واحد «قيمة» بلا نوع
            u8"نهاية\n");
    CompletionList list = fx.engine.completion(fx.uri, Position{1, 8});

    bool saw_print = false;
    for (const auto& item : list.items) {
        if (item.label != u8"اطبع") continue;
        saw_print = true;
        // النوع المختلَق «أيّ» (U+0623 U+064A) يجب ألّا يظهر في التوثيق.
        ASSERT_TRUE(item.documentation.value.find(u8": أي") == std::string::npos);
    }
    ASSERT_TRUE(saw_print);
}

// حارس انحدار: تلميح الكلمات المفتاحية كان مهرَّدًا بمفاتيح خاطئة لا تطابق المعجم
// (مصدر الحقيقة): «إرجاع» بدل «ارجع»، «اخرج» بدل «توقف»، «تطابق» بدل «طابق»،
// «تنفيذ» بدل «نفّذ»، «اختبار» بدل «اختبر» ⇒ لم يكن يظهر تلميح للكلمة الصحيحة.
// بعد التصحيح: التلميح يظهر للكلمات الصحيحة من المعجم.
// حارس انحدار: «ذري» (atomic) ليست كلمة في المعجم (مصدر الحقيقة) وكانت تُقترح
// كقصاصة وهمية. بعد الإزالة: يجب ألّا تظهر في الإكمال إطلاقًا.
TEST(LspCompletion, لا_تقترح_كلمة_وهمية_ذري) {
    EngineFixture fx;
    fx.open(u8"دالة س()\n"
            u8"    ذري\n"   // المستخدم يكتب «ذري»
            u8"نهاية\n");
    CompletionList list = fx.engine.completion(fx.uri, Position{1, 7});
    for (const auto& item : list.items)
        ASSERT_TRUE(item.label.find(u8"ذري") == std::string::npos);
}

TEST(LspHover, تلميح_الكلمات_المفتاحية_بمفاتيح_المعجم_الصحيحة) {
    EngineFixture fx;
    // كلّ كلمة مفتاحية صحيحة من مصدر الحقيقة على سطر مستقلّ.
    const char* correct[] = {u8"ارجع", u8"توقف", u8"طابق", u8"نفّذ", u8"اختبر"};
    fx.open(u8"ارجع\n"   // السطر 0
            u8"توقف\n"   // السطر 1
            u8"طابق\n"   // السطر 2
            u8"نفّذ\n"   // السطر 3
            u8"اختبر\n"); // السطر 4

    // (AR) لا يكفي وجود سلسلة «كلمة مفتاحية» العامّة (تظهر لأيّ مفتاح) — نُثبت أنّ
    //      التلميح يخصّ الكلمة الصحيحة بعينها: عنوان hover هو «🔑 كلمة مفتاحية: `الكلمة`».
    //      بهذا لو عاد مفتاح إلى تهجئة خاطئة فلن يطابق العنوان كلمته ⇒ يفشل الحارس.
    for (int line = 0; line <= 4; ++line) {
        auto hv = fx.engine.hover(fx.uri, Position{line, 1});
        ASSERT_TRUE(hv.has_value());
        ASSERT_TRUE(hv->contents.value.find(u8"كلمة مفتاحية") != std::string::npos);
        ASSERT_TRUE(hv->contents.value.find(correct[line]) != std::string::npos);
    }
}

// حارس انحدار سلبيّ: التهجئات القديمة الخاطئة الخمس يجب ألّا تُنتِج تلميح كلمة
// مفتاحية بعد توحيدها على المعجم — وإلّا فالمفتاح الميّت عاد. (لا تُعرَّف هذه
// التهجئات في المحلّل فلا يُلمّحها المحرّك كـ«كلمة مفتاحية».)
TEST(LspHover, لا_تلميح_للتهجئات_الخاطئة_السابقة) {
    EngineFixture fx;
    fx.open(u8"إرجاع\n"   // السطر 0 (خطأ سابق لـ ارجع)
            u8"اخرج\n"    // السطر 1 (خطأ سابق لـ توقف)
            u8"تطابق\n"   // السطر 2 (خطأ سابق لـ طابق)
            u8"تنفيذ\n"   // السطر 3 (خطأ سابق لـ نفّذ)
            u8"اختبار\n"); // السطر 4 (خطأ سابق لـ اختبر)

    for (int line = 0; line <= 4; ++line) {
        auto hv = fx.engine.hover(fx.uri, Position{line, 1});
        // إمّا لا تلميح إطلاقًا، أو تلميح ليس بعنوان «كلمة مفتاحية».
        if (hv.has_value())
            ASSERT_TRUE(hv->contents.value.find(u8"🔑 كلمة مفتاحية") == std::string::npos);
    }
}

TEST(LspDocumentSymbols, يستخرج_الدالة_المعرّفة) {
    EngineFixture fx;
    fx.open(SAMPLE);
    std::vector<DocumentSymbol> syms = fx.engine.document_symbols(fx.uri);
    bool found_func = false;
    for (const auto& s : syms)
        if (s.name.find(u8"اجمع") != std::string::npos) found_func = true;
    ASSERT_TRUE(found_func);
}

TEST(LspHover, لا_ينهار_على_موضع_رمز) {
    EngineFixture fx;
    fx.open(SAMPLE);
    // لا نؤكّد المحتوى (قد يكون nullopt) بل نؤكّد عدم الانهيار وصحّة البنية
    ASSERT_NO_THROW({
        auto h = fx.engine.hover(fx.uri, Position{0, 6});
        if (h.has_value()) (void)h->contents.value;
    });
}

TEST(LspDefinitionReferences, لا_تنهار_وتعيد_بنية_صحيحة) {
    EngineFixture fx;
    fx.open(SAMPLE);
    ASSERT_NO_THROW({
        auto def = fx.engine.definition(fx.uri, Position{1, 4});
        (void)def;
        auto refs = fx.engine.references(fx.uri, Position{0, 6});
        for (const auto& r : refs) { (void)r.uri; (void)r.range; }
    });
}

TEST(LspWorkspaceSymbols, يبحث_عبر_مساحة_العمل) {
    EngineFixture fx;
    fx.open(SAMPLE);
    ASSERT_NO_THROW({
        auto ws = fx.engine.workspace_symbols(u8"اجمع");
        (void)ws;
    });
}

// ══════════════════════════════════════════════════════════════════════════════
//  ④ التحرير والعرض
// ══════════════════════════════════════════════════════════════════════════════

TEST(LspSemanticTokens, البيانات_من_مضاعفات_الخمسة) {
    EngineFixture fx;
    fx.open(SAMPLE);
    SemanticTokensData tok = fx.engine.semantic_tokens_full(fx.uri);
    // ترميز LSP: كلّ رمز = 5 أعداد [Δسطر, Δعمود, طول, نوع, معدّلات]
    ASSERT_EQ(tok.data.size() % 5u, 0u);
}

TEST(LspFolding, يطوي_جسم_الدالة_متعدّد_الأسطر) {
    EngineFixture fx;
    fx.open(SAMPLE);
    std::vector<FoldingRange> folds = fx.engine.folding_ranges(fx.uri);
    // دالة من 4 أسطر ⇒ نطاق طيّ واحد على الأقل
    ASSERT_GT(folds.size(), 0u);
}

TEST(LspRename, تحضير_ثم_إعادة_تسمية_رمز) {
    EngineFixture fx;
    fx.open(SAMPLE);
    ASSERT_NO_THROW({
        auto prep = fx.engine.prepare_rename(fx.uri, Position{1, 4});
        (void)prep;
        auto edit = fx.engine.rename(fx.uri, Position{1, 4}, u8"المجموع");
        (void)edit;
    });
}

TEST(LspCodeActions, لا_تنهار_على_نطاق) {
    EngineFixture fx;
    fx.open(SAMPLE);
    Range r{Position{1, 0}, Position{1, 10}};
    ASSERT_NO_THROW({
        auto actions = fx.engine.code_actions(fx.uri, r, fx.last_diags);
        for (const auto& a : actions) (void)a.title;
    });
}

TEST(LspInlayHints, لا_تنهار_على_نطاق_المستند) {
    EngineFixture fx;
    fx.open(SAMPLE);
    Range r{Position{0, 0}, Position{4, 0}};
    ASSERT_NO_THROW({
        auto hints = fx.engine.inlay_hints(fx.uri, r);
        for (const auto& h : hints) (void)h.label;
    });
}

// حارس انحدار: أسماء الأنواع المستنتَجة مشتقّة من مصدر الحقيقة (types.yaml عبر
// sadTypeKindArabicName)، لا من تهريد منحرف. الخريطة {..} كانت تُعرض «قاموس»
// خطأً بينما اللغة تسمّيها «خريطة». التلميح المضمَّن لمتغيّر مستنتَج = «: <النوع>».
//
// اعتماد مقصود وموثَّق: التلميحات تظهر للمتغيّرات على **المستوى الأعلى** فقط
// (متغيّرات جسم الدالة لا تُفهرَس رموزًا مستقلّة، فلا تلميح لها). لذلك تُكتب
// المتغيّرات هنا في المستوى الأعلى عمدًا — وهذا ثابت سلوكيّ يفرضه inlay_hints.
TEST(LspInlayHints, نوع_الخريطة_من_مصدر_الحقيقة_لا_قاموس) {
    EngineFixture fx;
    // ثلاثة متغيّرات على المستوى الأعلى بأنواع مستنتَجة من حرفيّاتها.
    fx.open(u8"متغير خ = {\"أ\": 1}\n"   // خريطة (كان يُعرض «قاموس» المنحرف)
            u8"متغير ر = 5\n"            // رقم
            u8"متغير ن = \"مرحبا\"\n");  // نص
    Range r{Position{0, 0}, Position{4, 0}};
    auto hints = fx.engine.inlay_hints(fx.uri, r);

    bool saw_map = false;    // إثبات: «خريطة» تظهر
    bool saw_number = false; // إثبات تغطية إضافيّ: «رقم» تظهر
    bool saw_string = false; // إثبات تغطية إضافيّ: «نص» تظهر
    for (const auto& h : hints) {
        if (h.label.find(u8"خريطة") != std::string::npos) saw_map = true;
        if (h.label.find(u8"رقم") != std::string::npos) saw_number = true;
        if (h.label.find(u8"نص") != std::string::npos) saw_string = true;
        // نفي: الاسم المهرَّد المنحرف «قاموس» يجب ألّا يظهر إطلاقًا (مصدر الحقيقة).
        ASSERT_TRUE(h.label.find(u8"قاموس") == std::string::npos);
    }
    ASSERT_TRUE(saw_map);
    ASSERT_TRUE(saw_number);
    ASSERT_TRUE(saw_string);
}

// حارس انحدار (Fix 1): حرفيّة «لاشيء» نوعها الساكن Null (متمايز عن Void)، فيجب
// أن يُستنتَج تلميحها «عدم» من مصدر الحقيقة، لا «فراغ» (Void) ولا «غير_محدد».
// قبل إضافة Null إلى المُحوِّل كانت تسقط إلى «غير_محدد» فلا يظهر تلميح أصلًا.
TEST(LspInlayHints, حرفيّة_العدم_تُستنتَج_عدم_لا_فراغ) {
    EngineFixture fx;
    fx.open(u8"متغير ل = لاشيء\n"); // حرفيّة null ⇒ النوع الساكن Null ⇒ «عدم»
    Range r{Position{0, 0}, Position{2, 0}};
    auto hints = fx.engine.inlay_hints(fx.uri, r);

    bool saw_null = false;
    for (const auto& h : hints) {
        if (h.label.find(u8"عدم") != std::string::npos) saw_null = true;
        // «فراغ» (Void) لا يصحّ لحرفيّة null — نوعان متمايزان في types.yaml.
        ASSERT_TRUE(h.label.find(u8"فراغ") == std::string::npos);
    }
    ASSERT_TRUE(saw_null);
}

// حارس انحدار (Fix 2): دالة إرجاعها «فراغ» (Void) يجب ألّا تعرض تلميح نوع إرجاع.
// قبل توحيد الأسماء كان الحارس يقارن بـ«عدم» المهرَّد؛ بعد التوحيد صار Void اسمه
// «فراغ» فتوقّفت المقارنة عن المطابقة وظهر تلميح « ← فراغ» طفيليّ. الإصلاح يقارن
// باسم Void من المصدر فيُخفي التلميح كما كان القصد.
TEST(LspInlayHints, إرجاع_فراغ_لا_يُظهر_تلميح_نوع) {
    EngineFixture fx;
    fx.open(u8"دالة فراغ سجل(ر) {\n" // نوع الإرجاع يسبق الاسم: دالة <نوع> <اسم>(..)
            u8"  أرجع\n"
            u8"}\n");
    Range r{Position{0, 0}, Position{3, 0}};
    auto hints = fx.engine.inlay_hints(fx.uri, r);

    for (const auto& h : hints) {
        // لا يجوز ظهور «فراغ» في أيّ تلميح نوع إرجاع (Void = لا قيمة تُؤشَّر).
        ASSERT_TRUE(h.label.find(u8"فراغ") == std::string::npos);
    }
}

// حارس انحدار: إكمال الأعضاء بعد النقطة يقترح طرق الأنواع المدمجة المشتقّة من
// مصدر الحقيقة (type_methods.yaml ⇒ ALL_TYPE_METHODS المُولَّد). متغيّر مستنتَج
// «مصفوفة» ⇒ «قائمة.» تقترح «أضف» (push) وغيرها.
TEST(LspCompletion, إكمال_طرق_الأنواع_المدمجة_من_المصدر) {
    EngineFixture fx;
    fx.open(u8"متغير قائمة = [1، 2، 3]\n"
            u8"قائمة.\n");
    // المؤشّر بعد «قائمة.» (5 أحرف + نقطة = العمود 6).
    CompletionList list = fx.engine.completion(fx.uri, Position{1, 6});

    bool saw_add = false;        // «أضف» طريقة مصفوفة من المصدر
    bool saw_string_only = false; // «تحويل_كبير» نصّ-حصرًا: يجب ألّا تتسرّب لمصفوفة
    int  count_add = 0;           // حارس إزالة التكرار: «أضف» مرّة واحدة فقط
    int  count_dup = 0;           // «عد» مكرّرة في المصدر (count/count_alt) ⇒ مرّة واحدة
    for (const auto& item : list.items) {
        if (item.label == u8"أضف") { saw_add = true; ++count_add; }
        if (item.label == u8"تحويل_كبير") saw_string_only = true;
        if (item.label == u8"عد") ++count_dup;
    }
    ASSERT_TRUE(saw_add);
    ASSERT_FALSE(saw_string_only); // الترشيح حسب النوع المستنتَج «مصفوفة»
    // (AR) منع التكرار: مصدر الحقيقة يحوي صفّين «(مصفوفة، عد)» يختلفان بالمرادف
    //      الإنجليزيّ فقط (count/count_alt)؛ يجب ألّا يظهر العنوان مرّتين.
    ASSERT_EQ(count_add, 1);
    ASSERT_EQ(count_dup, 1);
}

// حارس انحدار: الترشيح حسب النوع يعمل لنوع «نص» أيضًا (لا «مصفوفة» فقط). متغيّر
// مستنتَج «نص» ⇒ «تحية.» تقترح «تحويل_كبير» (نصّ-حصرًا) ولا تتسرّب «أضف» (مصفوفة).
TEST(LspCompletion, إكمال_طرق_النوع_نص_من_المصدر) {
    EngineFixture fx;
    fx.open(u8"متغير تحية = \"مرحبا\"\n"
            u8"تحية.\n");
    // المؤشّر بعد «تحية.» (4 أحرف + نقطة = العمود 5، بوحدات UTF-16).
    CompletionList list = fx.engine.completion(fx.uri, Position{1, 5});

    bool saw_upper = false;     // «تحويل_كبير» طريقة نصّ من المصدر
    bool saw_array_only = false; // «أضف» مصفوفة-حصرًا: يجب ألّا تتسرّب لنصّ
    for (const auto& item : list.items) {
        if (item.label == u8"تحويل_كبير") saw_upper = true;
        if (item.label == u8"أضف") saw_array_only = true;
    }
    ASSERT_TRUE(saw_upper);
    ASSERT_FALSE(saw_array_only); // الترشيح حسب النوع المستنتَج «نص»
}

// ══════════════════════════════════════════════════════════════════════════════
//  نقطة الدخول
// ══════════════════════════════════════════════════════════════════════════════

int main(int argc, char** argv) {
    return SAD_RUN_TESTS();
}
