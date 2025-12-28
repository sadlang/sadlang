#!/usr/bin/env python3
"""
إضافة visitMatchStmt و visitWalrusExpr و visitSetComprehensionExpr 
لجميع الـ optimizer visitors
"""

import re

# قراءة الملف
with open("src/optimizer/optimization_pass.cpp", "r", encoding="utf-8") as f:
    content = f.read()

# الvisitors التي نحتاج تعديلها
visitors = [
    "DeadCodeEliminationVisitor",
    "ConstantFoldingVisitor",
    "ExpressionSimplificationVisitor",
    "LoopOptimizationVisitor",
    "FunctionInliningVisitor",
    "RedundantAssignmentVisitor"
]

# Methods التي نحتاج إضافتها
methods_to_add = """
    void visitWalrusExpr(Sad::AST::WalrusExpr& expr) override {
        if (expr.value) expr.value->accept(*this);
    }
    
    void visitSetComprehensionExpr(Sad::AST::SetComprehensionExpr& expr) override {
        if (expr.element) expr.element->accept(*this);
        if (expr.iterable) expr.iterable->accept(*this);
        if (expr.condition) expr.condition->accept(*this);
    }
    
    void visitMatchStmt(Sad::AST::MatchStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
        for (auto& c : stmt.cases) {
            if (c.guard) c.guard->accept(*this);
            for (auto& s : c.body) {
                if (s) s->accept(*this);
            }
        }
    }
"""

# إضافة #include
if '#include "parser/ast/pattern_nodes.h"' not in content:
    content = content.replace(
        '#include "parser/ast/declarations.h"',
        '#include "parser/ast/declarations.h"\n#include "parser/ast/pattern_nodes.h"'
    )

# إضافة methods لكل visitor
for visitor in visitors:
    # ابحث عن visitSwitchStmt في هذا الvisitor
    pattern = rf'(class {visitor}.*?)(void visitSwitchStmt\(Sad::AST::SwitchStmt& stmt\) override \{{[^}}]*\}})\s*(void visit(?:ForRange|VarDecl)Stmt)'
    
    def replace_func(match):
        before = match.group(1)
        switch_method = match.group(2)
        next_method = match.group(3)
        return f'{before}{switch_method}\n{methods_to_add}\n    {next_method}'
    
    content = re.sub(pattern, replace_func, content, flags=re.DOTALL, count=1)

# حفظ الملف
with open("src/optimizer/optimization_pass.cpp", "w", encoding="utf-8") as f:
    f.write(content)

print("✓ تم إضافة جميع الـ visitor methods بنجاح")
