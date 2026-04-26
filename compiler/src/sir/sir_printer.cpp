/**
 * =============================================================================
 * ملف: sir_printer.cpp
 * الوصف: طباعة SIR بصيغة مقروءة
 * المهمة: T267 - SIR pretty printer
 * المرحلة: Phase 27 - User Story 24 (SIR Layer)
 * =============================================================================
 *
 * 📄 دليل المبتدئ لطباعة SIR
 * ═══════════════════════════════
 *
 * ما هي طباعة SIR؟
 * ────────────────
 * نحول SIR إلى نص مقروء للتصحيح والفهم.
 *
 * مثال الإخراج:
 * ─────────────
 * ```
 * ; وحدة: برنامجي
 *
 * دالة رئيسية() -> لاشيء {
 * دخول:
 *   %0 = تخصيص(عدد)                ; س
 *   تخزين(%0, 10)
 *   %1 = تخصيص(عدد)                ; ص
 *   %2 = استعارة(%0)               ; &س
 *   تخزين(%1, %2)
 *   %3 = تحميل(%1)
 *   استدعاء(@اطبع, %3)
 *   إسقاط(%1)
 *   إسقاط(%0)
 *   أرجع
 * }
 * ```
 *
 * =============================================================================
 */

#include "sir_types.h"
#include "sir_opcodes.h"
#include <string>
#include <sstream>
#include <iomanip>

namespace sad::sir
{

    // =============================================================================
    // خيارات الطباعة
    // =============================================================================

    /**
     * خيارات طباعة SIR
     */
    struct PrinterOptions
    {
        bool showLineNumbers = true; // أرقام الأسطر
        bool showComments = true;    // التعليقات
        bool showTypes = true;       // أنواع القيم
        bool showOwnership = false;  // معلومات الملكية
        bool useColors = false;      // ألوان ANSI
        bool arabicKeywords = true;  // كلمات مفتاحية عربية
        int indentWidth = 2;         // عرض المسافة البادئة
    };

    // =============================================================================
    // ألوان ANSI
    // =============================================================================

    namespace colors
    {
        const char *RESET = "\033[0m";
        const char *RED = "\033[31m";
        const char *GREEN = "\033[32m";
        const char *YELLOW = "\033[33m";
        const char *BLUE = "\033[34m";
        const char *MAGENTA = "\033[35m";
        const char *CYAN = "\033[36m";
        const char *GRAY = "\033[90m";
    }

    // =============================================================================
    // طابعة SIR
    // =============================================================================

    /**
     * طابعة SIR
     */
    class SirPrinter
    {
    public:
        SirPrinter(const PrinterOptions &opts = {}) : opts_(opts) {}

        /**
         * طباعة وحدة
         */
        std::string print(const SirModule &module)
        {
            output_.str("");
            output_.clear();
            lineNum_ = 1;

            printHeader(module);

            for (const auto &func : module.functions)
            {
                printFunction(func);
                output_ << "\n";
            }

            return output_.str();
        }

        /**
         * طباعة دالة
         */
        std::string print(const SirFunction &func)
        {
            output_.str("");
            output_.clear();
            lineNum_ = 1;

            printFunction(func);

            return output_.str();
        }

        /**
         * طباعة تعليمة
         */
        std::string print(const SirInstruction &inst)
        {
            output_.str("");
            output_.clear();

            printInstruction(inst);

            return output_.str();
        }

    private:
        // طباعة الترويسة
        void printHeader(const SirModule &module)
        {
            comment("; وحدة: " + module.name);
            output_ << "\n";

            // طباعة الأنواع المخصصة
            if (!module.types.empty())
            {
                comment("; الأنواع المخصصة:");
                for (const auto &[name, type] : module.types)
                {
                    output_ << "; " << name << " = " << type->toString() << "\n";
                }
                output_ << "\n";
            }
        }

        // طباعة دالة
        void printFunction(const SirFunction &func)
        {
            // التوقيع
            keyword("دالة");
            output_ << " ";
            funcName(func.name);
            output_ << "(";

            for (size_t i = 0; i < func.params.size(); i++)
            {
                if (i > 0)
                    output_ << "، ";
                output_ << func.params[i].name << ": ";
                typeName(func.params[i].type);
            }

            output_ << ")";

            if (func.returnType && func.returnType->getKind() != SadTypeKind::Void)
            {
                output_ << " -> ";
                typeName(func.returnType);
            }

            output_ << " {\n";

            // الكتل
            for (const auto &block : func.blocks)
            {
                printBlock(block);
            }

            output_ << "}\n";
        }

