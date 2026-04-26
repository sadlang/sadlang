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
                // (AR) ״²״§״¯״© …״³״×ˆ‰ ״§„†״·״§‚ (sir_builder.h:599 - currentScopeLevel_)
                // (EN) Increase scope level
                currentScopeLevel_++;

                // (AR) ״¥״¶״§״© †״·״§‚ ״¬״¯״¯ (sir_builder.h:714 - scopeStack_)
                // (EN) Add new scope
                scopeStack_.push_back(std::unordered_map<std::string, VariableInfo>());
            }

            // ============================================================================
            // exitScope - ״§„״®״±ˆ״¬ …† ״§„†״·״§‚ ״§„״­״§„
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:589
            // ״§„״×ˆ‚״¹ / Signature: void exitScope();
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - currentScopeLevel_: sir_builder.h:599 (int)
            // - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
            // ============================================================================
            void SIRBuilder::exitScope()
            {
                // (AR) ״¥״²״§„״© ״§„†״·״§‚ ״§„״£״®״± (sir_builder.h:714 - scopeStack_)
                // (EN) Remove last scope
                if (!scopeStack_.empty())
                {
                    scopeStack_.pop_back();
                }

                // (AR) ״×‚„„ …״³״×ˆ‰ ״§„†״·״§‚ (sir_builder.h:599 - currentScopeLevel_)
                // (EN) Decrease scope level
                if (currentScopeLevel_ > 0)
                {
                    currentScopeLevel_--;
                }
            }

            // ============================================================================
            // addVariable - ״¥״¶״§״© …״×״÷״± „„†״·״§‚ ״§„״­״§„
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:591
            // ״§„״×ˆ‚״¹ / Signature: void addVariable(const VariableInfo& varInfo);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - varInfo: const VariableInfo& = sir_builder.h:139 (VariableInfo struct)
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
            // ============================================================================
            void SIRBuilder::addVariable(const VariableInfo &varInfo)
            {
                // (AR) ״¥״¶״§״© ״§„…״×״÷״± „„†״·״§‚ ״§„״­״§„ (sir_builder.h:714 - scopeStack_)
                // (EN) Add variable to current scope
                if (!scopeStack_.empty())
                {
                    scopeStack_.back()[varInfo.name] = varInfo;
                }
            }

            // ============================================================================
            // lookupVariable - ״§„״¨״­״« ״¹† …״×״÷״±  ״§„†״·״§‚״§״×
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:597
            // ״§„״×ˆ‚״¹ / Signature: VariableInfo* lookupVariable(const std::string& name);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - name: const std::string& (״§״³… ״§„…״×״÷״±)
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
            //
            // ״§„״¥״±״¬״§״¹ / Returns:
            // - VariableInfo*: …״₪״´״± „„…״×״÷״± ״£ˆ nullptr
            // ============================================================================
            VariableInfo *SIRBuilder::lookupVariable(const std::string &name)
            {
                // (AR) ״§„״¨״­״«  ״§„†״·״§‚״§״× …† ״§„״£״­״¯״« „„״£‚״¯… (sir_builder.h:714 - scopeStack_)
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
                // (AR) ״§„״¨״­״«  ״§„…״×״÷״±״§״× ״§„״¹״§…״© ״¹„‰ …״³״×ˆ‰ ״§„ˆ״­״¯״©
                // (EN) Fallback: search in module-level global variables
                // ״¹†״¯…״§ „״§ ״¹״«״± ״¹„‰ ״§„…״×״÷״±  ״§„†״·״§‚״§״× ״§„…״­„״©״ †״¨״­״«  ״§„ˆ״­״¯״©
                // When not found in local scopes, search in module globals
                // =====================================================================
                if (module_)
                {
                    auto globalVar = module_->getGlobalVariable(name);
                    if (globalVar)
                    {
                        // (AR) ״¥†״´״§״¡ VariableInfo „„…״×״÷״± ״§„״¹״§… ˆ״×״®״²†‡  ״£ˆ„ †״·״§‚
                        // (EN) Create VariableInfo for global var and cache in first scope
                        VariableInfo globalInfo;
                        globalInfo.name = name;
                        globalInfo.type = globalVar->getType();
                        globalInfo.registerName = name; // use same name as LLVM global
                        globalInfo.isGlobal = true;
                        globalInfo.isMutable = !globalVar->getIsConstant();
                        globalInfo.scopeLevel = 0;

                        // (AR) ״×״®״²†  ״£‚״¯… †״·״§‚ „„״¨״­״« ״§„״³״±״¹ „״§״­‚״§‹
                        // (EN) Cache in oldest scope for fast future lookups
                        if (!scopeStack_.empty())
                        {
                            scopeStack_.front()[name] = globalInfo;
                            return &scopeStack_.front()[name];
                        }
                    }
                }

                // (AR) „… ״¹״«״± ״¹„‰ ״§„…״×״÷״±
                // (EN) Variable not found
                return nullptr;
            }

            // ============================================================================
            // enterLoop - ״¯״®ˆ„ ״³״§‚ ״­„‚״©
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:607
            // ״§„״×ˆ‚״¹ / Signature: void enterLoop(const LoopContext& ctx);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - ctx: const LoopContext& = sir_builder.h:180 (LoopContext struct)
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
            // ============================================================================
            void SIRBuilder::enterLoop(const LoopContext &ctx)
            {
                // (AR) ״¥״¶״§״© ״³״§‚ ״­„‚״© ״¬״¯״¯ (sir_builder.h:639 - loopStack_)
                // (EN) Add new loop context
                loopStack_.push_back(ctx);
            }

            // ============================================================================
            // exitLoop - ״§„״®״±ˆ״¬ …† ״³״§‚ ״§„״­„‚״©
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:615
            // ״§„״×ˆ‚״¹ / Signature: void exitLoop();
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
            // ============================================================================
            void SIRBuilder::exitLoop()
            {
                // (AR) ״¥״²״§„״© ״³״§‚ ״§„״­„‚״© ״§„״£״®״± (sir_builder.h:639 - loopStack_)
                // (EN) Remove last loop context
                if (!loopStack_.empty())
                {
                    loopStack_.pop_back();
                }
            }

            // ============================================================================
            // getCurrentLoop - ״§„״­״µˆ„ ״¹„‰ ״³״§‚ ״§„״­„‚״© ״§„״­״§„״©
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:622
            // ״§„״×ˆ‚״¹ / Signature: LoopContext* getCurrentLoop();
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - loopStack_: sir_builder.h:639 (std::vector<LoopContext>)
            //
            // ״§„״¥״±״¬״§״¹ / Returns:
            // - LoopContext*: …״₪״´״± „״³״§‚ ״§„״­„‚״© ״§„״­״§„״© ״£ˆ nullptr
            // ============================================================================
            LoopContext *SIRBuilder::getCurrentLoop()
            {
                // (AR) ״¥״±״¬״§״¹ ״³״§‚ ״§„״­„‚״© ״§„״£״®״± (sir_builder.h:639 - loopStack_)
                // (EN) Return last loop context
                if (!loopStack_.empty())
                {
                    return &loopStack_.back();
                }

                // (AR) „״§ ״×ˆ״¬״¯ ״­„‚״© ״­״§„״©
                // (EN) No current loop
                return nullptr;
            }

            // ============================================================================
            // enterGenericScope - ״¯״®ˆ„ †״·״§‚ ״£†ˆ״§״¹ ״¹״§…״©
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:641
            // ״§„״×ˆ‚״¹ / Signature: void enterGenericScope(const std::vector<TypeParameter>& typeParams);
            //
            // ״§„…״¹״§…„״§״× / Parameters:
            // - typeParams: const std::vector<TypeParameter>& = sir_builder.h:187 (TypeParameter)
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - genericScopeStack_: sir_builder.h:643 (std::vector<GenericScope>)
            // ============================================================================
            void SIRBuilder::enterGenericScope(const std::vector<TypeParameter> &typeParams)
            {
                // (AR) ״¥†״´״§״¡ †״·״§‚ ״¬״¯״¯ ˆ״¥״¶״§״×‡ (sir_builder.h:643 - genericScopeStack_)
                // (EN) Create new scope and add it
                GenericScope scope;
                scope.typeParameters = typeParams;
                genericScopeStack_.push_back(scope);
            }

            // ============================================================================
            // exitGenericScope - ״§„״®״±ˆ״¬ …† †״·״§‚ ״§„״£†ˆ״§״¹ ״§„״¹״§…״©
            // ============================================================================
            // …״µ״¯״± ״§„״×״¹״± / Source: sir_builder.h:649
            // ״§„״×ˆ‚״¹ / Signature: void exitGenericScope();
            //
            // ״§„…״×״÷״±״§״× ״§„…״³״×״®״¯…״© / Used variables:
            // - genericScopeStack_: sir_builder.h:643 (std::vector<GenericScope>)
            // ============================================================================
            void SIRBuilder::exitGenericScope()
            {
                // (AR) ״¥״²״§„״© ״§„†״·״§‚ ״§„״£״®״± (sir_builder.h:643 - genericScopeStack_)
                // (EN) Remove last scope
                if (!genericScopeStack_.empty())
                {
                    genericScopeStack_.pop_back();
                }
            }


        } // namespace SIR
    } // namespace Compiler
} // namespace Sad

