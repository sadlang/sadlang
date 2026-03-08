// ======================================================================
// video_links.cpp - نظام روابط الفيديو التعليمية
// Video Links System for Educational Content
// ======================================================================
// بسم الله الرحمن الرحيم
//
// الوصف بالعربية:
//   نظام إدارة روابط الفيديو التعليمية المرتبطة بالدروس والأخطاء.
//   يوفر وصولاً سريعاً للمحتوى المرئي عند مواجهة المتعلم لمشكلة.
//
// English Description:
//   System for managing educational video links associated with lessons
//   and errors. Provides quick access to visual content when learners
//   encounter problems.
//
// المؤلف: فريق لغة ص
// التاريخ: 2026-01-23
// المرحلة: 30 - أستاذ ص (نظام التعليم المدمج)
// ======================================================================

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <algorithm>

namespace Sad {
namespace Teacher {

// ======================================================================
// أنواع الفيديو / Video Types
// ======================================================================

/**
 * @enum VideoType
 * @brief (AR) نوع الفيديو التعليمي
 * @brief (EN) Type of educational video
 */
enum class VideoType {
    CONCEPT_EXPLANATION,    // شرح مفهوم
    CODE_WALKTHROUGH,       // شرح كود خطوة بخطوة
    ERROR_SOLUTION,         // حل خطأ
    QUICK_TIP,              // نصيحة سريعة
    FULL_TUTORIAL,          // درس كامل
    LIVE_CODING,            // برمجة مباشرة
    INTERVIEW_PREP          // تحضير للمقابلات
};

/**
 * @enum VideoPlatform
 * @brief (AR) منصة الفيديو
 * @brief (EN) Video platform
 */
enum class VideoPlatform {
    YOUTUBE,
    VIMEO,
    SAD_PLATFORM,           // منصة لغة ص الرسمية
    UDEMY,
    COURSERA,
    LOCAL                   // ملف محلي
};

/**
 * @enum VideoLanguage
 * @brief (AR) لغة الفيديو
 * @brief (EN) Video language
 */
enum class VideoLanguage {
    ARABIC,
    ENGLISH,
    ARABIC_SUBTITLED,       // إنجليزي مع ترجمة عربية
    ENGLISH_SUBTITLED       // عربي مع ترجمة إنجليزية
};

// ======================================================================
// هياكل البيانات / Data Structures
// ======================================================================

/**
 * @struct VideoLink
 * @brief (AR) معلومات رابط الفيديو
 * @brief (EN) Video link information
 */
struct VideoLink {
    std::string id;                         // معرف الفيديو
    std::string titleArabic;                // العنوان بالعربية
    std::string titleEnglish;               // العنوان بالإنجليزية
    std::string url;                        // رابط الفيديو
    VideoType type;                         // نوع الفيديو
    VideoPlatform platform;                 // المنصة
    VideoLanguage language;                 // اللغة
    int durationSeconds;                    // المدة بالثواني
    std::string thumbnailUrl;               // صورة مصغرة
    std::string author;                     // المؤلف/القناة
    std::vector<std::string> relatedTopics; // المواضيع المرتبطة
    std::vector<std::string> relatedErrors; // الأخطاء المرتبطة
    std::string description;                // وصف مختصر
    int viewCount;                          // عدد المشاهدات
    float rating;                           // التقييم (0-5)
    bool isOfficial;                        // هل من المصادر الرسمية
};

/**
 * @struct VideoPlaylist
 * @brief (AR) قائمة تشغيل فيديوهات
 * @brief (EN) Video playlist
 */
struct VideoPlaylist {
    std::string id;                         // معرف القائمة
    std::string titleArabic;                // العنوان بالعربية
    std::string titleEnglish;               // العنوان بالإنجليزية
    std::string description;                // الوصف
    std::vector<std::string> videoIds;      // معرفات الفيديوهات
    int totalDurationSeconds;               // المدة الإجمالية
    std::string lessonId;                   // الدرس المرتبط (اختياري)
};

// ======================================================================
// فئة مدير الفيديو / Video Manager Class
// ======================================================================

/**
 * @class VideoLinkManager
 * @brief (AR) مدير روابط الفيديو التعليمية
 * @brief (EN) Educational video links manager
 */
class VideoLinkManager {
public:
    // ============================================================
    // Singleton Pattern / نمط المفرد
    // ============================================================
    