        // طباعة كتلة
        void printBlock(const SirBasicBlock &block)
        {
            // اسم الكتلة
            label(block.name + ":");
            output_ << "\n";

            // التعليمات
            for (const auto &inst : block.instructions)
            {
                printLineNumber();
                indent(1);
                printInstruction(inst);
                output_ << "\n";
            }
        }

        // طباعة تعليمة
        void printInstruction(const SirInstruction &inst)
        {
            // النتيجة
            if (inst.result)
            {
                value(*inst.result);
                output_ << " = ";
            }

            // العملية
            opcode(inst.opcode);

            // المعاملات
            if (!inst.operands.empty())
            {
                output_ << "(";
                for (size_t i = 0; i < inst.operands.size(); i++)
                {
                    if (i > 0)
                        output_ << "، ";
                    printOperand(inst.operands[i]);
                }
                output_ << ")";
            }

            // التعليق
            if (opts_.showComments && !inst.comment.empty())
            {
                output_ << "  ";
                comment("; " + inst.comment);
            }
        }

        // طباعة معامل
        void printOperand(const Operand &op)
        {
            std::visit([this](auto &&arg)
                       {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, ConstantOperand>) {
                constant(arg.toString());
            } else if constexpr (std::is_same_v<T, ValueOperand>) {
                value(arg.id);
            } else if constexpr (std::is_same_v<T, BlockOperand>) {
                label("bb" + std::to_string(arg.id));
            } else if constexpr (std::is_same_v<T, FunctionOperand>) {
                funcName("@" + arg.name);
            } else if constexpr (std::is_same_v<T, FieldOperand>) {
                output_ << "." << arg.name;
            } else if constexpr (std::is_same_v<T, TypeOperand>) {
                typeName(arg.type);
            } }, op);
        }

        // ═══════════════════════════════════════════════════════════════
        // مساعدات التنسيق
        // ═══════════════════════════════════════════════════════════════

        void printLineNumber()
        {
            if (opts_.showLineNumbers)
            {
                if (opts_.useColors)
                    output_ << colors::GRAY;
                output_ << std::setw(4) << lineNum_++ << " │ ";
                if (opts_.useColors)
                    output_ << colors::RESET;
            }
        }

        void indent(int level)
        {
            for (int i = 0; i < level * opts_.indentWidth; i++)
            {
                output_ << " ";
            }
        }

        void keyword(const std::string &kw)
        {
            if (opts_.useColors)
                output_ << colors::MAGENTA;
            output_ << kw;
            if (opts_.useColors)
                output_ << colors::RESET;
        }

        void opcode(Opcode op)
        {
            if (opts_.useColors)
                output_ << colors::CYAN;
            output_ << opcodeToString(op);
            if (opts_.useColors)
                output_ << colors::RESET;
        }

        void value(ValueId id)
        {
            if (opts_.useColors)
                output_ << colors::YELLOW;
            output_ << "%" << id;
            if (opts_.useColors)
                output_ << colors::RESET;
        }

        void constant(const std::string &c)
        {
            if (opts_.useColors)
                output_ << colors::GREEN;
            output_ << c;
            if (opts_.useColors)
                output_ << colors::RESET;
        }

        void label(const std::string &lbl)
        {
            if (opts_.useColors)
                output_ << colors::BLUE;
            output_ << lbl;
            if (opts_.useColors)
                output_ << colors::RESET;
        }

        void funcName(const std::string &name)
        {
            if (opts_.useColors)
                output_ << colors::GREEN;
            output_ << name;
            if (opts_.useColors)
                output_ << colors::RESET;
        }

        void typeName(const SadTypePtr &type)
        {
            if (opts_.useColors)
                output_ << colors::BLUE;
            output_ << (type ? type->toString() : "?");
            if (opts_.useColors)
                output_ << colors::RESET;
        }

        void comment(const std::string &c)
        {
            if (opts_.useColors)
                output_ << colors::GRAY;
            output_ << c;
            if (opts_.useColors)
                output_ << colors::RESET;
        }

