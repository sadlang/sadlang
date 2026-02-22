/**
 * =============================================================================
 * ملف: compose_gen.cpp
 * الوصف: مُولِّد docker-compose
 * المهمة: T1144 - Phase 112
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

namespace sad {
namespace tools {

/**
 * خدمة في docker-compose
 */
struct خدمة {
    std::string الاسم;
    std::string صورة;
    std::string بناء;
    std::vector<std::string> المنافذ;
    std::vector<std::string> البيئة;
    std::vector<std::string> الأحجام;
    std::vector<std::string> يعتمد_على;
    std::string إعادة_التشغيل = "unless-stopped";
};

/**
 * 📦 مُولِّد docker-compose
 */
class مولد_compose {
public:
    /**
     * توليد docker-compose.yml
     */
    std::string ولد(const std::vector<خدمة>& خدمات) const {
        std::ostringstream yaml;
        
        yaml << "# ═══════════════════════════════════════════════════════════════\n";
        yaml << "# docker-compose.yml مُولَّد بواسطة: ص حاوية\n";
        yaml << "# ═══════════════════════════════════════════════════════════════\n\n";
        
        yaml << "version: '3.8'\n\n";
        
        yaml << "services:\n";
        
        for (const auto& خ : خدمات) {
            yaml << "  " << خ.الاسم << ":\n";
            
            if (!خ.صورة.empty()) {
                yaml << "    image: " << خ.صورة << "\n";
            }
            
            if (!خ.بناء.empty()) {
                yaml << "    build: " << خ.بناء << "\n";
            }
            
            yaml << "    restart: " << خ.إعادة_التشغيل << "\n";
            
            if (!خ.المنافذ.empty()) {
                yaml << "    ports:\n";
                for (const auto& م : خ.المنافذ) {
                    yaml << "      - \"" << م << "\"\n";
                }
            }
            
            if (!خ.البيئة.empty()) {
                yaml << "    environment:\n";
                for (const auto& ب : خ.البيئة) {
                    yaml << "      - " << ب << "\n";
                }
            }
            
            if (!خ.الأحجام.empty()) {
                yaml << "    volumes:\n";
                for (const auto& ح : خ.الأحجام) {
                    yaml << "      - " << ح << "\n";
                }
            }
            
            if (!خ.يعتمد_على.empty()) {
                yaml << "    depends_on:\n";
                for (const auto& ي : خ.يعتمد_على) {
                    yaml << "      - " << ي << "\n";
                }
            }
            
            yaml << "\n";
        }
        
        // الأحجام
        yaml << "volumes:\n";
        yaml << "  db_data:\n";
        yaml << "  redis_data:\n";
        
        return yaml.str();
    }
    
    /**
     * إنشاء قالب تطبيق ويب
     */
    std::vector<خدمة> قالب_ويب() const {
        std::vector<خدمة> خدمات;
        
        // التطبيق
        خدمة تطبيق;
        تطبيق.الاسم = "app";
        تطبيق.بناء = ".";
        تطبيق.المنافذ = {"8080:8080"};
        تطبيق.البيئة = {"SAD_ENV=production", "DATABASE_URL=postgres://db:5432/app"};
        تطبيق.يعتمد_على = {"db", "redis"};
        خدمات.push_back(تطبيق);
        
        // قاعدة البيانات
        خدمة قاعدة;
        قاعدة.الاسم = "db";
        قاعدة.صورة = "postgres:15-alpine";
        قاعدة.المنافذ = {"5432:5432"};
        قاعدة.البيئة = {"POSTGRES_DB=app", "POSTGRES_USER=sad", "POSTGRES_PASSWORD_FILE=/run/secrets/db_password"};
        قاعدة.الأحجام = {"db_data:/var/lib/postgresql/data"};
        خدمات.push_back(قاعدة);
        
        // Redis
        خدمة كاش;
        كاش.الاسم = "redis";
        كاش.صورة = "redis:7-alpine";
        كاش.المنافذ = {"6379:6379"};
        كاش.الأحجام = {"redis_data:/data"};
        خدمات.push_back(كاش);
        
        return خدمات;
    }
};

} // namespace tools
} // namespace sad

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                    📦 ص حاوية compose - مُولِّد docker-compose               ║
╚══════════════════════════════════════════════════════════════════════════════╝

)" << std::endl;
    
    sad::tools::مولد_compose مولد;
    auto خدمات = مولد.قالب_ويب();
    
    std::cout << "📄 docker-compose.yml المُولَّد:\n";
    std::cout << "────────────────────────────────────────────\n";
    std::cout << مولد.ولد(خدمات);
    
    return 0;
}