    static VideoLinkManager& getInstance() {
        static VideoLinkManager instance;
        return instance;
    }
    
    VideoLinkManager(const VideoLinkManager&) = delete;
    VideoLinkManager& operator=(const VideoLinkManager&) = delete;
    
    // ============================================================
    // تحميل البيانات / Loading Data
    // ============================================================
    
    /**
     * @brief (AR) تحميل روابط الفيديو المضمنة
     * @brief (EN) Load embedded video links
     */
    void loadEmbeddedVideos() {
        if (m_loaded) return;
        
        loadOwnershipVideos();
        loadBorrowingVideos();
        loadPatternMatchingVideos();
        loadErrorSolutionVideos();
        loadBasicsVideos();
        createPlaylists();
        
        m_loaded = true;
    }
    
    // ============================================================
    // البحث عن الفيديو / Finding Videos
    // ============================================================
    
    /**
     * @brief (AR) الحصول على فيديو بمعرفه
     * @brief (EN) Get video by ID
     */
    std::optional<VideoLink> getVideo(const std::string& video_id) const {
        auto it = m_videos.find(video_id);
        if (it != m_videos.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * @brief (AR) البحث عن فيديوهات لموضوع
     * @brief (EN) Find videos for a topic
     */
    std::vector<VideoLink> getVideosForTopic(const std::string& topic) const {
        std::vector<VideoLink> result;
        for (const auto& [id, video] : m_videos) {
            for (const auto& t : video.relatedTopics) {
                if (t.find(topic) != std::string::npos || 
                    topic.find(t) != std::string::npos) {
                    result.push_back(video);
                    break;
                }
            }
        }
        return result;
    }
    
    /**
     * @brief (AR) الحصول على فيديوهات لخطأ معين
     * @brief (EN) Get videos for a specific error
     */
    std::vector<VideoLink> getVideosForError(const std::string& error_code) const {
        std::vector<VideoLink> result;
        for (const auto& [id, video] : m_videos) {
            for (const auto& err : video.relatedErrors) {
                if (err == error_code) {
                    result.push_back(video);
                    break;
                }
            }
        }
        return result;
    }
    
    /**
     * @brief (AR) الحصول على فيديوهات لدرس معين
     * @brief (EN) Get videos for a specific lesson
     */
    std::vector<VideoLink> getVideosForLesson(const std::string& lesson_id) const {
        std::vector<VideoLink> result;
        for (const auto& [id, video] : m_videos) {
            for (const auto& topic : video.relatedTopics) {
                if (topic == lesson_id) {
                    result.push_back(video);
                    break;
                }
            }
        }
        return result;
    }
    
    /**
     * @brief (AR) البحث في الفيديوهات بكلمة مفتاحية
     * @brief (EN) Search videos by keyword
     */
    std::vector<VideoLink> searchVideos(const std::string& query) const {
        std::vector<VideoLink> result;
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), 
                      lowerQuery.begin(), ::tolower);
        
        for (const auto& [id, video] : m_videos) {
            // البحث في العنوان العربي
            if (video.titleArabic.find(query) != std::string::npos) {
                result.push_back(video);
                continue;
            }
            
            // البحث في العنوان الإنجليزي
            std::string lowerTitle = video.titleEnglish;
            std::transform(lowerTitle.begin(), lowerTitle.end(), 
                          lowerTitle.begin(), ::tolower);
            if (lowerTitle.find(lowerQuery) != std::string::npos) {
                result.push_back(video);
                continue;
            }
            
            // البحث في الوصف
            if (video.description.find(query) != std::string::npos) {
                result.push_back(video);
            }
        }
        
        return result;
    }
    
