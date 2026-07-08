// بسم الله الرحمن الرحيم
/**
 * @file history_manager.cpp
 * @brief تنفيذ مدير تاريخ الأوامر / History Manager Implementation
 * 
 * @author Sad Language Team
 * @date January 2026
 */

#include "history_manager.h"
#include <algorithm>
#include <fstream>

namespace Sad {
namespace REPL {

HistoryManager::HistoryManager(size_t maxSize, const std::string& filename)
    : currentIndex_(0)
    , maxSize_(maxSize)
    , filename_(filename)
{
    history_.reserve(maxSize);
}

HistoryManager::~HistoryManager()
{
    // Auto-save on destruction / حفظ تلقائي عند التدمير
    if (!filename_.empty()) {
        save();
    }
}

void HistoryManager::add(const std::string& command)
{
    // Don't add empty commands / لا تضف أوامر فارغة
    if (command.empty()) {
        return;
    }
    
    // Don't add duplicates of the last command / لا تضف نسخ مكررة من آخر أمر
    if (!history_.empty() && history_.back() == command) {
        resetCursor();
        return;
    }
    
    // Add command / إضافة الأمر
    history_.push_back(command);
    
    // Enforce max size / تطبيق الحد الأقصى
    if (history_.size() > maxSize_) {
        history_.erase(history_.begin());
    }
    
    resetCursor();
}

std::string HistoryManager::previous()
{
    if (history_.empty()) {
        return "";
    }
    
    if (currentIndex_ > 0) {
        currentIndex_--;
    }
    
    return history_[currentIndex_];
}

std::string HistoryManager::next()
{
    if (history_.empty()) {
        return "";
    }
    
    if (currentIndex_ < history_.size() - 1) {
        currentIndex_++;
        return history_[currentIndex_];
    } else {
        currentIndex_ = history_.size();
        return "";
    }
}

std::vector<std::string> HistoryManager::search(const std::string& query) const
{
    std::vector<std::string> results;
    
    for (const auto& cmd : history_) {
        if (cmd.find(query) != std::string::npos) {
            results.push_back(cmd);
        }
    }
    
    return results;
}

void HistoryManager::clear()
{
    history_.clear();
    currentIndex_ = 0;
}

bool HistoryManager::save(const std::string& filename)
{
    std::string file = filename.empty() ? filename_ : filename;
    
    if (file.empty()) {
        return false;
    }
    
    std::ofstream out(file);
    if (!out.is_open()) {
        return false;
    }
    
    for (const auto& cmd : history_) {
        out << cmd << "\n";
    }
    
    out.close();
    return true;
}

bool HistoryManager::load(const std::string& filename)
{
    std::string file = filename.empty() ? filename_ : filename;
    
    if (file.empty()) {
        return false;
    }
    
    std::ifstream in(file);
    if (!in.is_open()) {
        return false;
    }
    
    clear();
    
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            history_.push_back(line);
            
            // Enforce max size / تطبيق الحد الأقصى
            if (history_.size() > maxSize_) {
                history_.erase(history_.begin());
            }
        }
    }
    
    in.close();
    resetCursor();
    
    return true;
}

} // namespace REPL
} // namespace Sad
