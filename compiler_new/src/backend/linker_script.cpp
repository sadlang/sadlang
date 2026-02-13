/**
 * @file linker_script.cpp
 * @brief (AR) دعم سكريبتات الرابط المخصصة للأنظمة المدمجة
 *        (EN) Custom linker script support for embedded systems
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     ██╗     ██╗███╗   ██╗██╗  ██╗███████╗██████╗                              ║
 * ║     ██║     ██║████╗  ██║██║ ██╔╝██╔════╝██╔══██╗                             ║
 * ║     ██║     ██║██╔██╗ ██║█████╔╝ █████╗  ██████╔╝                             ║
 * ║     ██║     ██║██║╚██╗██║██╔═██╗ ██╔══╝  ██╔══██╗                             ║
 * ║     ███████╗██║██║ ╚████║██║  ██╗███████╗██║  ██║                             ║
 * ║     ╚══════╝╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝                             ║
 * ║                                                                               ║
 * ║         ███████╗ ██████╗██████╗ ██╗██████╗ ████████╗                          ║
 * ║         ██╔════╝██╔════╝██╔══██╗██║██╔══██╗╚══██╔══╝                          ║
 * ║         ███████╗██║     ██████╔╝██║██████╔╝   ██║                             ║
 * ║         ╚════██║██║     ██╔══██╗██║██╔═══╝    ██║                             ║
 * ║         ███████║╚██████╗██║  ██║██║██║        ██║                             ║
 * ║         ╚══════╝ ╚═════╝╚═╝  ╚═╝╚═╝╚═╝        ╚═╝                             ║
 * ║                                                                               ║
 * ║                     سكريبت الرابط - Linker Script                             ║
 * ║                     لغة ص للتحكم في تخطيط الذاكرة                             ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يدير سكريبتات الرابط للأنظمة المدمجة:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     خريطة الذاكرة (Memory Map)                           │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    العنوان                   القسم                  الوصف               │
 *      │    ────────                  ───────                ──────              │
 *      │    0x0000_0000    ┌───────────────────┐    جدول المتجهات                │
 *      │                   │   .isr_vector     │    (Reset, NMI, HardFault...)   │
 *      │    0x0000_0200    ├───────────────────┤                                 │
 *      │                   │      .text        │    كود البرنامج                 │
 *      │    0x0001_0000    ├───────────────────┤                                 │
 *      │                   │     .rodata       │    بيانات للقراءة فقط           │
 *      │    0x2000_0000    ├───────────────────┤                                 │
 *      │                   │      .data        │    بيانات مهيأة                 │
 *      │                   ├───────────────────┤                                 │
 *      │                   │      .bss         │    بيانات غير مهيأة             │
 *      │                   ├───────────────────┤                                 │
 *      │                   │      .heap        │    الكومة (اختياري)             │
 *      │                   ├───────────────────┤                                 │
 *      │                   │     .stack        │    المكدس                       │
 *      │    0x2002_0000    └───────────────────┘                                 │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 *      الاستخدام في كود ص:
 *      @code
 *      #![بلا_مكتبة_قياسية]
 *      #![رابط(سكريبت = "memory.ld")]
 *      
 *      // أو تحديد الذاكرة مباشرة:
 *      #![ذاكرة(
 *          فلاش = { أصل: 0x08000000, حجم: 512ك },
 *          رام = { أصل: 0x20000000, حجم: 128ك }
 *      )]
 *      @endcode
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <memory>
#include <sstream>
#include <cstdint>
#include <algorithm>

namespace sad {
namespace compiler {
namespace backend {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع مناطق الذاكرة
//                    (EN) Memory Region Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @enum MemoryRegionType
 * @brief (AR) أنواع مناطق الذاكرة
 *        (EN) Memory region types
 */
enum class MemoryRegionType {
    FLASH,              // (AR) ذاكرة Flash (للقراءة فقط عادة)
    RAM,                // (AR) ذاكرة RAM
    ROM,                // (AR) ذاكرة ROM
    SRAM,               // (AR) SRAM سريعة
    CCM,                // (AR) Core-Coupled Memory
    EXTERNAL,           // (AR) ذاكرة خارجية
    PERIPHERAL,         // (AR) منطقة الأجهزة الطرفية
};

/**
 * @enum MemoryPermission
 * @brief (AR) صلاحيات الذاكرة
 *        (EN) Memory permissions
 */
