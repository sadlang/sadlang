// بسم الله الرحمن الرحيم
/**
 * @file history_manager.h
 * @brief مدير تاريخ الأوامر / Command History Manager
 * 
 * (AR) يدير تاريخ الأوامر المدخلة في REPL
 * (EN) Manages history of commands entered in REPL
 * 
 * الميزات / Features:
 * - حفظ وتحميل التاريخ من ملف
 * - البحث في التاريخ
 * - التنقل (السابق/التالي)
 * - حد أقصى لعدد العناصر
 * 
 * @author Sad Language Team
 * @date January 2026
 */

#pragma once

#include <string>
#include <vector>
#include <fstream>

namespace Sad {
namespace REPL {

/**
 * @brief مدير تاريخ الأوامر / Command History Manager
 */
class HistoryManager {
public:
    /**
     * @brief البناء / Constructor
     * @param maxSize الحد الأقصى لعدد العناصر / Maximum number of entries
     * @param filename اسم ملف التاريخ (اختياري) / History file name (optional)
     */
    explicit HistoryManager(size_t maxSize = 1000, const std::string& filename = "");
    
    /**
     * @brief المدمر / Destructor
     */
    ~HistoryManager();
    
    /**
     * @brief إضافة أمر للتاريخ / Add command to history
     * @param command الأمر / Command
     */
    void add(const std::string& command);
    
    /**
     * @brief الحصول على الأمر السابق / Get previous command
     * @return الأمر السابق / Previous command (empty if none)
     */
    std::string previous();
    
    /**
     * @brief الحصول على الأمر التالي / Get next command
     * @return الأمر التالي / Next command (empty if none)
     */
    std::string next();
    
    /**
     * @brief البحث في التاريخ / Search history
     * @param query نص البحث / Search query
     * @return الأوامر المطابقة / Matching commands
     */
    std::vector<std::string> search(const std::string& query) const;
    
    /**
     * @brief الحصول على جميع الأوامر / Get all commands
     * @return جميع الأوامر / All commands
     */
    const std::vector<std::string>& getAll() const { return history_; }
    
    /**
     * @brief مسح التاريخ / Clear history
     */
    void clear();
    
    /**
     * @brief حفظ التاريخ لملف / Save history to file
     * @param filename اسم الملف (اختياري) / File name (optional)
     * @return true إذا نجح / true if successful
     */
    bool save(const std::string& filename = "");
    
    /**
     * @brief تحميل التاريخ من ملف / Load history from file
     * @param filename اسم الملف (اختياري) / File name (optional)
     * @return true إذا نجح / true if successful
     */
    bool load(const std::string& filename = "");
    
    /**
     * @brief الحصول على عدد الأوامر / Get number of commands
     * @return العدد / Count
     */
    size_t size() const { return history_.size(); }
    
    /**
     * @brief فحص إذا كان التاريخ فارغ / Check if history is empty
     * @return true إذا كان فارغ / true if empty
     */
    bool empty() const { return history_.empty(); }
    
    /**
     * @brief إعادة تعيين المؤشر / Reset cursor
     */
    void resetCursor() { currentIndex_ = history_.size(); }

private:
    std::vector<std::string> history_;      ///< التاريخ / History
    size_t currentIndex_;                   ///< المؤشر الحالي / Current index
    size_t maxSize_;                        ///< الحد الأقصى / Maximum size
    std::string filename_;                  ///< اسم الملف / File name
};

} // namespace REPL
} // namespace Sad
