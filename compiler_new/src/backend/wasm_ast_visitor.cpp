/**
 * @file wasm_ast_visitor.cpp
 * @brief (AR) تنفيذ زائر AST لتوليد WebAssembly
 * @brief (EN) Implementation of AST Visitor for WebAssembly generation
 * 
 * @author فريق لغة ص
 * @date 2026-03-06
 */

#include "wasm_ast_visitor.h"
#include <sstream>
#include <algorithm>
#include <cstring>

namespace sad {
namespace wasm {

// ═══════════════════════════════════════════════════════════════════════════════
//  الدوال القياسية المستوردة من JavaScript
// ═══════════════════════════════════════════════════════════════════════════════

const WasmStdlibImport WASM_STDLIB_IMPORTS[] = {
    // === الطباعة ===
    {"env", "__sad_print",       "اطبع",       "(func (param i32))"},
    {"env", "__sad_print_int",   "اطبع_رقم",   "(func (param i32))"},
    {"env", "__sad_print_float", "اطبع_عشري",  "(func (param f64))"},
    {"env", "__sad_println",     "اطبع_سطر",   "(func (param i32))"},
    
    // === الرياضيات ===
    {"env", "__sad_sqrt",        "جذر",        "(func (param f64) (result f64))"},
    {"env", "__sad_pow",         "قوة",        "(func (param f64 f64) (result f64))"},
    {"env", "__sad_sin",         "جيب",        "(func (param f64) (result f64))"},
    {"env", "__sad_cos",         "جتا",        "(func (param f64) (result f64))"},
    {"env", "__sad_random",      "عشوائي",     "(func (result f64))"},
    
    // === الذاكرة ===
    {"env", "__sad_malloc",      "خصص",        "(func (param i32) (result i32))"},
    {"env", "__sad_free",        "حرر",        "(func (param i32))"},
    
    // === النصوص ===
    {"env", "__sad_str_len",     "طول",        "(func (param i32) (result i32))"},
    {"env", "__sad_str_concat",  "ضم",         "(func (param i32 i32) (result i32))"},
    
    // === DOM ===
    {"env", "__sad_dom_query",      "استعلم",      "(func (param i32) (result i32))"},
    {"env", "__sad_dom_create",     "أنشئ_عنصر",   "(func (param i32) (result i32))"},
    {"env", "__sad_dom_set_text",   "عيّن_نص",     "(func (param i32 i32))"},
    {"env", "__sad_dom_set_attr",   "عيّن_سمة",    "(func (param i32 i32 i32))"},
    {"env", "__sad_dom_append",     "أضف_ابن",     "(func (param i32 i32))"},
    {"env", "__sad_dom_remove",     "أزل",         "(func (param i32))"},
    {"env", "__sad_dom_on_click",   "عند_نقر",     "(func (param i32 i32))"},
    {"env", "__sad_dom_on_input",   "عند_إدخال",   "(func (param i32 i32))"},
    
    // === Console ===
    {"env", "__sad_console_log",    "سجّل",        "(func (param i32))"},
    {"env", "__sad_console_error",  "خطأ",         "(func (param i32))"},
    {"env", "__sad_console_time",   "وقت",         "(func (param i32))"},
    {"env", "__sad_console_timeEnd","نهاية_وقت",   "(func (param i32))"},
    
    // === الشبكة (Fetch) ===
    {"env", "__sad_fetch_start",    "جلب",         "(func (param i32 i32) (result i32))"},
    {"env", "__sad_fetch_result",   "نتيجة_جلب",   "(func (param i32) (result i32))"},
    
    // === التخزين المحلي ===
    {"env", "__sad_storage_get",    "اقرأ_مخزن",   "(func (param i32) (result i32))"},
    {"env", "__sad_storage_set",    "اكتب_مخزن",   "(func (param i32 i32))"},
};

const size_t WASM_STDLIB_IMPORTS_COUNT = sizeof(WASM_STDLIB_IMPORTS) / sizeof(WASM_STDLIB_IMPORTS[0]);

// ═══════════════════════════════════════════════════════════════════════════════
//  Constructor
// ═══════════════════════════════════════════════════════════════════════════════

WasmASTVisitor::WasmASTVisitor() {
    module_.name = "sad_module";
    registerStandardImports();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تسجيل الاستيرادات القياسية
// ═══════════════════════════════════════════════════════════════════════════════

void WasmASTVisitor::registerStandardImports() {
    for (size_t i = 0; i < WASM_STDLIB_IMPORTS_COUNT; ++i) {
        const auto& imp = WASM_STDLIB_IMPORTS[i];
        
        std::ostringstream ss;
        ss << "(import \"" << imp.module << "\" \"" << imp.wasmName << "\" "
           << imp.signature << ")";
        
        module_.imports.push_back(ss.str());
        
        // تسجيل الدالة في خريطة الفهارس
        module_.funcIndices[imp.wasmName] = module_.nextFuncIndex;
        module_.funcIndices[imp.arabicName] = module_.nextFuncIndex;
        module_.nextFuncIndex++;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  التجميع الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

std::string WasmASTVisitor::compile(Sad::AST::ASTNode* program) {
    if (!program) {
        addError("البرنامج فارغ");
        return "";
    }
    
    // زيارة جميع العقد في البرنامج
    program->accept(*this);
    
    if (hasErrors()) {
        return "";
    }
    
    return generateWat();
}

std::string WasmASTVisitor::getWatCode() const {
    return const_cast<WasmASTVisitor*>(this)->generateWat();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  توليد WAT النهائي
// ═══════════════════════════════════════════════════════════════════════════════

std::string WasmASTVisitor::generateWat() {
    std::ostringstream wat;
    
    wat << ";; ════════════════════════════════════════════════════════════════\n";
    wat << ";; مُولَّد تلقائياً بواسطة مترجم لغة ص (sadc)\n";
    wat << ";; Auto-generated by Sad Language Compiler\n";
    wat << ";; ════════════════════════════════════════════════════════════════\n\n";
    
    wat << "(module\n";
    
    // ─── الاستيرادات ───
    wat << "  ;; === الاستيرادات / Imports ===\n";
    for (const auto& imp : module_.imports) {
        wat << "  " << imp << "\n";
    }
    wat << "\n";
    
    // ─── الذاكرة ───
    wat << "  ;; === الذاكرة / Memory ===\n";
    wat << "  (memory (export \"memory\") " << module_.memoryPages << ")\n\n";
    
    // ─── قطع البيانات (النصوص) ───
    if (!module_.dataSegments.empty()) {
        wat << "  ;; === البيانات / Data Segments ===\n";
        for (const auto& seg : module_.dataSegments) {
            wat << "  " << seg << "\n";
        }
        wat << "\n";
    }
    
    // ─── الدوال ───
    wat << "  ;; === الدوال / Functions ===\n";
    for (const auto& func : module_.functions) {
        wat << "  (func $" << func.name;
        
        // تصدير الدالة
        if (func.exported) {
            wat << " (export \"" << func.name << "\")";
            if (!func.arabicName.empty() && func.arabicName != func.name) {
                wat << " (export \"" << func.arabicName << "\")";
            }
        }
        
        // المعاملات
        for (size_t i = 0; i < func.params.size(); ++i) {
            wat << " (param $p" << i << " " << wasmTypeToWat(func.params[i]) << ")";
        }
        
        // النتائج
        if (!func.results.empty()) {
            wat << " (result";
            for (auto r : func.results) {
                wat << " " << wasmTypeToWat(r);
            }
            wat << ")";
        }
        
        wat << "\n";
        
        // المتغيرات المحلية
        for (const auto& local : func.locals) {
            if (!local.isParam) {
                wat << "    (local $" << local.name << " " << wasmTypeToWat(local.type) << ")\n";
            }
        }
        
        // الجسم
        for (const auto& inst : func.body) {
            wat << "    " << inst << "\n";
        }
        
        wat << "  )\n\n";
    }
    
    // ─── نقطة البداية ───
    // إذا وجدت دالة رئيسية، صدّرها كـ _start
    if (module_.funcIndices.count("رئيسية") || module_.funcIndices.count("main")) {
        uint32_t mainIdx = module_.funcIndices.count("رئيسية") 
            ? module_.funcIndices["رئيسية"] 
            : module_.funcIndices["main"];
        wat << "  (export \"_start\" (func " << mainIdx << "))\n";
    }
    
    wat << ")\n";
    
    return wat.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

void WasmASTVisitor::emit(const std::string& instruction) {
    if (currentFunc_) {
        currentFunc_->body.push_back(instruction);
    }
}

uint32_t WasmASTVisitor::addLocal(const std::string& name, WasmValType type) {
    WasmLocal local;
    local.name = name;
    local.index = nextLocalIndex_++;
    local.type = type;
    local.isParam = false;
    
    currentLocals_[name] = local;
    
    if (currentFunc_) {
        currentFunc_->locals.push_back(local);
    }
    
    return local.index;
}

WasmLocal* WasmASTVisitor::findLocal(const std::string& name) {
    auto it = currentLocals_.find(name);
    return (it != currentLocals_.end()) ? &it->second : nullptr;
}

uint32_t WasmASTVisitor::findFunction(const std::string& name) {
    auto it = module_.funcIndices.find(name);
    if (it != module_.funcIndices.end()) {
        return it->second;
    }
    
    addError("دالة غير معرّفة: " + name);
    return 0;
}

uint32_t WasmASTVisitor::addString(const std::string& str) {
    // التحقق من وجود النص مسبقاً
    auto it = module_.stringOffsets.find(str);
    if (it != module_.stringOffsets.end()) {
        return it->second;
    }
    
    uint32_t offset = module_.dataOffset;
    
    // تحويل النص إلى bytes مع null terminator
    std::ostringstream seg;
    seg << "(data (i32.const " << offset << ") \"";
    
    // Escape النص
    for (unsigned char c : str) {
        if (c == '"' || c == '\\') {
            seg << '\\' << c;
        } else if (c >= 32 && c < 127) {
            seg << c;
        } else {
            // تكويد hex
            seg << "\\";
            seg << std::hex << ((c >> 4) & 0xF);
            seg << std::hex << (c & 0xF);
        }
    }
    seg << "\\00\")";  // null terminator
    
    module_.dataSegments.push_back(seg.str());
    module_.stringOffsets[str] = offset;
    module_.dataOffset += str.size() + 1;  // +1 للـ null terminator
    
    return offset;
}

WasmValType WasmASTVisitor::sadTypeToWasm(const std::string& type) {
    if (type == "رقم" || type == "عدد" || type == "ع32" || type == "int") {
        return WasmValType::I32;
    } else if (type == "ع64" || type == "long") {
        return WasmValType::I64;
    } else if (type == "عشري" || type == "عش32" || type == "float") {
        return WasmValType::F32;
    } else if (type == "عش64" || type == "double") {
        return WasmValType::F64;
    } else if (type == "فراغ" || type == "void" || type == "لاشيء") {
        return WasmValType::Void;
    }
    
    // الافتراضي: مؤشر i32
    return WasmValType::I32;
}

std::string WasmASTVisitor::wasmTypeToWat(WasmValType type) {
    switch (type) {
        case WasmValType::I32: return "i32";
        case WasmValType::I64: return "i64";
        case WasmValType::F32: return "f32";
        case WasmValType::F64: return "f64";
        case WasmValType::Void: return "";
    }
    return "i32";
}

void WasmASTVisitor::addError(const std::string& msg) {
    errors_.push_back(msg);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  زوار التعابير
// ═══════════════════════════════════════════════════════════════════════════════

void WasmASTVisitor::visitLiteralExpr(Sad::AST::LiteralExpr& expr) {
    const auto& val = expr.getValue();
    
    if (val.isNumber()) {
        // عدد صحيح
        emit("i32.const " + std::to_string(static_cast<int>(val.asNumber())));
    } else if (val.isDouble()) {
        // عدد عشري
        emit("f64.const " + std::to_string(val.asDouble()));
    } else if (val.isString()) {
        // نص → إضافة للبيانات وإرجاع المؤشر
        uint32_t offset = addString(val.asString());
        emit("i32.const " + std::to_string(offset));
    } else if (val.isBool()) {
        emit("i32.const " + std::string(val.asBool() ? "1" : "0"));
    } else {
        // null
        emit("i32.const 0");
    }
}

void WasmASTVisitor::visitVariableExpr(Sad::AST::VariableExpr& expr) {
    const std::string& name = expr.getName();
    
    WasmLocal* local = findLocal(name);
    if (local) {
        emit("local.get $" + name);
    } else {
        addError("متغير غير معرّف: " + name);
    }
}

void WasmASTVisitor::visitBinaryExpr(Sad::AST::BinaryExpr& expr) {
    // زيارة المعاملين (يضعان القيم على المكدس)
    expr.getLeft()->accept(*this);
    expr.getRight()->accept(*this);
    
    // العملية
    const std::string& op = expr.getOperator();
    
    if (op == "+")       emit("i32.add");
    else if (op == "-")  emit("i32.sub");
    else if (op == "*")  emit("i32.mul");
    else if (op == "/")  emit("i32.div_s");
    else if (op == "%")  emit("i32.rem_s");
    else if (op == "==") emit("i32.eq");
    else if (op == "!=") emit("i32.ne");
    else if (op == "<")  emit("i32.lt_s");
    else if (op == "<=") emit("i32.le_s");
    else if (op == ">")  emit("i32.gt_s");
    else if (op == ">=") emit("i32.ge_s");
    else if (op == "و" || op == "&&") emit("i32.and");
    else if (op == "أو" || op == "||") emit("i32.or");
    else {
        addError("عامل غير مدعوم: " + op);
    }
}

void WasmASTVisitor::visitUnaryExpr(Sad::AST::UnaryExpr& expr) {
    expr.getOperand()->accept(*this);
    
    const std::string& op = expr.getOperator();
    
    if (op == "-") {
        emit("i32.const 0");
        emit("i32.sub");  // 0 - x
    } else if (op == "!" || op == "ليس") {
        emit("i32.eqz");
    } else {
        addError("عامل أحادي غير مدعوم: " + op);
    }
}

void WasmASTVisitor::visitAssignExpr(Sad::AST::AssignExpr& expr) {
    // الحصول على المتغير
    const std::string& name = expr.getName();
    
    WasmLocal* local = findLocal(name);
    if (!local) {
        addError("متغير غير معرّف: " + name);
        return;
    }
    
    // تقييم القيمة
    expr.getValue()->accept(*this);
    
    // تخزين وإبقاء القيمة على المكدس
    emit("local.tee $" + name);
}

void WasmASTVisitor::visitCallExpr(Sad::AST::CallExpr& expr) {
    // تقييم المعاملات
    for (auto& arg : expr.getArgs()) {
        arg->accept(*this);
    }
    
    // استدعاء الدالة
    const std::string& funcName = expr.getCallee();
    uint32_t funcIdx = findFunction(funcName);
    
    emit("call " + std::to_string(funcIdx));
}

void WasmASTVisitor::visitTernaryExpr(Sad::AST::TernaryExpr& expr) {
    // الشرط
    expr.getCondition()->accept(*this);
    
    emit("if (result i32)");
    blockDepth_++;
    
    // الفرع الصحيح
    expr.getTrueExpr()->accept(*this);
    
    emit("else");
    
    // الفرع الخطأ
    expr.getFalseExpr()->accept(*this);
    
    emit("end");
    blockDepth_--;
}

void WasmASTVisitor::visitArrayExpr(Sad::AST::ArrayExpr& expr) {
    // نمثّل المصفوفة كمخزن متجاور i32 في الذاكرة الخطية
    // [elem0, elem1, ...] مع إرجاع مؤشر البداية
    const size_t count = expr.elements.size();
    const uint32_t bytes = static_cast<uint32_t>(count * 4);

    uint32_t mallocIdx = findFunction("__sad_malloc");
    std::string basePtrName = "__arr_ptr_" + std::to_string(nextLocalIndex_);
    addLocal(basePtrName, WasmValType::I32);

    emit("i32.const " + std::to_string(bytes));
    emit("call " + std::to_string(mallocIdx));
    emit("local.set $" + basePtrName);

    for (size_t i = 0; i < count; ++i) {
        emit("local.get $" + basePtrName);
        emit("i32.const " + std::to_string(static_cast<uint32_t>(i * 4)));
        emit("i32.add");

        expr.elements[i]->accept(*this);
        emit("i32.store");
    }

    // نتيجة تعبير المصفوفة هي مؤشر القاعدة
    emit("local.get $" + basePtrName);
}

void WasmASTVisitor::visitIndexExpr(Sad::AST::IndexExpr& expr) {
    // تحميل عنصر i32 من arr[index]
    expr.object->accept(*this);  // base pointer
    expr.index->accept(*this);   // index
    emit("i32.const 4");
    emit("i32.mul");
    emit("i32.add");
    emit("i32.load");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  زوار العبارات
// ═══════════════════════════════════════════════════════════════════════════════

void WasmASTVisitor::visitExprStmt(Sad::AST::ExprStmt& stmt) {
    stmt.getExpression()->accept(*this);
    emit("drop");  // تجاهل النتيجة
}

void WasmASTVisitor::visitVarDeclStmt(Sad::AST::VarDeclStmt& stmt) {
    const std::string& name = stmt.getName();
    std::string type = stmt.getType();
    
    // إضافة المتغير المحلي
    WasmValType wasmType = sadTypeToWasm(type);
    addLocal(name, wasmType);
    
    // تهيئة المتغير إذا وجدت قيمة
    if (stmt.getInitializer()) {
        stmt.getInitializer()->accept(*this);
        emit("local.set $" + name);
    }
}

void WasmASTVisitor::visitIfStmt(Sad::AST::IfStmt& stmt) {
    // الشرط
    stmt.getCondition()->accept(*this);
    
    emit("if");
    blockDepth_++;
    
    // الفرع الصحيح
    stmt.getThenBranch()->accept(*this);
    
    // الفرع الخطأ
    if (stmt.getElseBranch()) {
        emit("else");
        stmt.getElseBranch()->accept(*this);
    }
    
    emit("end");
    blockDepth_--;
}

void WasmASTVisitor::visitWhileStmt(Sad::AST::WhileStmt& stmt) {
    // بنية الحلقة في WASM:
    // block $break
    //   loop $continue
    //     br_if (not condition) $break
    //     body
    //     br $continue
    //   end
    // end
    
    emit("block $break_" + std::to_string(blockDepth_));
    blockDepth_++;
    
    emit("loop $continue_" + std::to_string(blockDepth_));
    blockDepth_++;
    
    loopStack_.push({blockDepth_ - 2, blockDepth_ - 1});
    
    // الشرط (معكوس)
    stmt.getCondition()->accept(*this);
    emit("i32.eqz");
    emit("br_if $break_" + std::to_string(blockDepth_ - 2));
    
    // الجسم
    stmt.getBody()->accept(*this);
    
    // العودة لبداية الحلقة
    emit("br $continue_" + std::to_string(blockDepth_ - 1));
    
    emit("end");  // loop
    blockDepth_--;
    
    emit("end");  // block
    blockDepth_--;
    
    loopStack_.pop();
}

void WasmASTVisitor::visitForRangeStmt(Sad::AST::ForRangeStmt& stmt) {
    // تنفيذ جزئي: إذا كانت المجموعة مصفوفة حرفية، نفكها وقت الترجمة
    // Full iterator lowering يحتاج runtime iterator protocol.
    auto* literalArray = dynamic_cast<Sad::AST::ArrayExpr*>(stmt.iterable.get());
    if (!literalArray) {
        addError("حلقة لكل مدعومة حالياً فقط مع مصفوفة حرفية في WASM");
        return;
    }

    if (!findLocal(stmt.variable)) {
        addLocal(stmt.variable, WasmValType::I32);
    }

    for (auto& element : literalArray->elements) {
        element->accept(*this);
        emit("local.set $" + stmt.variable);
        if (stmt.body) {
            stmt.body->accept(*this);
        }
    }
}

void WasmASTVisitor::visitReturnStmt(Sad::AST::ReturnStmt& stmt) {
    if (stmt.getValue()) {
        stmt.getValue()->accept(*this);
    }
    emit("return");
}

void WasmASTVisitor::visitBreakStmt(Sad::AST::BreakStmt& stmt) {
    if (loopStack_.empty()) {
        addError("توقف خارج حلقة");
        return;
    }
    emit("br $break_" + std::to_string(loopStack_.top().first));
}

void WasmASTVisitor::visitContinueStmt(Sad::AST::ContinueStmt& stmt) {
    if (loopStack_.empty()) {
        addError("استمر خارج حلقة");
        return;
    }
    emit("br $continue_" + std::to_string(loopStack_.top().second));
}

void WasmASTVisitor::visitBlockStmt(Sad::AST::BlockStmt& stmt) {
    for (auto& s : stmt.getStatements()) {
        s->accept(*this);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  زوار التصريحات
// ═══════════════════════════════════════════════════════════════════════════════

void WasmASTVisitor::visitFunctionDecl(Sad::AST::FunctionDecl& decl) {
    WasmFunc func;
    func.name = decl.getName();
    // اسم الدالة الأصلي (UTF-8) يُستخدم كاسم عربي أيضاً تلقائياً.
    func.arabicName = decl.getName();
    func.index = module_.nextFuncIndex++;
    func.exported = true;  // تصدير كل الدوال افتراضياً
    
    // تسجيل الدالة
    module_.funcIndices[func.name] = func.index;
    
    // إعداد السياق
    currentFunc_ = &func;
    currentLocals_.clear();
    nextLocalIndex_ = 0;
    
    // المعاملات
    for (const auto& param : decl.getParams()) {
        WasmLocal local;
        local.name = param.name;
        local.index = nextLocalIndex_++;
        local.type = sadTypeToWasm(param.type);
        local.isParam = true;
        
        currentLocals_[param.name] = local;
        func.params.push_back(local.type);
    }
    
    // نوع الإرجاع
    std::string retType = decl.getReturnType();
    if (!retType.empty() && retType != "فراغ" && retType != "void") {
        func.results.push_back(sadTypeToWasm(retType));
    }
    
    // تجميع الجسم
    if (decl.getBody()) {
        decl.getBody()->accept(*this);
    }
    
    // إضافة return إذا لم يكن موجوداً
    if (func.body.empty() || func.body.back().find("return") == std::string::npos) {
        if (!func.results.empty()) {
            emit("i32.const 0");  // قيمة افتراضية
        }
        emit("return");
    }
    
    // تحديث المتغيرات المحلية
    func.locals.clear();
    for (const auto& [name, local] : currentLocals_) {
        func.locals.push_back(local);
    }
    
    // إضافة الدالة للوحدة
    module_.functions.push_back(func);
    
    currentFunc_ = nullptr;
}

void WasmASTVisitor::visitImportStmt(Sad::AST::ImportStmt& stmt) {
    // تنفيذ أساسي: تسجيل alias في جدول الدوال إن طابق اسماً معروفاً
    // مثال: استورد io كـ دخل -> alias "دخل" يشير إلى دوال الوحدة io إذا كانت معرفة.
    const std::string fullName = stmt.getFullModuleName();
    if (stmt.alias.has_value()) {
        const std::string alias = stmt.alias.value();
        auto it = module_.funcIndices.find(fullName);
        if (it != module_.funcIndices.end()) {
            module_.funcIndices[alias] = it->second;
        }
    }
}

} // namespace wasm
} // namespace sad
