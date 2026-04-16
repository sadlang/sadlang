/**
 * =============================================================================
 * ملف: builtin_intrinsics.cpp
 * الوصف: ربط الدوال المدمجة بعمليات SIR
 * =============================================================================
 *
 * يقوم هذا الملف بتحويل استدعاءات الدوال المدمجة إلى تعليمات SIR المناسبة
 *
 * =============================================================================
 */

#include "sir_opcodes.h"
#include "sir_builder.h"
#include <string>
#include <unordered_map>
#include <functional>

namespace sad::sir
{

    // =============================================================================
    // جدول الدوال المدمجة
    // =============================================================================

    /**
     * نوع معالج الدالة المدمجة
     * يأخذ قائمة المعاملات ويُرجع تعليمة SIR
     */
    using BuiltinHandler = std::function<SirInstruction(
        const std::vector<ValueId> &args,
        std::optional<ValueId> result)>;

    /**
     * معلومات الدالة المدمجة
     */
    struct BuiltinInfo
    {
        std::string name;        // الاسم العربي
        std::string englishName; // الاسم الإنجليزي (للتوثيق)
        Opcode opcode;           // العملية المقابلة
        bool hasResult;          // هل لها نتيجة
        int argCount;            // عدد المعاملات
    };

    /**
     * جدول الدوال المدمجة
     */
    static const std::unordered_map<std::string, BuiltinInfo> builtinTable = {
        // --- الذاكرة المباشرة ---
        {"اقرأ_بايت", {"اقرأ_بايت", "raw_load8", Opcode::RawLoad8, true, 1}},
        {"اقرأ_كلمة", {"اقرأ_كلمة", "raw_load16", Opcode::RawLoad16, true, 1}},
        {"اقرأ_مزدوج", {"اقرأ_مزدوج", "raw_load32", Opcode::RawLoad32, true, 1}},
        {"اقرأ_رباعي", {"اقرأ_رباعي", "raw_load64", Opcode::RawLoad64, true, 1}},
        {"اكتب_بايت", {"اكتب_بايت", "raw_store8", Opcode::RawStore8, false, 2}},
        {"اكتب_كلمة", {"اكتب_كلمة", "raw_store16", Opcode::RawStore16, false, 2}},
        {"اكتب_مزدوج", {"اكتب_مزدوج", "raw_store32", Opcode::RawStore32, false, 2}},
        {"اكتب_رباعي", {"اكتب_رباعي", "raw_store64", Opcode::RawStore64, false, 2}},

        // --- منافذ I/O ---
        {"اقرأ_منفذ8", {"اقرأ_منفذ8", "port_in8", Opcode::PortIn8, true, 1}},
        {"اقرأ_منفذ16", {"اقرأ_منفذ16", "port_in16", Opcode::PortIn16, true, 1}},
        {"اقرأ_منفذ32", {"اقرأ_منفذ32", "port_in32", Opcode::PortIn32, true, 1}},
        {"اكتب_منفذ8", {"اكتب_منفذ8", "port_out8", Opcode::PortOut8, false, 2}},
        {"اكتب_منفذ16", {"اكتب_منفذ16", "port_out16", Opcode::PortOut16, false, 2}},
        {"اكتب_منفذ32", {"اكتب_منفذ32", "port_out32", Opcode::PortOut32, false, 2}},

        // --- التحكم بالمعالج ---
        {"عطّل_مقاطعات", {"عطّل_مقاطعات", "cli", Opcode::Cli, false, 0}},
        {"فعّل_مقاطعات", {"فعّل_مقاطعات", "sti", Opcode::Sti, false, 0}},
        {"توقف", {"توقف", "hlt", Opcode::Hlt, false, 0}},
        {"انتظر_قصير", {"انتظر_قصير", "pause", Opcode::Pause, false, 0}},
        {"لا_عملية", {"لا_عملية", "nop", Opcode::Nop, false, 0}},

        // --- سجلات التحكم ---
        {"اقرأ_سجل_تحكم0", {"اقرأ_سجل_تحكم0", "read_cr0", Opcode::ReadCr0, true, 0}},
        {"اقرأ_سجل_تحكم2", {"اقرأ_سجل_تحكم2", "read_cr2", Opcode::ReadCr2, true, 0}},
        {"اقرأ_سجل_تحكم3", {"اقرأ_سجل_تحكم3", "read_cr3", Opcode::ReadCr3, true, 0}},
        {"اقرأ_سجل_تحكم4", {"اقرأ_سجل_تحكم4", "read_cr4", Opcode::ReadCr4, true, 0}},
        {"اكتب_سجل_تحكم0", {"اكتب_سجل_تحكم0", "write_cr0", Opcode::WriteCr0, false, 1}},
        {"اكتب_سجل_تحكم3", {"اكتب_سجل_تحكم3", "write_cr3", Opcode::WriteCr3, false, 1}},
        {"اكتب_سجل_تحكم4", {"اكتب_سجل_تحكم4", "write_cr4", Opcode::WriteCr4, false, 1}},

        // --- العمليات الذرية ---
        {"تحميل_ذري", {"تحميل_ذري", "atomic_load", Opcode::AtomicLoad, true, 1}},
        {"تخزين_ذري", {"تخزين_ذري", "atomic_store", Opcode::AtomicStore, false, 2}},
        {"إضافة_ذرية", {"إضافة_ذرية", "atomic_add", Opcode::AtomicAdd, true, 2}},
        {"طرح_ذري", {"طرح_ذري", "atomic_sub", Opcode::AtomicSub, true, 2}},
        {"تبادل_ذري", {"تبادل_ذري", "atomic_xchg", Opcode::AtomicExchange, true, 2}},
        {"قارن_وبدّل_ذري", {"قارن_وبدّل_ذري", "atomic_cmpxchg", Opcode::AtomicCmpXchg, true, 3}},

        // --- حواجز الذاكرة ---
        {"حاجز_ذاكرة", {"حاجز_ذاكرة", "mem_fence", Opcode::MemFence, false, 0}},
        {"حاجز_قراءة", {"حاجز_قراءة", "load_fence", Opcode::LoadFence, false, 0}},
        {"حاجز_كتابة", {"حاجز_كتابة", "store_fence", Opcode::StoreFence, false, 0}},

        // --- المقاطعات ---
        {"استدعِ_مقاطعة", {"استدعِ_مقاطعة", "int", Opcode::Int, false, 1}},

        // --- الذاكرة الافتراضية ---
        {"أبطل_صفحة", {"أبطل_صفحة", "invlpg", Opcode::Invlpg, false, 1}},

        // --- جداول الواصفات ---
        {"حمّل_جدول_واصفات_عام", {"حمّل_جدول_واصفات_عام", "lgdt", Opcode::Lgdt, false, 2}},
        {"حمّل_جدول_مقاطعات", {"حمّل_جدول_مقاطعات", "lidt", Opcode::Lidt, false, 2}},
        {"حمّل_سجل_مهمة", {"حمّل_سجل_مهمة", "ltr", Opcode::Ltr, false, 1}},
    };

