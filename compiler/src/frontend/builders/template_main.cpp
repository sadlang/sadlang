// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include <string>
#include "sir_builder.h"
// (AR) إغلاقُ صادراتِ الوحدة — مشتركٌ مع المفسّر / (EN) shared with the interpreter
#include "module_export_closure.h"
#include "builders/template_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <unordered_set>
#include <functional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            namespace
            {
                // (AR) وصلُ مسارِ الوحدةِ باسمٍ كاملٍ منقوطٍ — نفسُ ما تفعله
                //      getFullModuleName في عُقَد AST، مكرَّرًا هنا لأنّ الجسمَ المشترك
                //      يستقبل الحقولَ لا العقدة.
                // (EN) Join a module path into a dotted full name — the same thing the AST
                //      nodes' getFullModuleName does, needed here because the shared body
                //      receives the fields rather than the node.
                std::string joinModulePath(const std::vector<std::string> &modulePath)
                {
                    return Sad::AST::joinModulePathToFullName(modulePath);
                }

                // (AR) أسماءُ ما تُتيحه الوحدةُ للاستيراد: هي وحدَها ما يجوز تأهيلُه بها.
                //      تشمل ما صدّرته بنفسها **وما أعادت تصديرَه** — فوحدةُ الواجهةِ
                //      التي لا تُعرّف شيئًا بل تُعيد بثَّ غيرها (نمطُ `وحدة_واجهة`)
                //      تُتيح رموزَ مصدرِها فعلًا، والمفسّرُ يقبل تأهيلَها بها. ولو
                //      اقتُصر على عقدتَي التصدير لصار شرطُ الانتماءِ كاذبًا عبرها
                //      فيُرفض ما هو مشروع.
                //      تعاودٌ محروسٌ بـvisitedModulePaths لأنّ إعادةَ التصديرِ قد تدور.
                // (EN) The names a module makes importable — the only ones qualifiable
                //      through it. Includes both its own exports AND what it re-exports: an
                //      interface module that defines nothing and merely re-broadcasts another
                //      (the `وحدة_واجهة` pattern) really does expose its source's symbols, and
                //      the interpreter accepts qualifying them through it. Reading only the
                //      export nodes would make the membership test lie about it and reject
                //      what is legitimate. Recursion is guarded by visitedModulePaths because
                //      re-exports may form cycles.
                void collectExportedNamesFromStatements(const Sad::AST::StmtList &moduleStatements,
                                                        std::set<std::string> &exportedNames)
                {
                    for (const auto &stmt : moduleStatements)
                    {
                        if (!stmt)
                            continue;

                        // (AR) إعادةُ تصديرٍ انتقائيّة: الأسماءُ الفعّالةُ («كـ» إن وُجدت).
                        // (EN) Selective re-export: the effective names (alias when present).
                        if (auto *reExport = dynamic_cast<Sad::AST::ReExportStmt *>(stmt.get()))
                        {
                            if (!reExport->isWildcard)
                                for (const auto &item : reExport->items)
                                    exportedNames.insert(item.getEffectiveName());
                            continue;
                        }

                        const Sad::AST::Statement *declaration = nullptr;
                        if (auto *exportDecl = dynamic_cast<Sad::AST::ExportDecl *>(stmt.get()))
                            declaration = exportDecl->declaration.get();
                        else if (auto *exportStmt = dynamic_cast<Sad::AST::ExportStmt *>(stmt.get()))
                            declaration = exportStmt->declaration.get();
                        if (!declaration)
                            continue;

                        if (auto *funcDecl = dynamic_cast<const Sad::AST::FunctionDecl *>(declaration))
                            exportedNames.insert(funcDecl->name);
                        else if (auto *varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(declaration))
                            exportedNames.insert(varDecl->name);
                        else if (auto *classDecl = dynamic_cast<const Sad::AST::ClassDecl *>(declaration))
                            exportedNames.insert(classDecl->name);
                        else if (auto *structDecl = dynamic_cast<const Sad::AST::StructDecl *>(declaration))
                            exportedNames.insert(structDecl->name);
                    }
                }

                // (AR) حارسُ نطاقِ دورةِ إعادةِ التصدير — يحذف مفتاحَه حتمًا.
                // (EN) Re-export cycle scope guard — unconditionally erases its key.
                class ReExportCycleScope
                {
                public:
                    ReExportCycleScope(std::unordered_set<std::string> &inFlight,
                                       std::string key)
                        : inFlight_(inFlight), key_(std::move(key)) {}

                    ReExportCycleScope(const ReExportCycleScope &) = delete;
                    ReExportCycleScope &operator=(const ReExportCycleScope &) = delete;

                    ~ReExportCycleScope() { inFlight_.erase(key_); }

                private:
                    std::unordered_set<std::string> &inFlight_;
                    std::string key_;
                };

                // ====================================================================
                // (AR) حارسُ نطاقِ الأسماء المستعارة: الاستعارةُ «كـ» ربطُ اسمٍ **خاصٌّ
                //      بالملفّ الذي كتبه**، فخريطةٌ واحدةٌ للبرنامج كلِّه تُسرّب استعارةَ
                //      وحدةٍ إلى مستعمِلها فتختطف أسماءً لم يستوردها (مدمَجًا مثلًا)
                //      بلا تشخيص. فتُبدَّل الخريطةُ بخريطةِ الوحدة قبل بناء أجسامها
                //      وتُستعاد بعده حتمًا — حتّى مع خروجٍ مبكّرٍ أو استثناء.
                // (EN) Import-alias scope guard: «as» is a binding private to the file that
                //      wrote it, so one program-wide map leaks a module's alias into its
                //      consumer and silently hijacks names it never imported. The map is
                //      swapped for the module's own before its bodies are built and restored
                //      unconditionally afterwards — early return or exception included.
                // ====================================================================
                //      وحراسةُ الخريطةِ وحدَها لا تكفي: احتياطيُّ القيمة يُخبّئ ما يحلُّه
                //      في النطاق العامّ **الدائم**، فيعبُر الربطُ حدَّ الوحدة من هناك.
                //      فيُستعاد أثرُ التخبئةِ أيضًا — ما بذَره الاحتياطيُّ في هذا النطاق
                //      وحدَه يُمحى، فلا يُمَسّ عامٌّ مشروعٌ يصادف الاسمَ.
                // (EN) Guarding the map alone is not enough: the value fallback caches what
                //      it resolves in the PERMANENT global scope, from where the binding
                //      crosses the module boundary anyway. The cache's effect is therefore
                //      undone too — only what the fallback seeded within this scope is
                //      erased, never a legitimate global that happens to share the name.
                // ====================================================================
                class ImportAliasScope
                {
                public:
                    // (AR) فضاءاتُ الوحدةِ تُنطَّق مع استعاراتها: كلتاهما ربطُ اسمٍ خاصٌّ
                    //      بالملفّ الذي كتبه، فحارسٌ واحدٌ لهما يمنع افتراقَ عمرَيهما.
                    // (EN) Module namespaces are scoped alongside the aliases: both are
                    //      bindings private to the file that wrote them, so one guard for
                    //      the two keeps their lifetimes from drifting apart.
                    ImportAliasScope(SIRBuilder &builder,
                                     std::unordered_map<std::string, std::string> replacement,
                                     std::unordered_map<std::string, std::string> namespaceReplacement)
                        : builder_(builder),
                          savedAliases_(std::move(builder.importAliases_)),
                          savedSeeded_(std::move(builder.aliasSeededGlobals_)),
                          savedNamespaces_(std::move(builder.moduleNamespaces_))
                    {
                        builder_.importAliases_ = std::move(replacement);
                        builder_.moduleNamespaces_ = std::move(namespaceReplacement);
                        builder_.aliasSeededGlobals_.clear();
                    }

                    ImportAliasScope(const ImportAliasScope &) = delete;
                    ImportAliasScope &operator=(const ImportAliasScope &) = delete;

                    ~ImportAliasScope()
                    {
                        if (!builder_.scopeStack_.empty())
                        {
                            for (const auto &seededName : builder_.aliasSeededGlobals_)
                                builder_.scopeStack_.front().erase(seededName);
                        }
                        builder_.aliasSeededGlobals_ = std::move(savedSeeded_);
                        builder_.importAliases_ = std::move(savedAliases_);
                        builder_.moduleNamespaces_ = std::move(savedNamespaces_);
                    }

                private:
                    SIRBuilder &builder_;
                    std::unordered_map<std::string, std::string> savedAliases_;
                    std::unordered_set<std::string> savedSeeded_;
                    std::unordered_map<std::string, std::string> savedNamespaces_;
                };

                // ====================================================================
                // (AR) حارسُ الملفِّ الجاري بناؤه. المسارُ النسبيُّ يُحَلُّ **بالنسبةِ إلى
                //      الملفِّ الذي كتبه**، وكان الطورُ 2 يُمرِّر `currentFilePath_` — ملفَّ
                //      الجذرِ دائمًا — فوحدةٌ في مجلَّدٍ آخرَ تكتب `.أختها` كان يُبحَث عنها
                //      في مجلَّدِ الجذرِ فلا تُوجَد. والطورُ 1 يفعل الصوابَ أصلًا (ينحدر
                //      بمسارِ الوحدةِ قاعدةً)، فهذا الحارسُ يُلحِق الطورَ 2 به فيتّفقان.
                //      — ISSUE-089-ب، ونظيرُ ما يفعله المفسّرُ في executeModuleAndExtractExports
                // (EN) Guard for the file currently being built. A relative path resolves
                //      RELATIVE TO THE FILE THAT WROTE IT, but Phase 2 passed
                //      `currentFilePath_` — always the root file — so a module in another
                //      directory writing `.sibling` was searched for in the root's directory
                //      and never found. Phase 1 already does the right thing (it descends
                //      with the module's own path as base); this guard brings Phase 2 into
                //      agreement. Mirrors the interpreter's executeModuleAndExtractExports.
                // ====================================================================
                class ModuleFilePathScope
                {
                public:
                    ModuleFilePathScope(SIRBuilder &builder, std::string moduleFilePath)
                        : builder_(builder), savedFilePath_(std::move(builder.currentFilePath_))
                    {
                        builder_.currentFilePath_ = std::move(moduleFilePath);
                    }

                    ModuleFilePathScope(const ModuleFilePathScope &) = delete;
                    ModuleFilePathScope &operator=(const ModuleFilePathScope &) = delete;

                    ~ModuleFilePathScope() { builder_.currentFilePath_ = std::move(savedFilePath_); }

                private:
                    SIRBuilder &builder_;
                    std::string savedFilePath_;
                };

                // ====================================================================
                // (AR) هُويّةُ الوحدةِ بوصفها **موضعًا** لا اسمًا. الاسمُ المطلقُ يُعرّف
                //      وحدتَه وحدَها، أمّا المسارُ النسبيُّ فيعني وحدةً مختلفةً بحسبِ
                //      الملفِّ الذي كتبه — فمفتاحٌ بالاسمِ وحدَه يخلط `.مصدر` في مجلَّدٍ
                //      بـ`.مصدر` في آخر. تُستعمل حيث يلزم **التمييزُ** لا العرض.
                // (EN) The module's identity as a LOCATION, not a name. An absolute name
                //      identifies exactly one module; a relative path means a different
                //      module depending on which file wrote it, so a name-only key conflates
                //      `.src` in one directory with `.src` in another. Used wherever
                //      DISCRIMINATION is needed — never for display.
                // ====================================================================
                std::string moduleIdentityKey(const std::string &writingFilePath,
                                              const std::vector<std::string> &modulePath)
                {
                    const std::string fullName = Sad::AST::joinModulePathToFullName(modulePath);
                    const bool isRelative =
                        !modulePath.empty() &&
                        (modulePath[0] == Sad::AST::kRelativePathPrefixCurrent ||
                         modulePath[0] == Sad::AST::kRelativePathPrefixParent);
                    if (!isRelative)
                        return fullName;
                    return writingFilePath + "::" + fullName;
                }

                // (AR) مسارُ ملفِّ الوحدةِ نصًّا. على ويندوز عبر from_wstring: استدعاءُ
                //      path::string() على اسمٍ عربيٍّ يتعطّل/يرمي خارجَ ترميزِ ANSI.
                // (EN) The module's file path as text. On Windows via from_wstring:
                //      path::string() on an Arabic name hangs/throws outside ANSI.
                std::string moduleFilePathAsText(const Modules::Module *module)
                {
#ifdef _WIN32
                    return sad::utf8::from_wstring(module->filePath.wstring());
#else
                    return module->filePath.string();
#endif
                }

                // ====================================================================
                // (AR) قائمة موحّدة لوحدات المكتبة القياسية التي يعترضها المترجم
                //      مباشرةً ويعامل دوالها كـ builtins بدون تحليل ملف .ص الخارجي.
                //      السبب الجذري لهذا الاستخراج هو منع انجراف قائمتين منفصلتين بين
                //      buildImportStmt و buildFromImportStmt، لأن أي اختلاف بينهما يولّد
                //      سلوكاً متناقضاً: استيراد كامل ينجح بينما الاستيراد الانتقائي من
                //      نفس الوحدة قد يحاول تحميل ملف wrapper خارجي ويؤدي إلى أخطاء ربط.
                //
                // (EN) Unified set of stdlib modules intercepted directly by the
                //      compiler and treated as builtins without parsing external .ص
                //      wrapper files. This shared helper prevents drift between the
                //      regular import and selective from-import paths.
                // ====================================================================
                const std::unordered_set<std::string> &getCompilerBuiltinStdlibModules()
                {
                    static const std::unordered_set<std::string> modules = {
                        "رياضيات", "math", "حساب",
                        "نصوص", "strings", "str", "سلاسل",
                        "أساسيات", "basics", "base", "ملفات",
                        "خرائط", "maps", "json", "xml",
                        "وقت", "time", "تاريخ",
                        "تأكيدات", "assertions", "اختبار",
                        "تزامن_متقدم", "async", "توازي",
                        "منصة", "platform", "نظام_تشغيل",
                        "شبكة", "شبكة_عالية", "http", "اتصالات",
                        "تشفير", "crypto", "أمان_بيانات",
                        "مقابس", "sockets", "tcp", "udp",
                        "ويبسوكت", "websocket", "ws",
                        // (AR) وحدة مدمجات النواة (المفسّر يسجّلها بلا أسماء بديلة) —
                        //      البتّيّات المولَّدة تعليمات LLVM تتطلّب اعتراضها هنا.
                        "نظام_منخفض",
                        // (AR) وحدة «عمليات» — بدائيّاتُ التشغيل تُولَّدُ في الخلفيّة
                        //      (‏processes_builtins_ops.cpp) فلا ملفَّ غلافٍ يُحلّل.
                        //      اسمٌ واحدٌ بلا بدائلَ في MODULE_ALIASES.
                        //
                        //      ⚠️ هذه القائمةُ مكتوبةٌ باليدِ وتوازي MODULE_ALIASES في
                        //      module_definitions.h — نظيرُ الانجرافِ الذي حُذف من
                        //      بوّابةِ الاستيراد (sir_builder_module_check.cpp). أثرُه
                        //      المقيسُ هنا: وحدةٌ مدمجاتُها منفَّذةٌ في المصرِّفِ تمامًا
                        //      تُرفَضُ برسالةِ «لم يُعثر على الوحدة»، أي تشخيصٌ يشير إلى
                        //      ملفٍّ ناقصٍ بينما العلّةُ سطرٌ في قائمةٍ يدويّة. توحيدُها
                        //      بالسجلِّ المركزيّ يغيّرُ سلوكَ وحداتٍ تحتاج ملفَّ غلافٍ
                        //      فعلًا، فهو قرارُ مالكٍ لا تنظيفٌ عابر — دَينٌ معلَن.
                        "عمليات"};
                    return modules;
                }

                bool isCompilerBuiltinStdlibModule(const std::string &moduleName)
                {
                    return getCompilerBuiltinStdlibModules().count(moduleName) > 0;
                }
            }

            // ============================================================================
            // بناء دالة قالب / Build Template Function
            // ============================================================================
            // Purpose (AR): حفظ دالة قالب للإنشاء لاحقاً عند الاستدعاء مع أنواع محددة
            // Purpose (EN): Store template function for later instantiation with specific types
            //
            // التعريف / Source: declarations.h:470 (TemplateFunctionDecl)
            // التوقيع / Signature: void buildTemplateFunction(AST::TemplateFunctionDecl*)
            //
            // TemplateFunctionDecl Members:
            // - typeParameters: std::vector<TypeParameter> (line 472)
            // - name: std::string (line 473)
            // - parameters: std::vector<Parameter> (line 474)
            // - returnType: Types::SadTypeKind (line 475)
            // - body: StmtPtr (line 476)
            // ============================================================================
            void TemplateBuilder::buildTemplateFunction(AST::TemplateFunctionDecl *templateDecl)
            {
                if (!templateDecl)
                {
                    return;
                }

                std::cout << "[Template] Registering template function: " << templateDecl->name << std::endl;
                std::cout << "[Template] Type parameters: ";
                for (const auto &typeParam : templateDecl->typeParameters)
                {
                    std::cout << typeParam.name << " ";
                }
                std::cout << std::endl;

                // (AR) حفظ دالة القالب في الجدول
                // (EN) Store template function in table
                b_.templateFunctions_[templateDecl->name] = templateDecl;

                std::cout << "[Template] Template function '" << templateDecl->name
                          << "' stored for later instantiation" << std::endl;
            }

            // ============================================================================
            // إنشاء نسخة محددة من دالة قالب / Instantiate Template Function
            // ============================================================================
            // Purpose (AR): إنشاء نسخة محددة من دالة القالب باستبدال معاملات الأنواع
            // Purpose (EN): Create concrete instance of template function with type substitution
            //
            // Example:
            // - Template: قالب<نوع ت> دالة أكبر(ت أ، ت ب) ت
            // - Call: أكبر<رقم>(5, 10)
            // - Instantiation: أكبر_i64(رقم أ، رقم ب) رقم
            // ============================================================================
            std::string TemplateBuilder::instantiateTemplate(const std::string &templateName,
                                                        const std::vector<SadTypeKind> &typeArguments)
            {
                // (AR) [Phase 4] واجهة قديمة — تفويض للنسخة الجديدة بدون const args
                // (EN) [Phase 4] Legacy overload — delegate to new version with no const args
                return instantiateTemplate(templateName, typeArguments, std::vector<SIROperand>{});
            }

            // ============================================================================
            // (AR) [Phase 4 — Monomorphization] إنشاء نسخة قالب مع const-generics
            // (EN) [Phase 4 — Monomorphization] Template instantiation with const-generics
            // ============================================================================
            std::string TemplateBuilder::instantiateTemplate(const std::string &templateName,
                                                        const std::vector<SadTypeKind> &typeArguments,
                                                        const std::vector<SIROperand> &constArguments)
            {
                std::cout << "[Template] Instantiating template: " << templateName << std::endl;

                // (AR) البحث عن دالة القالب
                // (EN) Find template function
                auto it = b_.templateFunctions_.find(templateName);
                if (it == b_.templateFunctions_.end())
                {
                    // (AR) تسجيل الخطأ في قائمة الأخطاء بدلاً من stderr فقط
                    // (EN) Record error in error list instead of only stderr
                    std::string errMsg = "دالة القالب غير موجودة / Template function not found: '" + templateName + "'";
                    std::cerr << "[Template Error] " << errMsg << std::endl;
                    b_.errors_.push_back(errMsg);
                    return "";
                }

                Sad::AST::TemplateFunctionDecl *templateDecl = it->second;

                // ================================================================
                // (AR) [Phase 4] فصل معاملات typename عن معاملات const
                //      الترتيب الأصلي محفوظ في templateDecl->typeParameters
                //      مع isConst لكل معامل. نُنشئ خرائط منفصلة:
                //        - typeParamNames: أسماء معاملات النوع فقط
                //        - constParamNames: أسماء معاملات الثابت فقط
                //      ثم نتحقق أن عدد typeArguments == |typeParams|
                //      وعدد constArguments == |constParams|.
                // ================================================================
                std::vector<std::string> typeParamNames;
                std::vector<std::string> constParamNames;
                for (const auto &p : templateDecl->typeParameters)
                {
                    if (p.isConst)
                        constParamNames.push_back(p.name);
                    else
                        typeParamNames.push_back(p.name);
                }

                // (AR) التحقق من عدد معاملات الأنواع
                // (EN) Check type argument count
                if (typeArguments.size() != typeParamNames.size())
                {
                    std::string errMsg = "عدد معاملات الأنواع غير متطابق / Type argument count mismatch for '" + templateName + "': expected " + std::to_string(typeParamNames.size()) + ", got " + std::to_string(typeArguments.size());
                    std::cerr << "[Template Error] " << errMsg << std::endl;
                    b_.errors_.push_back(errMsg);
                    return "";
                }

                // (AR) [Phase 4] التحقق من عدد المعاملات الثابتة
                // (EN) [Phase 4] Check const-arg count
                if (constArguments.size() != constParamNames.size())
                {
                    std::string errMsg = "عدد معاملات الثوابت غير متطابق / Const-arg count mismatch for '" + templateName + "': expected " + std::to_string(constParamNames.size()) + ", got " + std::to_string(constArguments.size());
                    std::cerr << "[Template Error] " << errMsg << std::endl;
                    b_.errors_.push_back(errMsg);
                    return "";
                }

                // (AR) إنشاء اسم الدالة المُنشأة: اسم_القالب_نوع1_نوع2
                // (EN) Create instantiated function name: template_name_type1_type2
                std::string instantiatedName = templateName;
                for (const auto &type : typeArguments)
                {
                    instantiatedName += "_";
                    switch (type)
                    {
                    case SadTypeKind::Integer:
                        instantiatedName += "i64";
                        break;
                    case SadTypeKind::Float:
                        instantiatedName += "f64";
                        break;
                    case SadTypeKind::String:
                        instantiatedName += "str";
                        break;
                    case SadTypeKind::Boolean:
                        instantiatedName += "bool";
                        break;
                    default:
                        instantiatedName += "obj";
                        break;
                    }
                }

                // ================================================================
                // (AR) [Phase 4] دمج قيم الثوابت في اسم النسخة لضمان نسخة فريدة
                //      لكل تركيبة (T، N). مثال: f<عشري،4> → f_f64_c4
                //      نستخدم بادئة "_c" قبل القيمة لتفادي الالتباس مع أسماء الأنواع.
                // (EN) [Phase 4] Embed const-arg values in instantiation name to
                //      keep specializations unique per (T,N). Prefix "_c" prevents
                //      collision with type-name suffixes.
                // ================================================================
                for (const auto &cArg : constArguments)
                {
                    instantiatedName += "_c";
                    switch (cArg.dataType)
                    {
                    case SadTypeKind::Integer:
                        instantiatedName += std::to_string(cArg.intValue);
                        break;
                    case SadTypeKind::Float:
                    {
                        // (AR) نُحوّل العشري لتمثيل آمن للأسماء (نقطة → s)
                        std::string s = std::to_string(cArg.floatValue);
                        for (auto &ch : s)
                            if (ch == '.' || ch == '-')
                                ch = '_';
                        instantiatedName += s;
                        break;
                    }
                    case SadTypeKind::Boolean:
                        instantiatedName += cArg.boolValue ? "T" : "F";
                        break;
                    case SadTypeKind::String:
                        // (AR) للنصوص نستخدم بصمة بسيطة من الطول والـhash لتفادي رموز خاصة
                        instantiatedName += "s" + std::to_string(cArg.name.size()) +
                                            "_" + std::to_string(std::hash<std::string>{}(cArg.name) & 0xFFFF);
                        break;
                    default:
                        instantiatedName += "x";
                        break;
                    }
                }

                std::cout << "[Template] Instantiated name: " << instantiatedName << std::endl;

                // (AR) التحقق من وجود النسخة بالفعل
                // (EN) Check if already instantiated
                if (b_.instantiatedTemplates_.find(instantiatedName) != b_.instantiatedTemplates_.end())
                {
                    std::cout << "[Template] Already instantiated, reusing" << std::endl;
                    return instantiatedName;
                }

                // (AR) إنشاء جدول استبدال الأنواع
                // (EN) Create type substitution map
                std::unordered_map<std::string, SadTypeKind> typeSubstitutions;
                for (size_t i = 0; i < typeParamNames.size(); ++i)
                {
                    typeSubstitutions[typeParamNames[i]] = typeArguments[i];
                    std::cout << "[Template] Substitution: " << typeParamNames[i]
                              << " -> " << static_cast<int>(typeArguments[i]) << std::endl;
                }

                // ================================================================
                // (AR) [Phase 4] جدول استبدال الثوابت — يُستخدم في b_.buildVariableAccess
                //      لاعتراض أي مرجع لمعامل ثابت وإصدار قيمة ثابتة فوراً.
                // (EN) [Phase 4] Const substitution table — consumed by
                //      b_.buildVariableAccess to short-circuit any reference to a
                //      const-generic param into an immediate Constant operand.
                // ================================================================
                std::unordered_map<std::string, SIROperand> constSubstitutions;
                for (size_t i = 0; i < constParamNames.size(); ++i)
                {
                    constSubstitutions[constParamNames[i]] = constArguments[i];
                    std::cout << "[Template] Const substitution: " << constParamNames[i] << std::endl;
                }

                // (AR) استبدال نوع الإرجاع
                // (EN) Substitute return type
                SadTypeKind returnType = b_.astTypeToSIRType(templateDecl->returnType);

                // (AR) إذا كان نوع الإرجاع OBJECT نستخدم أول استبدال نوع متاح كحل عملي
                // (EN) If return type is OBJECT, use the first available type substitution pragmatically
                if (templateDecl->returnType == Types::SadTypeKind::Class &&
                    !typeParamNames.empty())
                {
                    auto it = typeSubstitutions.find(typeParamNames.front());
                    if (it != typeSubstitutions.end())
                    {
                        returnType = it->second;
                    }
                }

                // ================================================================
                // (AR) [Phase 4] إذا لم يُعلَن نوع إرجاع (NONE → Void) لكن للقالب
                //      معاملات أنواع أو ثوابت، نستنتج نوع الإرجاع من أول بديل متاح:
                //      1) أول const param (إذا وُجد) — ارجع N → نوع N
                //      2) أول type param — ارجع قيمة من نوع T → نوع T
                //      هذا حل عملي شامل لدوال القوالب غير المُعلَنة العودة.
                // (EN) [Phase 4] If no return type declared (NONE → Void) but the
                //      template has type/const params, infer return type from the
                //      first available substitution:
                //      1) first const param (if any) — `return N` → type of N
                //      2) first type param — `return val:T` → T
                //      Pragmatic fallback for templates without explicit return type.
                // ================================================================
                if (returnType == SadTypeKind::Void)
                {
                    if (!constParamNames.empty())
                    {
                        auto cIt = constSubstitutions.find(constParamNames.front());
                        if (cIt != constSubstitutions.end())
                            returnType = cIt->second.dataType;
                    }
                    else if (!typeParamNames.empty())
                    {
                        auto tIt = typeSubstitutions.find(typeParamNames.front());
                        if (tIt != typeSubstitutions.end())
                            returnType = tIt->second;
                    }
                }

                // (AR) إنشاء دالة SIR جديدة
                // (EN) Create new SIR function
                auto sirFunction = std::make_shared<SIRFunction>(instantiatedName, returnType);

                // (AR) استبدال أنواع المعاملات
                // (EN) Substitute parameter types
                for (const auto &param : templateDecl->parameters)
                {
                    SadTypeKind paramType = b_.astTypeToSIRType(param.type);

                    // (AR) إذا كان نوع المعامل هو OBJECT، قد يكون معامل قالب
                    // (EN) If parameter type is OBJECT, might be template parameter
                    if (param.type == Types::SadTypeKind::Class)
                    {
                        // (AR) البحث في جدول الاستبدال باسم النوع المعلن (param.typeName)
                        // (EN) Look up type name in substitution table (param.typeName)
                        if (!param.typeName.empty())
                        {
                            auto substIt = typeSubstitutions.find(param.typeName);
                            if (substIt != typeSubstitutions.end())
                            {
                                paramType = substIt->second;
                            }
                        }
                        else
                        {
                            // (AR) افتراضي: البحث بترتيب معاملات القالب حسب ترتيب المعاملات
                            // (EN) Fallback: try matching by index using type param names
                            for (size_t ti = 0; ti < templateDecl->typeParameters.size(); ++ti)
                            {
                                auto substIt = typeSubstitutions.find(templateDecl->typeParameters[ti].name);
                                if (substIt != typeSubstitutions.end())
                                {
                                    paramType = substIt->second;
                                    break;
                                }
                            }
                        }
                    }

                    sirFunction->addParameter(Sad::Compiler::SIR::SIRParameter(param.name, paramType));
                }

                // (AR) بناء جسم الدالة مع استبدال الأنواع
                // (EN) Build function body with type substitution
                //
                // (AR) [إصلاح علة codegen — defer state leak]
                //      نستخدم b_.saveContext()/b_.restoreContext() بدلاً من حفظ
                //      b_.currentFunction_/b_.currentBlock_ يدوياً. السبب الجذري: defer
                //      infrastructure (b_.currentDeferStackReg_, b_.currentDeferExecutedFlagReg_,
                //      b_.deferredStatements_) كانت تتسرّب من الدالة الخارجية إلى دالة
                //      القالب instantiated، فعند ترجمة `ارجع` كان codegen يولّد
                //      تعليمات تشير إلى سجلات defer من نطاق آخر → "Undefined register"
                //      وتعطل وقت تشغيل. b_.saveContext يصفّر الحالة بالكامل.
                // (EN) [Codegen bug fix — defer state leak]
                //      Use b_.saveContext()/b_.restoreContext() instead of manually saving
                //      b_.currentFunction_/b_.currentBlock_. Root cause: defer infrastructure
                //      (defer stack/done regs, deferred stmts) leaked from outer
                //      function into instantiated template, causing return codegen
                //      to emit refs to defer registers from a different scope →
                //      "Undefined register" + runtime crash. b_.saveContext fully resets.
                auto savedCtx = b_.saveContext();

                b_.currentFunction_ = sirFunction;

                // (AR) إنشاء نطاق جديد
                // (EN) Create new scope
                b_.enterScope();

                // (AR) إضافة استبدالات الأنواع للنطاق الحالي (للاستخدام عند بناء الجسم)
                // (EN) Add type substitutions to current scope
                GenericScope genericScope;

                // (AR) تحويل typeParameters من AST إلى SIR
                // (EN) Convert typeParameters from AST to SIR
                for (const auto &astParam : templateDecl->typeParameters)
                {
                    TypeParameter sirParam(astParam.name);
                    genericScope.typeParameters.push_back(sirParam);
                }

                genericScope.typeSubstitutions = typeSubstitutions;
                // (AR) [Phase 4] دفع جدول الثوابت لاستخدامه أثناء بناء الجسم
                // (EN) [Phase 4] Push const substitutions for use during body building
                genericScope.constSubstitutions = constSubstitutions;
                b_.genericScopeStack_.push_back(genericScope);

                // (AR) إنشاء basic block للدخول
                // (EN) Create entry basic block
                auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                sirFunction->addBasicBlock(entryBlock);
                b_.currentBlock_ = entryBlock;

                // ================================================================
                // (AR) [إصلاح علة Phase 5] تسجيل معاملات الدالة كمتغيرات محلية
                //      كي يتمكن جسم القالب من الوصول إليها بالاسم.
                //      السبب الجذري للعلة: دوال القوالب instantiated كانت تنشأ
                //      بدون تسجيل معاملاتها في VariableManager، فعند ترجمة
                //      `ارجع أ` لا يُعثر على المتغير `أ` فيُولَّد كود قمامة
                //      (defer iterating over null array، ret 0 ثابت).
                //      الحل: نسخ النمط من sir_builder_statements_main.cpp:184
                // (EN) [Phase 5 Bug Fix] Register template function parameters
                //      as local variables so the template body can resolve them
                //      by name. Root cause: instantiated template functions were
                //      created without registering parameters in VariableManager,
                //      so `return أ` couldn't find `أ` and produced garbage IR
                //      (defer iterating null array, constant ret 0).
                //      Fix: mirror pattern from sir_builder_statements_main.cpp
                // ================================================================
                {
                    const auto &sirParams = sirFunction->getParameters();
                    for (size_t i = 0; i < sirParams.size(); ++i)
                    {
                        VariableInfo paramVar;
                        paramVar.name = sirParams[i].name;
                        paramVar.type = sirParams[i].type;
                        paramVar.registerName = "%" + sirParams[i].name;
                        paramVar.isMutable = true;
                        paramVar.isParameter = true;
                        paramVar.scopeLevel = b_.currentScopeLevel_;
                        b_.addVariable(paramVar);
                    }
                }

                // (AR) بناء جسم الدالة
                // (EN) Build function body
                if (templateDecl->body)
                {
                    b_.buildStatement(templateDecl->body.get());
                }

                // (AR) إزالة نطاق الأنواع العامة
                // (EN) Remove generic scope
                b_.genericScopeStack_.pop_back();

                // (AR) الخروج من النطاق
                // (EN) Exit scope
                b_.exitScope();

                // (AR) التأكد من وجود terminator
                // (EN) Ensure terminator exists
                if (b_.currentBlock_ && !b_.currentBlock_->instructions.empty())
                {
                    const auto &lastInst = b_.currentBlock_->instructions.back();
                    if (lastInst.opcode != SIROpcode::RET && lastInst.opcode != SIROpcode::RET_VOID)
                    {
                        if (returnType == SadTypeKind::Void)
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET_VOID;
                            b_.currentBlock_->addInstruction(retInst);
                        }
                        else
                        {
                            SIRInstruction retInst;
                            retInst.opcode = SIROpcode::RET;
                            retInst.operands.push_back(SIROperand::ConstantI64(0));
                            b_.currentBlock_->addInstruction(retInst);
                        }
                    }
                }

                // (AR) استعادة السياق
                // (EN) Restore context
                b_.restoreContext(std::move(savedCtx));

                // (AR) حفظ الدالة المُنشأة
                // (EN) Store instantiated function
                b_.instantiatedTemplates_[instantiatedName] = sirFunction;
                b_.module_->addFunction(sirFunction);

                // (AR) إضافة الدالة المُنشأة إلى جدول الدوال
                // (EN) Add instantiated function to function table
                FunctionInfo funcInfo;
                funcInfo.name = instantiatedName;
                funcInfo.returnType = returnType;
                funcInfo.parameters = sirFunction->getParameters();
                funcInfo.sirFunction = sirFunction;
                b_.functionTable_[instantiatedName] = funcInfo;

                std::cout << "[Template] Instantiation complete: " << instantiatedName << std::endl;

                return instantiatedName;
            }

            // ============================================================================
            // (AR) نظام الاستيراد والتصدير في المترجم
            // (EN) Import/Export System in Compiler
            // ============================================================================

            /**
             * @brief (AR) تعيين مسار الملف الحالي - يُستخدم لحل مسارات الاستيراد النسبية
             * @brief (EN) Set current file path - used for resolving relative import paths
             */
            void TemplateBuilder::setCurrentFilePath(const std::string &filePath)
            {
                b_.currentFilePath_ = filePath;

                // (AR) إنشاء محلل الوحدات إذا لم يكن موجوداً
                // (EN) Create module resolver if it doesn't exist
                if (!b_.moduleResolver_)
                {
                    b_.moduleResolver_ = std::make_unique<Modules::ModuleResolver>();

                    // (AR) إضافة مجلد الملف الحالي كمسار بحث
                    // (EN) Add current file's directory as search path
                    if (!filePath.empty())
                    {
                        try
                        {
                            auto fsPath = sad::utf8::make_path(filePath);
                            auto parentDir = fsPath.parent_path();
                            if (!parentDir.empty() && std::filesystem::exists(parentDir))
                            {
                                // (AR) نستخدم filesystem::path مباشرة لتجنب مشاكل ترميز UTF-8 مع ANSI
                                // (EN) Push filesystem::path directly to avoid UTF-8/ANSI encoding issues
                                b_.moduleResolver_->addSearchPathDirect(parentDir);
                            }
                        }
                        catch (const std::exception &e)
                        {
                            // (AR) تجاهل الأخطاء - مسارات البحث الافتراضية كافية
                            // (EN) Ignore errors - default search paths are sufficient
                            (void)e;
                        }
                    }
                }
            }

            /**
             * @brief (AR) معالجة وحدة مستوردة - تحليل ملف الوحدة وبناء دوالها
             * @brief (EN) Process imported module - parse module file and build its functions
             *
             * @details
             * (AR) آلية العمل:
             *      1. استخدام ModuleResolver للعثور على ملف الوحدة
             *      2. تحليل AST الوحدة
             *      3. استخراج الدوال المُصدَّرة
             *      4. بناء SIR لكل دالة مُصدَّرة وإضافتها للوحدة الحالية
             *
             * (EN) How it works:
             *      1. Use ModuleResolver to find module file
             *      2. Parse module AST
             *      3. Extract exported functions
             *      4. Build SIR for each exported function and add to current module
             */
            void TemplateBuilder::buildImportStmt(AST::ImportStmt *importStmt)
            {
                if (!importStmt)
                    return;

                // (AR) التحقق من وجود محلل الوحدات
                // (EN) Check module resolver exists
                if (!b_.moduleResolver_)
                {
                    b_.moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
                }

                std::string fullModuleName = importStmt->getFullModuleName();

                // (AR) التحقق مما إذا تمت معالجة الوحدة بالفعل
                // (EN) Check if module was already processed
                if (b_.processedModules_.count(fullModuleName))
                {
                    return;
                }

                // ====================================================================
                // (AR) اعتراض وحدات المكتبة القياسية — الدوال المضمنة مدعومة مباشرة
                //      في المترجم بدون الحاجة لتحليل ملف .ص الخارجي.
                //      عند استيراد وحدة قياسية، نُعلّمها كمعالجة ونعود فوراً —
                //      لأن جميع دوالها معروفة للمترجم كدوال مضمنة (builtins).
                // (EN) Stdlib module intercept — builtin functions are supported directly
                //      in the compiler without needing to parse the external .ص file.
                //      When importing a stdlib module, mark it as processed and return —
                //      all its functions are known to the compiler as builtins.
                // ====================================================================
                if (isCompilerBuiltinStdlibModule(fullModuleName))
                {
                    // (AR) تسجيل الوحدة كمعالجة — دوالها مضمنة في المترجم
                    // (EN) Mark module as processed — its functions are compiler builtins
                    b_.processedModules_.insert(fullModuleName);
                    return;
                }

                // (AR) إصدار تعليمة تحميل الوحدة في SIR
                // (EN) Emit MODULE_LOAD SIR instruction for linker tracking
                if (b_.currentBlock_)
                {
                    std::string moduleReg = b_.newTempRegister();
                    SIRInstruction loadInst(SIROpcode::MODULE_LOAD);
                    loadInst.result = SIROperand::Register(moduleReg, SadTypeKind::Integer);
                    loadInst.operands.push_back(SIROperand::ConstantString(fullModuleName));
                    loadInst.comment = "استيراد وحدة / import module: " + fullModuleName;
                    b_.currentBlock_->addInstruction(loadInst);

                    // (AR) إصدار تعليمة تهيئة الوحدة
                    // (EN) Emit MODULE_INIT instruction
                    SIRInstruction initInst(SIROpcode::MODULE_INIT);
                    initInst.operands.push_back(SIROperand::Register(moduleReg, SadTypeKind::Integer));
                    initInst.comment = "تهيئة وحدة / init module: " + fullModuleName;
                    b_.currentBlock_->addInstruction(initInst);
                }

                // (AR) تحميل الوحدة
                // (EN) Load module
                Modules::Module *module = b_.moduleResolver_->resolveModule(
                    importStmt->modulePath,
                    b_.currentFilePath_);

                if (!module)
                {
                    b_.errors_.push_back(
                        "خطأ: لم يُعثر على الوحدة '" + fullModuleName + "' / "
                                                                       "Error: Module '" +
                        fullModuleName + "' not found");
                    return;
                }

                // (AR) تمييز الوحدة كمعالجة
                // (EN) Mark module as processed
                b_.processedModules_.insert(fullModuleName);

                // ════════════════════════════════════════════════════════════════
                // (AR) المرحلة الأولى للوحدة: تسجيل تواقيع دوالّها **قبل** بناء أيّ
                //      جسم. نفسُ التمريرة التي يمرّ بها ملفُّ الدخول، لا نسخةٌ منها —
                //      فيستحيل أن يتباعد المساران ثانيةً.
                //
                //      بدونها كانت أجسامُ الوحدة تُبنى بترتيب السطور، فالنداءُ الأماميّ
                //      («أ» تنادي «ب» المعرَّفةَ بعدها) يفشل بـ«استدعاء دالة غير معرّفة»
                //      وتظهر رسالةٌ مضلِّلة عن الشرط لا عن سببه. والوحدةُ نفسُها تُصرَّف
                //      بلا خطأٍ حين تكون ملفَّ الدخول (#333).
                // (EN) The module's Phase 1: register its function signatures BEFORE
                //      building any body — the same pass the entry file runs, not a copy,
                //      so the two paths cannot diverge again. Without it, module bodies
                //      were built in source order and a forward call failed (#333).
                // ════════════════════════════════════════════════════════════════
                // (AR) `استورد م` يستورد كلَّ المُصدَّر؛ فالإغلاقُ يُحسَب شاملًا (isWildcard)،
                //      ولا يُبنى من الخاصّ إلّا ما تبلغه المُصدَّرات.
                // (EN) Plain `import m` imports every export, so the closure is computed in
                //      wildcard mode; private declarations are built only when the exports
                //      actually reach them.
                const std::set<std::string> plainImportPrivateClosure =
                    computeImportedPrivateClosure(module->ast, {}, /*isWildcard=*/true);

                // (AR) لقطةُ الجدول قبل التسجيل: تميّز ما أضافته هذه التمريرةُ عمّا كان
                //      موجودًا (محلّيًّا أو من وحدةٍ سابقة) كي لا يمسّ التشذيبُ إلّا بذورَها.
                // (EN) Table snapshot before registration: distinguishes what THIS pass adds
                //      from what already existed, so pruning touches only its own seeds.
                std::unordered_set<std::string> tableBeforePreRegister;
                tableBeforePreRegister.reserve(b_.functionTable_.size());
                for (const auto &entry : b_.functionTable_)
                    tableBeforePreRegister.insert(entry.first);

                b_.preRegisterFunctionSignatures(&module->ast);

                // ════════════════════════════════════════════════════════════════
                // (AR) تشذيبُ الخاصِّ غيرِ المبلوغ — سدُّ فجوةٍ بين تمريرتين.
                //
                //      التمريرةُ أعلاه تسجّل تواقيعَ **كلِّ** دوالِّ الوحدة (وهو لازمٌ
                //      للنداء الأماميّ داخلها، #333)، بينما حلقةُ البناء أدناه ترفض
                //      بناءَ الخاصِّ الذي لا تبلغه المُصدَّرات. فالنتيجةُ كانت: الاسمُ
                //      محلولٌ في جدول الدوالّ ⇒ يُبعَث نداءٌ مباشرٌ إليه من الملفّ
                //      المستورِد، وجسمُه لا يُبعَث قطُّ ⇒ يسقط **الرابط** برمزٍ غيرِ
                //      معرَّفٍ برسالةٍ إنجليزيّةٍ من lld بدل تشخيصٍ دلاليٍّ عربيّ.
                //
                //      المرجعُ الدلاليّ: «صدّر» تُتيح التصريحَ للاستيراد من وحدةٍ أخرى
                //      (grammar/20_declarations.yaml:gr.decl.export) ⇒ غيرُ المُصدَّرِ
                //      غيرُ مرئيٍّ للمستورِد. فحذفُ بذرتِه هو الدلالةُ الصحيحة، وأثرُه
                //      أنّ النداءَ يُشخَّص «استدعاء دالة غير معرّفة» في موضعه.
                // (EN) Prune unreachable private declarations — a gap between two passes.
                //      The pass above registers EVERY module function signature (required for
                //      intra-module forward calls, #333), while the build loop below refuses to
                //      build private declarations the exports cannot reach. Net effect: the name
                //      resolved in the function table ⇒ a direct call was emitted from the
                //      importing file, its body never was ⇒ the LINKER failed with an English
                //      undefined-symbol message instead of an Arabic semantic diagnostic.
                //      Semantics per SoT: «صدّر» is what makes a declaration importable, so a
                //      non-exported one is invisible to the importer — dropping the seed is the
                //      correct meaning, and the call is now diagnosed at its own call site.
                // ════════════════════════════════════════════════════════════════
                for (const auto &pruneStmt : module->ast)
                {
                    if (!pruneStmt)
                        continue;
                    // (AR) الخاصُّ وحدَه: المُصدَّر (ExportDecl/ExportStmt) لا يُلتقط هنا أصلًا.
                    // (EN) Private only: an exported declaration is not matched here at all.
                    auto *privateFunc = dynamic_cast<AST::FunctionDecl *>(pruneStmt.get());
                    if (!privateFunc)
                        continue;
                    if (plainImportPrivateClosure.find(privateFunc->name) !=
                        plainImportPrivateClosure.end())
                        continue;
                    if (tableBeforePreRegister.find(privateFunc->name) !=
                        tableBeforePreRegister.end())
                        continue;
                    b_.functionTable_.erase(privateFunc->name);
                    b_.preRegisteredImportNames_.erase(privateFunc->name);
                }

                // (AR) أجسامُ الوحدة تُبنى باستعاراتِ الوحدة وحدَها لا باستعاراتِ مستورِدها.
                // (EN) The module's bodies are built with the module's own aliases only.
                std::unordered_map<std::string, std::string> moduleImportAliases;
                std::unordered_map<std::string, std::string> moduleOwnNamespaces;
                collectFileImportBindings(module->ast, moduleImportAliases, moduleOwnNamespaces);
                ImportAliasScope moduleAliasScope(b_, std::move(moduleImportAliases),
                                                  std::move(moduleOwnNamespaces));

                // (AR) واستيراداتُها النسبيّةُ تُحَلُّ من مجلَّدِها هي — ISSUE-089-ب
                // (EN) And its relative imports resolve from its own directory
                ModuleFilePathScope moduleFileScope(b_, moduleFilePathAsText(module));

                // (AR) معالجة كل تصريح في الوحدة
                // (EN) Process each declaration in module
                for (const auto &stmt : module->ast)
                {
                    if (!stmt)
                        continue;

                    // (AR) معالجة عبارات الاستيراد الداخلية (استيراد متداخل/عابر)
                    // (EN) Process internal import statements (transitive/nested imports)
                    if (auto innerFromImport = dynamic_cast<AST::FromImportStmt *>(stmt.get()))
                    {
                        buildFromImportStmt(innerFromImport);
                        continue;
                    }
                    if (auto innerImport = dynamic_cast<AST::ImportStmt *>(stmt.get()))
                    {
                        buildImportStmt(innerImport);
                        continue;
                    }
                    // (AR) إعادةُ تصديرٍ داخلَ الوحدة تُتبَع إلى مصدرِها — ISSUE-089.
                    // (EN) A re-export inside the module is followed to its source — ISSUE-089.
                    if (auto innerReExport = dynamic_cast<Sad::AST::ReExportStmt *>(stmt.get()))
                    {
                        buildReExportStmt(innerReExport);
                        continue;
                    }

                    // (AR) استخراج الدوال (المُصدَّرة أو غير المُصدَّرة)
                    // (EN) Extract functions (exported or not)
                    AST::FunctionDecl *funcDecl = nullptr;
                    AST::VarDeclStmt *varDecl = nullptr;
                    AST::ClassDecl *classDecl = nullptr;
                    // (AR) [ISSUE-026] البنية «صدّر بنية» تُلتقط هنا أيضًا لتُبنى (كانت تُسقَط).
                    // (EN) [ISSUE-026] Capture an exported struct here too so it gets built.
                    Sad::AST::StructDecl *structDecl = nullptr;

                    // (AR) تحقق من تصدير صريح (ExportDecl)
                    // (EN) Check for explicit export (ExportDecl)
                    if (auto exportDecl = dynamic_cast<AST::ExportDecl *>(stmt.get()))
                    {
                        if (exportDecl->declaration)
                        {
                            funcDecl = dynamic_cast<AST::FunctionDecl *>(exportDecl->declaration.get());
                            varDecl = dynamic_cast<AST::VarDeclStmt *>(exportDecl->declaration.get());
                            classDecl = dynamic_cast<AST::ClassDecl *>(exportDecl->declaration.get());
                            structDecl = dynamic_cast<Sad::AST::StructDecl *>(exportDecl->declaration.get());
                        }
                    }
                    // (AR) تحقق من تصدير قديم (ExportStmt)
                    // (EN) Check for legacy export (ExportStmt)
                    else if (auto exportStmt = dynamic_cast<AST::ExportStmt *>(stmt.get()))
                    {
                        if (exportStmt->declaration)
                        {
                            funcDecl = dynamic_cast<AST::FunctionDecl *>(exportStmt->declaration.get());
                            varDecl = dynamic_cast<AST::VarDeclStmt *>(exportStmt->declaration.get());
                            classDecl = dynamic_cast<AST::ClassDecl *>(exportStmt->declaration.get());
                            structDecl = dynamic_cast<Sad::AST::StructDecl *>(exportStmt->declaration.get());
                        }
                    }
                    // (AR) تصريحٌ خاصّ (بلا «صدّر»): يُبنى إن كان مبلوغًا من مُصدَّرات الوحدة
                    //      فقط. كان `استورد م` يبني **كلَّ** تصريحٍ بلا قيد، فوحدةٌ فيها
                    //      «طول» خاصّةٌ تختطف المدمَجَ في الملفّ المستورِد — نفسُ عيب اختبار
                    //      061 لكن على هذا المسار، ولا يكشفه لأنّه يستعمل الصيغة الانتقائيّة.
                    // (EN) Private declaration (no «export»): built only if reachable from the
                    //      module's exports. Plain `import m` used to build EVERY declaration, so
                    //      a module with a private «طول» hijacked the builtin in the importing
                    //      file — the same defect as test 061 but on this path, which the test
                    //      does not cover because it uses the selective form.
                    else
                    {
                        funcDecl = dynamic_cast<AST::FunctionDecl *>(stmt.get());
                        varDecl = dynamic_cast<AST::VarDeclStmt *>(stmt.get());
                        classDecl = dynamic_cast<AST::ClassDecl *>(stmt.get());
                        structDecl = dynamic_cast<Sad::AST::StructDecl *>(stmt.get());

                        std::string privateName;
                        if (funcDecl)
                            privateName = funcDecl->name;
                        else if (varDecl)
                            privateName = varDecl->name;
                        else if (classDecl)
                            privateName = classDecl->name;
                        else if (structDecl)
                            privateName = structDecl->name;

                        if (!privateName.empty() &&
                            plainImportPrivateClosure.find(privateName) ==
                                plainImportPrivateClosure.end())
                            continue;
                    }

                    // (AR) بناء SIR للتصريحات المُكتشفة
                    // (EN) Build SIR for discovered declarations
                    if (funcDecl)
                    {
                        b_.buildFunction(funcDecl);
                    }
                    if (varDecl)
                    {
                        b_.buildGlobalVariable(varDecl);
                    }
                    if (classDecl)
                    {
                        b_.buildClass(classDecl);
                    }
                    // (AR) [ISSUE-026] بناء البنية المستوردة (buildStatement ⇒ addClass/classTable_)
                    //      مع حارس getClass لتفادي التسجيل المكرّر عند إعادة معالجة الوحدة.
                    // (EN) [ISSUE-026] Build the imported struct (buildStatement ⇒
                    //      addClass/classTable_) with a getClass guard to avoid double registration
                    //      when the module is re-processed.
                    if (structDecl && !b_.module_->getClass(structDecl->name))
                    {
                        b_.buildStatement(structDecl);
                    }
                }
            }

            /**
             * @brief (AR) تسجيل تواقيع دوالّ الوحدات المستوردة انتقائيًّا مسبقًا
             * @brief (EN) Pre-register selectively-imported module function signatures
             *
             * (AR) العلّة: الوحدات المستوردة تُبنى في الطور 2 (بعد الطور 1.7 للاستنتاج)،
             *      فحين يمسح inferParamTypesFromCallSites مواقع النداء في الوحدة الرئيسة
             *      لا تكون الدالّة المستوردة مسجَّلة في functionTable_ بعد ⇒ لا يُحدَّث نوع
             *      معاملها غير المصرَّح فيبقى Integer الافتراضيّ، فيُبنى جسمها على أساسه
             *      ويُطبع الوسيط النصّيّ مشوَّهًا (`مرحبا 140698...!`). الحلّ: نسجّل تواقيعها
             *      هنا — قبل الطور 1.7 — كي يستنتج المترجم أنواع معاملاتها من الوحدة الرئيسة،
             *      ثمّ يبنيها الطور 2 بالنوع المصحَّح (حارس التخطّي يُبنى إن لم يُبنَ بعد).
             * (EN) Bug: imported functions are built in Phase 2 (after Phase 1.7 inference),
             *      so when inferParamTypesFromCallSites scans the main module's call sites the
             *      imported function is not yet in functionTable_ ⇒ its undeclared param type
             *      stays default Integer, its body is built accordingly, and a string arg
             *      prints garbled. Fix: register the signatures here — before Phase 1.7 — so
             *      the compiler infers their param types from the main module, then Phase 2
             *      builds them with the corrected type (skip-guard builds if not yet built).
             */
            void TemplateBuilder::collectFileImportBindings(
                const Sad::AST::StmtList &fileStatements,
                std::unordered_map<std::string, std::string> &aliases,
                std::unordered_map<std::string, std::string> &moduleNamespaces)
            {
                for (const auto &stmt : fileStatements)
                {
                    if (!stmt)
                        continue;

                    // (AR) «استورد م [كـ ر]» يفتح فضاءَ أسماءٍ باسمِه الفعّال — المستعارِ
                    //      إن وُجد وإلّا آخرِ مقطعٍ من المسار (getEffectiveName). ISSUE-090.
                    // (EN) `استورد م [كـ ر]` opens a namespace under its effective name — the
                    //      alias if present, otherwise the path's last segment. ISSUE-090.
                    if (auto *plainImport = dynamic_cast<Sad::AST::ImportStmt *>(stmt.get()))
                    {
                        if (!plainImport->modulePath.empty())
                            moduleNamespaces[plainImport->getEffectiveName()] =
                                plainImport->getFullModuleName();
                        continue;
                    }

                    // (AR) الصيغتان تُنتجان FromImportStmt نفسَها؛ وإعادةُ التصدير تحمل
                    //      ImportItem بعينه، فتمرّ بالتقييد نفسِه (ISSUE-088).
                    // (EN) Both import orders produce FromImportStmt; re-export carries the
                    //      very same ImportItem, so it goes through the same recording.
                    const std::vector<Sad::AST::ImportItem> *items = nullptr;
                    // (AR) 🔑 التأهيلُ الكاملُ لازم: داخل Sad::Compiler::SIR يُحَلّ `AST::`
                    //      إلى مساحةِ الأسماء المتداخلةِ للأسماءِ المستعارةِ القديمة، وليس
                    //      فيها ReExportStmt ⇒ C2061 (والأسوأُ لو صادفتها لكانت نوعًا آخر).
                    // (EN) 🔑 Full qualification is required: inside Sad::Compiler::SIR the
                    //      bare `AST::` resolves to the nested legacy-alias namespace.
                    if (auto *fromImport = dynamic_cast<Sad::AST::FromImportStmt *>(stmt.get()))
                        items = &fromImport->items;
                    else if (auto *reExport = dynamic_cast<Sad::AST::ReExportStmt *>(stmt.get()))
                        items = &reExport->items;

                    if (!items)
                        continue;

                    for (const auto &item : *items)
                    {
                        if (!item.alias.has_value())
                            continue;
                        const std::string &aliasName = item.alias.value();
                        if (aliasName.empty() || aliasName == item.name)
                            continue;
                        aliases[aliasName] = item.name;
                    }
                }
            }

            void TemplateBuilder::preRegisterImportedSignatures(Sad::AST::StmtList *program)
            {
                if (!program)
                    return;

                if (!b_.moduleResolver_)
                {
                    b_.moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
                }

                // (AR) حارس الوحدات المُعالَجة مسبقًا — يمنع التكرار اللانهائيّ عند
                //      الاستيراد الدائريّ ويجنّب إعادة العمل. محلّيّ ⇒ يُعاد ضبطه لكلّ وحدة.
                // (EN) Visited-module guard — prevents infinite recursion on cyclic
                //      imports and avoids rework. Local ⇒ reset per module compilation.
                std::unordered_set<std::string> visitedPaths;

                // (AR) دالّة تعاوديّة: لكلّ عبارة `من م استورد ...` تسجّل تواقيع الرموز
                //      المستوردة فعلًا (المُصدَّرة) من الوحدة م، وتجمع شجرة م مرّةً للمسح،
                //      ثمّ تنحدر إلى استيرادات م المتعدّية بمسار ملفّ م (للاستيراد النسبيّ).
                // (EN) Recursive: for each `from m import ...` statement, register the
                //      signatures of the actually-imported (exported) symbols of m, collect
                //      m's AST once for scanning, then descend into m's transitive imports
                //      using m's own file path (so relative imports inside m resolve).
                std::function<void(Sad::AST::StmtList *, const std::string &)> processImportsIn =
                    [&](Sad::AST::StmtList *stmts, const std::string &basePath)
                {
                    if (!stmts)
                        return;
                    for (const auto &stmt : *stmts)
                    {
                        // ════════════════════════════════════════════════════════
                        // (AR) صيغتا الاستيراد كلتاهما، لا «من م استورد» وحدَها.
                        //
                        //      كانت هذه التمريرة تتجاهل `استورد م` تجاهلًا تامًّا،
                        //      فلا تُسجَّل تواقيعُ دوالّها قبل الطور 1.7 (استنتاج
                        //      أنواع المعاملات من مواقع النداء). والأثر: معاملُ
                        //      «مصفوفة» يبقى بلا نوعِ عنصر، فمتغيّرُ «لكل» عليه
                        //      يُبنى عددًا لا نصًّا — فتطبع الحلقةُ **عنوانًا**
                        //      مكان النصّ. ونفسُ الدالّة حرفًا بحرف تعمل صحيحةً
                        //      حين تُوضَع في ملفّ الدخول.
                        //
                        //      وهو تباعدُ مسارَي بناءٍ لا عيبُ كودٍ مكتوب — كنظير
                        //      #333 تمامًا، ولذلك يُعالَج بتعميم التمريرة لا
                        //      بحالةٍ خاصّةٍ ثالثة.
                        // (EN) Handle BOTH import forms. This pass ignored plain
                        //      `استورد م` entirely, so its signatures were absent
                        //      before Phase 1.7 param-type inference: an array
                        //      parameter kept no element type, and a `لكل` over it
                        //      built the loop variable as an integer — printing an
                        //      ADDRESS instead of the string. The identical function
                        //      works when placed in the entry file. Same build-path
                        //      asymmetry as #333, cured by generalising the pass.
                        // ════════════════════════════════════════════════════════
                        auto *fromImport = dynamic_cast<AST::FromImportStmt *>(stmt.get());
                        auto *plainImport = dynamic_cast<AST::ImportStmt *>(stmt.get());
                        // (AR) وإعادةُ التصديرِ كذلك: الطوران يجب أن يتّفقا، والطورُ 2
                        //      صار يُتبعها إلى مصدرِها (ISSUE-089) فلو بقيت مجهولةً هنا
                        //      لبُنيت أجسامُها بلا توقيعٍ مُسجَّلٍ سلفًا — وهو الاختلالُ
                        //      الموثَّقُ نفسُه في #333. والمسارُ الفارغُ («صدّر *» المجرّدة)
                        //      لا وحدةَ له فيُتخطّى.
                        // (EN) Re-exports too: the two phases must agree, and Phase 2 now
                        //      follows them to their source (ISSUE-089); leaving them unknown
                        //      here would build bodies with no pre-registered signature — the
                        //      very asymmetry documented in #333. An empty path (bare
                        //      «صدّر *») has no module and is skipped.
                        auto *reExport = dynamic_cast<Sad::AST::ReExportStmt *>(stmt.get());
                        if (reExport && reExport->modulePath.empty())
                            continue;
                        if (!fromImport && !plainImport && !reExport)
                            continue;

                        // (AR) `استورد م` يُتيح كلَّ رموز الوحدة — وهو ما يبنيه الطور 2
                        //      فعلًا (buildImportStmt يبني الدوالَّ مُصدَّرةً وغيرَ مُصدَّرة).
                        //      فبذرُها هنا لا يضيف سطحًا جديدًا؛ يجعل الاستنتاجَ يرى ما
                        //      سيوجد على أيّ حال. أمّا `من م استورد س` فيبقى محصورًا
                        //      بالمُصدَّر المطلوب للسبب الموثَّق أدناه.
                        // (EN) Plain import exposes every symbol — exactly what Phase 2
                        //      already builds — so seeding here adds no new surface; it
                        //      only lets inference see what will exist regardless.
                        const bool seedPrivate = (plainImport != nullptr);

                        // (AR) الحقولُ الثلاثةُ موحَّدةً: `من م استورد …` و`صدّر … من م`
                        //      يشتركان في الشكل، و`استورد م` مسارُه وحدَه.
                        // (EN) The three fields unified: `من م استورد …` and `صدّر … من م`
                        //      share a shape; plain `استورد م` contributes its path only.
                        const std::vector<std::string> &importedModulePath =
                            plainImport ? plainImport->modulePath
                                        : (fromImport ? fromImport->modulePath : reExport->modulePath);
                        const std::vector<Sad::AST::ImportItem> *selectiveItems =
                            fromImport ? &fromImport->items : (reExport ? &reExport->items : nullptr);
                        const bool selectiveIsWildcard =
                            fromImport ? fromImport->isWildcard : (reExport && reExport->isWildcard);

                        std::string fullModuleName = joinModulePath(importedModulePath);
                        // (AR) الوحدات القياسية دوالُّها مضمَّنة في المترجم — لا تُسجَّل هنا
                        // (EN) Stdlib modules are compiler builtins — skip
                        if (isCompilerBuiltinStdlibModule(fullModuleName))
                            continue;

                        // (AR) resolveModule يخبّئ النتائج ⇒ الطور 2 يعيد استخدام الشجرة ذاتها
                        // (EN) resolveModule caches ⇒ Phase 2 reuses the same AST
                        Modules::Module *module = b_.moduleResolver_->resolveModule(
                            importedModulePath,
                            basePath);
                        if (!module)
                            continue;

                        // (AR) حارس الزيارة بمفتاح مسار الملفّ المُحَلّ (لا الاسم) كي لا يخلط
                        //      وحدتين متطابقتَي الاسم في مجلّدين، ويجمع الشجرةَ وينحدر مرّةً.
                        //      🔑 على ويندوز نحوّل المسارَ عبر from_wstring(wstring): استدعاءُ
                        //      path::string() على اسمٍ عربيّ يتعطّل/يرمي (خارج ترميز ANSI) —
                        //      يطابق ما يفعله module_resolver عمدًا بـ.wstring().
                        // (EN) Visit guard keyed by the resolved file path (not the name) so two
                        //      identically-named modules in different dirs don't collide; body
                        //      collection and descent happen once.
                        //      🔑 On Windows convert via from_wstring(wstring): calling
                        //      path::string() on an Arabic name hangs/throws (outside ANSI) —
                        //      mirrors what module_resolver deliberately does with .wstring().
#ifdef _WIN32
                        std::string modulePathKey =
                            sad::utf8::from_wstring(module->filePath.wstring());
#else
                        std::string modulePathKey = module->filePath.string();
#endif
                        bool firstVisit = visitedPaths.insert(modulePathKey).second;

                        // (AR) رموزُ الوحدةِ المُصدَّرة — تُقيَّد في الطور 1 لأنّ فكَّ
                        //      التأهيلِ يقع في الاستنتاج (1.7) قبلَ بناءِ الأجسام.
                        // (EN) The module's exported symbols — recorded in Phase 1 because
                        //      un-qualification happens during inference (1.7), before bodies.
                        if (firstVisit)
                        {
                            std::set<std::string> visitedReExportPaths{modulePathKey};
                            collectModuleExportedNames(module->ast, modulePathKey,
                                                       b_.moduleExportedSymbols_[fullModuleName],
                                                       visitedReExportPaths);
                        }

                        // (AR) اجمع شجرة الوحدة مرّةً كي يمسحها inferParamTypesFromCallSites
                        //      (انتشارٌ متعدٍّ: نداءُ رسالة لتحية داخل أشكال يُرقّي تحية)
                        // (EN) Collect the module AST once so inferParamTypesFromCallSites
                        //      scans it (transitive: رسالة's تحية call inside أشكال widens تحية)
                        if (firstVisit)
                            b_.importedModuleBodies_.push_back(&module->ast);

                        // (AR) الرموز المطلوبة في هذه العبارة تحديدًا؛ اتّحادُ عبارات الوحدة
                        //      يتراكم طبيعيًّا لأنّنا نسجّل عند كلّ عبارة. حصرُ التسجيل بالرموز
                        //      المستوردة فعلًا (المُصدَّرة) حرجٌ: بذرُ دوالّ الوحدة الخاصّة في
                        //      functionTable_ يجعل isUserDefinedFunction صادقًا لاسمٍ يصادف
                        //      مدمَجًا (طول/حجم/جذر) فيَختطف المدمَجَ في الرئيسة ⇒ مرجعٌ غير
                        //      مُعرَّف أو دلالة خاطئة. لذا: المُصدَّرُ المطلوبُ فقط.
                        // (EN) Symbols requested by THIS statement; the module's union
                        //      accumulates naturally since we register at every statement.
                        //      Restricting to actually-imported (exported) symbols is CRITICAL:
                        //      seeding a module's private functions into functionTable_ makes
                        //      isUserDefinedFunction true for a name shadowing a builtin
                        //      (طول/حجم/جذر), hijacking it in the main module ⇒ undefined ref or
                        //      wrong semantics. So: only the exported, requested symbol.
                        std::unordered_set<std::string> requestedSymbols;
                        bool isWildcard = seedPrivate ? true : selectiveIsWildcard;
                        if (!isWildcard && selectiveItems)
                        {
                            for (const auto &item : *selectiveItems)
                                requestedSymbols.insert(item.name);
                        }

                        // (AR) الإغلاقُ التعدّيّ نفسُه الذي يستعمله الطور 2 — كي يتّفق الطوران.
                        //      `استورد م` يُحسَب شاملًا (isWildcard) فتُبذَر مُصدَّراتُ الوحدة
                        //      كلُّها ومَن تبلغه من الخاصّ.
                        // (EN) The same transitive closure Phase 2 uses — so both phases agree.
                        //      Plain `import m` is computed in wildcard mode.
                        std::set<std::string> requestedOrdered(requestedSymbols.begin(),
                                                               requestedSymbols.end());
                        std::set<std::string> seedPrivateClosure =
                            computeImportedPrivateClosure(module->ast, requestedOrdered, isWildcard);

                        for (const auto &mstmt : module->ast)
                        {
                            if (!mstmt)
                                continue;

                            // (AR) الطوران يجب أن يتوافقا: الطور 2 (buildFromImportStmt) صار
                            //      يبني كلَّ تصريحٍ خاصٍّ في الوحدة — لأنّ المُصدَّرَ المطلوب
                            //      يعتمد عليه — فيلزم بذرُه هنا أيضًا، وإلّا بقيت معاملاتُه
                            //      بلا استنتاجٍ فتُقرأ نصوصُها أعدادًا (يُطبع المؤشّر).
                            //      كان البذرُ مقصورًا على المُصدَّر، وعلى `استورد م` وحدَه
                            //      للخاصّ (seedPrivate)، فاختلّ التوافقُ مع الاستيراد الانتقائيّ.
                            //      ويبقى ترشيحُ الطلب مقصورًا على المُصدَّر، مطابقًا للطور 2.
                            // (EN) The two phases must agree: Phase 2 now builds every private
                            //      declaration of the module (the requested exports depend on
                            //      them), so they must be seeded here too — otherwise their
                            //      parameters stay un-inferred and string arguments are read as
                            //      integers (pointer printed). Seeding was limited to exported
                            //      symbols, and to plain `import m` for private ones, so it fell
                            //      out of step with selective imports. The request filter stays
                            //      confined to exported symbols, mirroring Phase 2.
                            AST::FunctionDecl *funcDecl = nullptr;
                            bool declIsExported = false;
                            if (auto *exportDecl = dynamic_cast<AST::ExportDecl *>(mstmt.get()))
                            {
                                declIsExported = true;
                                if (exportDecl->declaration)
                                    funcDecl = dynamic_cast<AST::FunctionDecl *>(exportDecl->declaration.get());
                            }
                            else if (auto *exportStmt = dynamic_cast<AST::ExportStmt *>(mstmt.get()))
                            {
                                declIsExported = true;
                                if (exportStmt->declaration)
                                    funcDecl = dynamic_cast<AST::FunctionDecl *>(exportStmt->declaration.get());
                            }
                            else
                            {
                                funcDecl = dynamic_cast<AST::FunctionDecl *>(mstmt.get());
                            }

                            if (!funcDecl)
                                continue;
                            if (declIsExported)
                            {
                                if (!isWildcard &&
                                    requestedSymbols.find(funcDecl->name) == requestedSymbols.end())
                                    continue;
                            }
                            else if (seedPrivateClosure.find(funcDecl->name) ==
                                     seedPrivateClosure.end())
                            {
                                // (AR) خاصٌّ غيرُ مبلوغٍ من المطلوب — لا يُبذَر ولا يُبنى،
                                //      كي لا يُظلّل مدمَجًا (اختبار 061).
                                // (EN) Private and unreachable — neither seeded nor built,
                                //      so it cannot shadow a builtin (test 061).
                                continue;
                            }
                            // (AR) لا تُعِد تسجيل رمز موجود سلفًا (محلّيّ الطور 1 أو بُذِر آنِفًا)
                            // (EN) Don't re-register an already-present symbol (local Phase-1 or seeded)
                            if (b_.functionTable_.find(funcDecl->name) != b_.functionTable_.end())
                                continue;

                            // (AR) توقيع بأنواع خام — يطابق تسجيل الطور 1 للدوالّ المحلّيّة.
                            //      sirFunction=nullptr علامةُ «سُجِّل ولم يُبنَ» يقرؤها حارس التخطّي.
                            // (EN) Raw-type signature — mirrors Phase 1's local registration.
                            //      sirFunction=nullptr marks "registered but not built" for the guard.
                            FunctionInfo funcInfo;
                            funcInfo.name = funcDecl->name;
                            if ((funcDecl->returnType == Types::SadTypeKind::Unknown ||
                                 funcDecl->returnType == Types::SadTypeKind::Void) &&
                                funcDecl->body)
                            {
                                funcInfo.returnType =
                                    inferReturnTypeFromBody(funcDecl->body.get(), funcDecl);
                            }
                            else if (funcDecl->returnType == Types::SadTypeKind::Unknown &&
                                     !funcDecl->body)
                            {
                                funcInfo.returnType = SadTypeKind::Void;
                            }
                            else
                            {
                                funcInfo.returnType = b_.astTypeToSIRType(funcDecl->returnType);
                            }
                            for (const auto &param : funcDecl->parameters)
                            {
                                funcInfo.parameters.push_back(
                                    SIRParameter(param.name, b_.astTypeToSIRType(param.type)));
                            }
                            funcInfo.sirFunction = nullptr;
                            funcInfo.astDecl = funcDecl;
                            b_.functionTable_[funcDecl->name] = funcInfo;
                            // (AR) علِّم البذرة كي يبنيها حارس التخطّي دون مسّ دلالة المحلّيّ
                            // (EN) Mark the seed so the skip-guard builds it without touching local semantics
                            b_.preRegisteredImportNames_.insert(funcDecl->name);
                        }

                        // (AR) انحدر إلى استيرادات هذه الوحدة المتعدّية مرّةً — بمسار ملفّها
                        //      كي تُحَلّ استيراداتها النسبيّة صحيحًا (رسالة⇒تحية عبر أشكال).
                        // (EN) Descend into this module's transitive imports once — using its
                        //      own file path so relative imports resolve (رسالة⇒تحية via أشكال).
                        if (firstVisit)
                            processImportsIn(&module->ast, modulePathKey);

                        // ════════════════════════════════════════════════════════════
                        // (AR) استنتاجُ أنواع المعاملات من مواقع النداء **داخل الوحدة**.
                        //      كانت هذه الآليّةُ تُشغَّل على البرنامج الرئيس وحدَه
                        //      (sir_builder_module.cpp)، فدالّةٌ في الوحدة لا تُنادى إلّا
                        //      من أختها داخلَها تبقى معاملاتُها Integer الافتراضيّة ⇒
                        //      يُقرأ النصُّ عددًا. وتجريبُ Any بدلًا من ذلك أسقط الترجمةَ
                        //      بتأكيد LLVM: فبواعثُ الأوپكودات المدمجة (طول/جزء في
                        //      backend/llvm/builders/platform/hardware_ffi_ops2.cpp) لا
                        //      تعي %SadDyn فتمرّر بنيةً حيث يُنتظَر مؤشّر.
                        //      الصوابُ إذًا: تشغيلُ الاستنتاج القائم على شجرة الوحدة،
                        //      وإبقاءُ الأنواع محسوسةً فلا تلمس بواعثَ غيرَ مهيّأة.
                        //      يُعاد مرّاتٍ لبلوغ نقطة ثبات، إذ قد يتوقّف نوعُ معاملِ دالّةٍ
                        //      على نوعِ عائدِ أخرى لم تُستنتَج بعد.
                        // (EN) Infer parameter types from call sites INSIDE the module.
                        //      This mechanism ran on the main program only, so a module
                        //      function called solely by its siblings kept the default
                        //      Integer parameters ⇒ strings read as numbers. Using Any
                        //      instead broke compilation with an LLVM assertion: the
                        //      builtin opcode emitters (length/substring in
                        //      hardware_ffi_ops2.cpp) are not %SadDyn-aware and passed a
                        //      struct where a pointer was expected. Running the existing
                        //      inference over the module AST keeps types concrete.
                        //      Repeated to a fixpoint: one parameter's type may depend on
                        //      another function's not-yet-inferred return type.
                        // ════════════════════════════════════════════════════════════
                        if (firstVisit)
                        {
                            // (AR) ابذر أنواعَ ثوابت الوحدة العُليا أوّلًا: الاستنتاجُ يقرأ
                            //      اسمَ الثابت في موقع النداء، وعوالمُ الوحدة لا تُبعَث في
                            //      module_ إلّا في الطور 2، فبدون البذرة يُقرأ الثابتُ
                            //      النصّيُّ عددًا ⇒ يبقى المعاملُ Integer ⇒ تُطوى مقارنتُه
                            //      بنصٍّ ثابتًا ⇒ «فشل بناء شرط إذا».
                            // (EN) Seed the module's top-level const types first: inference
                            //      reads the const's name at the call site, and module globals
                            //      are only emitted into module_ in Phase 2 — without the seed
                            //      a string const reads as a number ⇒ the parameter stays
                            //      Integer ⇒ its comparison to a string folds ⇒ build failure.
                            for (const auto &mstmt : module->ast)
                            {
                                if (!mstmt)
                                    continue;
                                const AST::Statement *inner = mstmt.get();
                                if (auto *exportDecl = dynamic_cast<AST::ExportDecl *>(mstmt.get()))
                                    inner = exportDecl->declaration ? exportDecl->declaration.get() : inner;
                                else if (auto *exportStmt = dynamic_cast<AST::ExportStmt *>(mstmt.get()))
                                    inner = exportStmt->declaration ? exportStmt->declaration.get() : inner;
                                auto *varDecl = dynamic_cast<const AST::VarDeclStmt *>(inner);
                                if (!varDecl || !varDecl->initializer)
                                    continue;
                                SadTypeKind declared = b_.astTypeToSIRType(varDecl->type);
                                SadTypeKind seeded = (declared != SadTypeKind::Integer &&
                                                      declared != SadTypeKind::Void &&
                                                      declared != SadTypeKind::Unknown)
                                                         ? declared
                                                         : inferExprType(varDecl->initializer.get());
                                b_.pendingGlobalTypes_[varDecl->name] = seeded;
                            }

                            constexpr int kParamInferencePasses = 3;
                            for (int pass = 0; pass < kParamInferencePasses; ++pass)
                                inferParamTypesFromCallSites(&module->ast);
                        }
                    }
                };

                processImportsIn(program, b_.currentFilePath_);

                // (AR) الاستعاراتُ تُقيَّد بعد المسح التعاوديّ ومن **هذا الملفّ** وحدَه؛
                //      ولكلّ وحدةٍ تُبنى أجسامُها خريطتُها عبر ImportAliasScope.
                // (EN) Aliases are recorded after the recursive scan, from THIS file only;
                //      each module whose bodies are built gets its own map via
                //      ImportAliasScope.
                if (program)
                    collectFileImportBindings(*program, b_.importAliases_, b_.moduleNamespaces_);
            }

            /**
             * @brief (AR) معالجة استيراد انتقائي: من وحدة استورد ...
             * @brief (EN) Process selective import: from module import ...
             */
            // (AR) حسابُ الإغلاقِ رُفع إلى `shared/ast/module_export_closure` نسخةً
            //      واحدةً يستهلكها المحرّكان — كانت هنا نسخةُ المصرِّفِ وحدَه، والمفسّرُ
            //      يحتاج المنطقَ نفسَه لدلالةِ «صدّر»، فكانت ستصير الخريطةَ اليدويّةَ
            //      الثالثة. والنسخةُ المشتركةُ تصلح كذلك عمى `ExportDecl` هنا: «صدّر»
            //      يُفكَّ لفُّ `ExportDecl` هناك، وتُقرأ الرايةُ احتياطًا كذلك.
            // (EN) The closure computation was lifted into
            //      `shared/ast/module_export_closure` as a single copy for both engines.
            //      It also fixes the ExportDecl blindness that lived here: «صدّر» is a
            //      unwrapped there, and the isExported flag is read as a fallback too.
            std::set<std::string> TemplateBuilder::computeImportedPrivateClosure(
                const std::vector<std::unique_ptr<Sad::AST::Statement>> &moduleAst,
                const std::set<std::string> &requestedSymbols,
                bool isWildcard)
            {
                return Sad::AST::computeExportedPrivateClosure(
                    moduleAst, requestedSymbols, isWildcard);
            }

            void TemplateBuilder::buildFromImportStmt(AST::FromImportStmt *fromImportStmt)
            {
                if (!fromImportStmt)
                    return;

                buildSelectiveImportFrom(fromImportStmt->modulePath,
                                         fromImportStmt->items,
                                         fromImportStmt->isWildcard);
            }

            // (AR) 🔑 التأهيلُ الكاملُ `Sad::AST::` لازم: `AST::` المجرّدةُ تُحَلّ داخلَ
            //      Sad::Compiler::SIR إلى مساحةِ الأسماء المستعارةِ القديمة (sir_builder.h)
            //      وليس فيها ReExportStmt.
            // (EN) 🔑 Full `Sad::AST::` qualification required: bare `AST::` resolves inside
            //      Sad::Compiler::SIR to the legacy-alias namespace, which lacks ReExportStmt.
            void TemplateBuilder::collectModuleExportedNames(
                const Sad::AST::StmtList &moduleStatements,
                const std::string &moduleFilePath,
                std::set<std::string> &exportedNames,
                std::set<std::string> &visitedModulePaths)
            {
                collectExportedNamesFromStatements(moduleStatements, exportedNames);

                if (!b_.moduleResolver_)
                    return;

                for (const auto &stmt : moduleStatements)
                {
                    auto *reExport = stmt ? dynamic_cast<Sad::AST::ReExportStmt *>(stmt.get())
                                          : nullptr;
                    if (!reExport || !reExport->isWildcard || reExport->modulePath.empty())
                        continue;

                    Modules::Module *sourceModule =
                        b_.moduleResolver_->resolveModule(reExport->modulePath, moduleFilePath);
                    if (!sourceModule)
                        continue;

#ifdef _WIN32
                    // (AR) 🔑 المفتاحُ من wstring: path::string() على اسمٍ عربيّ يتعطّل
                    //      خارجَ ترميز ANSI — نظيرُ ما يفعله module_resolver عمدًا.
                    // (EN) 🔑 Key via wstring: path::string() on an Arabic name breaks outside
                    //      the ANSI codepage — mirroring what module_resolver deliberately does.
                    std::string sourceKey = sad::utf8::from_wstring(sourceModule->filePath.wstring());
#else
                    std::string sourceKey = sourceModule->filePath.string();
#endif
                    if (!visitedModulePaths.insert(sourceKey).second)
                        continue;

                    collectModuleExportedNames(sourceModule->ast, sourceKey, exportedNames,
                                               visitedModulePaths);
                }
            }

            void TemplateBuilder::buildReExportStmt(Sad::AST::ReExportStmt *reExportStmt)
            {
                if (!reExportStmt)
                    return;

                // (AR) «صدّر *» المجرّدة: لا وحدةَ مصدرٍ ⇒ لا-عمليّة (module_nodes.h).
                // (EN) Bare «صدّر *»: no source module ⇒ no-op (module_nodes.h).
                if (reExportStmt->modulePath.empty())
                    return;

                // ════════════════════════════════════════════════════════════════
                // (AR) حارسُ الدورة. جسمُ الاستيرادِ الانتقائيِّ لا يتوقّف عند وحدةٍ
                //      سبق تحميلُها عمدًا (قد تُطلَب منها رموزٌ جديدة)، وهو صوابٌ
                //      للاستيراد لأنّ عبارتَه لا تعاود. أمّا إعادةُ التصديرِ فتعاود:
                //      وحدتان تُعيد كلٌّ تصديرَ الأخرى تُنتجان تعاودًا بلا قاع ⇒
                //      انهيارُ مكدّسٍ (0xC00000FD) بلا أيِّ تشخيص. فيُمسَك ما هو
                //      قيدَ الإتْباعِ الآن ويُقطع الدورُ عنده — والتشخيصُ يقع طبيعيًّا
                //      عند موضعِ النداءِ إن نقص رمز.
                // (EN) Cycle guard. The selective-import body deliberately does not stop at an
                //      already-loaded module (new symbols may be requested from it), which is
                //      right for imports since their statements do not recurse. Re-exports do:
                //      two modules re-exporting each other recurse without a floor ⇒ a stack
                //      overflow (0xC00000FD) with no diagnostic at all. So what is currently
                //      being followed is tracked and the cycle cut there; a genuinely missing
                //      symbol still gets diagnosed at its own call site.
                // ════════════════════════════════════════════════════════════════
                //      🔑 بحارسِ نطاقٍ لا بسطرَين متتاليَين: الحذفُ الذي يعتمد على بلوغِ
                //      السطرِ التالي يسقط مع أوّلِ خروجٍ مبكّرٍ أو استثناءٍ يُستحدَث في
                //      الجسم، فيبقى المفتاحُ عالقًا وتُصمَت إعادةُ تصديرٍ مشروعةٌ بعده.
                //      🔑 A scope guard, not two consecutive lines: an erase that depends on
                //      reaching the next statement dies with the first early return or
                //      exception later introduced into the body, leaving the key stuck and
                //      silently muting a subsequent legitimate re-export.
                // (AR) 🔑 والمفتاحُ مؤهَّلٌ بالملفِّ الذي كتبه متى كان المسارُ نسبيًّا:
                //      `.مصدر` **ليست اسمًا مطلقًا** — مجلَّدان فيهما `مصدر.ص` وكلٌّ
                //      يُعيد تصديرَه بالصيغةِ عينِها يتصادمان على مفتاحٍ واحدٍ، فيُقطَع
                //      الثاني وهو **مشروعٌ** ⇒ رمزٌ لا يُبنى ثمّ خطأُ رابطٍ مبهم. وهذه
                //      مرآةُ ما يفعله `ModuleResolver::resolveModule` أصلًا في مفتاحِ
                //      خبيئته (`currentFile + "::" + fullName`) — فيتّفق الحارسُ معه.
                // (EN) 🔑 The key is qualified by the file that wrote it whenever the path is
                //      relative: `.مصدر` is NOT an absolute name — two directories each
                //      holding `مصدر.ص` and re-exporting it identically collide on one key,
                //      so the second, LEGITIMATE re-export is cut ⇒ a symbol is never built
                //      and an opaque link error follows. Mirrors what
                //      `ModuleResolver::resolveModule` already does for its own cache key.
                const std::string reExportKey =
                    moduleIdentityKey(b_.currentFilePath_, reExportStmt->modulePath);
                if (!b_.reExportsInFlight_.insert(reExportKey).second)
                    return;
                ReExportCycleScope reExportScope(b_.reExportsInFlight_, reExportKey);

                buildSelectiveImportFrom(reExportStmt->modulePath,
                                         reExportStmt->items,
                                         reExportStmt->isWildcard);
            }

            void TemplateBuilder::buildSelectiveImportFrom(
                const std::vector<std::string> &modulePath,
                const std::vector<Sad::AST::ImportItem> &items,
                bool isWildcardImport)
            {
                if (!b_.moduleResolver_)
                {
                    b_.moduleResolver_ = std::make_unique<Modules::ModuleResolver>();
                }

                std::string fullModuleName = joinModulePath(modulePath);

                // (AR) تحسين جذري: إذا كانت الوحدة من المكتبة القياسية المعروفة للمترجم،
                //      فلا حاجة لتحليل ملف الوحدة أو تحميله. جميع الدوال مضمنة في المترجم.
                //      هذا يمنع أخطاء الرابط عند تحميل ملفات .ص القياسية التي تستدعي
                //      دوال داخلية غير موجودة في وقت التشغيل المضمن.
                // (EN) Root fix: if the module is a known stdlib module, skip parsing/loading
                //      the module file entirely. All functions are compiler builtins.
                //      This prevents linker errors from .ص wrappers calling undefined internals.
                if (isCompilerBuiltinStdlibModule(fullModuleName))
                {
                    // (AR) وحدة قياسية — جميع دوالها مضمنة، لا حاجة لتحميل الملف
                    // (EN) Stdlib module — all functions are builtins, skip file loading
                    b_.processedModules_.insert(fullModuleName);
                    return;
                }

                // (AR) التحقق مما إذا تمت معالجة الوحدة بالفعل
                //      للاستيراد الانتقائي: لا نتوقف — نكمل لاستيراد رموز جديدة
                // (EN) Check if module was already processed
                //      For selective import: don't stop — continue to import new symbols
                bool moduleAlreadyLoaded = b_.processedModules_.count(fullModuleName) > 0;

                // (AR) إصدار تعليمات SIR (فقط أول مرة)
                // (EN) Emit SIR instructions (first time only)
                if (!moduleAlreadyLoaded && b_.currentBlock_)
                {
                    std::string moduleReg = b_.newTempRegister();
                    SIRInstruction loadInst(SIROpcode::MODULE_LOAD);
                    loadInst.result = SIROperand::Register(moduleReg, SadTypeKind::Integer);
                    loadInst.operands.push_back(SIROperand::ConstantString(fullModuleName));
                    loadInst.comment = "تحميل وحدة انتقائي / selective import from: " + fullModuleName;
                    b_.currentBlock_->addInstruction(loadInst);

                    SIRInstruction initInst(SIROpcode::MODULE_INIT);
                    initInst.operands.push_back(SIROperand::Register(moduleReg, SadTypeKind::Integer));
                    b_.currentBlock_->addInstruction(initInst);

                    // (AR) إصدار MODULE_SYMBOL لكل رمز مطلوب
                    // (EN) Emit MODULE_SYMBOL for each requested symbol
                    if (!isWildcardImport)
                    {
                        for (const auto &item : items)
                        {
                            std::string symReg = b_.newTempRegister();
                            SIRInstruction symInst(SIROpcode::MODULE_SYMBOL);
                            symInst.result = SIROperand::Register(symReg, SadTypeKind::Integer);
                            symInst.operands.push_back(SIROperand::Register(moduleReg, SadTypeKind::Integer));
                            symInst.operands.push_back(SIROperand::ConstantString(item.name));
                            symInst.comment = "استيراد رمز / import symbol: " + item.name;
                            b_.currentBlock_->addInstruction(symInst);
                        }
                    }
                }

                // (AR) تحميل الوحدة (ModuleResolver يخبئ النتائج)
                // (EN) Load module (ModuleResolver caches results)
                Modules::Module *module = b_.moduleResolver_->resolveModule(
                    modulePath,
                    b_.currentFilePath_);

                if (!module)
                {
                    if (!moduleAlreadyLoaded)
                    {
                        b_.errors_.push_back(
                            "خطأ: لم يُعثر على الوحدة '" + fullModuleName + "' / "
                                                                           "Error: Module '" +
                            fullModuleName + "' not found");
                    }
                    return;
                }

                b_.processedModules_.insert(fullModuleName);

                // (AR) جمع أسماء الرموز المطلوبة (للاستيراد الانتقائي)
                // (EN) Collect requested symbol names (for selective import)
                std::unordered_set<std::string> requestedSymbols;
                bool isWildcard = isWildcardImport;

                if (!isWildcard)
                {
                    for (const auto &item : items)
                    {
                        requestedSymbols.insert(item.name);
                    }
                }

                // (AR) أيُّ تصريحٍ خاصٍّ تحتاجه الرموزُ المطلوبة (إغلاقٌ تعدّيّ)
                // (EN) Which private declarations the requested symbols need (transitive closure)
                std::set<std::string> requestedOrdered(requestedSymbols.begin(),
                                                       requestedSymbols.end());
                std::set<std::string> privateClosure =
                    computeImportedPrivateClosure(module->ast, requestedOrdered, isWildcard);

                // (AR) أجسامُ الوحدة تُبنى باستعاراتِ الوحدة وحدَها لا باستعاراتِ مستورِدها.
                // (EN) The module's bodies are built with the module's own aliases only.
                std::unordered_map<std::string, std::string> moduleImportAliases;
                std::unordered_map<std::string, std::string> moduleOwnNamespaces;
                collectFileImportBindings(module->ast, moduleImportAliases, moduleOwnNamespaces);
                ImportAliasScope moduleAliasScope(b_, std::move(moduleImportAliases),
                                                  std::move(moduleOwnNamespaces));

                // (AR) واستيراداتُها النسبيّةُ تُحَلُّ من مجلَّدِها هي — ISSUE-089-ب
                // (EN) And its relative imports resolve from its own directory
                ModuleFilePathScope moduleFileScope(b_, moduleFilePathAsText(module));

                // (AR) معالجة تصريحات الوحدة
                // (EN) Process module declarations
                for (const auto &stmt : module->ast)
                {
                    if (!stmt)
                        continue;

                    // (AR) معالجة عبارات الاستيراد الداخلية (استيراد متداخل/عابر)
                    // (EN) Process internal import statements (transitive/nested imports)
                    if (auto innerFromImport = dynamic_cast<AST::FromImportStmt *>(stmt.get()))
                    {
                        buildFromImportStmt(innerFromImport);
                        continue;
                    }
                    if (auto innerImport = dynamic_cast<AST::ImportStmt *>(stmt.get()))
                    {
                        buildImportStmt(innerImport);
                        continue;
                    }
                    // (AR) إعادةُ تصديرٍ داخلَ الوحدة تُتبَع إلى مصدرِها — ISSUE-089.
                    // (EN) A re-export inside the module is followed to its source — ISSUE-089.
                    if (auto innerReExport = dynamic_cast<Sad::AST::ReExportStmt *>(stmt.get()))
                    {
                        buildReExportStmt(innerReExport);
                        continue;
                    }

                    AST::FunctionDecl *funcDecl = nullptr;
                    AST::VarDeclStmt *varDecl = nullptr;
                    AST::ClassDecl *classDecl = nullptr;
                    // (AR) [ISSUE-026] البنية «صدّر بنية» عقدةٌ متمايزة StructDecl (لا ClassDecl)؛
                    //      كانت تُسقَط هنا فلا تُسجَّل في جدول الأصناف ⇒ «نقطة()» في الرئيسة
                    //      يفشل «دالة غير معرّفة». نلتقطها كي تُبنى كالصنف.
                    // (EN) [ISSUE-026] An exported struct is a distinct StructDecl node (not
                    //      ClassDecl); it was dropped here so it never entered the class table
                    //      ⇒ «نقطة()» in the main module failed as "undefined function". Capture
                    //      it so it is built like a class.
                    Sad::AST::StructDecl *structDecl = nullptr;
                    bool isExported = false;

                    if (auto exportDecl = dynamic_cast<AST::ExportDecl *>(stmt.get()))
                    {
                        isExported = true;
                        if (exportDecl->declaration)
                        {
                            funcDecl = dynamic_cast<AST::FunctionDecl *>(exportDecl->declaration.get());
                            varDecl = dynamic_cast<AST::VarDeclStmt *>(exportDecl->declaration.get());
                            classDecl = dynamic_cast<AST::ClassDecl *>(exportDecl->declaration.get());
                            structDecl = dynamic_cast<Sad::AST::StructDecl *>(exportDecl->declaration.get());
                        }
                    }
                    else if (auto exportStmt = dynamic_cast<AST::ExportStmt *>(stmt.get()))
                    {
                        isExported = true;
                        if (exportStmt->declaration)
                        {
                            funcDecl = dynamic_cast<AST::FunctionDecl *>(exportStmt->declaration.get());
                            varDecl = dynamic_cast<AST::VarDeclStmt *>(exportStmt->declaration.get());
                            classDecl = dynamic_cast<AST::ClassDecl *>(exportStmt->declaration.get());
                            structDecl = dynamic_cast<Sad::AST::StructDecl *>(exportStmt->declaration.get());
                        }
                    }
                    else
                    {
                        funcDecl = dynamic_cast<AST::FunctionDecl *>(stmt.get());
                        varDecl = dynamic_cast<AST::VarDeclStmt *>(stmt.get());
                        classDecl = dynamic_cast<AST::ClassDecl *>(stmt.get());
                        structDecl = dynamic_cast<Sad::AST::StructDecl *>(stmt.get());
                    }

                    // (AR) تحديد ما إذا كان الرمز مطلوباً
                    // (EN) Determine if symbol is requested
                    std::string symbolName;
                    if (funcDecl)
                        symbolName = funcDecl->name;
                    else if (varDecl)
                        symbolName = varDecl->name;
                    else if (classDecl)
                        symbolName = classDecl->name;
                    else if (structDecl)
                        symbolName = structDecl->name;
                    else
                        continue;

                    // ════════════════════════════════════════════════════════════════
                    // (AR) الوحدةُ وحدةُ ترجمةٍ لا قائمةَ رموز. كان الشرطُ يتخطّى كلَّ ما
                    //      لم يُطلَب صراحةً، فتُبنى الدالّةُ المُصدَّرةُ بلا مساعِداتها الخاصّة
                    //      ولا ثوابتِ وحدتها، فيفشل البناء بـ«استدعاء دالة غير معرّفة»
                    //      و«Undefined variable». أيْ استحال أن تكون للمكتبة بنيةٌ داخليّة:
                    //      كلُّ شيءٍ يجب أن يُصدَّر ويُطلَب، والثوابتُ لا سبيل إليها أصلًا.
                    //
                    //      القاعدةُ الصحيحة:
                    //        • رمزٌ مُصدَّر  ⇒ يُبنى إن طُلب (أو عند الاستيراد الشامل) —
                    //          فهو مرئيٌّ بالاسم وقد يصطدم بمحلّيٍّ في الملفّ المستورِد.
                    //        • رمزٌ خاصّ (بلا «صدّر») ⇒ يُبنى دائمًا — فهو داخليُّ الوحدة،
                    //          والمُصدَّرُ المطلوبُ يعتمد عليه. حارسُ «المحلّيُّ يفوز» أدناه
                    //          يمنع الازدواج إن تصادف اسمٌ.
                    //      ملاحظة: كانت isExported تُحسَب ولا تُستعمل قطّ — فلم يكن
                    //      للتصدير أثرٌ في المترجم. صار له معنًى هنا.
                    // (EN) A module is a compilation unit, not a symbol list. The condition
                    //      skipped everything not explicitly requested, so an exported
                    //      function was built without its private helpers or module
                    //      constants ⇒ "undefined function" / "Undefined variable". A library
                    //      could therefore have no internal structure at all.
                    //      Correct rule: exported symbols are selected by request; private
                    //      declarations are always built because the requested exports depend
                    //      on them. (isExported was computed but never used before.)
                    // ════════════════════════════════════════════════════════════════
                    if (isExported)
                    {
                        if (!isWildcard &&
                            requestedSymbols.find(symbolName) == requestedSymbols.end())
                            continue;
                    }
                    else
                    {
                        // (AR) تصريحٌ خاصّ (دالّةً كان أو ثابتًا أو صنفًا أو بنية): يُبنى إن
                        //      كان **مبلوغًا** من الرموز المطلوبة. بناءُ كلِّ خاصٍّ يُظلّل
                        //      المدمَجات (اختبار 061: وحدةٌ فيها «طول» خاصّةٌ تختطف مدمَجَ
                        //      «طول» في الملفّ المستورِد)، ويُسقط أصنافَ المستخدم صامتًا
                        //      حين يسبق صنفُ الوحدة صنفًا محلّيًّا بالاسم نفسِه.
                        // (EN) Private declaration (function, constant, class or struct): built
                        //      only if reachable from the requested symbols. Building every
                        //      private one shadows builtins (test 061) and can silently drop a
                        //      user class whose name the module also defines.
                        if (privateClosure.find(symbolName) == privateClosure.end())
                            continue;
                    }

                    // (AR) تخطي الرموز المبنية مسبقاً (لمنع التكرار عند إعادة معالجة الوحدة).
                    //      نبني هنا فقط بذرةَ استيرادٍ لم تُبنَ بعد (سجّلها
                    //      preRegisterImportedSignatures بـsirFunction=nullptr للاستنتاج).
                    //      نتخطّى إن: بُنيت فعلًا (sirFunction != nullptr)، أو كان الرمز مدخلًا
                    //      لم نبذره نحن (تسجيلُ الطور 1 لدالّة محلّيّة تحمل الاسم ذاته) — كي
                    //      يبقى المحلّيّ فائزًا ولا يُبنى الرمز مرّتين (SIRModule::addFunction
                    //      لا يُزيل التكرار).
                    // (EN) Skip already-built symbols (prevent duplication when re-processing).
                    //      Build here ONLY an as-yet-unbuilt import seed (registered by
                    //      preRegisterImportedSignatures with sirFunction=nullptr for inference).
                    //      Skip if: actually BUILT (sirFunction != nullptr), OR the entry is one
                    //      we did NOT seed (a Phase-1 local registration of the same name) — so
                    //      the local wins and the symbol isn't built twice (SIRModule::addFunction
                    //      does not de-duplicate).
                    if (funcDecl)
                    {
                        auto ftIt = b_.functionTable_.find(symbolName);
                        if (ftIt != b_.functionTable_.end())
                        {
                            bool alreadyBuilt = ftIt->second.sirFunction != nullptr;
                            bool isSeededImport =
                                b_.preRegisteredImportNames_.count(symbolName) > 0;
                            if (alreadyBuilt || !isSeededImport)
                            {
                                continue;
                            }
                        }
                    }
                    if (classDecl && b_.module_->getClass(symbolName))
                    {
                        continue;
                    }
                    // (AR) [ISSUE-026] البنية تُسجَّل أيضًا في جدول الأصناف (getClass) —
                    //      لا تُعِد بناءها إن سبق (تفادي addClass المكرّر).
                    // (EN) [ISSUE-026] A struct is also registered in the class table
                    //      (getClass) — don't rebuild it if already present (avoid double addClass).
                    if (structDecl && b_.module_->getClass(symbolName))
                    {
                        continue;
                    }

                    // (AR) بناء SIR
                    // (EN) Build SIR
                    if (funcDecl)
                    {
                        b_.buildFunction(funcDecl);
                    }
                    if (varDecl)
                    {
                        b_.buildGlobalVariable(varDecl);
                    }
                    if (classDecl)
                    {
                        b_.buildClass(classDecl);
                    }
                    // (AR) [ISSUE-026] بناء البنية المستوردة عبر buildStatement (يوجّهها إلى
                    //      statement_types.cpp:412 فتُحسَب حقولها وتُسجَّل بـaddClass/classTable_)
                    //      كما تفعل الوحدة الرئيسة في الطور 2أ-0 (ISSUE-051). تخطيط الحقول ذاتيّ
                    //      من الحقول المصرَّحة ⇒ لا حاجة لتسجيلٍ مسبق كالدوالّ.
                    // (EN) [ISSUE-026] Build the imported struct via buildStatement (routes to
                    //      statement_types.cpp:412 which computes fields and registers via
                    //      addClass/classTable_) exactly as the main module does in Phase 2A-0
                    //      (ISSUE-051). Field layout is self-contained from declared fields ⇒ no
                    //      pre-registration needed as with functions.
                    if (structDecl)
                    {
                        b_.buildStatement(structDecl);
                    }
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