enum class MemoryPermission {
    R = 0x01,           // (AR) قراءة
    W = 0x02,           // (AR) كتابة
    X = 0x04,           // (AR) تنفيذ
    RW = R | W,         // (AR) قراءة + كتابة
    RX = R | X,         // (AR) قراءة + تنفيذ
    RWX = R | W | X,    // (AR) الكل
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) هياكل البيانات
//                    (EN) Data Structures
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @struct MemoryRegion
 * @brief (AR) منطقة ذاكرة واحدة
 *        (EN) Single memory region
 */
struct MemoryRegion {
    std::string name;               // (AR) اسم المنطقة (مثل FLASH, RAM)
    std::string arabicName;         // (AR) الاسم بالعربية
    MemoryRegionType type;          // (AR) نوع المنطقة
    uint64_t origin;                // (AR) عنوان البداية
    uint64_t length;                // (AR) الحجم بالبايت
    int permissions;                // (AR) الصلاحيات
    
    // (AR) حساب نهاية المنطقة
    uint64_t end() const { return origin + length; }
};

/**
 * @struct OutputSection
 * @brief (AR) قسم الإخراج في سكريبت الرابط
 *        (EN) Output section in linker script
 */
struct OutputSection {
    std::string name;               // (AR) اسم القسم (مثل .text, .data)
    std::string arabicName;         // (AR) الاسم بالعربية
    std::string memoryRegion;       // (AR) المنطقة المستهدفة
    std::string loadRegion;         // (AR) منطقة التحميل (للـ .data المنسوخة)
    std::vector<std::string> inputSections;  // (AR) الأقسام المدخلة
    uint64_t alignment = 4;         // (AR) المحاذاة
    bool keep = false;              // (AR) لا تحذف حتى لو غير مستخدمة
    
    // (AR) رموز البداية والنهاية
    std::string startSymbol;
    std::string endSymbol;
    std::string loadSymbol;
};

/**
 * @struct LinkerScriptConfig
 * @brief (AR) تكوين سكريبت الرابط الكامل
 *        (EN) Complete linker script configuration
 */
struct LinkerScriptConfig {
    // (AR) نقطة الدخول
    std::string entryPoint = "_start";
    
    // (AR) مناطق الذاكرة
    std::vector<MemoryRegion> memoryRegions;
    
    // (AR) أقسام الإخراج
    std::vector<OutputSection> sections;
    
    // (AR) حجم المكدس
    uint64_t stackSize = 0x4000;  // (AR) 16KB افتراضياً
    
    // (AR) حجم الكومة
    uint64_t heapSize = 0;  // (AR) معطلة افتراضياً في no_std
    
    // (AR) الهدف
    std::string targetArch = "arm-none-eabi";
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) سمات الرابط العربية
//                    (EN) Arabic Linker Attributes
// ═══════════════════════════════════════════════════════════════════════════════

namespace سمات_الرابط {
    // (AR) السمات الرئيسية
    const std::string رابط = u8"رابط";
    const std::string سكريبت = u8"سكريبت";
    const std::string ذاكرة = u8"ذاكرة";
    const std::string قسم = u8"قسم";
    
    // (AR) أنواع الذاكرة
    const std::string فلاش = u8"فلاش";
    const std::string رام = u8"رام";
    const std::string روم = u8"روم";
    const std::string خارجية = u8"خارجية";
    
    // (AR) خصائص المنطقة
    const std::string أصل = u8"أصل";
    const std::string حجم = u8"حجم";
    const std::string قراءة = u8"قراءة";
    const std::string كتابة = u8"كتابة";
    const std::string تنفيذ = u8"تنفيذ";
    
    // (AR) أسماء الأقسام
    const std::string كود = u8"كود";              // .text
    const std::string بيانات_ثابتة = u8"بيانات_ثابتة";  // .rodata
    const std::string بيانات = u8"بيانات";        // .data
    const std::string صفرية = u8"صفرية";          // .bss
    const std::string مكدس = u8"مكدس";            // .stack
    const std::string كومة = u8"كومة";            // .heap
}

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) محلل سمات الرابط
//                    (EN) Linker Attribute Parser
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class LinkerAttributeParser
 * @brief (AR) محلل سمات الرابط من كود ص
 *        (EN) Linker attribute parser from Sad code
 */
