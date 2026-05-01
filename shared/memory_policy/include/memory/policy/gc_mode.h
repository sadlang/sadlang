// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file gc_mode.h
 * @brief نظام وضع الذاكرة الذكي - ذاكرة ص الذكية
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 * @phase المرحلة 4: US2 - أمان الذاكرة
 *
 * =============================================================================
 *                    ذاكرة ص الذكية - نظام الذاكرة المزدوج الثوري
 *                    Sad Smart Memory - Revolutionary Dual Memory System
 * =============================================================================
 *
 *     لغة ص تُقدِّم نظام ذاكرة ثوري يجمع بين سهولة التطوير وأداء الإنتاج:
 *
 *     ┌─────────────────────────────────────────────────────────────────────┐
 *     │                     وضع التطوير (--تطوير)                          │
 *     ├─────────────────────────────────────────────────────────────────────┤
 *     │  • جامع قمامة تلقائي (GC) للتجريب السريع                          │
 *     │  • لا حاجة للتفكير في الملكية أثناء التطوير                        │
 *     │  • مثالي للـ REPL والاختبار السريع                                 │
 *     │  • رسائل تعليمية تُوضِّح كيفية تحويل الكود للملكية                 │
 *     └─────────────────────────────────────────────────────────────────────┘
 *
 *     ┌─────────────────────────────────────────────────────────────────────┐
 *     │                     وضع الإنتاج (--إنتاج)                           │
 *     ├─────────────────────────────────────────────────────────────────────┤
 *     │  • نظام ملكية صارم مثل Rust (صفر overhead)                         │
 *     │  • فحص الاستعارة في وقت الترجمة                                   │
 *     │  • أداء أقصى بدون GC                                              │
 *     │  • للإصدارات النهائية والأنظمة الحرجة                              │
 *     └─────────────────────────────────────────────────────────────────────┘
 *
 *     ميزة ص الفريدة: المترجم يُحلِّل كود وضع التطوير ويقترح
 *     تحويلات تلقائية للملكية! أسهل لغة أنظمة للتعلم.
 *
 * =============================================================================
 *                              أمثلة الاستخدام
 * =============================================================================
 *
 *     // في ملف الكود: تحديد الوضع
 *     #[وضع_ذاكرة: تطوير]     // أو: إنتاج
 *
 *     // من سطر الأوامر
 *     ص بناء --تطوير مشروعي.ص    // وضع GC السهل
 *     ص بناء --إنتاج مشروعي.ص    // وضع الملكية الصارم
 *
 *     // المترجم يُظهر اقتراحات التحويل
 *     تحذير: المتغير 'بيانات' يمكن تحويله للملكية:
 *       الأصل:    بيانات = قائمة_جديدة()
 *       المقترح:  دع بيانات: قائمة<عدد> = قائمة_جديدة()  // ملكية صريحة
 */

#ifndef SAD_MEMORY_GC_MODE_H
#define SAD_MEMORY_GC_MODE_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <functional>

namespace Sad
{
    namespace Memory
    {

        // =============================================================================
        //                    تعدادات وضع الذاكرة / Memory Mode Enums
        // =============================================================================

        /**
         * @enum MemoryMode
         * @brief وضع إدارة الذاكرة / Memory management mode
         */
        enum class MemoryMode
        {
            /// وضع التطوير: GC تلقائي، سهولة الاستخدام
            Development,

            /// وضع الإنتاج: ملكية صارمة، أداء أقصى
            Production,

            /// (AR) وضع مختلط — **مهجور**، يُعامَل كـ GCOnly عند الاستخدام
            /// (EN) Hybrid mode — **DEPRECATED**, treated as GCOnly when encountered
            Hybrid,

            /// اكتشاف تلقائي بناءً على السياق
            Auto
        };

        /**
         * @enum GCStrategy
         * @brief استراتيجية جامع القمامة / Garbage collector strategy
         */
        enum class GCStrategy
        {
            /// بدون GC (وضع الإنتاج)
            None,

            /// عد المراجع البسيط (Reference Counting)
            ReferenceCounting,

            /// عد المراجع الذري (للخيوط المتعددة)
            AtomicReferenceCounting,

            /// Tracing GC (Mark & Sweep)
            Tracing,

            /// GC تدريجي (Incremental)
            Incremental
        };

        /**
         * @enum OwnershipMode
         * @brief مستوى صرامة نظام الملكية / Ownership strictness level
         */
        enum class OwnershipMode
        {
            /// بدون فحص ملكية (GC يدير كل شيء)
            Disabled,

            /// تحذيرات فقط (للتعلم)
            Warnings,

            /// صارم (أخطاء ترجمة)
            Strict,

            /// صارم جداً (كـ Rust)
            UltraStrict
        };

        // =============================================================================
        //                    إعدادات وضع الذاكرة / Memory Mode Settings
        // =============================================================================