        std::string opcodeToString(Opcode op)
        {
            if (opts_.arabicKeywords)
            {
                switch (op)
                {
                case Opcode::Alloc:
                    return "تخصيص";
                case Opcode::Borrow:
                    return "استعارة";
                case Opcode::BorrowMut:
                    return "استعارة_متغيرة";
                case Opcode::Move:
                    return "نقل";
                case Opcode::Copy:
                    return "نسخ";
                case Opcode::Drop:
                    return "إسقاط";
                case Opcode::Clone:
                    return "استنساخ";
                case Opcode::EndBorrow:
                    return "إنهاء_استعارة";
                case Opcode::Reborrow:
                    return "إعادة_استعارة";
                case Opcode::Project:
                    return "حقل";
                case Opcode::Deref:
                    return "فك_مرجع";
                case Opcode::Take:
                    return "أخذ";
                case Opcode::Load:
                    return "تحميل";
                case Opcode::Store:
                    return "تخزين";
                case Opcode::Add:
                    return "جمع";
                case Opcode::Sub:
                    return "طرح";
                case Opcode::Mul:
                    return "ضرب";
                case Opcode::Div:
                    return "قسمة";
                case Opcode::FloorDiv:
                    return "قسمة_صحيحة";
                case Opcode::Mod:
                    return "باقي";
                case Opcode::Eq:
                    return "يساوي";
                case Opcode::Ne:
                    return "لا_يساوي";
                case Opcode::Lt:
                    return "أقل";
                case Opcode::Le:
                    return "أقل_أو_يساوي";
                case Opcode::Gt:
                    return "أكبر";
                case Opcode::Ge:
                    return "أكبر_أو_يساوي";
                case Opcode::And:
                    return "و";
                case Opcode::Or:
                    return "أو";
                case Opcode::Not:
                    return "ليس";
                case Opcode::Jump:
                    return "قفز";
                case Opcode::Branch:
                    return "تفرع";
                case Opcode::Return:
                    return "أرجع";
                case Opcode::Call:
                    return "استدعاء";
                case Opcode::UICreateWidget:
                    return "إنشاء_عنصر";
                case Opcode::UISetProperty:
                    return "تعيين_خاصية";
                case Opcode::UIGetProperty:
                    return "قراءة_خاصية";
                case Opcode::UIAddChild:
                    return "إضافة_ابن";
                case Opcode::UIRemoveChild:
                    return "إزالة_ابن";
                case Opcode::UIBindEvent:
                    return "ربط_حدث";
                case Opcode::UIEmitEvent:
                    return "إطلاق_حدث";
                case Opcode::UISetLayout:
                    return "تعيين_تخطيط";
                case Opcode::UIRender:
                    return "عرض";
                case Opcode::UIRunApp:
                    return "تشغيل_تطبيق";
                default:
                    return "مجهول";
                }
            }
            else
            {
                switch (op)
                {
                case Opcode::Alloc:
                    return "alloc";
                case Opcode::Borrow:
                    return "borrow";
                case Opcode::BorrowMut:
                    return "borrow_mut";
                case Opcode::Move:
                    return "move";
                case Opcode::Copy:
                    return "copy";
                case Opcode::Drop:
                    return "drop";
                case Opcode::Clone:
                    return "clone";
                case Opcode::EndBorrow:
                    return "end_borrow";
                case Opcode::Reborrow:
                    return "reborrow";
                case Opcode::Project:
                    return "project";
                case Opcode::Deref:
                    return "deref";
                case Opcode::Take:
                    return "take";
                case Opcode::Load:
                    return "load";
                case Opcode::Store:
                    return "store";
                case Opcode::Add:
                    return "add";
                case Opcode::Sub:
                    return "sub";
                case Opcode::Mul:
                    return "mul";
                case Opcode::Div:
                    return "div";
                case Opcode::FloorDiv:
                    return "floordiv";
                case Opcode::Mod:
                    return "mod";
                case Opcode::Eq:
                    return "eq";
                case Opcode::Ne:
                    return "ne";
                case Opcode::Lt:
                    return "lt";
                case Opcode::Le:
                    return "le";
                case Opcode::Gt:
                    return "gt";
                case Opcode::Ge:
                    return "ge";
                case Opcode::And:
                    return "and";
                case Opcode::Or:
                    return "or";
                case Opcode::Not:
                    return "not";
                case Opcode::Jump:
                    return "jump";
                case Opcode::Branch:
                    return "branch";
                case Opcode::Return:
                    return "return";
                case Opcode::Call:
                    return "call";
                case Opcode::UICreateWidget:
                    return "ui.create";
                case Opcode::UISetProperty:
                    return "ui.set_prop";
                case Opcode::UIGetProperty:
                    return "ui.get_prop";
                case Opcode::UIAddChild:
                    return "ui.add_child";
                case Opcode::UIRemoveChild:
                    return "ui.remove_child";
                case Opcode::UIBindEvent:
                    return "ui.bind_event";
                case Opcode::UIEmitEvent:
                    return "ui.emit_event";
                case Opcode::UISetLayout:
                    return "ui.set_layout";
                case Opcode::UIRender:
                    return "ui.render";
                case Opcode::UIRunApp:
                    return "ui.run_app";
                default:
                    return "unknown";
                }
            }
        }

