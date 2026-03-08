/**
 * @file dwarf_generator.h
 * @brief مولد معلومات التصحيح DWARF
 * @brief (EN) DWARF debug info generator
 * 
 * @details يولد معلومات DWARF للتصحيح مع LLDB/GDB
 * 
 * @author سعيد
 * @date أكتوبر 2026
 */

#ifndef SAD_DWARF_GENERATOR_H
#define SAD_DWARF_GENERATOR_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;memory&gt;
#include &lt;cstdint&gt;

namespace Sad {
namespace Compiler {
namespace Debug {

//==============================================================================
// أنواع DWARF
//==============================================================================

enum class DWARFTag : uint16_t {
    DW_TAG_compile_unit = 0x11,
    DW_TAG_subprogram = 0x2e,
    DW_TAG_variable = 0x34,
    DW_TAG_formal_parameter = 0x05,
    DW_TAG_base_type = 0x24,
    DW_TAG_pointer_type = 0x0f,
    DW_TAG_array_type = 0x01,
    DW_TAG_structure_type = 0x13,
    DW_TAG_class_type = 0x02,
    DW_TAG_member = 0x0d,
    DW_TAG_lexical_block = 0x0b,
    DW_TAG_inlined_subroutine = 0x1d,
    DW_TAG_namespace = 0x39
};

enum class DWARFAttribute : uint16_t {
    DW_AT_name = 0x03,
    DW_AT_comp_dir = 0x1b,
    DW_AT_producer = 0x25,
    DW_AT_language = 0x13,
    DW_AT_low_pc = 0x11,
    DW_AT_high_pc = 0x12,
    DW_AT_stmt_list = 0x10,
    DW_AT_decl_file = 0x3a,
    DW_AT_decl_line = 0x3b,
    DW_AT_decl_column = 0x39,
    DW_AT_type = 0x49,
    DW_AT_location = 0x02,
    DW_AT_frame_base = 0x40,
    DW_AT_byte_size = 0x0b,
    DW_AT_encoding = 0x3e,
    DW_AT_external = 0x3f,
    DW_AT_inline = 0x20,
    DW_AT_abstract_origin = 0x31,
    DW_AT_call_file = 0x58,
    DW_AT_call_line = 0x59
};

enum class DWARFForm : uint8_t {
    DW_FORM_addr = 0x01,
    DW_FORM_data1 = 0x0b,
    DW_FORM_data2 = 0x05,
    DW_FORM_data4 = 0x06,
    DW_FORM_data8 = 0x07,
    DW_FORM_string = 0x08,
    DW_FORM_strp = 0x0e,
    DW_FORM_udata = 0x0f,
    DW_FORM_sdata = 0x0d,
    DW_FORM_ref4 = 0x13,
    DW_FORM_sec_offset = 0x17,
    DW_FORM_exprloc = 0x18,
    DW_FORM_flag_present = 0x19
};

enum class DWARFLanguage : uint16_t {
    DW_LANG_Sad = 0x8001,   // لغة ص (مخصصة)
    DW_LANG_C99 = 0x000c,
    DW_LANG_C_plus_plus_14 = 0x0021
};

enum class DWARFEncoding : uint8_t {
    DW_ATE_address = 0x01,
    DW_ATE_boolean = 0x02,
    DW_ATE_float = 0x04,
    DW_ATE_signed = 0x05,
    DW_ATE_unsigned = 0x07,
    DW_ATE_UTF = 0x10
};

//==============================================================================
// هياكل DWARF
//==============================================================================

/**
 * @brief موقع في الكود المصدري
 */
struct SourceLocation {
    std::string filename;
    int line;
    int column;
    uint64_t address;
    
    SourceLocation() : line(0), column(0), address(0) {}
    SourceLocation(const std::string&amp; f, int l, int c, uint64_t a = 0)
        : filename(f), line(l), column(c), address(a) {}
};

/**
 * @brief نوع في DWARF
 */
struct DIType {
    uint32_t id;
    std::string name;
    uint32_t byteSize;
    DWARFEncoding encoding;
    bool isPointer;
    bool isArray;
    uint32_t elementTypeId;
    
    DIType() : id(0), byteSize(0), encoding(DWARFEncoding::DW_ATE_signed),
               isPointer(false), isArray(false), elementTypeId(0) {}
};

/**
 * @brief متغير في DWARF
 */
struct DIVariable {
    std::string name;
    uint32_t typeId;
    SourceLocation location;
    bool isParameter;
    bool isLocal;
    int stackOffset;    // الإزاحة على المكدس
    int registerNum;    // رقم السجل (إذا كان في سجل)
    
    DIVariable() : typeId(0), isParameter(false), isLocal(true), 
                   stackOffset(0), registerNum(-1) {}
};

/**
 * @brief دالة في DWARF
 */
struct DISubprogram {
    std::string name;
    std::string linkageName;
    SourceLocation location;
    uint64_t lowPC;
    uint64_t highPC;
    uint32_t returnTypeId;
    std::vector&lt;DIVariable&gt; parameters;
    std::vector&lt;DIVariable&gt; locals;
    bool isInlined;
    bool isDefinition;
    
