/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: ui_bridge.h
 * المسار: interpreter/src/ui/ui_bridge.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * الوصف:
 * ------
 * جسر الربط بين المفسر ونظام واجهات ص (SadUI).
 *
 * يُحوّل شجرة العناصر من كائنات Value إلى شجرة IRNode،
 * ثم يُطلق نافذة سطح المكتب لعرض الواجهة.
 *
 * الاستخدام:
 * ──────────
 *   // من داخل _محرك_واجهات()
 *   UIBridge bridge;
 *   bridge.run(rootWidget, interpreter);
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_INTERPRETER_UI_BRIDGE_H
#define SAD_INTERPRETER_UI_BRIDGE_H

#include "value.h"
#include "sad_ui/ir.h"
#include "sad_ui/node.h"              // for Color
#include "sad_ui/state.h"             // نظام إدارة الحالة التفاعلية
#include "sad_ui/animation.h"         // محرك الحركات الموحّد
#include "sad_ui/reconciler.h"        // نظام المطابقة الذكية (Virtual DOM)
#include "sad_ui/nav.h"               // (توحيد كامل، LOW-1) مكدّس التنقّل المشترك — المصدر الوحيد
#include "sad_ui/platform_renderer.h" // PlatformWindow — الواجهة المجردة لنوافذ جميع المنصات

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <vector>
#include <mutex>
#include <chrono>

// (AR) نظام إعادة التحميل الحي / (EN) Hot Reload system
#include "hot_reload_engine.h"

namespace Sad
{
    namespace Interpreter
    {

        // تصريح أمامي
        class Interpreter;
        class WidgetBuilder;

        // ─── جسر التوحيد الكامل (Amelia LOW-1): مساعدات nav المكشوفة لـUIBridge ───
        // (AR) مُعرَّفتان في ui_core_builtins.cpp (تفويضٌ لمساعدات nav الداخليّة). تُمكّنان
        //      UIBridge من الرسم الحيّ من مكدّس sad::ui::nav (المصدر الوحيد) بلا مكدّسٍ مُوازٍ.
        /// الصفحة الحاليّة من nav ⇒ Data::Value (يستدعي البانِي إن وُجد ⇒ تفاعليّة).
        [[nodiscard]] Data::Value navCurrentPageValue();
        /// ابذر جذر التطبيق في nav (بانٍ إن دالّة، لقطة إن عنصر) — يُستدعى من UIBridge::run.
        void navSeedRoot(const Data::Value &root, Interpreter *interp);

        /**
         * @brief جسر الربط بين المفسر ونظام SadUI
         *
         * يقوم بـ:
         * 1. تحويل شجرة Widget (من Value) إلى شجرة IRNode
         * 2. إنشاء نافذة SDL2 وعرض الواجهة
         * 3. معالجة الأحداث وإعادتها للمفسر
         * 4. إدارة التفاعل (Hover, Focus, State)
         */
        class UIBridge
        {
        public:
            UIBridge();
            ~UIBridge();

            /// الحصول على المثيل النشط (للدوال المدمجة)
            static UIBridge *active() { return activeInstance_; }

            /**
             * @brief تشغيل الواجهة
             */
            bool run(const Data::Value &rootWidget, Interpreter *interpreter);

            /**
             * @brief تحويل شجرة Value إلى شجرة IRNode
             */
            std::shared_ptr<sad::ui::IRNode> convertToIR(const Data::Value &widget);

            /**
             * @brief تسجيل handler حدث (دالة لغة ص)
             * @param handler الدالة المعالجة
             * @param stableId معرّف مستقر (nodeId:eventType) — إن كان فارغاً يُولّد تلقائياً
             * @return معرّف فريد للـ handler
             */
            std::string registerHandler(const Data::Value &handler,
                                        const std::string &stableId = "");