    /**
     * @brief (AR) الحصول على الفيديوهات الرسمية فقط
     * @brief (EN) Get official videos only
     */
    std::vector<VideoLink> getOfficialVideos() const {
        std::vector<VideoLink> result;
        for (const auto& [id, video] : m_videos) {
            if (video.isOfficial) {
                result.push_back(video);
            }
        }
        return result;
    }
    
    /**
     * @brief (AR) الحصول على الفيديوهات حسب اللغة
     * @brief (EN) Get videos by language
     */
    std::vector<VideoLink> getVideosByLanguage(VideoLanguage lang) const {
        std::vector<VideoLink> result;
        for (const auto& [id, video] : m_videos) {
            if (video.language == lang) {
                result.push_back(video);
            }
        }
        return result;
    }
    
    // ============================================================
    // قوائم التشغيل / Playlists
    // ============================================================
    
    /**
     * @brief (AR) الحصول على قائمة تشغيل
     * @brief (EN) Get playlist
     */
    std::optional<VideoPlaylist> getPlaylist(const std::string& playlist_id) const {
        auto it = m_playlists.find(playlist_id);
        if (it != m_playlists.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    /**
     * @brief (AR) الحصول على قائمة تشغيل لدرس
     * @brief (EN) Get playlist for lesson
     */
    std::optional<VideoPlaylist> getPlaylistForLesson(const std::string& lesson_id) const {
        for (const auto& [id, playlist] : m_playlists) {
            if (playlist.lessonId == lesson_id) {
                return playlist;
            }
        }
        return std::nullopt;
    }
    
    // ============================================================
    // طباعة / Display
    // ============================================================
    
    /**
     * @brief (AR) طباعة معلومات الفيديو
     * @brief (EN) Print video info
     */
    void printVideoInfo(const VideoLink& video) const {
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║  🎬 " << video.titleArabic << "\n";
        std::cout << "║     " << video.titleEnglish << "\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║  ⏱️ المدة: " << formatDuration(video.durationSeconds) << "\n";
        std::cout << "║  📺 المنصة: " << getPlatformName(video.platform) << "\n";
        std::cout << "║  🌐 اللغة: " << getLanguageName(video.language) << "\n";
        std::cout << "║  👤 المؤلف: " << video.author << "\n";
        if (video.rating > 0) {
            std::cout << "║  ⭐ التقييم: " << video.rating << "/5\n";
        }
        std::cout << "║                                                              ║\n";
        std::cout << "║  📖 " << video.description << "\n";
        std::cout << "║                                                              ║\n";
        std::cout << "║  🔗 " << video.url << "\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    }
    
    /**
     * @brief (AR) طباعة قائمة الفيديوهات
     * @brief (EN) Print video list
     */
    void printVideoList(const std::vector<VideoLink>& videos) const {
        if (videos.empty()) {
            std::cout << "❌ لا توجد فيديوهات متاحة\n";
            return;
        }
        
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║              🎬 الفيديوهات التعليمية المتاحة                  ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        
        int idx = 1;
        for (const auto& video : videos) {
            std::cout << "║  " << idx++ << ". " << video.titleArabic << "\n";
            std::cout << "║     ⏱️ " << formatDuration(video.durationSeconds) 
                      << " | " << getPlatformName(video.platform);
            if (video.isOfficial) {
                std::cout << " ✓ رسمي";
            }
            std::cout << "\n";
            std::cout << "║                                                              ║\n";
        }
        
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║  💡 أدخل رقم الفيديو لعرض التفاصيل                            ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
    }
    
    /**
     * @brief (AR) فتح الفيديو في المتصفح
     * @brief (EN) Open video in browser
     */
    bool openInBrowser(const std::string& video_id) const {
        auto video = getVideo(video_id);
        if (!video) return false;
        
        std::string command;
#ifdef _WIN32
        command = "start \"\" \"" + video->url + "\"";
#elif __APPLE__
        command = "open \"" + video->url + "\"";
#else
        command = "xdg-open \"" + video->url + "\"";
#endif
        
        return system(command.c_str()) == 0;
    }

private:
    VideoLinkManager() : m_loaded(false) {}
    
    std::map<std::string, VideoLink> m_videos;
    std::map<std::string, VideoPlaylist> m_playlists;
    bool m_loaded;
    
    // ============================================================
    // دوال مساعدة / Helper Functions
    // ============================================================
    
    std::string formatDuration(int seconds) const {
        int hours = seconds / 3600;
        int minutes = (seconds % 3600) / 60;
        int secs = seconds % 60;
        
        std::string result;
        if (hours > 0) {
            result += std::to_string(hours) + " ساعة ";
        }
        if (minutes > 0) {
            result += std::to_string(minutes) + " دقيقة";
        }
        if (hours == 0 && minutes == 0) {
            result = std::to_string(secs) + " ثانية";
        }
        
        return result;
    }
    
    std::string getPlatformName(VideoPlatform platform) const {
        switch (platform) {
            case VideoPlatform::YOUTUBE: return "يوتيوب";
            case VideoPlatform::VIMEO: return "فيميو";
            case VideoPlatform::SAD_PLATFORM: return "منصة ص";
            case VideoPlatform::UDEMY: return "يوديمي";
            case VideoPlatform::COURSERA: return "كورسيرا";
            case VideoPlatform::LOCAL: return "محلي";
            default: return "غير معروف";
        }
    }
    
    std::string getLanguageName(VideoLanguage lang) const {
        switch (lang) {
            case VideoLanguage::ARABIC: return "عربي";
            case VideoLanguage::ENGLISH: return "إنجليزي";
            case VideoLanguage::ARABIC_SUBTITLED: return "إنجليزي (مترجم)";
            case VideoLanguage::ENGLISH_SUBTITLED: return "عربي (مترجم)";
            default: return "غير محدد";
        }
    }
    
    // ============================================================
    // تحميل الفيديوهات المضمنة / Loading Embedded Videos
    // ============================================================
    
    void loadOwnershipVideos() {
        VideoLink v1;
        v1.id = "ownership_intro_ar";
        v1.titleArabic = "مقدمة في نظام الملكية";
        v1.titleEnglish = "Introduction to Ownership System";
        v1.url = "https://sad-lang.org/videos/ownership-intro";
        v1.type = VideoType::CONCEPT_EXPLANATION;
        v1.platform = VideoPlatform::SAD_PLATFORM;
        v1.language = VideoLanguage::ARABIC;
        v1.durationSeconds = 900; // 15 دقيقة
        v1.author = "فريق لغة ص الرسمي";
        v1.relatedTopics = {"ownership_basics", "ملكية", "ownership"};
        v1.relatedErrors = {"ص0001", "ص0002"};
        v1.description = "شرح شامل لنظام الملكية في لغة ص وكيف يضمن سلامة الذاكرة";
        v1.isOfficial = true;
        v1.rating = 4.8f;
        m_videos[v1.id] = v1;
        
        VideoLink v2;
        v2.id = "ownership_move_ar";
        v2.titleArabic = "نقل الملكية - الفرق بين النسخ والنقل";
        v2.titleEnglish = "Ownership Move - Copy vs Move";
        v2.url = "https://sad-lang.org/videos/ownership-move";
        v2.type = VideoType::CODE_WALKTHROUGH;
        v2.platform = VideoPlatform::SAD_PLATFORM;
        v2.language = VideoLanguage::ARABIC;
        v2.durationSeconds = 720; // 12 دقيقة
        v2.author = "فريق لغة ص الرسمي";
        v2.relatedTopics = {"ownership_basics", "نقل", "move"};
        v2.relatedErrors = {"ص0001"};
        v2.description = "شرح الفرق بين نسخ القيم ونقلها مع أمثلة عملية";
        v2.isOfficial = true;
        v2.rating = 4.7f;
        m_videos[v2.id] = v2;
    }
    
    void loadBorrowingVideos() {
        VideoLink v1;
        v1.id = "borrowing_rules_ar";
        v1.titleArabic = "قواعد الاستعارة في لغة ص";
        v1.titleEnglish = "Borrowing Rules in Sad Language";
        v1.url = "https://sad-lang.org/videos/borrowing-rules";
        v1.type = VideoType::CONCEPT_EXPLANATION;
        v1.platform = VideoPlatform::SAD_PLATFORM;
        v1.language = VideoLanguage::ARABIC;
        v1.durationSeconds = 1080; // 18 دقيقة
        v1.author = "فريق لغة ص الرسمي";
        v1.relatedTopics = {"borrowing_rules", "استعارة", "borrowing"};
        v1.relatedErrors = {"ص0002", "ص0003"};
        v1.description = "شرح تفصيلي لقواعد الاستعارة والفرق بين الاستعارة القابلة وغير القابلة للتعديل";
        v1.isOfficial = true;
        v1.rating = 4.9f;
        m_videos[v1.id] = v1;
        
        VideoLink v2;
        v2.id = "borrowing_mutable_ar";
        v2.titleArabic = "الاستعارة القابلة للتعديل";
        v2.titleEnglish = "Mutable Borrowing";
        v2.url = "https://sad-lang.org/videos/mutable-borrowing";
        v2.type = VideoType::CODE_WALKTHROUGH;
        v2.platform = VideoPlatform::SAD_PLATFORM;
        v2.language = VideoLanguage::ARABIC;
        v2.durationSeconds = 600; // 10 دقائق
        v2.author = "فريق لغة ص الرسمي";
        v2.relatedTopics = {"borrowing_rules", "استعارة_متغيرة"};
        v2.relatedErrors = {"ص0003"};
        v2.description = "متى ولماذا نستخدم الاستعارة القابلة للتعديل";
        v2.isOfficial = true;
        v2.rating = 4.6f;
        m_videos[v2.id] = v2;
    }
    
    void loadPatternMatchingVideos() {
        VideoLink v1;
        v1.id = "pattern_match_intro_ar";
        v1.titleArabic = "مطابقة الأنماط - مقدمة شاملة";
        v1.titleEnglish = "Pattern Matching - Comprehensive Introduction";
        v1.url = "https://sad-lang.org/videos/pattern-matching";
        v1.type = VideoType::FULL_TUTORIAL;
        v1.platform = VideoPlatform::SAD_PLATFORM;
        v1.language = VideoLanguage::ARABIC;
        v1.durationSeconds = 1500; // 25 دقيقة
        v1.author = "فريق لغة ص الرسمي";
        v1.relatedTopics = {"pattern_matching_intro", "طابق", "match"};
        v1.relatedErrors = {"ص1001"};
        v1.description = "تعلم استخدام طابق للتعامل مع الحالات المختلفة بطريقة آمنة وواضحة";
        v1.isOfficial = true;
        v1.rating = 4.8f;
        m_videos[v1.id] = v1;
    }
    
    void loadErrorSolutionVideos() {
        VideoLink v1;
        v1.id = "error_0001_solution";
        v1.titleArabic = "حل خطأ ص0001 - نقل الملكية غير الصالح";
        v1.titleEnglish = "Fixing Error ص0001 - Invalid Ownership Transfer";
        v1.url = "https://sad-lang.org/videos/fix-error-0001";
        v1.type = VideoType::ERROR_SOLUTION;
        v1.platform = VideoPlatform::SAD_PLATFORM;
        v1.language = VideoLanguage::ARABIC;
        v1.durationSeconds = 420; // 7 دقائق
        v1.author = "فريق لغة ص الرسمي";
        v1.relatedTopics = {"ownership_basics"};
        v1.relatedErrors = {"ص0001"};
        v1.description = "شرح سريع لأسباب وحلول خطأ نقل الملكية غير الصالح";
        v1.isOfficial = true;
        v1.rating = 4.5f;
        m_videos[v1.id] = v1;
        
        VideoLink v2;
        v2.id = "error_0002_solution";
        v2.titleArabic = "حل خطأ ص0002 - استخدام بعد النقل";
        v2.titleEnglish = "Fixing Error ص0002 - Use After Move";
        v2.url = "https://sad-lang.org/videos/fix-error-0002";
        v2.type = VideoType::ERROR_SOLUTION;
        v2.platform = VideoPlatform::SAD_PLATFORM;
        v2.language = VideoLanguage::ARABIC;
        v2.durationSeconds = 480; // 8 دقائق
        v2.author = "فريق لغة ص الرسمي";
        v2.relatedTopics = {"ownership_basics", "borrowing_rules"};
        v2.relatedErrors = {"ص0002"};
        v2.description = "لماذا لا يمكنك استخدام متغير بعد نقل ملكيته وكيف تحل هذا";
        v2.isOfficial = true;
        v2.rating = 4.7f;
        m_videos[v2.id] = v2;
    }
    
    void loadBasicsVideos() {
        VideoLink v1;
        v1.id = "sad_intro_ar";
        v1.titleArabic = "مقدمة للغة ص - أول برنامج لك";
        v1.titleEnglish = "Introduction to Sad Language - Your First Program";
        v1.url = "https://sad-lang.org/videos/intro";
        v1.type = VideoType::FULL_TUTORIAL;
        v1.platform = VideoPlatform::SAD_PLATFORM;
        v1.language = VideoLanguage::ARABIC;
        v1.durationSeconds = 1200; // 20 دقيقة
        v1.author = "فريق لغة ص الرسمي";
        v1.relatedTopics = {"type_basics", "functions_basics", "مقدمة"};
        v1.description = "ابدأ رحلتك مع لغة ص من الصفر إلى كتابة أول برنامج";
        v1.isOfficial = true;
        v1.rating = 4.9f;
        m_videos[v1.id] = v1;
    }
    
    void createPlaylists() {
        VideoPlaylist pl1;
        pl1.id = "ownership_series";
        pl1.titleArabic = "سلسلة نظام الملكية الكاملة";
        pl1.titleEnglish = "Complete Ownership System Series";
        pl1.description = "تعلم نظام الملكية من البداية إلى الاحتراف";
        pl1.videoIds = {"ownership_intro_ar", "ownership_move_ar", 
                        "borrowing_rules_ar", "borrowing_mutable_ar"};
        pl1.totalDurationSeconds = 3300;
        pl1.lessonId = "ownership_basics";
        m_playlists[pl1.id] = pl1;
        
        VideoPlaylist pl2;
        pl2.id = "beginner_path";
        pl2.titleArabic = "مسار المبتدئين الكامل";
        pl2.titleEnglish = "Complete Beginner Path";
        pl2.description = "ابدأ من الصفر واتقن أساسيات لغة ص";
        pl2.videoIds = {"sad_intro_ar", "ownership_intro_ar", 
                        "pattern_match_intro_ar"};
        pl2.totalDurationSeconds = 3600;
        m_playlists[pl2.id] = pl2;
    }
};

// ======================================================================
// دوال مساعدة عامة / Global Helper Functions
// ======================================================================

/**
 * @brief (AR) الحصول على مدير الفيديو
 * @brief (EN) Get video manager
 */
VideoLinkManager& getVideoManager() {
    auto& manager = VideoLinkManager::getInstance();
    manager.loadEmbeddedVideos();
    return manager;
}

/**
 * @brief (AR) عرض الفيديوهات المرتبطة بخطأ
 * @brief (EN) Show videos related to an error
 */
void showVideosForError(const std::string& error_code) {
    auto& manager = getVideoManager();
    auto videos = manager.getVideosForError(error_code);
    
    if (videos.empty()) {
        std::cout << "لا توجد فيديوهات تعليمية لهذا الخطأ حالياً\n";
        return;
    }
    
    std::cout << "\n📺 فيديوهات تعليمية لحل الخطأ " << error_code << ":\n\n";
    manager.printVideoList(videos);
}

} // namespace Teacher
} // namespace Sad