        PrinterOptions opts_;
        std::ostringstream output_;
        int lineNum_ = 1;
    };

    // =============================================================================
    // طباعة مخطط بياني (DOT)
    // =============================================================================

    /**
     * طباعة SIR كرسم بياني DOT
     */
    class SirDotPrinter
    {
    public:
        std::string print(const SirFunction &func)
        {
            std::ostringstream dot;

            dot << "digraph \"" << func.name << "\" {\n";
            dot << "  rankdir=TB;\n";
            dot << "  node [shape=box, fontname=\"Courier New\"];\n";
            dot << "  edge [fontname=\"Courier New\"];\n\n";

            // الكتل كعقد
            for (const auto &block : func.blocks)
            {
                dot << "  " << block.name << " [label=\"";
                dot << block.name << ":\\l";

                for (const auto &inst : block.instructions)
                {
                    dot << "  " << escapeLabel(inst.toString()) << "\\l";
                }

                dot << "\"];\n";
            }

            dot << "\n";

            // الحواف (تدفق التحكم)
            for (const auto &block : func.blocks)
            {
                for (BlockId succ : block.successors)
                {
                    dot << "  " << block.name << " -> bb" << succ << ";\n";
                }
            }

            dot << "}\n";

            return dot.str();
        }

    private:
        std::string escapeLabel(const std::string &s)
        {
            std::string result;
            for (char c : s)
            {
                switch (c)
                {
                case '"':
                    result += "\\\"";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                case '\n':
                    result += "\\l";
                    break;
                default:
                    result += c;
                    break;
                }
            }
            return result;
        }
    };

} // namespace sad::sir

// =============================================================================
// واجهة C
// =============================================================================

extern "C"
{

    struct SadSirPrinter
    {
        sad::sir::SirPrinter *printer;
        sad::sir::SirDotPrinter *dotPrinter;
        sad::sir::PrinterOptions options;
    };

    SadSirPrinter *sad_sir_printer_new()
    {
        auto *ctx = new SadSirPrinter();
        ctx->printer = new sad::sir::SirPrinter(ctx->options);
        ctx->dotPrinter = new sad::sir::SirDotPrinter();
        return ctx;
    }

    void sad_sir_printer_set_options(SadSirPrinter *ctx,
                                     int showLineNumbers,
                                     int showComments,
                                     int useColors,
                                     int arabicKeywords)
    {
        ctx->options.showLineNumbers = showLineNumbers != 0;
        ctx->options.showComments = showComments != 0;
        ctx->options.useColors = useColors != 0;
        ctx->options.arabicKeywords = arabicKeywords != 0;

        delete ctx->printer;
        ctx->printer = new sad::sir::SirPrinter(ctx->options);
    }

    const char *sad_sir_print_module(SadSirPrinter *ctx, void *module)
    {
        // (AR) thread_local لتجنب سباق البيانات / (EN) thread_local to avoid data races
        thread_local std::string result;

        if (!ctx || !module)
        {
            result = "";
            return result.c_str();
        }

        auto *mod = static_cast<sad::sir::SirModule *>(module);
        result = ctx->printer->print(*mod);
        return result.c_str();
    }

    const char *sad_sir_print_function(SadSirPrinter *ctx, void *function)
    {
        thread_local std::string result;

        if (!ctx || !function)
        {
            result = "";
            return result.c_str();
        }

        auto *func = static_cast<sad::sir::SirFunction *>(function);
        result = ctx->printer->print(*func);
        return result.c_str();
    }

    const char *sad_sir_print_dot(SadSirPrinter *ctx, void *function)
    {
        thread_local std::string result;

        if (!ctx || !function)
        {
            result = "";
            return result.c_str();
        }

        auto *func = static_cast<sad::sir::SirFunction *>(function);
        result = ctx->dotPrinter->print(*func);
        return result.c_str();
    }

    void sad_sir_printer_free(SadSirPrinter *ctx)
    {
        if (ctx)
        {
            delete ctx->printer;
            delete ctx->dotPrinter;
            delete ctx;
        }
    }

} // extern "C"