class LinkerAttributeParser {
public:
    /**
     * @brief (AR) تحليل سمة ذاكرة
     *        (EN) Parse memory attribute
     */
    bool parseMemoryAttribute(
        const std::string& value,
        std::vector<MemoryRegion>& regions
    ) {
        // (AR) تحليل: ذاكرة(فلاش = { أصل: 0x08000000, حجم: 512ك })
        // (EN) Parse: memory(flash = { origin: 0x08000000, size: 512K })
        
        size_t pos = 0;
        while (pos < value.length()) {
            // (AR) تخطي المسافات
            while (pos < value.length() && std::isspace(value[pos])) pos++;
            
            // (AR) البحث عن اسم المنطقة
            std::string regionName;
            while (pos < value.length() && value[pos] != '=' && !std::isspace(value[pos])) {
                regionName += value[pos];
                pos++;
            }
            
            if (regionName.empty()) break;
            
            // (AR) إنشاء منطقة جديدة
            MemoryRegion region;
            region.name = translateRegionName(regionName);
            region.arabicName = regionName;
            region.type = getRegionType(region.name);
            region.permissions = static_cast<int>(getDefaultPermissions(region.type));
            
            // (AR) البحث عن القيم
            size_t braceStart = value.find('{', pos);
            size_t braceEnd = value.find('}', braceStart);
            
            if (braceStart != std::string::npos && braceEnd != std::string::npos) {
                std::string props = value.substr(braceStart + 1, braceEnd - braceStart - 1);
                parseRegionProperties(props, region);
                pos = braceEnd + 1;
            }
            
            regions.push_back(region);
            
            // (AR) تخطي الفاصلة
            while (pos < value.length() && (value[pos] == ',' || std::isspace(value[pos]))) pos++;
        }
        
        return !regions.empty();
    }
    
    /**
     * @brief (AR) تحليل سمة قسم
     *        (EN) Parse section attribute
     */
    bool parseSectionAttribute(const std::string& value, OutputSection& section) {
        // (AR) تحليل: #[قسم("اسم", منطقة = "FLASH")]
        
        size_t nameStart = value.find('"');
        size_t nameEnd = value.find('"', nameStart + 1);
        
        if (nameStart != std::string::npos && nameEnd != std::string::npos) {
            section.arabicName = value.substr(nameStart + 1, nameEnd - nameStart - 1);
            section.name = translateSectionName(section.arabicName);
        }
        
        // (AR) البحث عن المنطقة
        size_t regionPos = value.find(u8"منطقة");
        if (regionPos == std::string::npos) {
            regionPos = value.find("region");
        }
        
        if (regionPos != std::string::npos) {
            size_t regStart = value.find('"', regionPos);
            size_t regEnd = value.find('"', regStart + 1);
            if (regStart != std::string::npos && regEnd != std::string::npos) {
                section.memoryRegion = value.substr(regStart + 1, regEnd - regStart - 1);
            }
        }
        
        return !section.name.empty();
    }
    
private:
    std::string translateRegionName(const std::string& name) {
        if (name == سمات_الرابط::فلاش || name == "flash") return "FLASH";
        if (name == سمات_الرابط::رام || name == "ram") return "RAM";
        if (name == سمات_الرابط::روم || name == "rom") return "ROM";
        if (name == سمات_الرابط::خارجية || name == "external") return "EXTERNAL";
        return name;
    }
    
    std::string translateSectionName(const std::string& name) {
        if (name == سمات_الرابط::كود || name == "text") return ".text";
        if (name == سمات_الرابط::بيانات_ثابتة || name == "rodata") return ".rodata";
        if (name == سمات_الرابط::بيانات || name == "data") return ".data";
        if (name == سمات_الرابط::صفرية || name == "bss") return ".bss";
        if (name == سمات_الرابط::مكدس || name == "stack") return ".stack";
        if (name == سمات_الرابط::كومة || name == "heap") return ".heap";
        return "." + name;
    }
    
    MemoryRegionType getRegionType(const std::string& name) {
        if (name == "FLASH") return MemoryRegionType::FLASH;
        if (name == "RAM") return MemoryRegionType::RAM;
        if (name == "ROM") return MemoryRegionType::ROM;
        if (name == "SRAM") return MemoryRegionType::SRAM;
        if (name == "CCM") return MemoryRegionType::CCM;
        if (name == "EXTERNAL") return MemoryRegionType::EXTERNAL;
        return MemoryRegionType::RAM;
    }
    
