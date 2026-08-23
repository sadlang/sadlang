/**
 * @file statement_executor_modules.cpp
 * @brief (AR) تنفيذ نظام الاستيراد والتصدير في المفسر
 *        (EN) Implementation of import/export system in the interpreter
 * 
 * @details
 * (AR) هذا الملف يحتوي على تنفيذ جمل الاستيراد والتصدير:
 *      1. visitImportStmt - استيراد وحدة كاملة (مثل: استورد رياضيات)
 *      2. visitFromImportStmt - استيراد انتقائي (مثل: من رياضيات استورد جذر)
 *      3. visitExportDecl - تصدير تصريح (مثل: صدّر دالة حساب)
 *      4. visitExportStmt - تصدير تصريح (الإصدار القديم)
 * 
 *      آلية العمل:
 *      ═══════════
 *      عند مواجهة جملة استيراد:
 *      1. يُستخدم ModuleResolver للبحث عن ملف الوحدة
 *      2. يُحمَّل الملف ويُحلَّل إلى AST
 *      3. يُنفَّذ AST الوحدة في نطاق معزول (scope)
 *      4. تُستخرج الرموز المُصدَّرة (الدوال، الأصناف، المتغيرات)
 *      5. تُسجَّل الرموز في النطاق الحالي
 * 
 *      أنماط الاستيراد المدعومة:
 *      ═══════════════════════════
 *      - استورد رياضيات           → كل الصادرات كفضاء أسماء
 *      - استورد رياضيات كـ ر       → كل الصادرات كفضاء أسماء باسم مستعار
 *      - من رياضيات استورد جذر     → استيراد دالة محددة
 *      - من رياضيات استورد *       → استيراد كل الصادرات مباشرة
 *      - من رياضيات استورد جذر كـ ج → استيراد بإسم مستعار
 * 
 *      أنماط التصدير المدعومة:
 *      ═══════════════════════════
 *      - صدّر دالة حساب(...)       → تصدير دالة
 *      - صدّر صنف شخص(...)        → تصدير صنف
 *      - صدّر متغير س = 10         → تصدير متغير
 * 
 * @author فريق تطوير لغة ص / Sad Language Development Team
 * @date فبراير 2026 / February 2026
 * @version 1.0.0
 */

#include "statement_executor.h"
#include "module_nodes.h"
#include "declarations.h"
// (AR) إغلاقُ صادراتِ الوحدة — مشتركٌ مع المصرِّف / (EN) shared with the compiler
#include "module_export_closure.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "class_manager.h"
#include "builtin_module_registry.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <set>

