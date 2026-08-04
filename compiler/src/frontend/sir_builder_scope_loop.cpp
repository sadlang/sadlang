// ============================================================================
// sir_builder_scope_loop.cpp — إدارة النطاقات والحلقات
// (AR) enterScope, exitScope, addVariable, lookupVariable, enterLoop, exitLoop, generic scopes
// (EN) Scope management, variable management, loop context, generic scopes
// تم استخراج هذا الملف من sir_builder_helpers.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <limits>
#include <functional>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            void SIRBuilder::enterScope()
            {
                // (AR) زيادة مستوى النطاق (sir_builder.h:599 - currentScopeLevel_)
                // (EN) Increase scope level
                currentScopeLevel_++;

                // (AR) إضافة نطاق جديد (sir_builder.h:714 - scopeStack_)
                // (EN) Add new scope
                scopeStack_.push_back(std::unordered_map<std::string, VariableInfo>());
            }

            // ============================================================================
            // exitScope - الخروج من النطاق الحالي
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:589
            // التوقيع / Signature: void exitScope();
            //
            // المتغيرات المستخدمة / Used variables:
            // - currentScopeLevel_: sir_builder.h:599 (int)
            // - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
            // ============================================================================
            void SIRBuilder::exitScope()
            {
                // (AR) إزالة النطاق الأخير (sir_builder.h:714 - scopeStack_)
                // (EN) Remove last scope
                if (!scopeStack_.empty())
                {
                    scopeStack_.pop_back();
                }

                // (AR) تقليل مستوى النطاق (sir_builder.h:599 - currentScopeLevel_)
                // (EN) Decrease scope level
                if (currentScopeLevel_ > 0)
                {
                    currentScopeLevel_--;
                }
            }

            // ============================================================================
            // addVariable - إضافة متغير للنطاق الحالي
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:591
            // التوقيع / Signature: void addVariable(const VariableInfo& varInfo);
            //
            // المعاملات / Parameters:
            // - varInfo: const VariableInfo& = sir_builder.h:139 (VariableInfo struct)
            //
            // المتغيرات المستخدمة / Used variables:
            // - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
            // ============================================================================
            void SIRBuilder::addVariable(const VariableInfo &varInfo)
            {
                // (AR) إضافة المتغير للنطاق الحالي (sir_builder.h:714 - scopeStack_)
                // (EN) Add variable to current scope
                if (!scopeStack_.empty())
                {
                    scopeStack_.back()[varInfo.name] = varInfo;
                }
            }

            // ============================================================================
            // lookupVariable - البحث عن متغير في النطاقات
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:597
            // التوقيع / Signature: VariableInfo* lookupVariable(const std::string& name);
            //
            // المعاملات / Parameters:
            // - name: const std::string& (اسم المتغير)
            //
            // المتغيرات المستخدمة / Used variables:
            // - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
            //
            // الإرجاع / Returns:
            // - VariableInfo*: مؤشر للمتغير أو nullptr
            // ============================================================================
            VariableInfo *SIRBuilder::lookupVariable(const std::string &name)
            {
                // (AR) البحث في النطاقات من الأحدث للأقدم (sir_builder.h:714 - scopeStack_)
                // (EN) Search in scopes from newest to oldest
                for (auto scopeIt = scopeStack_.rbegin(); scopeIt != scopeStack_.rend(); ++scopeIt)
                {
                    auto it = scopeIt->find(name);
                    if (it != scopeIt->end())
                    {
                        return &(it->second);
                    }
                }

                // =====================================================================
                // (AR) البحث في المتغيرات العامة على مستوى الوحدة
                // (EN) Fallback: search in module-level global variables
                // عندما لا يُعثر على المتغير في النطاقات المحلية، نبحث في الوحدة
                // When not found in local scopes, search in module globals
                // =====================================================================
                if (module_)
                {
                    auto globalVar = module_->getGlobalVariable(name);
                    if (globalVar)
                    {
                        // (AR) إنشاء VariableInfo للمتغير العام وتخزينه في أول نطاق
                        // (EN) Create VariableInfo for global var and cache in first scope
                        VariableInfo globalInfo;
                        globalInfo.name = name;
                        globalInfo.type = globalVar->getType();
                        globalInfo.registerName = name; // use same name as LLVM global
                        globalInfo.isGlobal = true;
                        globalInfo.isMutable = !globalVar->getIsConstant();
                        globalInfo.scopeLevel = 0;

                        // (AR) تخزين في أقدم نطاق للبحث السريع لاحقاً
                        // (EN) Cache in oldest scope for fast future lookups
                        if (!scopeStack_.empty())
                        {
                            scopeStack_.front()[name] = globalInfo;
                            return &scopeStack_.front()[name];
                        }
                    }
                }

                // =====================================================================
                // (AR) احتياطيُّ الأسماء المستعارة: «من م استورد قيمة كـ ق» يبني العامَّ
                //      باسمه الأصليّ، فيُحَلُّ المستعارُ إليه هنا. الحلُّ محصورٌ في عوامّ
                //      الوحدة عمدًا — الرمزُ المستورَدُ عامٌّ دائمًا — فلا يمسح النطاقاتِ
                //      المحلّيّة كي لا يُختطف متغيّرٌ محلّيٌّ يصادف الاسمَ الأصليَّ فتُقرأ
                //      قيمةٌ خاطئةٌ صامتةً بدل خطأٍ صريح.
                //      وسقفُ القفزات وحدَه هو ما يقطع الدورةَ (ق⇒ك⇒ق): لا حارسَ زيارةٍ هنا.
                // (EN) Alias fallback: «from m import قيمة as ق» builds the global under its
                //      original name, so the alias resolves to it here. Deliberately limited
                //      to module globals — an imported symbol is always global — so it never
                //      scans local scopes and cannot capture an unrelated local sharing the
                //      original name (which would yield a silently wrong value instead of a
                //      clear error). The hop cap alone breaks cycles (ق⇒ك⇒ق).
                // =====================================================================
                if (!importAliases_.empty())
                {
                    constexpr int kMaxAliasHops = 8;
                    std::string resolved = name;
                    for (int hop = 0; hop < kMaxAliasHops; ++hop)
                    {
                        auto aliasIt = importAliases_.find(resolved);
                        if (aliasIt == importAliases_.end() || aliasIt->second == resolved)
                            break;
                        resolved = aliasIt->second;

                        if (!module_)
                            break;
                        auto globalVar = module_->getGlobalVariable(resolved);
                        if (globalVar && !scopeStack_.empty())
                        {
                            VariableInfo aliasInfo;
                            aliasInfo.name = name;
                            aliasInfo.type = globalVar->getType();
                            aliasInfo.registerName = resolved;
                            aliasInfo.isGlobal = true;
                            aliasInfo.isMutable = !globalVar->getIsConstant();
                            aliasInfo.scopeLevel = 0;
                            // (AR) التخبئةُ في النطاق العامّ **الدائم**، فتُمسَك لتُمحى
                            //      عند مغادرة نطاقِ الوحدة — وإلّا عبَر الربطُ حدَّها.
                            // (EN) Cached in the PERMANENT global scope, so the key is
                            //      tracked for erasure when the module's scope exits.
                            scopeStack_.front()[name] = aliasInfo;
                            aliasSeededGlobals_.insert(name);
                            return &scopeStack_.front()[name];
                        }
                    }
                }

                // (AR) لم يُعثر على المتغير
                // (EN) Variable not found
                return nullptr;
            }

            bool SIRBuilder::isModuleQualifiedSymbol(const std::string &namespaceName,
                                                     const std::string &symbolName,
                                                     std::string &ambiguityDiagnostic)
            {
                ambiguityDiagnostic.clear();

                if (moduleNamespaces_.empty())
                    return false;

                auto namespaceIt = moduleNamespaces_.find(namespaceName);
                if (namespaceIt == moduleNamespaces_.end())
                    return false;

                // (AR) الشرط ٢: المتغيّرُ الحقيقيُّ يفوز على فضاءِ الوحدة.
                // (EN) Condition 2: a real variable wins over the module namespace.
                if (lookupVariable(namespaceName) != nullptr)
                    return false;

                // (AR) الشرط ٣: الرمزُ من هذه الوحدةِ بعينِها.
                // (EN) Condition 3: the symbol belongs to this very module.
                const std::string &qualifyingModule = namespaceIt->second;
                auto symbolsIt = moduleExportedSymbols_.find(qualifyingModule);
                if (symbolsIt == moduleExportedSymbols_.end() ||
                    symbolsIt->second.find(symbolName) == symbolsIt->second.end())
                    return false;

                // (AR) الإبهام: وحدةٌ أخرى مستوردةٌ في هذا الملفّ تُصدّر الاسمَ نفسَه.
                //      التسطيحُ يجعل الرمزَين واحدًا فيفوز آخِرُ ما بُني — فنرفض
                //      ونُشخّص بدل أن نُرجع قيمةَ الوحدةِ الخطأ بلا أثر.
                // (EN) Ambiguity: another module imported in this file exports the same name.
                //      Flattening makes them one symbol and the last built wins, so we refuse
                //      and diagnose rather than return the wrong module's value silently.
                for (const auto &otherNamespace : moduleNamespaces_)
                {
                    if (otherNamespace.second == qualifyingModule)
                        continue;
                    auto otherSymbolsIt = moduleExportedSymbols_.find(otherNamespace.second);
                    if (otherSymbolsIt == moduleExportedSymbols_.end())
                        continue;
                    if (otherSymbolsIt->second.find(symbolName) == otherSymbolsIt->second.end())
                        continue;

                    ambiguityDiagnostic =
                        "خطأ: الرمز '" + symbolName + "' مُصدَّرٌ من وحدتين مستوردتين معًا ('" +
                        qualifyingModule + "' و'" + otherNamespace.second +
                        "') فالتأهيل مُبهَم — 💡 جرّب: من " + qualifyingModule + " استورد " +
                        symbolName + " كـ اسم_آخر / Error: symbol '" + symbolName +
                        "' is exported by two imported modules ('" + qualifyingModule + "', '" +
                        otherNamespace.second + "'); the qualification is ambiguous";
                    return false;
                }

                return true;
            }

            // ============================================================================
            // enterLoop - دخول سياق حلقة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:607
            // التوقيع / Signature: void enterLoop(const LoopContext& ctx);
            //
            // المعاملات / Parameters:
            // - ctx: const LoopContext& = sir_builder.h:180 (LoopContext struct)
            //
            // المتغيرات المستخدمة / Used variables:
            // - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
            // ============================================================================
            void SIRBuilder::enterLoop(const LoopContext &ctx)
            {
                // (AR) إضافة سياق حلقة جديد (sir_builder.h:639 - loopStack_)
                // (EN) Add new loop context
                loopStack_.push_back(ctx);
                // (AR) الحاجز ٧: سجّل عمق «حاول» عند دخول الحلقة ليعرف «قف/أكمل» كم «حاول»
                //      يخرج منها. / (EN) Barrier 7: record the «try» depth at loop entry so
                //      break/continue know how many «try»s they exit.
                loopStack_.back().tryDepthAtEntry = currentTryDepth_;
            }

            // ============================================================================
            // exitLoop - الخروج من سياق الحلقة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:615
            // التوقيع / Signature: void exitLoop();
            //
            // المتغيرات المستخدمة / Used variables:
            // - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
            // ============================================================================
            void SIRBuilder::exitLoop()
            {
                // (AR) إزالة سياق الحلقة الأخير (sir_builder.h:639 - loopStack_)
                // (EN) Remove last loop context
                if (!loopStack_.empty())
                {
                    loopStack_.pop_back();
                }
            }

            // ============================================================================
            // getCurrentLoop - الحصول على سياق الحلقة الحالية
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:622
            // التوقيع / Signature: LoopContext* getCurrentLoop();
            //
            // المتغيرات المستخدمة / Used variables:
            // - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
            //
            // الإرجاع / Returns:
            // - LoopContext*: مؤشر لسياق الحلقة الحالية أو nullptr
            // ============================================================================
            LoopContext *SIRBuilder::getCurrentLoop()
            {
                // (AR) إرجاع سياق الحلقة الأخير (sir_builder.h:639 - loopStack_)
                // (EN) Return last loop context
                if (!loopStack_.empty())
                {
                    return &loopStack_.back();
                }

                // (AR) لا توجد حلقة حالية
                // (EN) No current loop
                return nullptr;
            }

            // ============================================================================
            // enterGenericScope - دخول نطاق أنواع عامة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:641
            // التوقيع / Signature: void enterGenericScope(const std::vector<TypeParameter>& typeParams);
            //
            // المعاملات / Parameters:
            // - typeParams: const std::vector<TypeParameter>& = sir_builder.h:187 (TypeParameter)
            //
            // المتغيرات المستخدمة / Used variables:
            // - genericScopeStack_: sir_builder.h:643 (std::vector<GenericScope>)
            // ============================================================================
            void SIRBuilder::enterGenericScope(const std::vector<TypeParameter> &typeParams)
            {
                // (AR) إنشاء نطاق جديد وإضافته (sir_builder.h:643 - genericScopeStack_)
                // (EN) Create new scope and add it
                GenericScope scope;
                scope.typeParameters = typeParams;
                genericScopeStack_.push_back(scope);
            }

            // ============================================================================
            // exitGenericScope - الخروج من نطاق الأنواع العامة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:649
            // التوقيع / Signature: void exitGenericScope();
            //
            // المتغيرات المستخدمة / Used variables:
            // - genericScopeStack_: sir_builder.h:643 (std::vector<GenericScope>)
            // ============================================================================
            void SIRBuilder::exitGenericScope()
            {
                // (AR) إزالة النطاق الأخير (sir_builder.h:643 - genericScopeStack_)
                // (EN) Remove last scope
                if (!genericScopeStack_.empty())
                {
                    genericScopeStack_.pop_back();
                }
            }


        } // namespace SIR
    } // namespace Compiler
} // namespace Sad

