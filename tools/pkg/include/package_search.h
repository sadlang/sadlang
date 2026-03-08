/**
 * @file package_search.h
 * @brief محرك بحث الحزم - sad-pkg search
 * @brief (EN) Package search engine - sad-pkg search
 * 
 * @author عمر
 * @date نوفمبر 2026
 */

#ifndef SAD_PKG_SEARCH_H
#define SAD_PKG_SEARCH_H

#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;map&gt;
#include &lt;memory&gt;
#include &lt;chrono&gt;
#include &lt;functional&gt;
#include &lt;optional&gt;

namespace Sad {
namespace Pkg {

//==============================================================================
// أنواع البيانات
//==============================================================================

/**
 * @brief معلومات الحزمة
 */
struct PackageInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string descriptionAr;  // وصف عربي
    std::string author;
    std::string license;
    std::string homepage;
    std::string repository;
    
    std::vector&lt;std::string&gt; keywords;
    std::vector&lt;std::string&gt; categories;
    std::map&lt;std::string, std::string&gt; dependencies;
    
    uint64_t downloads;
    double rating;
    int ratingCount;
    
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point updatedAt;
    
    bool isOfficial;
    bool isVerified;
    bool isDeprecated;
};

/**
 * @brief نتيجة البحث
 */
struct SearchResult {
    PackageInfo package;
    double relevanceScore;
    std::string matchedField;
    std::string highlightedName;
    std::string highlightedDescription;
};

/**
 * @brief خيارات البحث
 */
struct SearchOptions {
    int limit = 20;
    int offset = 0;
    
    std::string sortBy = "relevance";  // relevance, downloads, rating, updated, name
    bool sortDescending = true;
    
    std::vector&lt;std::string&gt; categories;
    std::vector&lt;std::string&gt; keywords;
    std::string author;
    std::string license;
    
    bool officialOnly = false;
    bool verifiedOnly = false;
    bool excludeDeprecated = true;
    
    std::optional&lt;std::chrono::system_clock::time_point&gt; updatedAfter;
    std::optional&lt;uint64_t&gt; minDownloads;
    std::optional&lt;double&gt; minRating;
};

/**
 * @brief إحصائيات البحث
 */
struct SearchStats {
    int totalResults;
    int returnedResults;
    double searchTimeMs;
    std::string query;
};

//==============================================================================
// محرك البحث
//==============================================================================

/**
 * @brief محرك بحث الحزم
 */
class PackageSearchEngine {
public:
    /**
     * @brief بحث نصي
     */
    std::vector&lt;SearchResult&gt; search(
        const std::string&amp; query,
        const SearchOptions&amp; options = {}
    );
    
    /**
     * @brief بحث متقدم
     */
    std::vector&lt;SearchResult&gt; advancedSearch(
        const std::map&lt;std::string, std::string&gt;&amp; filters,
        const SearchOptions&amp; options = {}
    );
    
    /**
     * @brief بحث بالتصنيف
     */
    std::vector&lt;SearchResult&gt; browseCategory(
        const std::string&amp; category,
        const SearchOptions&amp; options = {}
    );
    
    /**
     * @brief الحزم الشائعة
     */
    std::vector&lt;PackageInfo&gt; trending(int limit = 10);
    
    /**
     * @brief الحزم الجديدة
     */
    std::vector&lt;PackageInfo&gt; recent(int limit = 10);
    
    /**
     * @brief الحزم الأكثر تحميلاً
     */
    std::vector&lt;PackageInfo&gt; mostDownloaded(int limit = 10);
    
    /**
     * @brief الحزم الأعلى تقييماً
     */
    std::vector&lt;PackageInfo&gt; topRated(int limit = 10);
    
    /**
     * @brief حزم المؤلف
     */
    std::vector&lt;PackageInfo&gt; byAuthor(const std::string&amp; author);
    
    /**
     * @brief الحصول على إحصائيات آخر بحث
     */
    SearchStats getLastSearchStats() const { return lastStats_; }
    
    /**
     * @brief اقتراحات الإكمال التلقائي
     */
    std::vector&lt;std::string&gt; autocomplete(const std::string&amp; prefix, int limit = 10);
    
    /**
     * @brief حزم مشابهة
     */
    std::vector&lt;PackageInfo&gt; similar(const std::string&amp; packageName, int limit = 5);
    
    /**
     * @brief الحزم الموصى بها
     */
    std::vector&lt;PackageInfo&gt; recommended(const std::vector&lt;std::string&gt;&amp; installed);

private:
    SearchStats lastStats_;
    
    // فهرسة البحث
    struct SearchIndex;
    std::unique_ptr&lt;SearchIndex&gt; index_;
    