    MemoryPermission getDefaultPermissions(MemoryRegionType type) {
        switch (type) {
            case MemoryRegionType::FLASH:
            case MemoryRegionType::ROM:
                return MemoryPermission::RX;
            case MemoryRegionType::RAM:
            case MemoryRegionType::SRAM:
            case MemoryRegionType::CCM:
            case MemoryRegionType::EXTERNAL:
                return MemoryPermission::RW;
            case MemoryRegionType::PERIPHERAL:
                return MemoryPermission::RW;
            default:
                return MemoryPermission::RW;
        }
    }
    
    void parseRegionProperties(const std::string& props, MemoryRegion& region) {
        // (AR) تحليل: أصل: 0x08000000, حجم: 512ك
        
        // (AR) البحث عن الأصل
        size_t originPos = props.find(سمات_الرابط::أصل);
        if (originPos == std::string::npos) {
            originPos = props.find("origin");
        }
        if (originPos != std::string::npos) {
            region.origin = parseNumber(props, originPos);
        }
        
        // (AR) البحث عن الحجم
        size_t sizePos = props.find(سمات_الرابط::حجم);
        if (sizePos == std::string::npos) {
            sizePos = props.find("size");
        }
        if (sizePos != std::string::npos) {
            region.length = parseNumber(props, sizePos);
        }
    }
    