        /**
         * @struct MemoryModeSettings
         * @brief إعدادات شاملة لوضع الذاكرة / Comprehensive memory mode settings
         */
        struct MemoryModeSettings
        {
            /// الوضع الرئيسي
            MemoryMode mode = MemoryMode::Auto;

            /// استراتيجية GC (إذا كان مُفعَّلاً)
            GCStrategy gcStrategy = GCStrategy::ReferenceCounting;

            /// مستوى صرامة الملكية
            OwnershipMode ownershipMode = OwnershipMode::Warnings;

            /// تفعيل اقتراحات التحويل للملكية
            bool enableOwnershipSuggestions = true;

            /// تفعيل كاشف الدورات المرجعية
            bool enableCycleDetection = true;

            /// حد الذاكرة للـ GC (بالميغابايت)
            size_t gcMemoryLimitMB = 256;

            /// تفعيل التصحيح المُفصَّل
            bool debugMode = false;

            /// تفعيل الرسائل التعليمية
            bool teacherMode = true;

            // =========================================================================
            //                    الإعدادات الافتراضية / Default Settings
            // =========================================================================

            /**
             * @brief (AR) إعدادات وضع GC الافتراضي (--gc) — يحل محل --dev المهجور
             * @brief (EN) Default GC mode settings (--gc) — replaces deprecated --dev
             */
            static MemoryModeSettings gcDefaults()
            {
                MemoryModeSettings settings;
                settings.mode = MemoryMode::Development;
                settings.gcStrategy = GCStrategy::Tracing;
                settings.ownershipMode = OwnershipMode::Disabled;
                settings.enableOwnershipSuggestions = false;
                settings.enableCycleDetection = true;
                settings.debugMode = false;
                settings.teacherMode = false;
                return settings;
            }

            /**
             * @brief إعدادات وضع التطوير / Development mode settings
             */
            static MemoryModeSettings developmentDefaults()
            {
                MemoryModeSettings settings;
                settings.mode = MemoryMode::Development;
                settings.gcStrategy = GCStrategy::ReferenceCounting;
                settings.ownershipMode = OwnershipMode::Warnings;
                settings.enableOwnershipSuggestions = true;
                settings.enableCycleDetection = true;
                settings.debugMode = true;
                settings.teacherMode = true;
                return settings;
            }

            /**
             * @brief إعدادات وضع الإنتاج / Production mode settings
             */
            static MemoryModeSettings productionDefaults()
            {
                MemoryModeSettings settings;
                settings.mode = MemoryMode::Production;
                settings.gcStrategy = GCStrategy::None;
                settings.ownershipMode = OwnershipMode::UltraStrict;
                settings.enableOwnershipSuggestions = false;
                settings.enableCycleDetection = false; // لا حاجة مع الملكية
                settings.debugMode = false;
                settings.teacherMode = false;
                return settings;
            }

            /**
             * @brief إعدادات وضع التعلم / Learning mode settings
             */
            static MemoryModeSettings learningDefaults()
            {
                MemoryModeSettings settings;
                settings.mode = MemoryMode::Development;
                settings.gcStrategy = GCStrategy::ReferenceCounting;
                settings.ownershipMode = OwnershipMode::Warnings;
                settings.enableOwnershipSuggestions = true;
                settings.enableCycleDetection = true;
                settings.debugMode = true;
                settings.teacherMode = true; // رسائل تعليمية مُفصَّلة
                return settings;
            }

            /**
             * @brief إعدادات وضع النواة / Kernel (no_std) mode settings
             *
             *     عند تفعيل #![بلا_مكتبة_قياسية]:
             *     - لا GC (ملكية صرفة)
             *     - ملكية صارمة جداً (كـ Rust)
             *     - لا اقتراحات (المبرمج يعرف ما يفعل)
             *     - لا كشف دورات (لا GC أصلاً)
             */
            static MemoryModeSettings kernelDefaults()
            {
                MemoryModeSettings settings;
                settings.mode = MemoryMode::Production;
                settings.gcStrategy = GCStrategy::None;
                settings.ownershipMode = OwnershipMode::UltraStrict;
                settings.enableOwnershipSuggestions = false;
                settings.enableCycleDetection = false;
                settings.gcMemoryLimitMB = 0; // لا GC في النواة
                settings.debugMode = false;
                settings.teacherMode = false;
                return settings;
            }
        };

        // =============================================================================
        //                    مدير وضع الذاكرة / Memory Mode Manager
        // =============================================================================

        /**
         * @class MemoryModeManager
         * @brief المدير الرئيسي لأوضاع الذاكرة / Main memory mode manager
         *
         *     يُدير التبديل بين أوضاع الذاكرة المختلفة ويُوفِّر:
         *     - اكتشاف تلقائي للوضع المناسب
         *     - اقتراحات تحويل من GC للملكية
         *     - رسائل تعليمية للمبتدئين
         */
        class MemoryModeManager
        {
        public:
            // =========================================================================
            //                    البناء والتهيئة / Construction
            // =========================================================================