    DISubprogram() : lowPC(0), highPC(0), returnTypeId(0), 
                     isInlined(false), isDefinition(true) {}
};

/**
 * @brief صنف/بنية في DWARF
 */
struct DIClass {
    std::string name;
    uint32_t byteSize;
    SourceLocation location;
    std::vector&lt;std::pair&lt;std::string, uint32_t&gt;&gt; members; // (اسم، نوع)
    std::vector&lt;uint32_t&gt; memberOffsets;
    
    DIClass() : byteSize(0) {}
};

/**
 * @brief وحدة الترجمة
 */
struct DICompileUnit {
    std::string filename;
    std::string directory;
    std::string producer;
    DWARFLanguage language;
    std::vector&lt;DIType&gt; types;
    std::vector&lt;DISubprogram&gt; functions;
    std::vector&lt;DIClass&gt; classes;
    std::vector&lt;DIVariable&gt; globals;
    
    DICompileUnit() : language(DWARFLanguage::DW_LANG_Sad) {}
};

//==============================================================================
// مولد DWARF
//==============================================================================

/**
 * @brief مولد معلومات التصحيح DWARF
 */
class DWARFGenerator {
public:
    DWARFGenerator();
    ~DWARFGenerator();
    
    //==========================================================================
    // إنشاء وحدة الترجمة
    //==========================================================================
    
    /**
     * @brief بدء وحدة ترجمة جديدة
     */
    void beginCompileUnit(const std::string&amp; filename, const std::string&amp; directory);
    
    /**
     * @brief إنهاء وحدة الترجمة
     */
    void endCompileUnit();
    
    //==========================================================================
    // الأنواع
    //==========================================================================
    
    /**
     * @brief إضافة نوع أساسي
     */
    uint32_t addBasicType(const std::string&amp; name, uint32_t byteSize, DWARFEncoding encoding);
    
    /**
     * @brief إضافة نوع مؤشر
     */
    uint32_t addPointerType(uint32_t pointeeTypeId);
    
    /**
     * @brief إضافة نوع مصفوفة
     */
    uint32_t addArrayType(uint32_t elementTypeId, uint64_t count);
    
    /**
     * @brief إضافة صنف/بنية
     */
    uint32_t addClass(const DIClass&amp; cls);
    
    //==========================================================================
    // الدوال
    //==========================================================================
    
    /**
     * @brief بدء دالة
     */
    void beginFunction(const std::string&amp; name, const SourceLocation&amp; loc,
                       uint64_t lowPC, uint32_t returnTypeId);
    
    /**
     * @brief إضافة معامل
     */
    void addParameter(const std::string&amp; name, uint32_t typeId, int stackOffset);
    
    /**
     * @brief إضافة متغير محلي
     */
    void addLocalVariable(const std::string&amp; name, uint32_t typeId, 
                          const SourceLocation&amp; loc, int stackOffset);
    
    /**
     * @brief إنهاء الدالة
     */
    void endFunction(uint64_t highPC);
    
    //==========================================================================
    // جدول الأسطر
    //==========================================================================
    
    /**
     * @brief إضافة سطر إلى جدول الأسطر
     */
    void addLineEntry(const SourceLocation&amp; loc);
    
    /**
     * @brief إضافة نطاق inline
     */
    void addInlinedCall(const std::string&amp; calleeName, const SourceLocation&amp; callSite,
                        uint64_t lowPC, uint64_t highPC);
    
    //==========================================================================
    // الإنتاج
    //==========================================================================
    
    /**
     * @brief إنتاج قسم .debug_info
     */
    std::vector&lt;uint8_t&gt; generateDebugInfo();
    
    /**
     * @brief إنتاج قسم .debug_abbrev
     */
    std::vector&lt;uint8_t&gt; generateDebugAbbrev();
    
    /**
     * @brief إنتاج قسم .debug_line
     */
    std::vector&lt;uint8_t&gt; generateDebugLine();
    
    /**
     * @brief إنتاج قسم .debug_str
     */
    std::vector&lt;uint8_t&gt; generateDebugStr();
    
    /**
     * @brief إنتاج كل الأقسام
     */
    std::map&lt;std::string, std::vector&lt;uint8_t&gt;&gt; generateAllSections();

private:
    class Impl;
    std::unique_ptr&lt;Impl&gt; impl_;
};

//==============================================================================
// أنواع Sad المدمجة
//==============================================================================

namespace SadTypes {

/**
 * @brief الحصول على معرف نوع رقم
 */
uint32_t getNumberTypeId();

/**
 * @brief الحصول على معرف نوع عشري
 */
uint32_t getFloatTypeId();

/**
 * @brief الحصول على معرف نوع نص
 */
uint32_t getStringTypeId();

/**
 * @brief الحصول على معرف نوع منطقي
 */
uint32_t getBoolTypeId();

/**
 * @brief تسجيل الأنواع المدمجة في المولد
 */
void registerBuiltinTypes(DWARFGenerator&amp; gen);

} // namespace SadTypes

} // namespace Debug
} // namespace Compiler
} // namespace Sad

#endif // SAD_DWARF_GENERATOR_H