    uint64_t parseNumber(const std::string& str, size_t startPos) {
        // (AR) البحث عن الرقم بعد :
        size_t colonPos = str.find(':', startPos);
        if (colonPos == std::string::npos) return 0;
        
        size_t numStart = colonPos + 1;
        while (numStart < str.length() && std::isspace(str[numStart])) numStart++;
        
        std::string numStr;
        while (numStart < str.length() && 
               (std::isxdigit(str[numStart]) || str[numStart] == 'x' || 
                str[numStart] == 'X' || str[numStart] == 'k' || str[numStart] == 'K' ||
                str[numStart] == 'm' || str[numStart] == 'M' || str[numStart] == u8"ك"[0])) {
            numStr += str[numStart];
            numStart++;
        }
        
        // (AR) تحويل الرقم
        uint64_t multiplier = 1;
        if (numStr.back() == 'k' || numStr.back() == 'K' || numStr.find(u8"ك") != std::string::npos) {
            multiplier = 1024;
            numStr.pop_back();
        } else if (numStr.back() == 'm' || numStr.back() == 'M') {
            multiplier = 1024 * 1024;
            numStr.pop_back();
        }
        
        uint64_t value = 0;
        if (numStr.find("0x") == 0 || numStr.find("0X") == 0) {
            value = std::stoull(numStr.substr(2), nullptr, 16);
        } else {
            value = std::stoull(numStr);
        }
        
        return value * multiplier;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولّد سكريبت الرابط
//                    (EN) Linker Script Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class LinkerScriptGenerator
 * @brief (AR) مولّد سكريبتات الرابط
 *        (EN) Linker script generator
 */
class LinkerScriptGenerator {
public:
    /**
     * @brief (AR) توليد سكريبت رابط كامل
     *        (EN) Generate complete linker script
     */
    std::string generate(const LinkerScriptConfig& config) {
        std::ostringstream script;
        
        // (AR) التعليقات والرأس
        script << generateHeader(config);
        
        // (AR) نقطة الدخول
        script << "ENTRY(" << config.entryPoint << ")\n\n";
        
        // (AR) تعريف مناطق الذاكرة
        script << generateMemorySection(config);
        
        // (AR) تعريف الأقسام
        script << generateSections(config);
        
        return script.str();
    }
    
    /**
     * @brief (AR) توليد سكريبت رابط ARM Cortex-M
     *        (EN) Generate ARM Cortex-M linker script
     */
    std::string generateARMCortexM(
        uint64_t flashOrigin,
        uint64_t flashSize,
        uint64_t ramOrigin,
        uint64_t ramSize,
        uint64_t stackSize = 0x4000
    ) {
        LinkerScriptConfig config;
        config.entryPoint = "Reset_Handler";
        config.stackSize = stackSize;
        
        // (AR) إضافة مناطق الذاكرة
        config.memoryRegions.push_back({
            "FLASH", u8"فلاش", MemoryRegionType::FLASH,
            flashOrigin, flashSize, static_cast<int>(MemoryPermission::RX)
        });
        config.memoryRegions.push_back({
            "RAM", u8"رام", MemoryRegionType::RAM,
            ramOrigin, ramSize, static_cast<int>(MemoryPermission::RWX)
        });
        
        // (AR) إضافة الأقسام القياسية
        addDefaultARMSections(config);
        
        return generate(config);
    }
    
    /**
     * @brief (AR) توليد سكريبت رابط x86 bare-metal
     *        (EN) Generate x86 bare-metal linker script
     */
    std::string generateX86BareMetal(uint64_t loadAddress = 0x100000) {
        std::ostringstream script;
        
        script << "/* " << u8"سكريبت رابط x86 bare-metal للغة ص" << " */\n";
        script << "/* Sad Language x86 Bare-metal Linker Script */\n\n";
        
        script << "OUTPUT_FORMAT(elf64-x86-64)\n";
        script << "ENTRY(_start)\n\n";
        
        script << "SECTIONS\n";
        script << "{\n";
        script << "    . = " << std::hex << "0x" << loadAddress << ";\n\n";
        
        // (AR) قسم .boot
        script << "    /* " << u8"قسم الإقلاع" << " */\n";
        script << "    .boot : {\n";
        script << "        KEEP(*(.multiboot))\n";
        script << "        KEEP(*(.multiboot2))\n";
        script << "    }\n\n";
        
        // (AR) قسم .text
        script << "    /* " << u8"قسم الكود" << " */\n";
        script << "    .text : {\n";
        script << "        *(.text._start)\n";
        script << "        *(.text.boot)\n";
        script << "        *(.text*)\n";
        script << "    }\n\n";
        
        // (AR) قسم .rodata
        script << "    /* " << u8"بيانات للقراءة فقط" << " */\n";
        script << "    .rodata : {\n";
        script << "        *(.rodata*)\n";
        script << "    }\n\n";
        
        // (AR) محاذاة للصفحات
        script << "    . = ALIGN(4096);\n\n";
        
        // (AR) قسم .data
        script << "    /* " << u8"بيانات مهيأة" << " */\n";
        script << "    .data : {\n";
        script << "        *(.data*)\n";
        script << "    }\n\n";
        
        // (AR) قسم .bss
        script << "    /* " << u8"بيانات غير مهيأة" << " */\n";
        script << "    .bss : {\n";
        script << "        __bss_start = .;\n";
        script << "        *(.bss*)\n";
        script << "        *(COMMON)\n";
        script << "        __bss_end = .;\n";
        script << "    }\n\n";
        
        // (AR) المكدس
        script << "    . = ALIGN(16);\n";
        script << "    __stack_bottom = .;\n";
        script << "    . += 0x10000;  /* 64KB stack */\n";
        script << "    __stack_top = .;\n\n";
        
        script << "    __kernel_end = .;\n";
        script << "}\n";
        
        return script.str();
    }
    
    /**
     * @brief (AR) توليد سكربت ربط تلقائي بناءً على الهدف
     *        (EN) Auto-generate linker script based on target triple
     */
    std::string generateForTarget(const std::string& targetTriple) {
        // x86_64 bare-metal
        if (targetTriple.find("x86_64") != std::string::npos &&
            (targetTriple.find("none") != std::string::npos || 
             targetTriple.find("bare") != std::string::npos ||
             targetTriple.find("elf") != std::string::npos)) {
            return generateX86BareMetal(0x100000);
        }
        
        // i686 bare-metal
        if ((targetTriple.find("i686") != std::string::npos || 
             targetTriple.find("i386") != std::string::npos) &&
            (targetTriple.find("none") != std::string::npos || 
             targetTriple.find("bare") != std::string::npos)) {
            return generateX86BareMetal(0x100000);
        }
        
        // ARM Cortex-M (Thumb)
        if (targetTriple.find("thumb") != std::string::npos ||
            (targetTriple.find("arm") != std::string::npos && 
             targetTriple.find("none-eabi") != std::string::npos)) {
            // STM32F4 defaults
            return generateARMCortexM(
                0x08000000, 512 * 1024,     // 512KB Flash
                0x20000000, 128 * 1024,     // 128KB RAM
                0x4000                       // 16KB Stack
            );
        }
        
        // AArch64 bare-metal
        if (targetTriple.find("aarch64") != std::string::npos && 
            targetTriple.find("none") != std::string::npos) {
            return generateX86BareMetal(0x80000);  // RPi3/4 load address
        }
        
        // RISC-V bare-metal
        if (targetTriple.find("riscv") != std::string::npos) {
            return generateX86BareMetal(0x80000000);  // RISC-V default
        }
        
        // الافتراضي: x86 kernel
        return generateX86BareMetal(0x100000);
    }
    
private:
    std::string generateHeader(const LinkerScriptConfig& config) {
        std::ostringstream header;
        
        header << "/* ═══════════════════════════════════════════════════════════════ */\n";
        header << "/* " << u8"سكريبت رابط لغة ص" << "                                        */\n";
        header << "/* Sad Language Linker Script                                      */\n";
        header << "/* " << u8"الهدف: " << config.targetArch << "                                   */\n";
        header << "/* ═══════════════════════════════════════════════════════════════ */\n\n";
        
        return header.str();
    }
    
    std::string generateMemorySection(const LinkerScriptConfig& config) {
        std::ostringstream mem;
        
        mem << "/* " << u8"تعريف مناطق الذاكرة" << " */\n";
        mem << "MEMORY\n";
        mem << "{\n";
        
        for (const auto& region : config.memoryRegions) {
            mem << "    " << region.name << " (" << getPermissionString(region.permissions) << ") : ";
            mem << "ORIGIN = 0x" << std::hex << region.origin << ", ";
            mem << "LENGTH = 0x" << std::hex << region.length;
            mem << "  /* " << region.arabicName << " */\n";
        }
        
        mem << "}\n\n";
        
        return mem.str();
    }
    
    std::string generateSections(const LinkerScriptConfig& config) {
        std::ostringstream sections;
        
        sections << "/* " << u8"تعريف أقسام الإخراج" << " */\n";
        sections << "SECTIONS\n";
        sections << "{\n";
        
        for (const auto& section : config.sections) {
            sections << "    /* " << section.arabicName << " */\n";
            sections << "    " << section.name;
            
            if (!section.loadRegion.empty() && section.loadRegion != section.memoryRegion) {
                sections << " : AT(__" << section.name.substr(1) << "_load)";
            } else {
                sections << " :";
            }
            
            sections << " {\n";
            
            // (AR) رمز البداية
            if (!section.startSymbol.empty()) {
                sections << "        " << section.startSymbol << " = .;\n";
            }
            
            // (AR) المحاذاة
            sections << "        . = ALIGN(" << section.alignment << ");\n";
            
            // (AR) أقسام الإدخال
            for (const auto& input : section.inputSections) {
                if (section.keep) {
                    sections << "        KEEP(*(" << input << "))\n";
                } else {
                    sections << "        *(" << input << ")\n";
                }
            }
            
            // (AR) رمز النهاية
            if (!section.endSymbol.empty()) {
                sections << "        " << section.endSymbol << " = .;\n";
            }
            
            sections << "    }";
            
            if (!section.memoryRegion.empty()) {
                sections << " > " << section.memoryRegion;
            }
            
            sections << "\n\n";
        }
        
        // (AR) المكدس
        sections << "    /* " << u8"المكدس" << " */\n";
        sections << "    . = ALIGN(8);\n";
        sections << "    __stack_bottom = .;\n";
        sections << "    . += 0x" << std::hex << config.stackSize << ";  /* Stack size */\n";
        sections << "    __stack_top = .;\n\n";
        
        // (AR) الكومة (إذا كانت مفعلة)
        if (config.heapSize > 0) {
            sections << "    /* " << u8"الكومة" << " */\n";
            sections << "    __heap_start = .;\n";
            sections << "    . += 0x" << std::hex << config.heapSize << ";  /* Heap size */\n";
            sections << "    __heap_end = .;\n\n";
        }
        
        sections << "    __end = .;\n";
        sections << "}\n";
        
        return sections.str();
    }
    
    std::string getPermissionString(int perms) {
        std::string result;
        if (perms & static_cast<int>(MemoryPermission::R)) result += "r";
        if (perms & static_cast<int>(MemoryPermission::W)) result += "w";
        if (perms & static_cast<int>(MemoryPermission::X)) result += "x";
        return result;
    }
    
    void addDefaultARMSections(LinkerScriptConfig& config) {
        // (AR) جدول المتجهات
        config.sections.push_back({
            ".isr_vector", u8"جدول_المتجهات", "FLASH", "",
            {".isr_vector"}, 4, true, "__isr_vector_start", "__isr_vector_end", ""
        });
        
        // (AR) قسم الكود
        config.sections.push_back({
            ".text", u8"كود", "FLASH", "",
            {".text*", ".glue_7", ".glue_7t"}, 4, false, "", "", ""
        });
        
        // (AR) بيانات للقراءة فقط
        config.sections.push_back({
            ".rodata", u8"بيانات_ثابتة", "FLASH", "",
            {".rodata*"}, 4, false, "", "", ""
        });
        
        // (AR) قسم init/fini
        config.sections.push_back({
            ".init_array", u8"مُنشئات", "FLASH", "",
            {".init_array*"}, 4, true, "__init_array_start", "__init_array_end", ""
        });
        
        // (AR) بيانات مهيأة (تُنسخ من Flash إلى RAM)
        config.sections.push_back({
            ".data", u8"بيانات", "RAM", "FLASH",
            {".data*"}, 4, false, "__data_start", "__data_end", "__data_load"
        });
        
        // (AR) بيانات غير مهيأة
        config.sections.push_back({
            ".bss", u8"صفرية", "RAM", "",
            {".bss*", "COMMON"}, 4, false, "__bss_start", "__bss_end", ""
        });
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مدير سكريبتات الرابط
//                    (EN) Linker Script Manager
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class LinkerScriptManager
 * @brief (AR) مدير سكريبتات الرابط
 *        (EN) Linker script manager
 */
class LinkerScriptManager {
public:
    LinkerScriptManager() = default;
    
    /**
     * @brief (AR) تحديد المعمارية الهدف
     *        (EN) Set target architecture
     */
    void setTargetArch(const std::string& arch) {
        config_.targetArch = arch;
    }
    
    /**
     * @brief (AR) إضافة منطقة ذاكرة
     *        (EN) Add memory region
     */
    void addMemoryRegion(const MemoryRegion& region) {
        config_.memoryRegions.push_back(region);
    }
    
    /**
     * @brief (AR) إضافة قسم
     *        (EN) Add section
     */
    void addSection(const OutputSection& section) {
        config_.sections.push_back(section);
    }
    
    /**
     * @brief (AR) تحديد حجم المكدس
     *        (EN) Set stack size
     */
    void setStackSize(uint64_t size) {
        config_.stackSize = size;
    }
    
    /**
     * @brief (AR) تحديد حجم الكومة
     *        (EN) Set heap size
     */
    void setHeapSize(uint64_t size) {
        config_.heapSize = size;
    }
    
    /**
     * @brief (AR) توليد السكريبت
     *        (EN) Generate script
     */
    std::string generateScript() {
        return generator_.generate(config_);
    }
    
    /**
     * @brief (AR) توليد سكريبت من قالب محدد مسبقاً
     *        (EN) Generate script from predefined template
     */
    std::string generateFromTemplate(const std::string& templateName) {
        if (templateName == "stm32f4" || templateName == u8"ستم32ف4") {
            return generator_.generateARMCortexM(
                0x08000000, 1024 * 1024,    // (AR) 1MB Flash
                0x20000000, 192 * 1024,     // (AR) 192KB RAM
                0x4000                       // (AR) 16KB Stack
            );
        }
        else if (templateName == "stm32f1" || templateName == u8"ستم32ف1") {
            return generator_.generateARMCortexM(
                0x08000000, 64 * 1024,      // (AR) 64KB Flash
                0x20000000, 20 * 1024,      // (AR) 20KB RAM
                0x1000                       // (AR) 4KB Stack
            );
        }
        else if (templateName == "rp2040" || templateName == u8"رازبيري_بيكو") {
            return generator_.generateARMCortexM(
                0x10000000, 2 * 1024 * 1024,  // (AR) 2MB Flash
                0x20000000, 264 * 1024,       // (AR) 264KB RAM
                0x2000                         // (AR) 8KB Stack
            );
        }
        else if (templateName == "x86_kernel" || templateName == u8"نواة_x86") {
            return generator_.generateX86BareMetal(0x100000);
        }
        else if (templateName == "aarch64_bare" || templateName == u8"نواة_aarch64") {
            return generator_.generateForTarget("aarch64-unknown-none-elf");
        }
        else if (templateName == "riscv64_bare" || templateName == u8"نواة_riscv64") {
            return generator_.generateForTarget("riscv64-unknown-none-elf");
        }
        
        return generator_.generate(config_);
    }
    
private:
    LinkerScriptConfig config_;
    LinkerScriptGenerator generator_;
    LinkerAttributeParser parser_;
};

} // namespace backend
} // namespace compiler
} // namespace sad