    // حساب صلة النتيجة
    double calculateRelevance(const PackageInfo&amp; pkg, const std::string&amp; query);
    
    // تمييز النص المطابق
    std::string highlight(const std::string&amp; text, const std::string&amp; query);
    
    // تطبيق الفلاتر
    bool matchesFilters(const PackageInfo&amp; pkg, const SearchOptions&amp; opts);
    
    // ترتيب النتائج
    void sortResults(std::vector&lt;SearchResult&gt;&amp; results, const SearchOptions&amp; opts);
};

//==============================================================================
// فهرس الحزم
//==============================================================================

/**
 * @brief فهرس الحزم المحلي
 */
class PackageIndex {
public:
    /**
     * @brief تحميل الفهرس من الخادم
     */
    bool update();
    
    /**
     * @brief تحميل من ملف مخبأ
     */
    bool loadFromCache();
    
    /**
     * @brief حفظ في الذاكرة المخبأة
     */
    bool saveToCache();
    
    /**
     * @brief الحصول على معلومات حزمة
     */
    std::optional&lt;PackageInfo&gt; getPackage(const std::string&amp; name);
    
    /**
     * @brief الحصول على إصدارات متاحة
     */
    std::vector&lt;std::string&gt; getVersions(const std::string&amp; name);
    
    /**
     * @brief التحقق من وجود حزمة
     */
    bool exists(const std::string&amp; name);
    
    /**
     * @brief عدد الحزم في الفهرس
     */
    size_t count() const { return packages_.size(); }
    
    /**
     * @brief آخر تحديث
     */
    std::chrono::system_clock::time_point lastUpdated() const { return lastUpdated_; }
    
    /**
     * @brief قائمة التصنيفات
     */
    std::vector&lt;std::string&gt; getCategories() const;
    
    /**
     * @brief إحصائيات الفهرس
     */
    struct IndexStats {
        size_t totalPackages;
        size_t totalAuthors;
        uint64_t totalDownloads;
        std::chrono::system_clock::time_point lastUpdated;
    };
    IndexStats getStats() const;

private:
    std::map&lt;std::string, PackageInfo&gt; packages_;
    std::chrono::system_clock::time_point lastUpdated_;
    std::string cacheFile_;
    std::string registryUrl_ = "https://registry.sad-pkg.org";
};

//==============================================================================
// عميل السجل
//==============================================================================

/**
 * @brief عميل سجل الحزم
 */
class RegistryClient {
public:
    explicit RegistryClient(const std::string&amp; registryUrl);
    
    /**
     * @brief بحث في السجل
     */
    std::vector&lt;SearchResult&gt; search(const std::string&amp; query, const SearchOptions&amp; opts);
    
    /**
     * @brief تحميل معلومات حزمة
     */
    std::optional&lt;PackageInfo&gt; fetchPackage(const std::string&amp; name);
    
    /**
     * @brief تحميل الفهرس الكامل
     */
    std::vector&lt;PackageInfo&gt; fetchAll();
    
    /**
     * @brief التحقق من الاتصال
     */
    bool ping();
    
    /**
     * @brief تعيين timeout
     */
    void setTimeout(int milliseconds) { timeout_ = milliseconds; }

private:
    std::string registryUrl_;
    int timeout_ = 30000;
};

//==============================================================================
// واجهة سطر الأوامر
//==============================================================================

/**
 * @brief أوامر البحث CLI
 */
namespace CLI {
    /**
     * @brief sad-pkg search <query>
     */
    int searchCommand(int argc, char** argv);
    
    /**
     * @brief sad-pkg browse [category]
     */
    int browseCommand(int argc, char** argv);
    
    /**
     * @brief sad-pkg info <package>
     */
    int infoCommand(int argc, char** argv);
    
    /**
     * @brief sad-pkg trending
     */
    int trendingCommand(int argc, char** argv);
    
    /**
     * @brief طباعة نتائج البحث
     */
    void printResults(const std::vector&lt;SearchResult&gt;&amp; results, bool verbose = false);
    
    /**
     * @brief طباعة معلومات حزمة
     */
    void printPackageInfo(const PackageInfo&amp; pkg);
}

//==============================================================================
// أمثلة الاستخدام
//==============================================================================

/**
 * @brief أمثلة الاستخدام
 * 
 * @code
 * # بحث في الحزم
 * sad-pkg search http
 * 
 * # بحث بالتصنيف
 * sad-pkg search --category=network http
 * 
 * # الحزم الشائعة
 * sad-pkg trending
 * 
 * # معلومات حزمة
 * sad-pkg info sad-http
 * 
 * # تصفح التصنيفات
 * sad-pkg browse graphics
 * @endcode
 */

} // namespace Pkg
} // namespace Sad

#endif // SAD_PKG_SEARCH_H