            /**
             * @brief تسجيل handler حدث مع ربطه بكائن مالك (للحفاظ على سياق هذا)
             *
             * (AR) عندما يُسجّل handler من داخل بناء() لمكون واجهة، نحفظ مرجع
             *      الكائن المالك حتى عند تنفيذ الحدث يُعرَّف هذا ومتغيرات الحقول.
             * (EN) When registering a handler from inside a UI component's بناء(),
             *      we store the owner object reference so that `هذا` and fields
             *      are defined when the handler executes.
             *
             * @param handler الدالة المعالجة
             * @param owner الكائن المالك (مكون الواجهة)
             * @param stableId معرّف مستقر (nodeId:eventType) — إن كان فارغاً يُولّد تلقائياً
             * @return معرّف فريد للـ handler
             */
            std::string registerHandler(const Data::Value &handler,
                                        Data::ObjectInstance *owner,
                                        const std::string &stableId = "");

            /**
             * @brief استدعاء handler مسجّل بالمعرّف
             */
            Data::Value invokeHandler(const std::string &handlerId,
                                      const std::vector<Data::Value> &args = {});

            /**
             * @brief إعادة بناء شجرة IR من الـ widget الجذري
             *
             * يُستدعى بعد تنفيذ أي handler لأن حالة الـ widget قد تكون تغيرت.
             * يُعيد تحويل شجرة الـ widget بالكامل ويُحدّث محتوى النافذة.
             */
            void rebuildUI();

            // ─── نظام التنقل (Navigation Stack) ──────

            /**
             * @brief الانتقال إلى صفحة جديدة (push)
             * @param page عنصر الواجهة للصفحة الجديدة
             */
            void navigateTo(const Data::Value &page);

            /**
             * @brief (AR) الانتقال إلى صفحة جديدة مع تحريك انتقالي بصري
             * @brief (EN) Navigate to new page with visual page transition animation
             * @param page عنصر الواجهة للصفحة الجديدة
             * @param transitionType اسم نوع الانتقال (مثل "ظهور"، "انزلاق_يمين")
             * @param durationSec مدة الانتقال بالثواني
             */
            void navigateWithTransition(const Data::Value &page,
                                        const std::string &transitionType,
                                        float durationSec = 0.3f);

            /**
             * @brief (AR) الانتقال مع تحريك خروج + دخول (Exit + Entry Transition)
             * @brief (EN) Navigate with exit + entry page transition
             * @param page عنصر الصفحة الجديدة
             * @param entryTransition نوع تحريك الدخول (على الصفحة الجديدة)
             * @param exitTransition نوع تحريك الخروج (على الصفحة الحالية)
             * @param durationSec مدة كل تحريك بالثواني
             */
            void navigateWithExitTransition(const Data::Value &page,
                                            const std::string &entryTransition,
                                            const std::string &exitTransition,
                                            float durationSec = 0.3f);

            /**
             * @brief (AR) العودة للصفحة السابقة مع تحريك انتقالي
             * @brief (EN) Navigate back with visual page transition
             * @param transitionType نوع التحريك (مثل "ظهور"، "انزلاق_يسار")
             * @param durationSec مدة الانتقال بالثواني
             * @return true إذا كان هناك صفحة للعودة إليها
             */
            bool navigateBackWithTransition(const std::string &transitionType,
                                            float durationSec = 0.3f);

            /**
             * @brief العودة للصفحة السابقة (pop)
             * @return true إذا كان هناك صفحة للعودة إليها
             */
            bool navigateBack();

            /**
             * @brief العودة للصفحة الرئيسية (pop to root)
             */
            void navigateToRoot();

            /**
             * @brief استبدال الصفحة الحالية بصفحة جديدة
             */
            void replacePage(const Data::Value &page);

            /**
             * @brief عدد الصفحات في كومة التنقل
             */
            // (توحيد كامل، LOW-1) العمق من مكدّس nav المكتبيّ (المصدر الوحيد) لا مكدّسٍ مُوازٍ.
            [[nodiscard]] size_t getNavigationDepth() const { return sad::ui::nav().depth(); }

            /**
             * @brief عرض رسالة منبثقة (SnackBar) لمدة محددة
             * @param message نص الرسالة
             * @param durationMs مدة العرض بالمللي ثانية
             */
            void showSnackbar(const std::string &message, int durationMs = 3000);

