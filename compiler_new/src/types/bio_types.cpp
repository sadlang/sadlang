/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                    أنواع الحوسبة الحيوية - لغة ص                          ║
 * ║                Bio-Computing Types - Sad Language                         ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * الملف: bio_types.cpp
 * File: bio_types.cpp
 * 
 * الوصف: تنفيذ أنواع الحوسبة الحيوية (DNA/RNA/Protein) في لغة ص
 * Description: Implementation of Bio-Computing types for genetic algorithms
 * 
 * المهمة: T471 - دعم الحوسبة الحيوية والخوارزميات الجينية
 * Task: T471 - Bio-Computing and Genetic Algorithms Support
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              المفاهيم الأساسية
 *                              Core Concepts
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * 🧬 DNA (الحمض النووي الريبوزي منقوص الأكسجين):
 *    - 4 قواعد نيتروجينية: A (أدينين), T (ثايمين), G (جوانين), C (سايتوسين)
 *    - ترميز المعلومات الوراثية
 *    - التطابق التكميلي: A-T و G-C
 * 
 * 🧬 RNA (الحمض النووي الريبوزي):
 *    - مشابه للـDNA لكن يستخدم U (يوراسيل) بدلاً من T
 *    - دور في نسخ وترجمة الجينات
 * 
 * 🧬 Protein (البروتينات):
 *    - سلاسل من الأحماض الأمينية (20 حمض أساسي)
 *    - تُترجم من mRNA عبر الكودونات (3 قواعد = 1 حمض أميني)
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              أمثلة الاستخدام
 *                              Usage Examples
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * // مثال 1: تسلسل DNA
 * // Example 1: DNA sequence
 * دع تسلسل = DNA::من_نص("ATGCGATCG")؛
 * 
 * // مثال 2: تحويل DNA إلى RNA
 * // Example 2: Transcribe DNA to RNA
 * دع رنا = تسلسل.انسخ_إلى_RNA()؛
 * 
 * // مثال 3: ترجمة RNA إلى بروتين
 * // Example 3: Translate RNA to protein
 * دع بروتين = رنا.ترجم_إلى_بروتين()؛
 * 
 * // مثال 4: التسلسل التكميلي
 * // Example 4: Complementary sequence
 * دع تكميلي = تسلسل.تكميلي()؛
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "../../include/types/type.h"
#include <string>
#include <vector>
#include <array>
#include <map>
#include <variant>
#include <optional>
#include <stdexcept>
#include <algorithm>

namespace Sad {
namespace TypeSystem {

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                          القواعد النيتروجينية
 *                          Nitrogenous Bases
 * ═══════════════════════════════════════════════════════════════════════════════
 */

/**
 * قواعد DNA الأربعة
 * Four DNA bases
 */
enum class DNABase : char {
    Adenine = 'A',    // أدينين
    Thymine = 'T',    // ثايمين
    Guanine = 'G',    // جوانين
    Cytosine = 'C',   // سايتوسين
    Unknown = 'N'     // غير معروف
};

/**
 * قواعد RNA الأربعة
 * Four RNA bases
 */
enum class RNABase : char {
    Adenine = 'A',    // أدينين
    Uracil = 'U',     // يوراسيل (بدلاً من الثايمين)
    Guanine = 'G',    // جوانين
    Cytosine = 'C',   // سايتوسين
    Unknown = 'N'     // غير معروف
};

/**
 * الأحماض الأمينية العشرون
 * Twenty amino acids
 */
enum class AminoAcid : char {
    // الأحماض الأمينية الأساسية / Essential amino acids
    Alanine = 'A',          // ألانين
    Arginine = 'R',         // أرجينين
    Asparagine = 'N',       // أسباراجين
    AsparticAcid = 'D',     // حمض الأسبارتيك
    Cysteine = 'C',         // سيستين
    GlutamicAcid = 'E',     // حمض الجلوتاميك
    Glutamine = 'Q',        // جلوتامين
    Glycine = 'G',          // جلايسين
    Histidine = 'H',        // هيستيدين
    Isoleucine = 'I',       // آيزوليوسين
    Leucine = 'L',          // ليوسين
    Lysine = 'K',           // لايسين
    Methionine = 'M',       // ميثيونين (بداية)
    Phenylalanine = 'F',    // فينيل ألانين
    Proline = 'P',          // برولين
    Serine = 'S',           // سيرين
    Threonine = 'T',        // ثريونين
    Tryptophan = 'W',       // تربتوفان
    Tyrosine = 'Y',         // تيروسين
    Valine = 'V',           // فالين
    
    // رموز خاصة / Special codes
    Stop = '*',             // كودون التوقف
    Unknown = 'X'           // غير معروف
};

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                          جدول الكودونات الجيني
 *                          Genetic Codon Table
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * جدول ترجمة الكودونات (3 قواعد RNA) إلى أحماض أمينية
 * Table for translating codons (3 RNA bases) to amino acids
 */
class GeneticCode {
private:
    std::map<std::string, AminoAcid> codon_table_;
    