    // =============================================================================
    // واجهة استعلام الدوال المدمجة
    // =============================================================================

    /**
     * التحقق من أن الاسم دالة مدمجة
     */
    bool isBuiltinFunction(const std::string &name)
    {
        return builtinTable.find(name) != builtinTable.end();
    }

    /**
     * الحصول على معلومات الدالة المدمجة
     */
    const BuiltinInfo *getBuiltinInfo(const std::string &name)
    {
        auto it = builtinTable.find(name);
        if (it != builtinTable.end())
        {
            return &it->second;
        }
        return nullptr;
    }

    /**
     * إنشاء تعليمة SIR لدالة مدمجة
     */
    SirInstruction createBuiltinInstruction(
        const std::string &name,
        const std::vector<Operand> &args,
        std::optional<ValueId> result,
        SadTypePtr resultType = nullptr)
    {
        auto info = getBuiltinInfo(name);
        if (!info)
        {
            // دالة غير مدمجة - إرجاع تعليمة فارغة
            return SirInstruction{Opcode::Nop, std::nullopt, {}, {}, "unknown builtin: " + name};
        }

        SirInstruction inst;
        inst.opcode = info->opcode;
        inst.operands = args;
        inst.result = info->hasResult ? result : std::nullopt;
        inst.resultType = resultType;
        inst.comment = info->englishName;

        return inst;
    }