            /**
             * @brief المنشئ الافتراضي / Default constructor
             */
            MemoryModeManager();

            /**
             * @brief المنشئ بإعدادات مخصصة / Constructor with custom settings
             */
            explicit MemoryModeManager(const MemoryModeSettings &settings);

            ~MemoryModeManager();

            // =========================================================================
            //                    تهيئة الوضع / Mode Configuration
            // =========================================================================

            /**
             * @brief تعيين الوضع من سطر الأوامر
             * @param flag العلم ("--gc" أو "--إنتاج" أو "--تعلم")
             * @return نجاح التعيين
             *
             *     مثال:
             *         manager.setModeFromFlag("--gc");
             *         manager.setModeFromFlag("--production");
             */
            bool setModeFromFlag(const std::string &flag);

            /**
             * @brief تعيين الوضع من سمة في الكود
             * @param attribute السمة ("#[وضع_ذاكرة: تطوير]")
             * @return نجاح التعيين
             */
            bool setModeFromAttribute(const std::string &attribute);

            /**
             * @brief تعيين الإعدادات مباشرة
             */
            void setSettings(const MemoryModeSettings &settings);

            /**
             * @brief الحصول على الإعدادات الحالية
             */
            const MemoryModeSettings &getSettings() const;

            // =========================================================================
            //                    الاستعلام / Querying
            // =========================================================================

            /**
             * @brief هل نحن في وضع التطوير؟
             */
            bool isDevelopmentMode() const;

            /**
             * @brief هل نحن في وضع الإنتاج؟
             */
            bool isProductionMode() const;

            /**
             * @brief هل GC مُفعَّل؟
             */
            bool isGCEnabled() const;

            /**
             * @brief هل فحص الملكية مُفعَّل؟
             */
            bool isOwnershipCheckingEnabled() const;

            /**
             * @brief هل اقتراحات التحويل مُفعَّلة؟
             */
            bool areSuggestionsEnabled() const;

            /**
             * @brief هل نحن في وضع النواة (بلا مكتبة قياسية)؟
             */
            bool isNoStdMode() const;

            // =========================================================================
            //                    فرض وضع النواة / No-std Enforcement
            // =========================================================================

            /**
             * @brief فرض وضع الملكية الصارمة لوضع بلا مكتبة قياسية
             *
             *     يُفعَّل تلقائياً عند #![بلا_مكتبة_قياسية]:
             *     - GCStrategy::None (بلا جامع قمامة)
             *     - OwnershipMode::UltraStrict (ملكية صارمة جداً)
             *     - لا كشف دورات، لا اقتراحات
             */
            void enforceNoStdMode();

            /**
             * @brief التحقق من توافق الإعدادات مع وضع no_std
             * @return رسالة خطأ إذا كان هناك تعارض، أو سلسلة فارغة
             *
             *     مثال تعارض: --gc=refcount مع --بلا-مكتبة-قياسية
             */
            std::string validateNoStdCompatibility() const;

            // =========================================================================
            //                    الأسماء العربية / Arabic Names
            // =========================================================================

            /**
             * @brief الحصول على اسم الوضع بالعربية
             */
            std::string getModeNameArabic() const;

            /**
             * @brief الحصول على وصف الوضع بالعربية
             */
            std::string getModeDescriptionArabic() const;

            /**
             * @brief الحصول على اسم استراتيجية GC بالعربية
             */
            std::string getGCStrategyNameArabic() const;

            // =========================================================================
            //                    التنقيح / Debugging
            // =========================================================================

            /**
             * @brief طباعة معلومات الوضع الحالي
             */
            void dump() const;

        private:
            MemoryModeSettings settings_;
            bool noStdMode_ = false; // وضع بلا مكتبة قياسية مُفعّل

            /**
             * @brief تحليل علم سطر الأوامر
             */
            std::optional<MemoryMode> parseCommandLineFlag(const std::string &flag) const;

            /**
             * @brief تحليل سمة الكود
             */
            std::optional<MemoryMode> parseCodeAttribute(const std::string &attribute) const;
        };

        // =============================================================================
        //                    الدوال المساعدة / Helper Functions
        // =============================================================================

        /**
         * @brief تحويل وضع الذاكرة لنص عربي
         */
        std::string memoryModeToArabic(MemoryMode mode);

        /**
         * @brief تحويل وضع الذاكرة لنص إنجليزي
         */
        std::string memoryModeToEnglish(MemoryMode mode);

        /**
         * @brief تحويل استراتيجية GC لنص عربي
         */
        std::string gcStrategyToArabic(GCStrategy strategy);

        /**
         * @brief تحويل مستوى الملكية لنص عربي
         */
        std::string ownershipModeToArabic(OwnershipMode mode);

    } // namespace Memory
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_MEMORY_GC_MODE_H
