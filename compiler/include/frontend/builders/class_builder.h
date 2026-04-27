// ======================================================================
// class_builder.h - بنّاء الأصناف والسمات / Class & Trait Builder
// ======================================================================
// (AR) Phase 6 — Step 5: صنف منفصل يجمع 5 methods لبناء الأصناف:
//      buildClass (الرئيسية), buildClassConstructor, buildClassOperator,
//      buildTrait, buildImpl.
// (EN) Phase 6 — Step 5: standalone class grouping 5 builders for
//      class/trait construction.
// ======================================================================

#pragma once

#include "../sir_types.h"
#include "../sir_instruction.h"

#include <memory>
#include <string>
#include <vector>

// (AR) إعلانات مسبقة لأنواع AST الحقيقية
namespace Sad { namespace AST {
    class ClassDecl;
    class ConstructorDecl;
    class OperatorDecl;
    class TraitDecl;
    class ImplDecl;
}}

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            class SIRBuilder;
            class SIRClass;
            struct BuildResult;

            /**
             * @brief (AR) بنّاء الأصناف والسمات (5 methods)
             * @brief (EN) Builder for classes/traits (5 methods)
             */
            class ClassBuilder
            {
            public:
                explicit ClassBuilder(SIRBuilder &b) : b_(b) {}

                void buildClass(Sad::AST::ClassDecl *classDecl);

                void buildClassConstructor(Sad::AST::ClassDecl *classDecl, std::shared_ptr<SIRClass> sirClass, Sad::AST::ConstructorDecl *ctorDecl);

                void buildClassOperator(Sad::AST::ClassDecl *classDecl, std::shared_ptr<SIRClass> sirClass, Sad::AST::OperatorDecl *operatorDecl);

                void buildTrait(Sad::AST::TraitDecl *traitDecl);

                void buildImpl(Sad::AST::ImplDecl *implDecl);

            private:
                SIRBuilder &b_;
            };

        } // namespace SIR
    }     // namespace Compiler
} // namespace Sad