    // =============================================================================
    // جدول English aliases (للمطورين الذين يفضلون الإنجليزية)
    // =============================================================================

    static const std::unordered_map<std::string, std::string> englishToArabic = {
        // Memory
        {"raw_load8", "اقرأ_بايت"},
        {"raw_load16", "اقرأ_كلمة"},
        {"raw_load32", "اقرأ_مزدوج"},
        {"raw_load64", "اقرأ_رباعي"},
        {"raw_store8", "اكتب_بايت"},
        {"raw_store16", "اكتب_كلمة"},
        {"raw_store32", "اكتب_مزدوج"},
        {"raw_store64", "اكتب_رباعي"},

        // I/O Ports
        {"port_in8", "اقرأ_منفذ8"},
        {"port_in16", "اقرأ_منفذ16"},
        {"port_in32", "اقرأ_منفذ32"},
        {"port_out8", "اكتب_منفذ8"},
        {"port_out16", "اكتب_منفذ16"},
        {"port_out32", "اكتب_منفذ32"},

        // CPU Control
        {"cli", "عطّل_مقاطعات"},
        {"sti", "فعّل_مقاطعات"},
        {"hlt", "توقف"},
        {"pause", "انتظر_قصير"},
        {"nop", "لا_عملية"},

        // Control Registers
        {"read_cr0", "اقرأ_سجل_تحكم0"},
        {"read_cr2", "اقرأ_سجل_تحكم2"},
        {"read_cr3", "اقرأ_سجل_تحكم3"},
        {"read_cr4", "اقرأ_سجل_تحكم4"},
        {"write_cr0", "اكتب_سجل_تحكم0"},
        {"write_cr3", "اكتب_سجل_تحكم3"},
        {"write_cr4", "اكتب_سجل_تحكم4"},

        // Atomics
        {"atomic_load", "تحميل_ذري"},
        {"atomic_store", "تخزين_ذري"},
        {"atomic_add", "إضافة_ذرية"},
        {"atomic_sub", "طرح_ذري"},
        {"atomic_xchg", "تبادل_ذري"},
        {"atomic_cmpxchg", "قارن_وبدّل_ذري"},

        // Fences
        {"mfence", "حاجز_ذاكرة"},
        {"lfence", "حاجز_قراءة"},
        {"sfence", "حاجز_كتابة"},

        // Interrupts
        {"int", "استدعِ_مقاطعة"},

        // Virtual Memory
        {"invlpg", "أبطل_صفحة"},

        // Descriptor Tables
        {"lgdt", "حمّل_جدول_واصفات_عام"},
        {"lidt", "حمّل_جدول_مقاطعات"},
        {"ltr", "حمّل_سجل_مهمة"},
    };

    /**
     * تحويل اسم إنجليزي إلى عربي
     */
    const char *englishToArabicName(const std::string &englishName)
    {
        auto it = englishToArabic.find(englishName);
        if (it != englishToArabic.end())
        {
            return it->second.c_str();
        }
        return nullptr;
    }

} // namespace sad::sir

// =============================================================================
// واجهة C
// =============================================================================

extern "C"
{

    int sad_is_builtin(const char *name)
    {
        return sad::sir::isBuiltinFunction(name) ? 1 : 0;
    }

    const char *sad_builtin_english_name(const char *arabicName)
    {
        auto info = sad::sir::getBuiltinInfo(arabicName);
        return info ? info->englishName.c_str() : nullptr;
    }

    int sad_builtin_opcode(const char *name)
    {
        auto info = sad::sir::getBuiltinInfo(name);
        return info ? static_cast<int>(info->opcode) : -1;
    }

    int sad_builtin_has_result(const char *name)
    {
        auto info = sad::sir::getBuiltinInfo(name);
        return info ? (info->hasResult ? 1 : 0) : 0;
    }

    int sad_builtin_arg_count(const char *name)
    {
        auto info = sad::sir::getBuiltinInfo(name);
        return info ? info->argCount : -1;
    }

} // extern "C"
