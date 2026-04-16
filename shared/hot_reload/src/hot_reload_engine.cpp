/**
 * @file hot_reload_engine.cpp
 * @brief (AR) تنفيذ محرك إعادة التحميل الحي
 * @brief (EN) Hot Reload Engine implementation
 * 
 * @author فريق لغة ص
 * @date July 2025
 */

#include "hot_reload_engine.h"
#include "interpreter_core.h"
#include "function_manager.h"
#include "class_manager.h"
#include "error_manager.h"
#include "declarations.h"
#include "module_nodes.h"
#include "utf8_utils.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>

namespace Sad {
namespace HotReload {

// ═══════════════════════════════════════════════════════════════════════════════
// البناء والهدم / Constructor and Destructor
// ═══════════════════════════════════════════════════════════════════════════════

HotReloadEngine::HotReloadEngine(const HotReloadOptions& options)
    : options_(options) {
    fileWatcher_ = std::make_unique<FileWatcher>(options.debounceMs);
}

HotReloadEngine::~HotReloadEngine() {
    stop();
}

// ═══════════════════════════════════════════════════════════════════════════════
// واجهة عامة / Public Interface
// ═══════════════════════════════════════════════════════════════════════════════

void HotReloadEngine::attach(Sad::Interpreter::Interpreter* interpreter,
                              const std::string& mainFilePath) {
    interpreter_ = interpreter;
    mainFilePath_ = std::filesystem::absolute(mainFilePath).string();
    
    // (AR) قراءة الكود المصدري الأولي
    mainSource_ = readFile(mainFilePath_);
    
    // (AR) مراقبة الملف الرئيسي
    fileWatcher_->watchFile(mainFilePath_);
    
    // (AR) مراقبة مجلد الملف الرئيسي
    auto mainDir = std::filesystem::path(mainFilePath_).parent_path().string();
    if (!mainDir.empty()) {
        fileWatcher_->watchDirectory(mainDir, false);
    }
    
    // (AR) تعيين callback مراقب الملفات
    fileWatcher_->setCallback([this](const FileChange& change) {
        if (change.type == FileChangeType::Modified || change.type == FileChangeType::Created) {
            std::lock_guard<std::mutex> lock(reloadMutex_);
            pendingFilePath_ = change.filePath;
            pendingReload_.store(true);
        }
    });
    
    if (options_.showNotifications) {
        std::cerr << "\033[36m🔄 نظام إعادة التحميل الحي جاهز — مراقبة: "
                  << mainFilePath_ << "\033[0m" << std::endl;
    }
}

void HotReloadEngine::addWatchFile(const std::string& filePath) {
    fileWatcher_->watchFile(filePath);
}

void HotReloadEngine::addWatchDirectory(const std::string& dirPath) {
    fileWatcher_->watchDirectory(dirPath);
}

void HotReloadEngine::setRebuildUICallback(RebuildUICallback callback) {
    rebuildUICallback_ = std::move(callback);
}

void HotReloadEngine::setReloadCallback(ReloadCallback callback) {
    reloadCallback_ = std::move(callback);
}

void HotReloadEngine::start() {
    if (active_.load()) return;
    active_.store(true);
    fileWatcher_->start();
    
    if (options_.showNotifications) {
        std::cerr << "\033[32m✅ بدء المراقبة...\033[0m" << std::endl;
    }
}

void HotReloadEngine::stop() {
    active_.store(false);
    fileWatcher_->stop();
}

ReloadResult HotReloadEngine::poll() {
    // (AR) استخدام poll() من FileWatcher إذا لم يكن في وضع الخيط
    if (!fileWatcher_->isRunning()) {
        fileWatcher_->poll();
    }
    
    // (AR) التحقق من وجود تحميل معلّق
    if (pendingReload_.load()) {
        pendingReload_.store(false);
        
        std::string filePath;
        {
            std::lock_guard<std::mutex> lock(reloadMutex_);
            filePath = pendingFilePath_;
            pendingFilePath_.clear();
        }
        
        if (!filePath.empty()) {
            auto result = performReload(filePath);
            lastResult_ = result;
            
            if (reloadCallback_) {
                reloadCallback_(result);
            }
            
            return result;
        }
    }
    
    return {ReloadStatus::NoChange, "", 0, 0, 0.0, {}};
}

ReloadResult HotReloadEngine::forceReload() {
    auto result = performReload(mainFilePath_);
    lastResult_ = result;
    
    if (reloadCallback_) {
        reloadCallback_(result);
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// التنفيذ الخاص / Private Implementation
// ═══════════════════════════════════════════════════════════════════════════════

ReloadResult HotReloadEngine::performReload(const std::string& filePath) {
    auto startTime = std::chrono::steady_clock::now();
    
    ReloadResult result;
    result.status = ReloadStatus::NoChange;
    
    // (AR) قراءة الملف الجديد
    std::string newSource = readFile(filePath);
    if (newSource.empty()) {
        result.status = ReloadStatus::RuntimeError;
        result.message = "فشل قراءة الملف: " + filePath;
        result.errors.push_back(result.message);
        return result;
    }
    
    // (AR) التحقق من عدم التغيير
    // (EN) Check if source actually changed
    if (filePath == mainFilePath_ && newSource == mainSource_) {
        result.status = ReloadStatus::NoChange;
        result.message = "لم يتغير المحتوى";
        return result;
    }
    
    if (options_.showNotifications) {
        std::cerr << "\033[33m🔄 تم اكتشاف تغيير — جاري إعادة التحميل...\033[0m" << std::endl;
    }
    
    // (AR) إعادة التحليل
    std::vector<std::unique_ptr<AST::Statement>> newProgram;
    std::vector<std::string> parseErrors;
    
    if (!reparseSource(newSource, filePath, newProgram, parseErrors)) {
        result.status = ReloadStatus::ParseError;
        result.message = "خطأ في التحليل النحوي";
        result.errors = std::move(parseErrors);
        
        if (options_.showNotifications) {
            std::cerr << "\033[91m❌ فشل إعادة التحميل — أخطاء نحوية:\033[0m" << std::endl;
            for (const auto& err : result.errors) {
                std::cerr << "   " << err << std::endl;
            }
        }
        
        return result;
    }
    
    // (AR) تحديث الدوال والأصناف مع حفظ الحالة
    try {
        if (options_.preserveState) {
            result.updatedFunctions = updateFunctions(newProgram);
            result.updatedClasses = updateClasses(newProgram);
        } else {
            // (AR) إعادة تنفيذ كاملة (Hot Restart)
            interpreter_->reset();
            auto execResult = interpreter_->execute(newProgram);
            if (!execResult.success) {
                result.status = ReloadStatus::RuntimeError;
                result.message = execResult.errorMessage;
                result.errors.push_back(execResult.errorMessage);
                return result;
            }
        }
        
        // (AR) تحديث الكود المصدري المحفوظ
        if (filePath == mainFilePath_) {
            mainSource_ = newSource;
        }
        
        // (AR) إعادة بناء الواجهة إذا كان هناك callback
        if (rebuildUICallback_) {
            rebuildUICallback_();
        }
        
        result.status = ReloadStatus::Success;
        reloadCount_.fetch_add(1);
        
    } catch (const std::exception& e) {
        result.status = ReloadStatus::RuntimeError;
        result.message = e.what();
        result.errors.push_back(e.what());
        
        if (options_.showNotifications) {
            std::cerr << "\033[91m❌ خطأ وقت التشغيل: " << e.what() << "\033[0m" << std::endl;
        }
        
        return result;
    }
    
    // (AR) حساب زمن التنفيذ
    auto endTime = std::chrono::steady_clock::now();
    result.reloadTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // (AR) رسالة النجاح
    result.message = "تم إعادة التحميل بنجاح";
    
    if (options_.showNotifications) {
        std::cerr << "\033[32m✅ تم إعادة التحميل في "
                  << static_cast<int>(result.reloadTimeMs) << "ms"
                  << " — " << result.updatedFunctions << " دالة"
                  << " + " << result.updatedClasses << " صنف"
                  << " (إعادة #" << reloadCount_.load() << ")"
                  << "\033[0m" << std::endl;
    }
    
    // (AR) اكتشاف الاستيرادات الجديدة
    if (options_.watchImports) {
        discoverImports(newProgram);
    }
    
    return result;
}

bool HotReloadEngine::reparseSource(const std::string& source,
                                     const std::string& filename,
                                     std::vector<std::unique_ptr<AST::Statement>>& outProgram,
                                     std::vector<std::string>& outErrors) {
    // (AR) مسح الأخطاء السابقة
    Sad::Errors::ErrorManager::getInstance().clear();
    Sad::Errors::ErrorManager::getInstance().setSourceCode(source, filename);
    
    // (AR) تحليل معجمي
    Sad::Lexer::LexerCore lexer(source);
    
    // (AR) تحليل نحوي
    Sad::Parser::ParserCore parser(lexer);
    outProgram = parser.parseProgram();
    
    if (parser.hasErrors()) {
        // (AR) جمع الأخطاء
        auto& errorMgr = Sad::Errors::ErrorManager::getInstance();
        if (errorMgr.hasErrors()) {
            for (const auto& diag : errorMgr.getAllDiagnostics()) {
                outErrors.push_back(diag.getMessage(Sad::Errors::Language::ARABIC));
            }
        }
        return false;
    }
    
    return true;
}

int HotReloadEngine::updateFunctions(const std::vector<std::unique_ptr<AST::Statement>>& program) {
    if (!interpreter_) return 0;
    
    int updated = 0;
    auto& funcManager = interpreter_->getFunctionManager();
    
    for (const auto& stmt : program) {
        // (AR) فحص إذا كانت الجملة تعريف دالة (FunctionDecl)
        auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(stmt.get());
        if (funcDecl) {
            const std::string& name = funcDecl->name;
            
            // (AR) إذا الدالة موجودة — إعادة تعريفها (حفظ الحالة)
            if (funcManager.hasFunction(name)) {
                // (AR) حذف التعريف القديم وإضافة الجديد
                funcManager.removeFunction(name);
            }
            
            // (AR) تنفيذ جملة تعريف الدالة لإعادة تسجيلها
            try {
                interpreter_->executeStatement(*stmt);
                updated++;
            } catch (const std::exception& e) {
                // (AR) تسجيل الخطأ والمتابعة
                std::cerr << "⚠️ فشل تحديث الدالة '" << name << "': " << e.what() << std::endl;
            }
        }
    }
    
    return updated;
}

int HotReloadEngine::updateClasses(const std::vector<std::unique_ptr<AST::Statement>>& program) {
    if (!interpreter_) return 0;
    
    int updated = 0;
    auto* classManager = Data::ClassManager::getInstance();
    if (!classManager) return 0;
    
    for (const auto& stmt : program) {
        // (AR) فحص إذا كانت الجملة تعريف صنف (ClassDecl)
        auto* classDecl = dynamic_cast<AST::ClassDecl*>(stmt.get());
        if (classDecl) {
            const std::string& name = classDecl->name;
            
            // (AR) إعادة تعريف الصنف (الكائنات الموجودة تحتفظ بحالتها)
            try {
                interpreter_->executeStatement(*stmt);
                updated++;
            } catch (const std::exception& e) {
                std::cerr << "⚠️ فشل تحديث الصنف '" << name << "': " << e.what() << std::endl;
            }
        }
    }
    
    return updated;
}

void HotReloadEngine::executeNewStatements(const std::vector<std::unique_ptr<AST::Statement>>& program) {
    if (!interpreter_) return;
    
    for (const auto& stmt : program) {
        // (AR) تخطي تعريفات الدوال والأصناف (تم تحديثها)
        if (dynamic_cast<AST::FunctionDecl*>(stmt.get())) continue;
        if (dynamic_cast<AST::ClassDecl*>(stmt.get())) continue;
        
        // (AR) تنفيذ الجمل العمومية الأخرى
        try {
            interpreter_->executeStatement(*stmt);
        } catch (...) {
            // (AR) تجاهل الأخطاء في الجمل العمومية عند Hot Reload
        }
    }
}

void HotReloadEngine::discoverImports(const std::vector<std::unique_ptr<AST::Statement>>& program) {
    namespace fs = std::filesystem;
    
    for (const auto& stmt : program) {
        // (AR) البحث عن جمل الاستيراد
        auto* importStmt = dynamic_cast<AST::ImportStmt*>(stmt.get());
        if (importStmt) {
            // (AR) حل مسار الملف المستورد
            std::string modulePath;
            for (size_t i = 0; i < importStmt->modulePath.size(); ++i) {
                if (i > 0) modulePath += "/";
                modulePath += importStmt->modulePath[i];
            }
            
            // (AR) تحويل مسار الوحدة إلى مسار ملف
            auto mainDir = fs::path(mainFilePath_).parent_path();
            
            // (AR) محاولات مختلفة لإيجاد الملف
            std::vector<std::string> candidates = {
                (mainDir / (modulePath + ".\xD8\xB5")).string(),   // .ص
                (mainDir / (modulePath + ".sad")).string(),
                (mainDir / modulePath).string(),
            };
            
            for (const auto& candidate : candidates) {
                if (fs::exists(candidate)) {
                    fileWatcher_->watchFile(candidate);
                    break;
                }
            }
        }
    }
}

std::string HotReloadEngine::readFile(const std::string& path) {
    auto file = sad::utf8::open_ifstream(path);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace HotReload
} // namespace Sad