    /**
     * تهيئة الجدول الجيني القياسي
     * Initialize standard genetic code
     */
    void initializeStandardCode() {
        // كودونات الألانين (Alanine) - GCU, GCC, GCA, GCG
        codon_table_["GCU"] = AminoAcid::Alanine;
        codon_table_["GCC"] = AminoAcid::Alanine;
        codon_table_["GCA"] = AminoAcid::Alanine;
        codon_table_["GCG"] = AminoAcid::Alanine;
        
        // كودونات الأرجينين (Arginine) - CGU, CGC, CGA, CGG, AGA, AGG
        codon_table_["CGU"] = AminoAcid::Arginine;
        codon_table_["CGC"] = AminoAcid::Arginine;
        codon_table_["CGA"] = AminoAcid::Arginine;
        codon_table_["CGG"] = AminoAcid::Arginine;
        codon_table_["AGA"] = AminoAcid::Arginine;
        codon_table_["AGG"] = AminoAcid::Arginine;
        
        // كودونات الميثيونين (Methionine - Start codon) - AUG
        codon_table_["AUG"] = AminoAcid::Methionine;
        
        // كودونات التوقف (Stop codons) - UAA, UAG, UGA
        codon_table_["UAA"] = AminoAcid::Stop;
        codon_table_["UAG"] = AminoAcid::Stop;
        codon_table_["UGA"] = AminoAcid::Stop;
        
        // ... يمكن إضافة باقي الكودونات (64 كودون)
    }
    
public:
    GeneticCode() {
        initializeStandardCode();
    }
    
    /**
     * ترجمة كودون إلى حمض أميني
     * Translate codon to amino acid
     */
    AminoAcid translate(const std::string& codon) const {
        auto it = codon_table_.find(codon);
        if (it != codon_table_.end()) {
            return it->second;
        }
        return AminoAcid::Unknown;
    }
};

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                             نوع DNA
 *                             DNA Type
 * ═══════════════════════════════════════════════════════════════════════════════
 */
class DNAType : public Type {
public:
    DNAType() : Type(TypeKind::Any) {}
    
    std::string getArabicName() const override { return "دنا"; }
    std::string getEnglishName() const override { return "DNA"; }
    std::string toString() const override { return "DNA"; }
    
    bool equals(const Type* other) const override {
        return other && other->getKind() == getKind();
    }
    
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<DNAType>();
    }
};

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                             نوع RNA
 *                             RNA Type
 * ═══════════════════════════════════════════════════════════════════════════════
 */
class RNAType : public Type {
public:
    RNAType() : Type(TypeKind::Any) {}
    
    std::string getArabicName() const override { return "رنا"; }
    std::string getEnglishName() const override { return "RNA"; }
    std::string toString() const override { return "RNA"; }
    
    bool equals(const Type* other) const override {
        return other && other->getKind() == getKind();
    }
    
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<RNAType>();
    }
};

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                           نوع البروتين
 *                           Protein Type
 * ═══════════════════════════════════════════════════════════════════════════════
 */
class ProteinType : public Type {
public:
    ProteinType() : Type(TypeKind::Any) {}
    
    std::string getArabicName() const override { return "بروتين"; }
    std::string getEnglishName() const override { return "Protein"; }
    std::string toString() const override { return "Protein"; }
    
    bool equals(const Type* other) const override {
        return other && other->getKind() == getKind();
    }
    
    std::shared_ptr<Type> clone() const override {
        return std::make_shared<ProteinType>();
    }
};

/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *                          عمليات DNA الأساسية
 *                          Basic DNA Operations
 * ═══════════════════════════════════════════════════════════════════════════════
 */
namespace DNAOperations {
    /**
     * القاعدة التكميلية
     * Complementary base
     * A <-> T, G <-> C
     */
    DNABase complement(DNABase base) {
        switch (base) {
            case DNABase::Adenine: return DNABase::Thymine;
            case DNABase::Thymine: return DNABase::Adenine;
            case DNABase::Guanine: return DNABase::Cytosine;
            case DNABase::Cytosine: return DNABase::Guanine;
            default: return DNABase::Unknown;
        }
    }
    
    /**
     * تحويل DNA إلى RNA (النسخ)
     * Transcribe DNA to RNA
     * T -> U, الباقي يبقى كما هو
     */
    RNABase transcribe(DNABase base) {
        switch (base) {
            case DNABase::Adenine: return RNABase::Adenine;
            case DNABase::Thymine: return RNABase::Uracil;  // T -> U
            case DNABase::Guanine: return RNABase::Guanine;
            case DNABase::Cytosine: return RNABase::Cytosine;
            default: return RNABase::Unknown;
        }
    }
}

} // namespace TypeSystem
} // namespace Sad