namespace Sad {
namespace Interpreter {

// =========================================================================
// (AR) تنفيذ وحدة مُحمَّلة واستخراج رموزها المُصدَّرة
// (EN) Execute a loaded module and extract its exported symbols
// =========================================================================

Data::Value StatementExecutor::executeModuleAndExtractExports(Modules::Module* module) {
    if (!module || module->ast.empty()) {
        return Data::Value(Data::Value::MapType());
    }
    
    // (AR) التحقق من الذاكرة المخبئية - إذا سبق تنفيذ هذه الوحدة نعيد الصادرات المخبأة
    // (EN) Check cache - if module was already executed, return cached exports
    // (AR) استخدام اسم الوحدة بدلاً من المسار لتجنب مشاكل الترميز العربي
    // (EN) Use module name instead of path to avoid Arabic encoding issues
    std::string moduleKey = module->fullName;
    auto cacheIt = executedModuleExports_.find(moduleKey);
    if (cacheIt != executedModuleExports_.end()) {
        return cacheIt->second;
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) قاطعُ الدور: الخبيئةُ أعلاه تُقيَّد **بعد** اكتمالِ التنفيذ وحدَه، فهي
    //      لا تحرس التعاود. وحدتان تُعيد كلٌّ تصديرَ الأخرى تعودان إلى هنا وهما
    //      قيدَ التنفيذ ⇒ تعاودٌ بلا نهايةٍ ينتهي بانهيارِ مكدّسٍ بلا تشخيص.
    //      والعودةُ بخريطةٍ فارغةٍ — لا برفعِ خطأ — لأنّ الدورةَ ليست خطأً في
    //      ذاتها: الوحدةُ الجاريةُ ستُكمِل صادراتِها بعد عودتنا، فتلتقطها
    //      إعادةُ التصديرِ من الطرفِ الآخر. (نظيرُ `reExportsInFlight_` في المصرّف.)
    // (EN) Cycle breaker: the cache above is only populated AFTER execution
    //      completes, so it does not guard recursion. Two mutually re-exporting
    //      modules re-enter here while still in flight ⇒ unbounded recursion and
    //      a silent stack overflow. We return an EMPTY map rather than raising,
    //      because a cycle is not itself an error: the in-flight module finishes
    //      its own exports after we return, and the other side picks them up.
    //      (Mirror of the compiler's `reExportsInFlight_`.)
    // ═════════════════════════════════════════════════════════════════════
    if (!modulesInExecution_.insert(moduleKey).second) {
        cycleCutTargets_.push_back(moduleKey);
        return Data::Value(Data::Value::MapType());
    }
    // (AR) لقطةُ السجلّ: ما بعدَها هو القطوعُ الواقعةُ **تحتَ** هذا التنفيذ
    // (EN) Log snapshot: everything after it are the cuts that happened BENEATH this one
    const size_t cycleCutsBeforeExecution = cycleCutTargets_.size();
    // (AR) حارسٌ يُخرِج المفتاحَ حتمًا — حتّى إن رُفع خطأٌ من داخلِ تنفيذِ الوحدة
    // (EN) RAII guard: removes the key even if module execution throws
    struct ModuleExecutionScope {
        std::unordered_set<std::string>& inFlight;
        const std::string& key;
        ~ModuleExecutionScope() { inFlight.erase(key); }
    } moduleExecutionScope{modulesInExecution_, moduleKey};


    // (AR) حفظ الحالة الحالية لمنفذ العبارات
    // (EN) Save current state of statement executor
    auto savedFlowControl = flowControl_;
    auto savedReturnValue = returnValue_;
    auto savedExportedSymbols = exportedSymbols_;
    auto savedCurrentFilePath = currentFilePath_;
    auto savedCurrentNamespace = currentNamespace_;
    
    // (AR) تعيين مسار الملف الجديد (مسار الوحدة)
    // (EN) Set new file path (module path)
    // (AR) استخدام u8string لدعم المسارات العربية
    // (EN) Use u8string to support Arabic paths
#ifdef _WIN32
    currentFilePath_ = module->filePath.u8string();
#else
    currentFilePath_ = module->filePath.string();
#endif
    
    // (AR) مسح الرموز المُصدَّرة للوحدة الجديدة
    // (EN) Clear exported symbols for new module
    exportedSymbols_.clear();
    
    // (AR) إنشاء نطاق جديد معزول للوحدة
    // (EN) Create isolated scope for module
    scopeManager_.pushScope(Data::ScopeType::BLOCK, "module:" + module->name);
    
    // (AR) حفظ أسماء الأصناف قبل تنفيذ الوحدة لاكتشاف الأصناف الجديدة
    // (EN) Snapshot class names before module execution to detect new classes
    std::set<std::string> classNamesBefore;
    {
        auto* classManager = Data::ClassManager::getInstance();
        if (classManager) {
            auto allNames = classManager->getAllClassNames();
            classNamesBefore.insert(allNames.begin(), allNames.end());
        }
    }
    
    // (AR) حفظ أسماء الدوال قبل تنفيذ الوحدة لاكتشاف الدوال الجديدة
    // (EN) Snapshot function names before module execution to detect new functions
    std::set<std::string> funcNamesBefore;
    {
        auto allFuncNames = functionManager_.getFunctionNames();
        funcNamesBefore.insert(allFuncNames.begin(), allFuncNames.end());
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // (AR) دوالُّ الوحدةِ الخاصّة: تصريحُ دالّةٍ في أعلى شجرتِها **غيرُ** ملفوفٍ
    //      بـ«صدّر». وهي التمييزُ الذي كان ناقصًا: كتلةُ «إعادةِ التصديرِ
    //      التلقائيّ» أدناه تعيد تصديرَ كلِّ دالّةٍ جديدةٍ لتُمرِّرَ ما استوردته
    //      الوحدةُ بـ«*»، لكنّها لم تكن تفرّق بين ما **مرّ** عبر الوحدة وما
    //      **عُرّف** فيها خاصًّا ⇒ فأبطلت «صدّر» رأسًا. المعرَّفُ هنا يُستثنى.
    // (EN) The module's private functions: a top-level FunctionDecl NOT wrapped
    //      in ExportDecl. This was the missing distinction — the auto re-export
    //      block below re-exports every new function so wildcard-imported
    //      symbols pass through, but it could not tell a symbol that PASSED
    //      THROUGH from one DEFINED here privately, nullifying «صدّر».
    // ═════════════════════════════════════════════════════════════════════
    //      ويُحسَب «هل للوحدةِ تصديرٌ صريح؟» هنا قبل التنفيذ، لا أثناءه: الإغلاقُ
    //      أدناه يحتاج الجوابَ مقدَّمًا، وكان يُضبَط داخلَ حلقةِ التنفيذِ فيصل
    //      متأخّرًا عن موضعِ الحاجة.
    // (EN) "Does this module export explicitly?" is answered here, before
    //      execution rather than during it: the closure below needs the answer
    //      up front, and it used to be set inside the execution loop — too late.
    // ═════════════════════════════════════════════════════════════════════
    bool hasExplicitExports = false;
    std::set<std::string> privateFuncsDeclaredHere;
    for (const auto& declStmt : module->ast) {
        if (!declStmt) continue;
        // (AR) «صدّر تصريح» تُلَفُّ بعقدةِ `ExportDecl`/`ExportStmt`، والرايةُ
        //      `isExported` تُقرأ احتياطًا لمن يبنيها مباشرةً. المُصدَّرُ لا
        //      يُفهرَس خاصًّا، والخاصُّ وحدَه يخضع للحجب.
        // (EN) «صدّر <decl>» is wrapped in an ExportDecl/ExportStmt node; the
        //      isExported flag is read as a fallback for directly-built nodes.
        //      An export is never indexed as private; only privates are hidden.
        const AST::Statement* inner = declStmt.get();
        bool wrappedExport = false;
        if (auto* ed = dynamic_cast<AST::ExportDecl*>(declStmt.get())) {
            inner = ed->declaration.get();
            wrappedExport = true;
        } else if (auto* es = dynamic_cast<AST::ExportStmt*>(declStmt.get())) {
            inner = es->declaration.get();
            wrappedExport = true;
        }
        if (!inner) continue;

        if (auto* fnDecl = dynamic_cast<const AST::FunctionDecl*>(inner)) {
            if (wrappedExport || fnDecl->isExported) {
                hasExplicitExports = true;
                exportedSymbols_.insert(fnDecl->name);
            } else {
                privateFuncsDeclaredHere.insert(fnDecl->name);
            }
        } else if (auto* clsDecl = dynamic_cast<const AST::ClassDecl*>(inner)) {
            if (wrappedExport || clsDecl->isExported) {
                hasExplicitExports = true;
                exportedSymbols_.insert(clsDecl->name);
            }
        }
    }

    // ═════════════════════════════════════════════════════════════════════
    // (AR) لا يُحجَب كلُّ خاصّ: دالّةٌ مُصدَّرةٌ تنادي مساعدًا خاصًّا، فحجبُ
    //      المساعِدِ يُعطِب المُصدَّرَ نفسَه — أيْ تستحيل على المكتبةِ بنيةٌ
    //      داخليّة. المحجوبُ هو ما **لا يبلغه** المُصدَّرُ تعدّيًا. والحسابُ
    //      مشتركٌ مع المصرِّف (`shared/ast`)، فلا تتباعد الإجابتان.
    // (EN) Not every private symbol is hidden: an exported function calling a
    //      private helper breaks if the helper is hidden — a library could then
    //      have no internals at all. What is hidden is what the exports do NOT
    //      transitively reach. The computation is shared with the compiler
    //      (`shared/ast`), so the two answers cannot drift apart.
    // ═════════════════════════════════════════════════════════════════════
    // (AR) اسمٌ له تحميلٌ مُصدَّرٌ وآخرُ خاصّ ليس خاصًّا: الحذفُ يقع **بالاسم** فيمسح
    //      التحميلاتِ كلَّها بما فيها المُصدَّر. (`صدّر دالة س(أ)` مع `دالة س(أ، ب)`
    //      كان يُفقِد `س(41)` رأسًا.)
    // (EN) A name with both an exported and a private overload is not private: removal is
    //      BY NAME and would erase every overload including the exported one.
    for (const auto& exportedName : exportedSymbols_)
        privateFuncsDeclaredHere.erase(exportedName);

    if (hasExplicitExports) {
        const std::set<std::string> reachablePrivate =
            AST::computeExportedPrivateClosure(module->ast, {}, /*isWildcard=*/true);
        for (const auto& reached : reachablePrivate)
            privateFuncsDeclaredHere.erase(reached);
    }

    // (AR) هل هذا الرمزُ خاصٌّ بهذه الوحدة؟ يشمل الاسمَ المصرَّحَ واسمَ الإغلاقِ
    //      المشتقَّ منه (`س__closure_N`) الذي يولّده تنفيذُ التصريح.
    // (EN) Is this symbol private to this module? Covers both the declared name
    //      and the derived closure name (`f__closure_N`) execution generates.
    auto isPrivateModuleSymbol = [&privateFuncsDeclaredHere](const std::string& symbol) {
        if (privateFuncsDeclaredHere.count(symbol)) return true;
        const size_t marker = symbol.find("__closure_");
        return marker != std::string::npos &&
               privateFuncsDeclaredHere.count(symbol.substr(0, marker)) > 0;
    };

    // (AR) المرحلة 1: تنفيذ كل جمل الوحدة
    // (EN) Phase 1: Execute all module statements

    for (auto& stmt : module->ast) {
        if (!stmt) continue;
        
        // (AR) التحقق إذا كانت الوحدة تحتوي على تصدير صريح
        // (EN) Check if module has explicit exports
        if (dynamic_cast<AST::ExportDecl*>(stmt.get()) != nullptr ||
            dynamic_cast<AST::ExportStmt*>(stmt.get()) != nullptr) {
            hasExplicitExports = true;
        }
        
        // (AR) تنفيذ الجملة
        // (EN) Execute statement
        stmt->accept(*this);
        
        // (AR) إعادة تعيين التحكم بالتدفق بعد كل جملة
        // (EN) Reset flow control after each statement
        if (flowControl_ == FlowControl::RETURN) {
            flowControl_ = FlowControl::NONE;
        }
    }
    
    // (AR) المرحلة 2: جمع الرموز المُصدَّرة
    // (EN) Phase 2: Collect exported symbols
    Data::Value::MapType moduleExports;
    
    if (hasExplicitExports) {
        // (AR) تصدير صريح: فقط الرموز المحددة بـ صدّر
        // (EN) Explicit export: only symbols marked with export
        for (const auto& symbolName : exportedSymbols_) {
            // (AR) البحث عن الرمز كمتغير
            // (EN) Look for symbol as variable
            if (variableManager_.exists(symbolName)) {
                moduleExports[symbolName] = variableManager_.get(symbolName);
                continue;
            }
            
            // (AR) البحث عن الرمز كدالة
            // (EN) Look for symbol as function
            if (functionManager_.hasFunction(symbolName)) {
                moduleExports[symbolName] = Data::Value(std::string("__func__:" + symbolName));
                continue;
            }
            
            // (AR) البحث عن الرمز كصنف
            // (EN) Look for symbol as class
            auto* classManager = Data::ClassManager::getInstance();
            if (classManager && classManager->hasClass(symbolName)) {
                moduleExports[symbolName] = Data::Value(std::string("__class__:" + symbolName));
                continue;
            }
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) إعادة تصدير تلقائي: المتغيرات والأصناف المستوردة عبر * تمر عبر الوحدة
        //      المتغيرات التي تبدأ بـ _ تُعتبر خاصة ولا تُصدَّر
        // (EN) Auto re-export: variables and classes imported via * pass through
        //      Variables starting with _ are considered private and not exported
        // ═══════════════════════════════════════════════════════════════
        {
            // (AR) إعادة تصدير المتغيرات من النطاق (بما فيها المستوردة عبر *)
            // (EN) Re-export variables from scope (including wildcard-imported ones)
            auto varNames = variableManager_.getVariableNames();
            for (const auto& varName : varNames) {
                if (varName.find("__") == 0) continue;       // internal
                if (varName.find("_") == 0) continue;        // private by convention
                if (moduleExports.find(varName) != moduleExports.end()) continue;
                // (AR) هنا مكمنُ التسريب: `دالة س(...)` تُسجَّل **متغيّرَ إغلاق**
                //      باسمها لا دالّةً في مُدير الدوالّ، فكانت تمرّ من هذه
                //      الحلقةِ إلى الصادرات فتُرى خارج وحدتها رغم غياب «صدّر».
                // (EN) The actual leak: `دالة س(...)` registers as a CLOSURE
                //      VARIABLE under its own name, not as a function in the
                //      function manager, so it slipped through this loop into
                //      the exports and stayed visible without «صدّر».
                if (isPrivateModuleSymbol(varName)) continue;
                const auto* val = variableManager_.tryGet(varName);
                if (val) {
                    moduleExports[varName] = *val;
                }
            }
            
            // (AR) إعادة تصدير الأصناف الجديدة (المستوردة عبر *)
            // (EN) Re-export new classes (wildcard-imported ones)
            auto* classManager2 = Data::ClassManager::getInstance();
            if (classManager2) {
                auto allClassNames = classManager2->getAllClassNames();
                for (const auto& className : allClassNames) {
                    if (classNamesBefore.find(className) == classNamesBefore.end()) {
                        if (className.find("__") == 0) continue;
                        if (className.find("_") == 0) continue;
                        if (moduleExports.find(className) != moduleExports.end()) continue;
                        moduleExports[className] = Data::Value(std::string("__class__:" + className));
                    }
                }
            }
            
            // (AR) إعادة تصدير الدوال الجديدة (المستوردة عبر *)
            // (EN) Re-export new functions (wildcard-imported ones)
            auto allFuncNames = functionManager_.getFunctionNames();
            for (const auto& funcName : allFuncNames) {
                if (funcNamesBefore.find(funcName) == funcNamesBefore.end()) {
                    if (funcName.find("__") == 0) continue;
                    if (funcName.find("_") == 0) continue;
                    if (moduleExports.find(funcName) != moduleExports.end()) continue;
                    // (AR) عُرّفت هنا بلا «صدّر» ⇒ خاصّة، لا تُعاد تصديرًا
                    // (EN) Declared here without «صدّر» ⇒ private, not re-exported
                    if (isPrivateModuleSymbol(funcName)) continue;
                    auto overloads = functionManager_.getFunctionOverloads(funcName);
                    bool hasUserDefined = false;
                    for (const auto& funcDef : overloads) {
                        if (funcDef && !funcDef->hasNativeImplementation()) {
                            hasUserDefined = true;
                            break;
                        }
                    }
                    if (hasUserDefined) {
                        moduleExports[funcName] = Data::Value(std::string("__func__:" + funcName));
                    }
                }
            }
        }
    } else {
        // (AR) لا يوجد تصدير صريح: نصدّر كل الدوال المُعرَّفة من المستخدم
        // (EN) No explicit export: export all user-defined functions
        auto allFunctions = functionManager_.getFunctionNames();
        for (const auto& funcName : allFunctions) {
            // (AR) تجاهل الدوال الداخلية والمُضمَّنة
            // (EN) Skip internal and built-in functions
            if (funcName.find("__") == 0) continue;
            
            // (AR) نتحقق أن الدالة ليست مُضمَّنة عبر getFunctionOverloads
            // (EN) Check that function is not built-in via getFunctionOverloads
            auto overloads = functionManager_.getFunctionOverloads(funcName);
            if (overloads.empty()) {
                continue;
            }
            // (AR) إذا كان أي إصدار غير مُضمَّن، نصدّره
            // (EN) If any version is non-native, export it
            bool hasUserDefined = false;
            for (const auto& funcDef : overloads) {
                if (funcDef && !funcDef->hasNativeImplementation()) {
                    hasUserDefined = true;
                    break;
                }
            }
            if (hasUserDefined) {
                moduleExports[funcName] = Data::Value(std::string("__func__:" + funcName));
            }
        }
        
        // (AR) تصدير ضمني للأصناف المعرّفة في الوحدة
        // (EN) Implicit export of classes defined in the module
        {
            auto* classManager = Data::ClassManager::getInstance();
            if (classManager) {
                auto allClassNames = classManager->getAllClassNames();
                for (const auto& className : allClassNames) {
                    // (AR) نصدّر فقط الأصناف الجديدة التي عُرِّفت أثناء تنفيذ الوحدة
                    // (EN) Only export classes that were defined during module execution
                    if (classNamesBefore.find(className) == classNamesBefore.end()) {
                        moduleExports[className] = Data::Value(std::string("__class__:" + className));
                    }
                }
            }
        }

        // (AR) تصدير ضمني للمتغيرات/الثوابت المعرّفة في الوحدة
        // (EN) Implicit export of module-level variables/constants
        {
            auto varNames = variableManager_.getVariableNames();
            for (const auto& varName : varNames) {
                // (AR) تجاهل المتغيرات الداخلية والمُصدَّرة بالفعل
                // (EN) Skip internal variables and already-exported ones
                if (varName.find("__") == 0) continue;
                if (moduleExports.find(varName) != moduleExports.end()) continue;
                const auto* val = variableManager_.tryGet(varName);
                if (val) {
                    moduleExports[varName] = *val;
                }
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) المرحلة 3: التقاط متغيرات الوحدة وربطها بالدوال المُعرّفة
    // (EN) Phase 3: Capture module-level variables and attach to defined functions
    // ═══════════════════════════════════════════════════════════════
    {
        // (AR) جمع جميع متغيرات الوحدة (متغيرات وثوابت) من النطاق الحالي
        // (EN) Collect all module variables (vars and consts) from current scope
        std::unordered_map<std::string, Data::Value> moduleVars;
        auto varNames = variableManager_.getVariableNames();
        for (const auto& varName : varNames) {
            const auto* val = variableManager_.tryGet(varName);
            if (val) {
                moduleVars[varName] = *val;
            }
        }
        
        // (AR) إذا وُجدت متغيرات، نربطها بجميع الدوال الجديدة المُعرّفة في هذه الوحدة
        // (EN) If variables found, attach them to all new functions defined in this module
        if (!moduleVars.empty()) {
            auto allFuncNames = functionManager_.getFunctionNames();
            for (const auto& funcName : allFuncNames) {
                // (AR) نتجاهل الدوال التي كانت موجودة قبل تنفيذ الوحدة
                // (EN) Skip functions that existed before module execution
                if (funcNamesBefore.find(funcName) != funcNamesBefore.end()) {
                    continue;
                }
                // (AR) ربط المتغيرات الملتقطة بالدالة لدعم النطاق المعجمي
                // (EN) Attach captured variables to function for lexical scoping support
                auto overloads = functionManager_.getFunctionOverloads(funcName);
                for (auto& funcDef : overloads) {
                    if (funcDef && !funcDef->hasNativeImplementation()) {
                        funcDef->setCaptures(moduleVars);
                    }
                }
            }

            // (AR) ع-1: الربط نفسه لأصناف هذه الوحدة — كانت الطرق بلا التقاطٍ
            //      فيُقرأ ثابتُ الوحدةِ «لاشيء» داخلها عند الاستيراد بينما تراه
            //      الدوالُّ الحرّة. تُحقَن moduleCaptures في نطاق كل تنفيذ لجسم
            //      طريقة عبر utils/class_module_captures.h.
            // (EN) ع-1: same attachment for this module's classes — methods had
            //      no captures, so module constants read as null inside imported
            //      class methods while free functions saw them. Injected into
            //      every method body scope via utils/class_module_captures.h.
            {
                // (AR) المعيار «ملف المصدر» لا «لقطة الأسماء قبل التنفيذ»: تسجيل
                //      الصنف يقع في تمريرة رفع التصريحات **قبل** أخذ اللقطة،
                //      فكانت اللقطة تتخطى أصناف الوحدة كلها (قِيس بالتشخيص).
                //      sourceFile يُملأ عند التسجيل من currentFilePath_ وهو هنا
                //      مسار الوحدة نفسها.
                // (EN) Criterion is the class's sourceFile, not a names-before
                //      snapshot: class registration happens in the declaration
                //      hoisting pass BEFORE the snapshot, so the snapshot skipped
                //      every module class (measured). sourceFile is filled at
                //      registration from currentFilePath_, which here is the
                //      module's own path.
                auto* classManagerForCaptures = Data::ClassManager::getInstance();
                if (classManagerForCaptures) {
                    for (const auto& className : classManagerForCaptures->getAllClassNames()) {
                        auto* classDefinition = classManagerForCaptures->getClass(className);
                        if (!classDefinition) {
                            continue;
                        }
                        if (classDefinition->sourceFile != currentFilePath_) {
                            continue;
                        }
                        classDefinition->moduleCaptures = moduleVars;
                    }
                }
            }
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // (AR) دلالةُ «صدّر»: غيرُ المُصدَّرِ لا يُرى من خارجِ وحدته.
    //
    //      مصدرُ الحقيقةِ صريح (grammar/20_declarations.yaml، gr.decl.export):
    //      «صدّر تسبق تصريحًا فتُتيحه للاستيراد من وحدات أخرى» ⇒ فما لم يُصدَّر
    //      لا يُتاح. وكان المفسّرُ يخالف ذلك بحكم آليّته لا بقرار: تنفيذُ جملِ
    //      الوحدةِ يسجّل **كلَّ** دوالّها في مُديرِ الدوالِّ العامّ، فتبقى منظورةً
    //      بعد انتهاء الوحدة سواءٌ صُدّرت أم لا. فدالّةٌ خاصّةٌ في وحدةٍ تظهر في
    //      الملفّ المستورِد — والمصرِّفُ يرفضها ⇒ تباعدٌ صريح.
    //
    //      نحذف هنا ما سجّلته الوحدةُ ولم تُصدِّره، حصرًا (funcNamesBefore يمنع
    //      المساسَ بما كان مسجَّلًا قبلها: المدمَجاتُ ودوالُّ المستورِد).
    // (EN) «صدّر» semantics: a non-exported declaration is invisible outside its
    //      module. The SoT is explicit — export is what makes a declaration
    //      importable. The interpreter violated this mechanically, not by design:
    //      executing the module's statements registers EVERY function in the global
    //      function manager, so private ones stayed visible while the compiler
    //      rejected them. We drop exactly what this module added and did not export;
    //      funcNamesBefore guarantees nothing pre-existing is touched.
    // ═════════════════════════════════════════════════════════════════════
    // (AR) الحجبُ مشروطٌ بأن تستعمل الوحدةُ «صدّر» أصلًا. فوحدةٌ لم تُصدِّر شيئًا
    //      قطُّ تبقى تُتيح كلَّ دوالِّها (العُرفُ القائم، ويعتمد عليه
    //      `_وحدة_تمرير_المصفوفات.ص`)؛ وبلا هذا الشرطِ كانت تُمسَح دوالُّها
    //      كلُّها فيصير فرعُ «التصديرِ الضمنيِّ» أعلاه بناءً على ما حُذف لتوّه.
    // (EN) Hiding applies only if the module uses «صدّر» at all. A module that never
    //      exports keeps every function visible (the existing convention, relied on by
    //      `_وحدة_تمرير_المصفوفات.ص`); without this guard all of them were erased and
    //      the implicit-export branch above built exports from just-deleted functions.
    if (hasExplicitExports)
    {
        // (AR) نحذف ما عرّفته هذه الوحدةُ **خاصًّا** حصرًا — لا كلَّ جديدٍ غيرِ
        //      مُصدَّر. فالوحدةُ قد تستورد وحدةً مدمَجةً (`استورد شبكة`) فتُسجَّل
        //      مدمَجاتُها جديدةً وغيرَ مُصدَّرة، وحذفُها يُفقِد المكتبةَ أدواتِها
        //      بعد سطرٍ واحدٍ من استعمالها (انهارت `stdlib/شبكات.ص` هكذا).
        // (EN) Drop only what this module declared PRIVATE, not everything new
        //      and unexported: a module may `استورد` a builtin module, whose
        //      functions are then new and unexported — deleting them stripped
        //      the library of its own tools (this broke stdlib/شبكات.ص).
        auto allFuncNamesNow = functionManager_.getFunctionNames();
        for (const auto& funcName : allFuncNamesNow) {
            if (funcNamesBefore.find(funcName) != funcNamesBefore.end()) continue;
            if (!isPrivateModuleSymbol(funcName)) continue;
            functionManager_.removeFunction(funcName);
        }
    }

    // (AR) الخروج من نطاق الوحدة
    // (EN) Exit module scope
    scopeManager_.popScope();

    // (AR) استعادة الحالة السابقة
    // (EN) Restore previous state
    flowControl_ = savedFlowControl;
    returnValue_ = savedReturnValue;
    exportedSymbols_ = savedExportedSymbols;
    currentFilePath_ = savedCurrentFilePath;
    currentNamespace_ = savedCurrentNamespace;
    
    // (AR) تخزين النتيجة في الذاكرة المخبئية — **إلّا** إن قُطع دورٌ تحت هذا
    //      التنفيذ، فصادراتُنا حينئذٍ ناقصةٌ بحكمِ القطعِ لا بحكمِ الوحدة، وتخبئتُها
    //      تُثبّت النقصَ فيصير الناتجُ رهينَ ترتيبِ سطورِ الاستيراد.
    // (EN) Cache the result — UNLESS a cycle was cut beneath this execution, in which
    //      case our exports are incomplete by virtue of the cut, not of the module;
    //      caching them freezes the loss and makes the result order-dependent.
    Data::Value result(moduleExports);
    bool cutTargetedAnAncestor = false;
    for (size_t cut = cycleCutsBeforeExecution; cut < cycleCutTargets_.size(); ++cut) {
        if (cycleCutTargets_[cut] != moduleKey) {
            cutTargetedAnAncestor = true;
            break;
        }
    }
    if (!cutTargetedAnAncestor) {
        executedModuleExports_[moduleKey] = result;
    }
    
    return result;
}

// =========================================================================
// (AR) جملة الاستيراد الكامل: استورد وحدة [كـ اسم_مستعار]
// (EN) Full import statement: import module [as alias]
// =========================================================================

void StatementExecutor::visitImportStmt(AST::ImportStmt& node) {
    // (AR) التحقق من وجود محلل الوحدات
    // (EN) Check module resolver exists
    if (!moduleResolver_) {
        throw ExecutionError(
            "خطأ: نظام الاستيراد غير مُهيَّأ. لا يوجد محلل وحدات.\n"
            "Error: Import system not initialized. No module resolver."
        );
    }
    
    // (AR) بناء الاسم الكامل للوحدة من المسار
    // (EN) Build full module name from path
    std::string fullModuleName = node.getFullModuleName();
    
    // (AR) تحديد الاسم الفعلي (الاسم المستعار أو اسم الوحدة)
    // (EN) Determine effective name (alias or module name)
    std::string effectiveName = node.getEffectiveName();
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) التحقق من الوحدات المُضمّنة أولاً (تحميل كسول)
    // (EN) Check builtin modules first (lazy loading)
    // ═══════════════════════════════════════════════════════════════
    auto& builtinRegistry = BuiltinModuleRegistry::getInstance();
    if (builtinRegistry.isBuiltinModule(fullModuleName)) {
        // (AR) تحميل الوحدة المُضمّنة (يسجّل الدوال في FunctionManager)
        // (EN) Load builtin module (registers functions in FunctionManager)
        builtinRegistry.loadModule(fullModuleName);
        
        // (AR) بناء خريطة الصادرات من الدوال المسجلة
        // (EN) Build exports map from registered functions
        Data::Value::MapType exportsMap;
        for (const auto& funcName : builtinRegistry.getExportedFunctions(fullModuleName)) {
            exportsMap[funcName] = Data::Value(std::string("__func__:" + funcName));
        }
        
        Data::Value moduleExports(exportsMap);
        loadedModuleNamespaces_[effectiveName] = moduleExports;
        
        // (AR) تحسين أداء: بحث واحد / (EN) Performance: single lookup
        variableManager_.defineOrAssign(effectiveName, moduleExports);
        return;
    }
    
    // (AR) استخدام ModuleResolver للبحث عن الوحدة وتحميلها
    // (EN) Use ModuleResolver to find and load module
    Modules::Module* module = moduleResolver_->resolveModule(
        node.modulePath,
        currentFilePath_
    );
    
    if (!module) {
        // (AR) خطأ: الوحدة غير موجودة — مع اقتراحات ذكية
        // (EN) Error: Module not found — with smart suggestions
        std::string searchedPaths;
        for (const auto& path : moduleResolver_->getSearchPaths()) {
            searchedPaths += "  - " + path.string() + "\n";
        }
        
        // (AR) البحث عن وحدات مشابهة في نفس المجلد
        // (EN) Search for similar modules in the same directory
        std::string suggestions;
        if (node.modulePath.size() >= 2) {
            // (AR) بناء المسار الأب (مثل رسومات.أساس)
            // (EN) Build parent path (e.g. graphics.base)
            std::vector<std::string> parentPath(node.modulePath.begin(), node.modulePath.end() - 1);
            std::string targetName = node.modulePath.back();
            
            for (const auto& searchPath : moduleResolver_->getSearchPaths()) {
                auto dirPath = searchPath;
                for (const auto& part : parentPath) {
                    dirPath /= part;
                }
                if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath)) {
                    std::vector<std::string> available;
                    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                        if (entry.path().extension().string() == ".ص" ||
                            entry.path().extension().string() == ".\xd8\xb5") {
                            available.push_back(entry.path().stem().string());
                        }
                    }
                    if (!available.empty()) {
                        suggestions += "الوحدات المتوفرة في '" + parentPath.back() + "':\n";
                        for (const auto& name : available) {
                            suggestions += "  - " + name + "\n";
                        }
                    }
                    break;
                }
            }
        }
        
        std::string errorMsg = 
            "خطأ: لم يتم العثور على الوحدة '" + fullModuleName + "'\n"
            "المسارات التي تم البحث فيها:\n" + searchedPaths;
        
        if (!suggestions.empty()) {
            errorMsg += suggestions;
        }
        
        errorMsg += "تأكد من أن الملف '" + fullModuleName + ".ص' موجود في أحد هذه المسارات.";
        
        throw ExecutionError(errorMsg);
    }
    
    // (AR) تنفيذ AST الوحدة واستخراج الرموز المُصدَّرة
    // (EN) Execute module AST and extract exported symbols
    Data::Value moduleExports = executeModuleAndExtractExports(module);
    
    // (AR) تسجيل الوحدة كمتغير Map في النطاق الحالي
    // (EN) Register module as Map variable in current scope
    if (moduleExports.isMap()) {
        // (AR) حفظ فضاء أسماء الوحدة
        // (EN) Save module namespace
        loadedModuleNamespaces_[effectiveName] = moduleExports;
        
        // (AR) تسجيل كمتغير Map
        // (EN) Register as Map variable
        variableManager_.defineOrAssign(effectiveName, moduleExports);
    }
}

// =========================================================================
// (AR) جملة الاستيراد الانتقائي: من وحدة استورد رمز1، رمز2
// (EN) Selective import statement: from module import symbol1, symbol2
// =========================================================================

void StatementExecutor::visitFromImportStmt(AST::FromImportStmt& node) {
    // (AR) التحقق من وجود محلل الوحدات
    // (EN) Check module resolver exists
    if (!moduleResolver_) {
        throw ExecutionError(
            "خطأ: نظام الاستيراد غير مُهيَّأ. لا يوجد محلل وحدات.\n"
            "Error: Import system not initialized. No module resolver."
        );
    }
    
    // (AR) بناء الاسم الكامل للوحدة
    // (EN) Build full module name
    std::string fullModuleName = node.getFullModuleName();
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) التحقق من الوحدات المُضمّنة أولاً (تحميل كسول)
    // (EN) Check builtin modules first (lazy loading)
    // ═══════════════════════════════════════════════════════════════
    auto& builtinRegistry = BuiltinModuleRegistry::getInstance();
    if (builtinRegistry.isBuiltinModule(fullModuleName)) {
        // (AR) الاستيرادُ الانتقائيُّ يُبلَّغ للسجلّ فلا يبقى مرئيًّا إلّا ما طُلِب.
        //      كان يُسجَّل كلُّ محتوى الوحدةِ ثمّ يُتحقَّقُ من وجودِ الأسماءِ فقط —
        //      فحواجزُ الأسماءِ المتصادمةِ تُكسَرُ لبرامجَ لم تطلبها (قِيس: `رقم`).
        // (EN) The selective import is now reported to the registry so only the requested
        //      names stay visible. Previously the whole module was registered and the
        //      selective clause merely *verified* the names existed — so a colliding name
        //      broke programs that never asked for it (measured on `رقم`).
        std::vector<std::string> requestedNames;
        if (!node.isWildcard) {
            for (const auto& item : node.items) {
                requestedNames.push_back(item.name);
            }
        }
        builtinRegistry.loadModule(fullModuleName, requestedNames);
        
        // (AR) بناء خريطة الصادرات
        // (EN) Build exports map
        Data::Value::MapType exportsMap;
        for (const auto& funcName : builtinRegistry.getExportedFunctions(fullModuleName)) {
            exportsMap[funcName] = Data::Value(std::string("__func__:" + funcName));
        }
        
        if (node.isWildcard) {
            // (AR) من وحدة استورد * — الدوال مسجلة بالفعل في FunctionManager
            // (EN) from module import * — functions already registered in FunctionManager
            // No additional action needed since functions are globally available
        } else {
            // (AR) استيراد انتقائي — التحقق من وجود الأسماء المطلوبة
            // (EN) Selective import — verify requested names exist
            for (const auto& item : node.items) {
                if (exportsMap.find(item.name) == exportsMap.end()) {
                    throw ExecutionError(
                        "\u062e\u0637\u0623: \u0627\u0644\u0631\u0645\u0632 '" + item.name + 
                        "' \u063a\u064a\u0631 \u0645\u0648\u062c\u0648\u062f \u0641\u064a \u0627\u0644\u0648\u062d\u062f\u0629 \u0627\u0644\u0645\u0636\u0645\u0646\u0629 '" + fullModuleName + "'\n"
                        "Error: Symbol '" + item.name + "' not found in builtin module '" + fullModuleName + "'"
                    );
                }
            }
        }
        return;
    }
    
    // (AR) تحميل الوحدة
    // (EN) Load module
    Modules::Module* module = moduleResolver_->resolveModule(
        node.modulePath,
        currentFilePath_
    );
    
    if (!module) {
        // (AR) خطأ: الوحدة غير موجودة — مع اقتراحات
        // (EN) Error: Module not found — with suggestions
        std::string searchedPaths;
        for (const auto& path : moduleResolver_->getSearchPaths()) {
            searchedPaths += "  - " + path.string() + "\n";
        }
        
        // (AR) البحث عن وحدات مشابهة
        // (EN) Search for similar modules
        std::string suggestions;
        if (node.modulePath.size() >= 2) {
            std::vector<std::string> parentPath(node.modulePath.begin(), node.modulePath.end() - 1);
            for (const auto& searchPath : moduleResolver_->getSearchPaths()) {
                auto dirPath = searchPath;
                for (const auto& part : parentPath) {
                    dirPath /= part;
                }
                if (std::filesystem::exists(dirPath) && std::filesystem::is_directory(dirPath)) {
                    std::vector<std::string> available;
                    for (const auto& entry : std::filesystem::directory_iterator(dirPath)) {
                        if (entry.path().extension().string() == ".ص" ||
                            entry.path().extension().string() == ".\xd8\xb5") {
                            available.push_back(entry.path().stem().string());
                        }
                    }
                    if (!available.empty()) {
                        suggestions += "الوحدات المتوفرة في '" + parentPath.back() + "':\n";
                        for (const auto& name : available) {
                            suggestions += "  - " + name + "\n";
                        }
                    }
                    break;
                }
            }
        }
        
        std::string errorMsg = 
            "خطأ: لم يتم العثور على الوحدة '" + fullModuleName + "'\n"
            "المسارات التي تم البحث فيها:\n" + searchedPaths;
        
        if (!suggestions.empty()) {
            errorMsg += suggestions;
        }
        
        errorMsg += "تأكد من صحة مسار الوحدة. الصيغة: استورد * من اسم_الوحدة";
        
        throw ExecutionError(errorMsg);
    }
    
    // (AR) تنفيذ AST الوحدة واستخراج الصادرات
    // (EN) Execute module AST and extract exports
    Data::Value moduleExports = executeModuleAndExtractExports(module);
    
    if (!moduleExports.isMap()) {
        throw ExecutionError(
            "خطأ: فشل في استخراج صادرات الوحدة '" + fullModuleName + "'"
        );
    }
    
    const auto& exportsMap = moduleExports.toMap();
    
    if (node.isWildcard) {
        // ═══════════════════════════════════════════════════════════════
        // (AR) استيراد عام: من وحدة استورد *
        // (EN) Wildcard import: from module import *
        // ═══════════════════════════════════════════════════════════════
        
        for (const auto& [name, value] : exportsMap) {
            if (value.isString() && value.toString().find("__func__:") == 0) {
                // (AR) الدالة مُسجَّلة بالفعل في FunctionManager
                // (EN) Function already registered in FunctionManager
                continue;
            }
            
            // (AR) تسجيل المتغير
            // (EN) Register variable
            variableManager_.defineOrAssign(name, value);
        }
        
    } else {
        // ═══════════════════════════════════════════════════════════════
        // (AR) استيراد انتقائي: من وحدة استورد رمز1، رمز2
        // (EN) Selective import: from module import sym1, sym2
        // ═══════════════════════════════════════════════════════════════
        
        for (const auto& item : node.items) {
            std::string originalName = item.name;
            std::string importedName = item.getEffectiveName();
            
            // (AR) البحث عن الرمز في صادرات الوحدة
            // (EN) Look for symbol in module exports
            auto it = exportsMap.find(originalName);
            
            if (it != exportsMap.end()) {
                const Data::Value& value = it->second;
                
                if (value.isString() && value.toString().find("__func__:") == 0) {
                    // (AR) دالة مُسجَّلة - إذا كان هناك اسم مستعار نسجل بالاسم الجديد
                    // (EN) Registered function - if alias, register with new name
                    if (item.alias.has_value() && item.alias.value() != originalName) {
                        // (AR) نستخدم getFunctionOverloads للحصول على كل إصدارات الدالة
                        // (EN) Use getFunctionOverloads to get all versions of the function
                        auto overloads = functionManager_.getFunctionOverloads(originalName);
                        for (const auto& funcDef : overloads) {
                            if (funcDef && funcDef->getBody()) {
                                functionManager_.defineFunction(
                                    importedName,
                                    funcDef->getParameters(),
                                    funcDef->getBody(),
                                    funcDef->getFunctionDecl()
                                );
                            }
                        }
                    }
                    continue;
                }
                
                // (AR) متغير - تسجيل بالاسم الفعلي
                // (EN) Variable - register with effective name
                variableManager_.defineOrAssign(importedName, value);
                
            } else {
                // (AR) غير موجود في الخريطة - نتحقق من FunctionManager
                // (EN) Not in map - check FunctionManager
                if (functionManager_.hasFunction(originalName)) {
                    if (item.alias.has_value() && item.alias.value() != originalName) {
                        auto overloads = functionManager_.getFunctionOverloads(originalName);
                        for (const auto& funcDef : overloads) {
                            if (funcDef && funcDef->getBody()) {
                                functionManager_.defineFunction(
                                    importedName,
                                    funcDef->getParameters(),
                                    funcDef->getBody(),
                                    funcDef->getFunctionDecl()
                                );
                            }
                        }
                    }
                } else {
                    // (AR) الرمز غير موجود
                    // (EN) Symbol not found
                    std::string availableSymbols;
                    for (const auto& [symName, _] : exportsMap) {
                        availableSymbols += "  - " + symName + "\n";
                    }
                    
                    throw ExecutionError(
                        "خطأ: الرمز '" + originalName + "' غير موجود في الوحدة '" + fullModuleName + "'\n"
                        "الرموز المتوفرة:\n" + availableSymbols
                    );
                }
            }
        }
    }
}

// =========================================================================
// (AR) جملة التصدير (الإصدار الجديد): صدّر دالة|صنف|متغير ...
// (EN) Export declaration (new version): export function|class|var ...
// =========================================================================

void StatementExecutor::visitExportDecl(AST::ExportDecl& node) {
    if (!node.declaration) {
        return;
    }
    
    // (AR) تنفيذ التصريح الداخلي (يُسجَّل في المدير المناسب)
    // (EN) Execute inner declaration (registers in appropriate manager)
    node.declaration->accept(*this);
    
    // (AR) استخراج اسم الرمز المُصدَّر وتمييزه
    // (EN) Extract exported symbol name and mark it
    std::string exportedName = node.getExportedName();
    
    if (!exportedName.empty()) {
        exportedSymbols_.insert(exportedName);
    }
}

// =========================================================================
// (AR) جملة التصدير (الإصدار القديم للتوافق)
// (EN) Export statement (legacy version for compatibility)
// =========================================================================

void StatementExecutor::visitExportStmt(AST::ExportStmt& node) {
    if (!node.declaration) {
        return;
    }
    
    // (AR) تنفيذ التصريح الداخلي
    // (EN) Execute inner declaration
    node.declaration->accept(*this);
    
    // (AR) استخراج اسم الرمز المُصدَّر
    // (EN) Extract exported symbol name
    std::string exportedName;
    
    if (auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(node.declaration.get())) {
        exportedName = funcDecl->name;
    } else if (auto* classDecl = dynamic_cast<AST::ClassDecl*>(node.declaration.get())) {
        exportedName = classDecl->name;
    } else if (auto* varDecl = dynamic_cast<AST::VarDeclStmt*>(node.declaration.get())) {
        exportedName = varDecl->name;
    } else if (auto* enumDecl = dynamic_cast<AST::EnumDecl*>(node.declaration.get())) {
        exportedName = enumDecl->name;
    } else if (auto* structDecl = dynamic_cast<AST::StructDecl*>(node.declaration.get())) {
        exportedName = structDecl->name;
    }
    
    if (!exportedName.empty()) {
        exportedSymbols_.insert(exportedName);
    }
}

// =========================================================================
// (AR) إعادة التصدير: صدّر * من وحدة / صدّر عنصر1، عنصر2 من وحدة
// (EN) Re-export: export * from module / export item1, item2 from module
// =========================================================================

void StatementExecutor::visitReExportStmt(AST::ReExportStmt& node) {
    // (AR) صدّر * مجرّد (مسار فارغ) = «صدّر كل رموز الوحدة الحاليّة». لا وحدة
    //      مصدرٍ خارجيّة تُحمَّل؛ الرموز معرَّفة في هذه الوحدة أصلًا فلا عمل هنا.
    //      (عمليًّا لا يصل المفسّر لهذه الحالة عبر «استورد رسومات» لأنّ «رسومات»
    //      وحدة مدمجة تُسجَّل أصلًا في C++ ولا تُحلَّل من مصدر .ص؛ لكن نحرسها
    //      للاتّساق مع المحلّل المشترك ولأيّ مصدر .ص يستعمل «صدّر *» — RFC 0001، P0-3.)
    // (EN) Bare export * (empty path) = export all current-module symbols.
    //      No external module to load → no-op (parity with the shared parser).
    //      In practice unreached via «استورد رسومات» (built-in module, never
    //      parsed from .ص source), but guarded for any .ص source using «صدّر *».
    if (node.modulePath.empty()) {
        return;
    }

    // (AR) نحتاج محلل الوحدات
    if (!moduleResolver_) {
        throw ExecutionError(
            "خطأ: نظام الاستيراد غير مُهيَّأ. لا يوجد محلل وحدات.\n"
            "Error: Import system not initialized. No module resolver."
        );
    }
    
    // (AR) بناء الاسم الكامل للوحدة
    // (AR) الوصلُ الموحَّدُ في `shared/ast` — يحرس البادئةَ النسبيّة (ISSUE-089-ب)
    // (EN) The unified join in `shared/ast` — guards the relative prefix
    std::string fullModuleName = AST::joinModulePathToFullName(node.modulePath);
    
    // (AR) التحقق من الوحدات المُضمّنة
    auto& builtinRegistry = BuiltinModuleRegistry::getInstance();
    if (builtinRegistry.isBuiltinModule(fullModuleName)) {
        builtinRegistry.loadModule(fullModuleName);
        
        auto exportedFuncs = builtinRegistry.getExportedFunctions(fullModuleName);
        if (node.isWildcard) {
            for (const auto& funcName : exportedFuncs) {
                exportedSymbols_.insert(funcName);
            }
        } else {
            for (const auto& item : node.items) {
                if (std::find(exportedFuncs.begin(), exportedFuncs.end(), item.name) != exportedFuncs.end()) {
                    std::string effectiveName = item.alias.has_value() ? item.alias.value() : item.name;
                    exportedSymbols_.insert(effectiveName);
                } else {
                    throw ExecutionError(
                        "خطأ: الرمز '" + item.name + "' غير موجود في الوحدة المضمنة '" + fullModuleName + "'"
                    );
                }
            }
        }
        return;
    }
    
    // (AR) تحميل الوحدة
    Modules::Module* module = moduleResolver_->resolveModule(
        node.modulePath,
        currentFilePath_
    );
    
    if (!module) {
        throw ExecutionError(
            "خطأ: لم يتم العثور على الوحدة '" + fullModuleName + "' لإعادة التصدير.\n"
            "Error: Module '" + fullModuleName + "' not found for re-export."
        );
    }
    
    // (AR) تنفيذ الوحدة واستخراج صادراتها
    Data::Value moduleExports = executeModuleAndExtractExports(module);
    
    if (!moduleExports.isMap()) {
        throw ExecutionError(
            "خطأ: فشل في استخراج صادرات الوحدة '" + fullModuleName + "' لإعادة التصدير."
        );
    }
    
    const auto& exportsMap = moduleExports.toMap();
    
    if (node.isWildcard) {
        // (AR) صدّر * من وحدة — إعادة تصدير كل شيء
        for (const auto& [name, value] : exportsMap) {
            // (AR) تسجيل الرمز في النطاق الحالي
            if (value.isString() && value.toString().find("__func__:") == 0) {
                // (AR) الدالة مُسجَّلة بالفعل في FunctionManager
            } else if (value.isString() && value.toString().find("__class__:") == 0) {
                // (AR) الصنف مُسجَّل بالفعل في ClassManager
            } else {
                // (AR) متغير/ثابت — نسجله في النطاق الحالي
                variableManager_.defineOrAssign(name, value);
            }
            exportedSymbols_.insert(name);
        }
    } else {
        // (AR) صدّر عنصر1، عنصر2 من وحدة — إعادة تصدير انتقائية
        for (const auto& item : node.items) {
            std::string effectiveName = item.alias.has_value() ? item.alias.value() : item.name;
            
            auto it = exportsMap.find(item.name);
            if (it != exportsMap.end()) {
                const Data::Value& value = it->second;
                
                if (value.isString() && value.toString().find("__func__:") == 0) {
                    // (AR) دالة — إذا كان هناك اسم مستعار نسجل نسخة
                    if (item.alias.has_value() && item.alias.value() != item.name) {
                        auto overloads = functionManager_.getFunctionOverloads(item.name);
                        for (const auto& funcDef : overloads) {
                            if (funcDef && funcDef->getBody()) {
                                functionManager_.defineFunction(
                                    effectiveName,
                                    funcDef->getParameters(),
                                    funcDef->getBody(),
                                    funcDef->getFunctionDecl()
                                );
                            }
                        }
                    }
                } else if (value.isString() && value.toString().find("__class__:") == 0) {
                    // (AR) صنف — مُسجَّل بالفعل
                } else {
                    variableManager_.defineOrAssign(effectiveName, value);
                }
                
                exportedSymbols_.insert(effectiveName);
            } else if (functionManager_.hasFunction(item.name)) {
                if (item.alias.has_value() && item.alias.value() != item.name) {
                    auto overloads = functionManager_.getFunctionOverloads(item.name);
                    for (const auto& funcDef : overloads) {
                        if (funcDef && funcDef->getBody()) {
                            functionManager_.defineFunction(
                                effectiveName,
                                funcDef->getParameters(),
                                funcDef->getBody(),
                                funcDef->getFunctionDecl()
                            );
                        }
                    }
                }
                exportedSymbols_.insert(effectiveName);
            } else {
                std::string availableSymbols;
                for (const auto& [symName, _] : exportsMap) {
                    availableSymbols += "  - " + symName + "\n";
                }
                throw ExecutionError(
                    "خطأ: الرمز '" + item.name + "' غير موجود في الوحدة '" + fullModuleName + "' لإعادة التصدير.\n"
                    "الرموز المتوفرة:\n" + availableSymbols
                );
            }
        }
    }
}

} // namespace Interpreter
} // namespace Sad