            /**
             * @brief إغلاق النافذة وإنهاء التطبيق
             */
            void closeWindow();

            /**
             * @brief تعيين عنوان النافذة
             */
            void setWindowTitle(const std::string &title);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام الحالة التفاعلية (Reactive State System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief الحصول على مخزن الحالة المركزي
             */
            sad::ui::StateStore &getStateStore() { return stateStore_; }

            /**
             * @brief هل تغيّرت الحالة منذ آخر إعادة بناء؟ (للتجميع الذكي)
             */
            bool isStateDirty() const { return stateChanged_; }

            /**
             * @brief تعليم أن الحالة تغيّرت (يُستدعى من عيّن_حالة)
             */
            void markStateDirty() { stateChanged_ = true; }

            /**
             * @brief الحصول على المفسر (لدوال المراقبة والمحسوبة)
             */
            Interpreter *getInterpreter() const { return interpreter_; }

            /**
             * @brief الاحتفاظ بمراقب حالة (لمنع انتهاء صلاحية weak_ptr)
             */
            void keepObserver(std::shared_ptr<sad::ui::StateObserver> observer)
            {
                stateObservers_.push_back(std::move(observer));
            }

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 1: المؤقتات (Timer System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief تنفيذ دالة بعد مدة محددة (مللي ثانية) — مرة واحدة
             * @return معرّف المؤقت لإمكانية الإلغاء
             */
            int scheduleTimeout(const Data::Value &callback, int delayMs);

            /**
             * @brief تنفيذ دالة كل مدة محددة (مللي ثانية) — تكراري
             * @return معرّف المؤقت لإمكانية الإلغاء
             */
            int scheduleInterval(const Data::Value &callback, int intervalMs);

            /**
             * @brief إلغاء مؤقت بمعرّفه
             */
            void cancelTimer(int timerId);

            /**
             * @brief تحديث المؤقتات — يُستدعى من حلقة الأحداث
             */
            void updateTimers();

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 2: الحافظة (Clipboard System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief نسخ نص إلى الحافظة
             */
            bool copyToClipboard(const std::string &text);

            /**
             * @brief لصق نص من الحافظة
             */
            std::string pasteFromClipboard();

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 3: الحوارات (Dialog System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief عرض حوار تنبيه بسيط (رسالة + زر موافق)
             */
            void showAlert(const std::string &title, const std::string &message);

            /**
             * @brief عرض حوار تأكيد (رسالة + موافق/إلغاء)
             * @return true إذا ضغط المستخدم موافق
             */
            bool showConfirm(const std::string &title, const std::string &message);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 4: التخزين المحلي (Local Storage)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief تخزين قيمة نصية بمفتاح
             */
            void storageSet(const std::string &key, const std::string &value);

            /**
             * @brief استرجاع قيمة مخزنة بمفتاح
             */
            std::string storageGet(const std::string &key);

            /**
             * @brief حذف قيمة مخزنة بمفتاح
             */
            void storageRemove(const std::string &key);

            /**
             * @brief مسح جميع القيم المخزنة
             */
            void storageClear();

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 5: الشاشة الكاملة (Fullscreen System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief تبديل وضع الشاشة الكاملة
             */
            void toggleFullscreen();

            /**
             * @brief هل النافذة في وضع الشاشة الكاملة؟
             */
            bool isFullscreen() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 6: معلومات النافذة (Window Info System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief الحصول على أبعاد النافذة {عرض، ارتفاع}
             */
            std::pair<int, int> getWindowSize() const;

            /**
             * @brief الحصول على موقع الفأرة {س، ص}
             */
            std::pair<int, int> getMousePosition() const;

            /**
             * @brief الحصول على كثافة الشاشة (DPI scale)
             */
            float getDisplayScale() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 7: الإشعارات (Notification System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief إظهار إشعار نظام (OS notification)
             */
            void showNotification(const std::string &title, const std::string &body);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 8: الصوت (Audio System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief تشغيل ملف صوتي WAV عبر SDL2 Audio
             * @return معرّف الصوت لإمكانية الإيقاف
             */
            int playSound(const std::string &filePath, bool loop = false);

            /**
             * @brief إيقاف صوت بمعرّفه
             */
            void stopSound(int soundId);

            /**
             * @brief إيقاف جميع الأصوات
             */
            void stopAllSounds();

            /**
             * @brief ضبط مستوى الصوت العام (0.0 - 1.0)
             */
            void setVolume(float volume);

            /**
             * @brief الحصول على مستوى الصوت الحالي
             */
            float getVolume() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 9: سحب وإفلات الملفات (Drag & Drop)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief تسجيل دالة لمعالجة الملفات المُسقطة على النافذة
             */
            void setDropHandler(const Data::Value &handler);

            /**
             * @brief معالجة حدث إفلات ملف (يُستدعى من النافذة)
             */
            void handleFileDrop(const std::string &filePath);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 10: لقطة الشاشة (Screenshot System)
            // ═══════════════════════════════════════════════════════════════════════════

            /**
             * @brief التقاط لقطة شاشة وحفظها كملف PNG
             */
            bool takeScreenshot(const std::string &outputPath);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 11: معلومات الجهاز (Device Info)
            // ═══════════════════════════════════════════════════════════════════════════

            std::string getOSName() const;
            int getCPUCount() const;
            int getRAMSize() const;
            std::string getLanguage() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 12: المؤشر (Cursor)
            // ═══════════════════════════════════════════════════════════════════════════

            void setCursorStyle(const std::string &style);
            void showCursor(bool visible);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 13: فتح رابط خارجي (Open URL)
            // ═══════════════════════════════════════════════════════════════════════════

            void openURL(const std::string &url);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 14: وقت النظام (System Time)
            // ═══════════════════════════════════════════════════════════════════════════

            double getTimestamp() const;
            std::string getDateTime() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 15: الملفات (File I/O)
            // ═══════════════════════════════════════════════════════════════════════════

            std::string readTextFile(const std::string &path);
            bool writeTextFile(const std::string &path, const std::string &content);
            bool fileExists(const std::string &path);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 16: اختيار ملف (File Picker)
            // ═══════════════════════════════════════════════════════════════════════════

            std::string openFilePicker(const std::string &filter = "");
            std::string saveFilePicker(const std::string &defaultName = "");

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 17: الحافظة المتقدمة (Advanced Clipboard)
            // ═══════════════════════════════════════════════════════════════════════════

            bool hasClipboardText() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 18: اهتزاز (Vibration/Haptic)
            // ═══════════════════════════════════════════════════════════════════════════

            void vibrate(int durationMs = 100);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 19: سجل التطبيق (Application Log)
            // ═══════════════════════════════════════════════════════════════════════════

            void logMessage(const std::string &level, const std::string &message);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 20: التخزين المحلي — حفظ/تحميل من القرص
            // ═══════════════════════════════════════════════════════════════════════════

            void loadStorageFromDisk();
            void saveStorageToDisk();

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 21: الرسوم المتحركة — محرك موحّد لجميع المنصات
            //  (Animation System — Unified Cross-Platform Engine)
            // ═══════════════════════════════════════════════════════════════════════════

            /// بدء تحريكة رقمية (عرض، ارتفاع، شفافية، ...)
            int startAnimation(const std::string &target, const std::string &property,
                               double from, double to, int durationMs, const std::string &easing = "linear");

            /// بدء تحريكة لون (من لون إلى لون)
            int startColorAnimation(const std::string &target, const std::string &property,
                                    float fromR, float fromG, float fromB, float fromA,
                                    float toR, float toG, float toB, float toA,
                                    int durationMs, const std::string &easing = "linear");

            /// إيقاف تحريكة بمعرّف
            void stopAnimation(int animId);

            /// إيقاف كل تحريكات عقدة
            void stopNodeAnimations(const std::string &target);

            /// إيقاف كل التحريكات
            void stopAllAnimations();

            /// استعلام حالة تحريكة
            std::string getAnimationState(int animId) const;

            /// الحصول على القيمة الحالية لخاصية متحركة
            double getAnimatedValue(const std::string &target, const std::string &property, double defaultValue) const;

            /// هل توجد تحريكات نشطة؟
            bool hasActiveAnimations() const;

            /// تحديث جميع التحريكات — يستدعى في كل إطار
            bool updateAnimations();

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 22: الشبكة/الإنترنت (Network System)
            // ═══════════════════════════════════════════════════════════════════════════

            std::string httpRequest(const std::string &method, const std::string &url,
                                    const std::string &body = "", const std::string &headers = "");
            bool isOnline() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 23: JSON
            // ═══════════════════════════════════════════════════════════════════════════

            std::string jsonParse(const std::string &jsonStr, const std::string &key);
            std::string jsonStringify(const std::vector<std::pair<std::string, std::string>> &pairs);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 24: التشفير (Crypto System)
            // ═══════════════════════════════════════════════════════════════════════════

            std::string encryptText(const std::string &text, const std::string &key);
            std::string decryptText(const std::string &encrypted, const std::string &key);
            std::string hashText(const std::string &text, const std::string &algorithm = "sha256");

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 25: قاعدة بيانات مفتاح/قيمة (Database System)
            // ═══════════════════════════════════════════════════════════════════════════

            bool dbOpen(const std::string &dbName);
            std::string dbExecute(const std::string &query);
            void dbClose();

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 26: الموقع الجغرافي (Geolocation)
            // ═══════════════════════════════════════════════════════════════════════════

            std::pair<double, double> getLocation() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 27: الكاميرا (Camera System)
            // ═══════════════════════════════════════════════════════════════════════════

            bool openCamera();
            std::string capturePhoto(const std::string &outputPath = "");

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 28: المستشعرات (Sensors System)
            // ═══════════════════════════════════════════════════════════════════════════

            std::string readSensor(const std::string &sensorType) const;
            std::string availableSensors() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 29: المشاركة (Share System)
            // ═══════════════════════════════════════════════════════════════════════════

            bool shareText(const std::string &text, const std::string &title = "");
            bool shareFile(const std::string &filePath, const std::string &mimeType = "");

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 30: الأذونات (Permissions System)
            // ═══════════════════════════════════════════════════════════════════════════

            std::string requestPermission(const std::string &permission);
            bool checkPermission(const std::string &permission) const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 31: السمات (Themes System)
            // ═══════════════════════════════════════════════════════════════════════════

            void setTheme(const std::string &themeName);
            std::string getTheme() const;
            std::string getSystemTheme() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 32: التدويل (Internationalization / i18n)
            // ═══════════════════════════════════════════════════════════════════════════

            std::string translate(const std::string &key) const;
            void setLocale(const std::string &locale);
            std::string getLocale() const;
            void addTranslation(const std::string &key, const std::string &value);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 33: الوصولية (Accessibility)
            // ═══════════════════════════════════════════════════════════════════════════

            void screenReaderAnnounce(const std::string &text);
            double getSystemFontScale() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 34: التخطيط المتجاوب (Responsive Layout)
            // ═══════════════════════════════════════════════════════════════════════════

            int getScreenWidth() const;
            int getScreenHeight() const;
            double getPixelDensity() const;
            std::string getDeviceType() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 35: الطباعة (Print System)
            // ═══════════════════════════════════════════════════════════════════════════

            bool printPage(const std::string &content, const std::string &title = "");

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 36: البيانات الحيوية (Biometrics)
            // ═══════════════════════════════════════════════════════════════════════════

            bool authenticateBiometric(const std::string &reason = "");
            bool hasBiometricSupport() const;

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 37: الإيماءات المتقدمة (Advanced Gestures)
            // ═══════════════════════════════════════════════════════════════════════════

            void setSwipeHandler(const Data::Value &handler);
            void setPinchHandler(const Data::Value &handler);
            void setRotateHandler(const Data::Value &handler);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 38: الخرائط (Maps System)
            // ═══════════════════════════════════════════════════════════════════════════

            int createMap(double lat, double lng, int zoom);
            void addMapMarker(int mapId, double lat, double lng, const std::string &label);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 39: الرسم الحر (Canvas/Drawing)
            // ═══════════════════════════════════════════════════════════════════════════

            int createCanvas(int width, int height);
            void drawLine(int canvasId, int x1, int y1, int x2, int y2, const std::string &color, int thickness = 1);
            void drawCircle(int canvasId, int cx, int cy, int radius, const std::string &color, bool fill = true);
            void drawRect(int canvasId, int x, int y, int w, int h, const std::string &color, bool fill = true);
            void clearCanvas(int canvasId);

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام 40: البطارية (Battery System)
            // ═══════════════════════════════════════════════════════════════════════════

            int getBatteryLevel() const;
            bool isBatteryCharging() const;

        private:
            /// (AR) تحويل عقدة واحدة (تكراري)
            std::shared_ptr<sad::ui::IRNode> convertNodeToIR(
                const Data::Value &widget,
                int depth = 0);

            /// (AR) تسجيل أحداث WidgetBuilder المخزنة كحقول في eventHandlers_
            void registerWidgetBuilderEvents(
                const std::shared_ptr<sad::ui::IRNode> &irNode,
                Sad::Interpreter::WidgetBuilder *wb);

            /// تحويل نوع العنصر (نص → UINodeType::Text)
            sad::ui::UINodeType stringToNodeType(const std::string &typeName);

            /// تحويل اللون من نص إلى Color
            sad::ui::Color parseColor(const std::string &colorStr);

            /// معالج الأحداث من النافذة
            void handleEvent(
                sad::ui::IREventType eventType,
                const std::string &handlerId,
                const sad::ui::IRNode *node,
                const sad::ui::EventData &eventData = sad::ui::EventData{});

            /// استخراج خصائص الأحداث وتحويلها إلى IREvent
            void extractEventProperties(
                const std::string &key,
                const Data::Value &val,
                std::shared_ptr<sad::ui::IRNode> &irNode);

            /// هل هذه الخاصية اسم حدث؟
            bool isEventProperty(const std::string &key) const;

            Interpreter *interpreter_ = nullptr;

            // ═══════════════════════════════════════════════════════════════════
            //  نظام إدارة الأحداث الذكي (Smart Event Handler System)
            //  ────────────────────────────────────────────────────────────
            //  (AR) يستخدم معرّفات مستقرة مبنية على هوية العقدة ونوع الحدث
            //       بدلاً من عداد يتصاعد ويُمسح كل إعادة بناء.
            //       نظام الأجيال يتتبع أي المعالجات ما زالت حيّة.
            //  (EN) Uses stable IDs based on node identity + event type
            //       instead of a counter that resets every rebuild.
            //       Generation system tracks which handlers are still alive.
            // ═══════════════════════════════════════════════════════════════════

            /// (AR) سجل معالج حدث واحد — يحمل الدالة والمالك ورقم الجيل
            /// (EN) Single event handler entry — holds callback, owner, and generation
            struct HandlerEntry
            {
                Data::Value handler;                   ///< دالة المعالج
                Data::ObjectInstance *owner = nullptr; ///< الكائن المالك (اختياري) — مؤشر خام مُدار بـGC
                uint64_t generation = 0;               ///< رقم الجيل عند التسجيل
            };

            /// تخزين handlers الأحداث (معرّف مستقر → سجل المعالج)
            std::unordered_map<std::string, HandlerEntry> eventHandlers_;

            /// (AR) تخزين الكائن المالك لكل handler — لتنفيذ الأحداث في سياق هذا
            /// (EN) Owner object for each handler — to execute events in هذا context
            std::unordered_map<std::string, Data::ObjectInstance *> handlerOwners_;

            /// عدّاد لتوليد معرّفات فريدة (احتياطي للحالات غير المعروفة)
            std::atomic<uint64_t> handlerCounter_{0};

            /// (AR) رقم الجيل الحالي — يزداد مع كل إعادة بناء
            /// (EN) Current generation number — increments with each rebuild
            uint64_t handlerGeneration_{0};

            /// (AR) عنصر الجذر الابتدائيّ (محتوى run الأوّل + احتياطُ rebuildUI الآمن).
            ///      الرسم الحيّ يبني من sad::ui::nav (navCurrentPageValue) لا من هذا.
            Data::Value rootWidget_;

            // (Amelia LOW-2) أُزيل builderFunc_: بعد التوحيد الكامل صار الرسم من مكدّس
            //   sad::ui::nav المبذور (navCurrentPageValue يستدعي بانِي الصفحة الحاليّة)، فلم
            //   يبقَ لباني الجذر المنفصل قارئ. كان write-only ووثيقةً مضلِّلة.

            /// مؤشر للنافذة (لتحديث المحتوى عند إعادة البناء)
            /// (AR) نستخدم PlatformWindow* بدلاً من void* لدعم جميع المنصات
            /// (EN) Using PlatformWindow* instead of void* to support all platforms
            sad::ui::PlatformWindow *activeWindow_ = nullptr;

            // (توحيد كامل، Amelia LOW-1) أُزيل المكدّس المُوازي navigationStack_: مكدّس
            //   sad::ui::nav المكتبيّ هو **المصدر الوحيد** (عمقًا وبنيةً ورسمًا حيًّا). كان
            //   يُحفَظ بالتوازي بلقطات Data::Value ويُتزامَن بالاتّفاق ⇒ عرضةٌ لانحرافٍ صامت.

            /// (AR) نوع الانتقال المُعلّق — يُستخدم في rebuildUI لتطبيق تحريك الصفحة
            /// (EN) Pending page transition type — used in rebuildUI for animated content switch
            std::string pendingTransitionType_;

            /// (AR) مدة الانتقال المُعلّق بالثواني
            /// (EN) Pending transition duration in seconds
            float pendingTransitionDuration_ = 0.3f;

            /// (AR) نوع تحريك الخروج المُعلّق
            /// (EN) Pending exit transition type
            std::string pendingExitTransitionType_;

            /// (AR) مدة تحريك الخروج المُعلّق بالثواني
            /// (EN) Pending exit transition duration in seconds
            float pendingExitTransitionDuration_ = 0.3f;

            /// المثيل النشط (للوصول من الدوال المدمجة)
            static UIBridge *activeInstance_;

            // ─── أعضاء الأنظمة الجديدة ────

            /// نظام المؤقتات
            struct TimerEntry
            {
                int id;
                Data::Value callback;
                uint32_t fireTimeMs; // متى يُطلَق (absolute tick)
                int intervalMs;      // 0 = مرة واحدة (timeout), >0 = تكراري
                bool cancelled = false;
            };
            std::vector<TimerEntry> timers_;
            std::atomic<int> nextTimerId_{1};

            /// نظام التخزين المحلي
            std::unordered_map<std::string, std::string> localStorage_;

            /// نظام الشاشة الكاملة
            bool isFullscreen_ = false;

            /// نظام سحب وإفلات الملفات
            Data::Value dropHandler_;

            /// نظام الصوت — تم نقل البيانات والتطبيق إلى PlatformWindow
            /// الدوال هنا تفوض إلى activeWindow_
            void initAudio();
            void cleanupAudio();
            static void audioCallback(void *userdata, uint8_t *stream, int len);

            /// مسار ملف التخزين المحلي على القرص
            std::string storagePath_;

            // ─── نظام الحالة التفاعلية ────

            /// مخزن الحالة المركزي
            sad::ui::StateStore stateStore_;

            /// علم التغيير الذكي — يُعيَّن عند تغيير حالة عبر عيّن_حالة()
            bool stateChanged_ = false;

            /// شجرة IR السابقة — لمقارنة الأشجار (Tree Diffing)
            std::shared_ptr<sad::ui::IRNode> previousIR_;

            /// (AR) نظام المطابقة الذكية — يُقارن ويُطبّق التغييرات الموضعية فقط
            /// (EN) Smart reconciler — diffs and patches only changed parts
            sad::ui::Reconciler reconciler_;

            /// مراقبو الحالة — نحتفظ بهم لمنع انتهاء صلاحية weak_ptr في StateStore
            std::vector<std::shared_ptr<sad::ui::StateObserver>> stateObservers_;

            /// مقارنة شجرتين IR لاكتشاف التغييرات البصرية
            static bool treesEqual(const std::shared_ptr<sad::ui::IRNode> &a,
                                   const std::shared_ptr<sad::ui::IRNode> &b);

            // ─── أعضاء الأنظمة 21-40 ────

            /// نظام 21: الرسوم المتحركة — محرك موحّد
            sad::ui::AnimationEngine animEngine_;
            /// وقت بداية المحرك (std::chrono — لا يعتمد على SDL أو أي منصة)
            std::chrono::steady_clock::time_point engineStartTime_ = std::chrono::steady_clock::now();
            /// عداد معرّفات التحريكات
            std::atomic<int> nextAnimId_{1};
            /// ربط معرّف التحريكة بمفتاح (nodeId:property)
            struct AnimIdEntry
            {
                std::string nodeId;
                std::string property;
                bool active = true;
            };
            std::unordered_map<int, AnimIdEntry> animIdMap_;

            /// نظام 25: قاعدة البيانات
            struct DBEntry
            {
                std::string name;
                std::unordered_map<std::string, std::string> data;
                bool open = false;
            };
            DBEntry currentDb_;

            /// نظام 31: السمات
            std::string currentTheme_ = "فاتح";

            /// نظام 32: التدويل
            std::string currentLocale_ = "ar";
            std::unordered_map<std::string, std::string> translations_;

            /// نظام 37: الإيماءات
            Data::Value swipeHandler_;
            Data::Value pinchHandler_;
            Data::Value rotateHandler_;

            /// نظام 38: الخرائط
            struct MapEntry
            {
                int id = 0;
                double lat = 0, lng = 0;
                int zoom = 10;
                struct Marker
                {
                    double lat, lng;
                    std::string label;
                };
                std::vector<Marker> markers;
            };
            std::vector<MapEntry> maps_;
            std::atomic<int> nextMapId_{1};

            /// نظام 39: الرسم الحر
            struct CanvasEntry
            {
                int id = 0;
                int width = 0, height = 0;
                std::vector<uint8_t> pixels;
            };
            std::vector<CanvasEntry> canvases_;
            std::atomic<int> nextCanvasId_{1};

            // ═══════════════════════════════════════════════════════════════════════════
            //  نظام إعادة التحميل الحي (Hot Reload System)
            // ═══════════════════════════════════════════════════════════════════════════
            std::unique_ptr<Sad::HotReload::HotReloadEngine> hotReloadEngine_;

        public:
            /**
             * @brief (AR) تفعيل نظام إعادة التحميل الحي
             * @brief (EN) Enable Hot Reload system
             *
             * @param sourceFilePath (AR) مسار الملف المصدري الرئيسي / (EN) Main source file path
             * @param options (AR) خيارات إعادة التحميل / (EN) Hot Reload options
             */
            void enableHotReload(const std::string &sourceFilePath,
                                 const Sad::HotReload::HotReloadOptions &options = {});

            /**
             * @brief (AR) فحص إعادة التحميل — يُستدعى من حلقة الأحداث
             * @brief (EN) Poll hot reload — called from event loop
             */
            void pollHotReload();

            /**
             * @brief (AR) هل نظام إعادة التحميل نشط؟
             * @brief (EN) Is hot reload active?
             */
            bool isHotReloadActive() const
            {
                return hotReloadEngine_ && hotReloadEngine_->isActive();
            }

            /// (AR) استرجاع آخر HTML مولّد (لـ WASM)
            /// (EN) Get last generated HTML (for WASM)
            static const std::string &getLastGeneratedHtml();
        };

    } // namespace Interpreter
} // namespace Sad

#endif // SAD_INTERPRETER_UI_BRIDGE_H
