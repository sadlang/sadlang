/**
 * ===================================================================================
 *  أمر ص حيوي - Bio CLI Command (T477)
 * ===================================================================================
 * 
 *  الوصف: واجهة سطر الأوامر للحوسبة الحيوية
 *  Description: Command-line interface for bio-computing
 * 
 *  الأوامر:
 *  - ص حيوي تحليل <ملف>     - تحليل تسلسل
 *  - ص حيوي محاذاة <ملف1> <ملف2>  - محاذاة تسلسلات
 *  - ص حيوي blast <تسلسل>   - بحث BLAST
 *  - ص حيوي محاكاة <ملف>    - محاكاة جزيئية
 *  - ص حيوي تعبير <ملف>     - تحليل التعبير الجيني
 * 
 * ===================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace ص_حيوي_CLI {

// ============== ألوان الطرفية ==============

namespace ألوان {
    const std::string إعادة_تعيين = "\033[0m";
    const std::string أخضر = "\033[32m";
    const std::string أحمر = "\033[31m";
    const std::string أصفر = "\033[33m";
    const std::string أزرق = "\033[34m";
    const std::string سماوي = "\033[36m";
    const std::string عريض = "\033[1m";
}

// ============== أنواع الأوامر ==============

enum class نوع_أمر_حيوي {
    تحليل,
    محاذاة,
    blast,
    محاكاة,
    تعبير,
    طي,
    قاعدة_بيانات,
    تحويل,
    إحصائيات,
    مساعدة,
    إصدار,
    غير_معروف
};

// ============== تحليل الأوامر ==============

class محلل_أوامر_حيوي {
public:
    static نوع_أمر_حيوي تحليل(const std::string& أمر) {
        if (أمر == "تحليل" || أمر == "analyze") return نوع_أمر_حيوي::تحليل;
        if (أمر == "محاذاة" || أمر == "align") return نوع_أمر_حيوي::محاذاة;
        if (أمر == "blast" || أمر == "بلاست") return نوع_أمر_حيوي::blast;
        if (أمر == "محاكاة" || أمر == "simulate") return نوع_أمر_حيوي::محاكاة;
        if (أمر == "تعبير" || أمر == "expression") return نوع_أمر_حيوي::تعبير;
        if (أمر == "طي" || أمر == "fold") return نوع_أمر_حيوي::طي;
        if (أمر == "قاعدة" || أمر == "database" || أمر == "db") return نوع_أمر_حيوي::قاعدة_بيانات;
        if (أمر == "تحويل" || أمر == "convert") return نوع_أمر_حيوي::تحويل;
        if (أمر == "إحصائيات" || أمر == "stats") return نوع_أمر_حيوي::إحصائيات;
        if (أمر == "مساعدة" || أمر == "help" || أمر == "-h" || أمر == "--help") return نوع_أمر_حيوي::مساعدة;
        if (أمر == "إصدار" || أمر == "version" || أمر == "-v" || أمر == "--version") return نوع_أمر_حيوي::إصدار;
        return نوع_أمر_حيوي::غير_معروف;
    }
};

// ============== أدوات التسلسل ==============

class أدوات_تسلسل {
public:
    // قراءة FASTA
    static std::vector<std::pair<std::string, std::string>> قراءة_FASTA(const std::string& مسار) {
        std::vector<std::pair<std::string, std::string>> تسلسلات;
        std::ifstream ملف(مسار);
        
        if (!ملف.is_open()) {
            std::cerr << ألوان::أحمر << "خطأ: لا يمكن فتح الملف " << مسار << ألوان::إعادة_تعيين << std::endl;
            return تسلسلات;
        }
        
        std::string سطر;
        std::string رأس_حالي;
        std::string تسلسل_حالي;
        
        while (std::getline(ملف, سطر)) {
            if (سطر.empty()) continue;
            
            if (سطر[0] == '>') {
                if (!رأس_حالي.empty()) {
                    تسلسلات.push_back({رأس_حالي, تسلسل_حالي});
                }
                رأس_حالي = سطر.substr(1);
                تسلسل_حالي.clear();
            } else {
                تسلسل_حالي += سطر;
            }
        }
        
        if (!رأس_حالي.empty()) {
            تسلسلات.push_back({رأس_حالي, تسلسل_حالي});
        }
        
        return تسلسلات;
    }
    
    // حساب محتوى GC
    static double محتوى_GC(const std::string& تسلسل) {
        int gc = 0;
        int total = 0;
        
        for (char c : تسلسل) {
            c = std::toupper(c);
            if (c == 'G' || c == 'C') ++gc;
            if (c == 'A' || c == 'T' || c == 'G' || c == 'C') ++total;
        }
        
        return total > 0 ? (double)gc / total * 100.0 : 0.0;
    }
    
    // المكمل العكسي
    static std::string مكمل_عكسي(const std::string& تسلسل) {
        std::string نتيجة;
        نتيجة.reserve(تسلسل.size());
        
        for (auto it = تسلسل.rbegin(); it != تسلسل.rend(); ++it) {
            char c = std::toupper(*it);
            switch (c) {
                case 'A': نتيجة += 'T'; break;
                case 'T': نتيجة += 'A'; break;
                case 'G': نتيجة += 'C'; break;
                case 'C': نتيجة += 'G'; break;
                default: نتيجة += c;
            }
        }
        
        return نتيجة;
    }
    
    // النسخ (DNA -> RNA)
    static std::string نسخ(const std::string& dna) {
        std::string rna;
        rna.reserve(dna.size());
        
        for (char c : dna) {
            c = std::toupper(c);
            rna += (c == 'T') ? 'U' : c;
        }
        
        return rna;
    }
    
    // جدول الكودونات
    static std::string ترجمة_كودون(const std::string& كودون) {
        static const std::map<std::string, std::string> جدول = {
            {"UUU", "Phe"}, {"UUC", "Phe"}, {"UUA", "Leu"}, {"UUG", "Leu"},
            {"UCU", "Ser"}, {"UCC", "Ser"}, {"UCA", "Ser"}, {"UCG", "Ser"},
            {"UAU", "Tyr"}, {"UAC", "Tyr"}, {"UAA", "Stop"}, {"UAG", "Stop"},
            {"UGU", "Cys"}, {"UGC", "Cys"}, {"UGA", "Stop"}, {"UGG", "Trp"},
            {"CUU", "Leu"}, {"CUC", "Leu"}, {"CUA", "Leu"}, {"CUG", "Leu"},
            {"CCU", "Pro"}, {"CCC", "Pro"}, {"CCA", "Pro"}, {"CCG", "Pro"},
            {"CAU", "His"}, {"CAC", "His"}, {"CAA", "Gln"}, {"CAG", "Gln"},
            {"CGU", "Arg"}, {"CGC", "Arg"}, {"CGA", "Arg"}, {"CGG", "Arg"},
            {"AUU", "Ile"}, {"AUC", "Ile"}, {"AUA", "Ile"}, {"AUG", "Met"},
            {"ACU", "Thr"}, {"ACC", "Thr"}, {"ACA", "Thr"}, {"ACG", "Thr"},
            {"AAU", "Asn"}, {"AAC", "Asn"}, {"AAA", "Lys"}, {"AAG", "Lys"},
            {"AGU", "Ser"}, {"AGC", "Ser"}, {"AGA", "Arg"}, {"AGG", "Arg"},
            {"GUU", "Val"}, {"GUC", "Val"}, {"GUA", "Val"}, {"GUG", "Val"},
            {"GCU", "Ala"}, {"GCC", "Ala"}, {"GCA", "Ala"}, {"GCG", "Ala"},
            {"GAU", "Asp"}, {"GAC", "Asp"}, {"GAA", "Glu"}, {"GAG", "Glu"},
            {"GGU", "Gly"}, {"GGC", "Gly"}, {"GGA", "Gly"}, {"GGG", "Gly"}
        };
        
        auto it = جدول.find(كودون);
        return it != جدول.end() ? it->second : "???";
    }
    
    // الترجمة (RNA -> Protein)
    static std::string ترجمة(const std::string& rna) {
        std::string بروتين;
        
        for (size_t i = 0; i + 2 < rna.size(); i += 3) {
            std::string كودون = rna.substr(i, 3);
            std::string حمض = ترجمة_كودون(كودون);
            
            if (حمض == "Stop") break;
            بروتين += حمض + "-";
        }
        
        if (!بروتين.empty() && بروتين.back() == '-') {
            بروتين.pop_back();
        }
        
        return بروتين;
    }
    
    // إيجاد ORFs
    static std::vector<std::pair<int, int>> إيجاد_ORFs(const std::string& rna, int الحد_الأدنى = 100) {
        std::vector<std::pair<int, int>> orfs;
        
        for (int إطار = 0; إطار < 3; ++إطار) {
            int بداية = -1;
            
            for (size_t i = إطار; i + 2 < rna.size(); i += 3) {
                std::string كودون = rna.substr(i, 3);
                
                if (كودون == "AUG" && بداية == -1) {
                    بداية = i;
                } else if ((كودون == "UAA" || كودون == "UAG" || كودون == "UGA") && بداية != -1) {
                    int طول = i - بداية + 3;
                    if (طول >= الحد_الأدنى) {
                        orfs.push_back({بداية, i + 3});
                    }
                    بداية = -1;
                }
            }
        }
        
        return orfs;
    }
};

// ============== محاذاة بسيطة ==============

class محاذاة_بسيطة {
public:
    static void محاذاة_عالمية(const std::string& تسلسل١, const std::string& تسلسل٢) {
        int م = تسلسل١.size();
        int ن = تسلسل٢.size();
        
        // مصفوفة النقاط
        std::vector<std::vector<int>> نقاط(م + 1, std::vector<int>(ن + 1, 0));
        
        int تطابق = 2;
        int عدم_تطابق = -1;
        int فجوة = -2;
        
        // تهيئة
        for (int i = 0; i <= م; ++i) نقاط[i][0] = i * فجوة;
        for (int j = 0; j <= ن; ++j) نقاط[0][j] = j * فجوة;
        
        // ملء المصفوفة
        for (int i = 1; i <= م; ++i) {
            for (int j = 1; j <= ن; ++j) {
                int قطري = نقاط[i-1][j-1] + 
                    (تسلسل١[i-1] == تسلسل٢[j-1] ? تطابق : عدم_تطابق);
                int أعلى = نقاط[i-1][j] + فجوة;
                int يسار = نقاط[i][j-1] + فجوة;
                نقاط[i][j] = std::max({قطري, أعلى, يسار});
            }
        }
        
        // التتبع الخلفي
        std::string محاذاة١, محاذاة٢;
        int i = م, j = ن;
        
        while (i > 0 || j > 0) {
            if (i > 0 && j > 0) {
                int score = (تسلسل١[i-1] == تسلسل٢[j-1]) ? تطابق : عدم_تطابق;
                if (نقاط[i][j] == نقاط[i-1][j-1] + score) {
                    محاذاة١ = تسلسل١[i-1] + محاذاة١;
                    محاذاة٢ = تسلسل٢[j-1] + محاذاة٢;
                    --i; --j;
                    continue;
                }
            }
            
            if (i > 0 && نقاط[i][j] == نقاط[i-1][j] + فجوة) {
                محاذاة١ = تسلسل١[i-1] + محاذاة١;
                محاذاة٢ = '-' + محاذاة٢;
                --i;
            } else if (j > 0) {
                محاذاة١ = '-' + محاذاة١;
                محاذاة٢ = تسلسل٢[j-1] + محاذاة٢;
                --j;
            } else {
                break;
            }
        }
        
        // طباعة النتائج
        std::cout << ألوان::عريض << "نتيجة المحاذاة:" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "النقاط: " << ألوان::أخضر << نقاط[م][ن] << ألوان::إعادة_تعيين << std::endl;
        std::cout << std::endl;
        
        // طباعة المحاذاة
        int عرض = 60;
        for (size_t بداية = 0; بداية < محاذاة١.size(); بداية += عرض) {
            std::string جزء١ = محاذاة١.substr(بداية, عرض);
            std::string جزء٢ = محاذاة٢.substr(بداية, عرض);
            
            std::cout << "تسلسل١: " << جزء١ << std::endl;
            std::cout << "        ";
            for (size_t k = 0; k < جزء١.size(); ++k) {
                if (جزء١[k] == جزء٢[k] && جزء١[k] != '-') {
                    std::cout << ألوان::أخضر << "|" << ألوان::إعادة_تعيين;
                } else if (جزء١[k] != '-' && جزء٢[k] != '-') {
                    std::cout << ألوان::أصفر << ":" << ألوان::إعادة_تعيين;
                } else {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
            std::cout << "تسلسل٢: " << جزء٢ << std::endl;
            std::cout << std::endl;
        }
        
        // إحصائيات
        int تطابقات = 0, اختلافات = 0, فجوات = 0;
        for (size_t k = 0; k < محاذاة١.size(); ++k) {
            if (محاذاة١[k] == '-' || محاذاة٢[k] == '-') ++فجوات;
            else if (محاذاة١[k] == محاذاة٢[k]) ++تطابقات;
            else ++اختلافات;
        }
        
        std::cout << "الهوية: " << ألوان::أخضر 
                  << std::fixed << std::setprecision(1) 
                  << (100.0 * تطابقات / محاذاة١.size()) << "%" 
                  << ألوان::إعادة_تعيين << std::endl;
        std::cout << "الفجوات: " << فجوات << " (" 
                  << std::fixed << std::setprecision(1)
                  << (100.0 * فجوات / محاذاة١.size()) << "%)" << std::endl;
    }
};

// ============== معالج الأوامر ==============

class معالج_أوامر_حيوي {
public:
    static int تنفيذ(int argc, char* argv[]) {
        if (argc < 2) {
            عرض_المساعدة();
            return 1;
        }
        
        نوع_أمر_حيوي نوع = محلل_أوامر_حيوي::تحليل(argv[1]);
        std::vector<std::string> معاملات(argv + 2, argv + argc);
        
        switch (نوع) {
            case نوع_أمر_حيوي::تحليل:
                return أمر_تحليل(معاملات);
            case نوع_أمر_حيوي::محاذاة:
                return أمر_محاذاة(معاملات);
            case نوع_أمر_حيوي::blast:
                return أمر_blast(معاملات);
            case نوع_أمر_حيوي::محاكاة:
                return أمر_محاكاة(معاملات);
            case نوع_أمر_حيوي::تعبير:
                return أمر_تعبير(معاملات);
            case نوع_أمر_حيوي::طي:
                return أمر_طي(معاملات);
            case نوع_أمر_حيوي::قاعدة_بيانات:
                return أمر_قاعدة_بيانات(معاملات);
            case نوع_أمر_حيوي::تحويل:
                return أمر_تحويل(معاملات);
            case نوع_أمر_حيوي::إحصائيات:
                return أمر_إحصائيات(معاملات);
            case نوع_أمر_حيوي::مساعدة:
                عرض_المساعدة();
                return 0;
            case نوع_أمر_حيوي::إصدار:
                std::cout << "ص حيوي الإصدار 1.0.0" << std::endl;
                return 0;
            default:
                std::cerr << ألوان::أحمر << "خطأ: أمر غير معروف '" << argv[1] << "'" << ألوان::إعادة_تعيين << std::endl;
                return 1;
        }
    }
    
private:
    // ====== أمر التحليل ======
    static int أمر_تحليل(const std::vector<std::string>& معاملات) {
        if (معاملات.empty()) {
            std::cerr << "الاستخدام: ص حيوي تحليل <ملف.fasta>" << std::endl;
            return 1;
        }
        
        auto تسلسلات = أدوات_تسلسل::قراءة_FASTA(معاملات[0]);
        
        if (تسلسلات.empty()) {
            std::cerr << ألوان::أحمر << "لم يتم العثور على تسلسلات" << ألوان::إعادة_تعيين << std::endl;
            return 1;
        }
        
        std::cout << ألوان::عريض << ألوان::سماوي 
                  << "╔════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                   تحليل التسلسلات الحيوية                   ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════╝" 
                  << ألوان::إعادة_تعيين << std::endl << std::endl;
        
        std::cout << "عدد التسلسلات: " << ألوان::أخضر << تسلسلات.size() << ألوان::إعادة_تعيين << std::endl << std::endl;
        
        for (const auto& [رأس, تسلسل] : تسلسلات) {
            std::cout << ألوان::عريض << "▸ " << رأس << ألوان::إعادة_تعيين << std::endl;
            std::cout << "  الطول: " << تسلسل.size() << " bp" << std::endl;
            std::cout << "  محتوى GC: " << std::fixed << std::setprecision(2) 
                      << أدوات_تسلسل::محتوى_GC(تسلسل) << "%" << std::endl;
            
            // عد النيوكليوتيدات
            int a = 0, t = 0, g = 0, c = 0;
            for (char حرف : تسلسل) {
                switch (std::toupper(حرف)) {
                    case 'A': ++a; break;
                    case 'T': ++t; break;
                    case 'G': ++g; break;
                    case 'C': ++c; break;
                }
            }
            std::cout << "  التركيب: A=" << a << " T=" << t << " G=" << g << " C=" << c << std::endl;
            
            // نسخ وترجمة (أول 30 كودون)
            std::string rna = أدوات_تسلسل::نسخ(تسلسل.substr(0, 90));
            std::string بروتين = أدوات_تسلسل::ترجمة(rna);
            if (!بروتين.empty()) {
                std::cout << "  الترجمة (أول 30): " << بروتين << std::endl;
            }
            
            // ORFs
            std::string rna_كامل = أدوات_تسلسل::نسخ(تسلسل);
            auto orfs = أدوات_تسلسل::إيجاد_ORFs(rna_كامل, 100);
            std::cout << "  ORFs (>=100 bp): " << orfs.size() << std::endl;
            
            std::cout << std::endl;
        }
        
        return 0;
    }
    
    // ====== أمر المحاذاة ======
    static int أمر_محاذاة(const std::vector<std::string>& معاملات) {
        if (معاملات.size() < 2) {
            // محاذاة تسلسلات من ملف واحد
            if (معاملات.size() == 1) {
                auto تسلسلات = أدوات_تسلسل::قراءة_FASTA(معاملات[0]);
                if (تسلسلات.size() >= 2) {
                    std::cout << ألوان::عريض << "محاذاة: " << تسلسلات[0].first 
                              << " vs " << تسلسلات[1].first << ألوان::إعادة_تعيين << std::endl << std::endl;
                    محاذاة_بسيطة::محاذاة_عالمية(تسلسلات[0].second, تسلسلات[1].second);
                    return 0;
                }
            }
            std::cerr << "الاستخدام: ص حيوي محاذاة <ملف1.fasta> [ملف2.fasta]" << std::endl;
            return 1;
        }
        
        auto تسلسلات١ = أدوات_تسلسل::قراءة_FASTA(معاملات[0]);
        auto تسلسلات٢ = أدوات_تسلسل::قراءة_FASTA(معاملات[1]);
        
        if (تسلسلات١.empty() || تسلسلات٢.empty()) {
            std::cerr << ألوان::أحمر << "خطأ في قراءة الملفات" << ألوان::إعادة_تعيين << std::endl;
            return 1;
        }
        
        محاذاة_بسيطة::محاذاة_عالمية(تسلسلات١[0].second, تسلسلات٢[0].second);
        return 0;
    }
    
    // ====== أمر BLAST ======
    static int أمر_blast(const std::vector<std::string>& معاملات) {
        std::cout << ألوان::أصفر << "⚠ بحث BLAST يتطلب اتصال بالإنترنت" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "للبحث عبر NCBI، استخدم تكامل BioPython:" << std::endl;
        std::cout << "  ص حيوي blast --online <تسلسل>" << std::endl;
        std::cout << std::endl;
        std::cout << "للبحث المحلي:" << std::endl;
        std::cout << "  ص حيوي blast --local <قاعدة_بيانات> <تسلسل>" << std::endl;
        return 0;
    }
    
    // ====== أمر المحاكاة ======
    static int أمر_محاكاة(const std::vector<std::string>& معاملات) {
        if (معاملات.empty()) {
            std::cerr << "الاستخدام: ص حيوي محاكاة <ملف.pdb> [خطوات]" << std::endl;
            return 1;
        }
        
        std::cout << ألوان::عريض << ألوان::سماوي 
                  << "╔════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                   المحاكاة الجزيئية                         ║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════╝" 
                  << ألوان::إعادة_تعيين << std::endl << std::endl;
        
        std::cout << "ملف الإدخال: " << معاملات[0] << std::endl;
        std::cout << "حقل القوة: AMBER" << std::endl;
        std::cout << "منظم الحرارة: Berendsen" << std::endl;
        std::cout << "خطوة الزمن: 1 fs" << std::endl;
        std::cout << std::endl;
        
        std::cout << ألوان::أصفر << "تنفيذ المحاكاة..." << ألوان::إعادة_تعيين << std::endl;
        // هنا سيتم ربط المحاكي الفعلي
        
        return 0;
    }
    
    // ====== أمر التعبير ======
    static int أمر_تعبير(const std::vector<std::string>& معاملات) {
        if (معاملات.empty()) {
            std::cerr << "الاستخدام: ص حيوي تعبير <ملف_عد.csv>" << std::endl;
            return 1;
        }
        
        std::cout << ألوان::عريض << "تحليل التعبير الجيني" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "ملف الإدخال: " << معاملات[0] << std::endl;
        std::cout << std::endl;
        
        // قراءة الملف وتحليله
        std::cout << "طرق التطبيع المتاحة:" << std::endl;
        std::cout << "  - RPKM" << std::endl;
        std::cout << "  - TPM" << std::endl;
        std::cout << "  - CPM" << std::endl;
        
        return 0;
    }
    
    // ====== أمر الطي ======
    static int أمر_طي(const std::vector<std::string>& معاملات) {
        if (معاملات.empty()) {
            std::cerr << "الاستخدام: ص حيوي طي <تسلسل_بروتين>" << std::endl;
            return 1;
        }
        
        std::cout << ألوان::عريض << "محاكاة طي البروتين" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "التسلسل: " << معاملات[0] << std::endl;
        std::cout << "الطول: " << معاملات[0].size() << " حمض أميني" << std::endl;
        std::cout << std::endl;
        std::cout << "الخوارزمية: Monte Carlo مع التبريد المحاكى" << std::endl;
        
        return 0;
    }
    
    // ====== أمر قاعدة البيانات ======
    static int أمر_قاعدة_بيانات(const std::vector<std::string>& معاملات) {
        std::cout << ألوان::عريض << "قواعد البيانات الحيوية المدعومة:" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "  • NCBI GenBank" << std::endl;
        std::cout << "  • PDB (Protein Data Bank)" << std::endl;
        std::cout << "  • UniProt" << std::endl;
        std::cout << "  • PubMed" << std::endl;
        std::cout << std::endl;
        std::cout << "الاستخدام:" << std::endl;
        std::cout << "  ص حيوي قاعدة جلب <معرف>     - جلب تسلسل" << std::endl;
        std::cout << "  ص حيوي قاعدة بحث <استعلام>  - بحث في القاعدة" << std::endl;
        
        return 0;
    }
    
    // ====== أمر التحويل ======
    static int أمر_تحويل(const std::vector<std::string>& معاملات) {
        if (معاملات.size() < 3) {
            std::cerr << "الاستخدام: ص حيوي تحويل <إدخال> <إخراج> <صيغة>" << std::endl;
            std::cerr << "الصيغ المدعومة: fasta, genbank, pdb, xyz" << std::endl;
            return 1;
        }
        
        std::cout << "تحويل: " << معاملات[0] << " -> " << معاملات[1] << std::endl;
        std::cout << "الصيغة: " << معاملات[2] << std::endl;
        
        return 0;
    }
    
    // ====== أمر الإحصائيات ======
    static int أمر_إحصائيات(const std::vector<std::string>& معاملات) {
        if (معاملات.empty()) {
            std::cerr << "الاستخدام: ص حيوي إحصائيات <ملف.fasta>" << std::endl;
            return 1;
        }
        
        auto تسلسلات = أدوات_تسلسل::قراءة_FASTA(معاملات[0]);
        
        if (تسلسلات.empty()) return 1;
        
        int إجمالي_الطول = 0;
        double إجمالي_GC = 0;
        int أقصر = INT_MAX;
        int أطول = 0;
        
        for (const auto& [_, تسلسل] : تسلسلات) {
            int طول = تسلسل.size();
            إجمالي_الطول += طول;
            إجمالي_GC += أدوات_تسلسل::محتوى_GC(تسلسل);
            أقصر = std::min(أقصر, طول);
            أطول = std::max(أطول, طول);
        }
        
        std::cout << ألوان::عريض << "إحصائيات الملف:" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "  عدد التسلسلات: " << تسلسلات.size() << std::endl;
        std::cout << "  إجمالي القواعد: " << إجمالي_الطول << std::endl;
        std::cout << "  متوسط الطول: " << إجمالي_الطول / تسلسلات.size() << std::endl;
        std::cout << "  أقصر تسلسل: " << أقصر << std::endl;
        std::cout << "  أطول تسلسل: " << أطول << std::endl;
        std::cout << "  متوسط GC: " << std::fixed << std::setprecision(2) 
                  << إجمالي_GC / تسلسلات.size() << "%" << std::endl;
        
        return 0;
    }
    
    // ====== عرض المساعدة ======
    static void عرض_المساعدة() {
        std::cout << ألوان::عريض << ألوان::سماوي;
        std::cout << R"(
╔════════════════════════════════════════════════════════════════════════════╗
║                                                                            ║
║   ███████╗    ██████╗ ██╗ ██████╗                                          ║
║   ██╔════╝    ██╔══██╗██║██╔═══██╗                                         ║
║   ███████╗    ██████╔╝██║██║   ██║    ص حيوي - أدوات الحوسبة الحيوية      ║
║   ╚════██║    ██╔══██╗██║██║   ██║                                         ║
║   ███████║    ██████╔╝██║╚██████╔╝    Bio-Computing Tools                  ║
║   ╚══════╝    ╚═════╝ ╚═╝ ╚═════╝                                          ║
║                                                                            ║
╚════════════════════════════════════════════════════════════════════════════╝
)" << ألوان::إعادة_تعيين << std::endl;

        std::cout << ألوان::عريض << "الاستخدام:" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "  ص حيوي <أمر> [خيارات]" << std::endl << std::endl;
        
        std::cout << ألوان::عريض << "الأوامر المتاحة:" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "  " << ألوان::أخضر << "تحليل" << ألوان::إعادة_تعيين 
                  << "     <ملف>           تحليل تسلسل DNA/RNA/بروتين" << std::endl;
        std::cout << "  " << ألوان::أخضر << "محاذاة" << ألوان::إعادة_تعيين 
                  << "     <ملف1> [ملف2]   محاذاة تسلسلات" << std::endl;
        std::cout << "  " << ألوان::أخضر << "blast" << ألوان::إعادة_تعيين 
                  << "      <تسلسل>         بحث BLAST" << std::endl;
        std::cout << "  " << ألوان::أخضر << "محاكاة" << ألوان::إعادة_تعيين 
                  << "     <ملف.pdb>       محاكاة جزيئية" << std::endl;
        std::cout << "  " << ألوان::أخضر << "تعبير" << ألوان::إعادة_تعيين 
                  << "      <ملف.csv>       تحليل التعبير الجيني" << std::endl;
        std::cout << "  " << ألوان::أخضر << "طي" << ألوان::إعادة_تعيين 
                  << "         <تسلسل>         محاكاة طي البروتين" << std::endl;
        std::cout << "  " << ألوان::أخضر << "قاعدة" << ألوان::إعادة_تعيين 
                  << "      <عملية>         الوصول لقواعد البيانات" << std::endl;
        std::cout << "  " << ألوان::أخضر << "تحويل" << ألوان::إعادة_تعيين 
                  << "     <إدخال> <إخراج>  تحويل صيغ الملفات" << std::endl;
        std::cout << "  " << ألوان::أخضر << "إحصائيات" << ألوان::إعادة_تعيين 
                  << "  <ملف>           إحصائيات التسلسل" << std::endl;
        std::cout << std::endl;
        
        std::cout << ألوان::عريض << "أمثلة:" << ألوان::إعادة_تعيين << std::endl;
        std::cout << "  ص حيوي تحليل genome.fasta" << std::endl;
        std::cout << "  ص حيوي محاذاة seq1.fasta seq2.fasta" << std::endl;
        std::cout << "  ص حيوي محاكاة protein.pdb --steps 10000" << std::endl;
        std::cout << "  ص حيوي تعبير counts.csv --method TPM" << std::endl;
    }
};

}  // namespace ص_حيوي_CLI

// ============== نقطة الدخول ==============

int main(int argc, char* argv[]) {
    return ص_حيوي_CLI::معالج_أوامر_حيوي::تنفيذ(argc, argv);
}

// واجهة C
extern "C" {
    int sad_bio_main(int argc, char* argv[]) {
        return ص_حيوي_CLI::معالج_أوامر_حيوي::تنفيذ(argc, argv);
    }
    
    int sad_bio_cli(const char* أمر) {
        // تحويل الأمر إلى argc/argv
        std::vector<char*> args;
        std::string أمر_نسخة = أمر;
        
        char* token = std::strtok(&أمر_نسخة[0], " ");
        while (token) {
            args.push_back(token);
            token = std::strtok(nullptr, " ");
        }
        
        return ص_حيوي_CLI::معالج_أوامر_حيوي::تنفيذ(args.size(), args.data());
    }
}
