/**
 * ملف: ui_bridge_platform.cpp
 * المسار: interpreter/src/ui/ui_bridge_platform.cpp
 *
 * الوصف: أنظمة المنصة — التخزين، الصوت، الملفات، الشبكة، الأجهزة — مُقتطع من ui_bridge.cpp
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * مرخص تحت رخصة MIT
 */

#include "ui_bridge.h"
#include "interpreter_core.h"

#include "sad_ui/types.h"
#include "sad_ui/ir.h"
#include "sad_ui/nav.h" // (HIGH-1) استُخدم مباشرةً في تصفير مكدّس التنقّل عند Hot Restart
#include "sad_ui/window_control.h" // (Amelia H1/L3) تصفير المتحكّم المشترك عند Hot Restart

#ifdef SAD_UI_USE_SDL2
#include <SDL.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace Sad
{
    namespace Interpreter
    {
        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 4: التخزين المحلي (Local Storage)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::storageSet(const std::string &key, const std::string &value)
        {
            localStorage_[key] = value;
            saveStorageToDisk();
        }

        std::string UIBridge::storageGet(const std::string &key)
        {
            auto it = localStorage_.find(key);
            if (it != localStorage_.end())
            {
                return it->second;
            }
            return "";
        }

        void UIBridge::storageRemove(const std::string &key)
        {
            localStorage_.erase(key);
            saveStorageToDisk();
        }

        void UIBridge::storageClear()
        {
            localStorage_.clear();
            saveStorageToDisk();
        }

        void UIBridge::loadStorageFromDisk()
        {
            if (storagePath_.empty())
            {
#ifdef _WIN32
                const char *appdata = std::getenv("APPDATA");
                storagePath_ = appdata ? std::string(appdata) + "\\sad_storage.dat" : "sad_storage.dat";
#else
                const char *home = std::getenv("HOME");
                storagePath_ = home ? std::string(home) + "/.sad_storage.dat" : "sad_storage.dat";
#endif
            }
            std::ifstream in(storagePath_);
            if (!in.is_open())
                return;

            localStorage_.clear();
            std::string line;
            while (std::getline(in, line))
            {
                auto sep = line.find('\t');
                if (sep != std::string::npos)
                {
                    std::string k = line.substr(0, sep);
                    std::string v = line.substr(sep + 1);
                    localStorage_[k] = v;
                }
            }
        }

        void UIBridge::saveStorageToDisk()
        {
            if (storagePath_.empty())
            {
#ifdef _WIN32
                const char *appdata = std::getenv("APPDATA");
                storagePath_ = appdata ? std::string(appdata) + "\\sad_storage.dat" : "sad_storage.dat";
#else
                const char *home = std::getenv("HOME");
                storagePath_ = home ? std::string(home) + "/.sad_storage.dat" : "sad_storage.dat";
#endif
            }
            std::ofstream out(storagePath_);
            if (!out.is_open())
                return;
            for (const auto &[k, v] : localStorage_)
            {
                out << k << '\t' << v << '\n';
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 5: الشاشة الكاملة (Fullscreen System)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::toggleFullscreen()
        {
            if (!activeWindow_)
                return;
            isFullscreen_ = !isFullscreen_;
            activeWindow_->setFullscreen(isFullscreen_);
        }

        bool UIBridge::isFullscreen() const
        {
            return isFullscreen_;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 6: معلومات النافذة (Window Info System)
        // ═══════════════════════════════════════════════════════════════════════════════

        std::pair<int, int> UIBridge::getWindowSize() const
        {
            if (!activeWindow_)
                return {0, 0};
            return {activeWindow_->getWidth(), activeWindow_->getHeight()};
        }

        std::pair<int, int> UIBridge::getMousePosition() const
        {
            if (!activeWindow_)
                return {0, 0};
            return activeWindow_->getMousePosition();
        }

        float UIBridge::getDisplayScale() const
        {
            if (!activeWindow_)
                return 1.0f;
            return activeWindow_->getDisplayScale();
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 7: الإشعارات (Notification System)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::showNotification(const std::string &title, const std::string &body)
        {
            // الإشعارات تُنفّذ كرسالة في الكونسول + SnackBar
            std::cout << "[\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1] " // [إشعار]
                      << title << ": " << body << std::endl;
            showSnackbar(title + " — " + body, 5000);
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 8: الصوت (Audio System) — SDL2 Audio API حقيقي
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::initAudio()
        {
            if (activeWindow_)
                activeWindow_->initAudio();
        }

        void UIBridge::cleanupAudio()
        {
            if (activeWindow_)
                activeWindow_->cleanupAudio();
        }

        void UIBridge::audioCallback(void *userdata, uint8_t *stream, int len)
        {
            // تم نقل التطبيق إلى DesktopWindow::audioCallback
            (void)userdata;
            (void)stream;
            (void)len;
        }

        int UIBridge::playSound(const std::string &filePath, bool loop)
        {
            if (activeWindow_)
                return activeWindow_->playSound(filePath, loop);
            return -1;
        }

        void UIBridge::stopSound(int soundId)
        {
            if (activeWindow_)
                activeWindow_->stopSound(soundId);
        }

        void UIBridge::stopAllSounds()
        {
            if (activeWindow_)
                activeWindow_->stopAllSounds();
        }

        void UIBridge::setVolume(float volume)
        {
            if (activeWindow_)
                activeWindow_->setVolume(volume);
        }

        float UIBridge::getVolume() const
        {
            if (activeWindow_)
                return activeWindow_->getVolume();
            return 1.0f;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 9: سحب وإفلات (Drag & Drop)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::setDropHandler(const Data::Value &handler)
        {
            dropHandler_ = handler;
        }

        void UIBridge::handleFileDrop(const std::string &filePath)
        {
            if (dropHandler_.isVoid() || !interpreter_)
                return;

            std::vector<Data::Value> args;
            args.push_back(Data::Value(filePath));

            if (dropHandler_.isFunction())
            {
                auto funcRef = dropHandler_.toFunction();
                if (funcRef)
                {
                    try
                    {
                        interpreter_->callUserFunction(funcRef->registeredName, args);
                        rebuildUI();
                    }
                    catch (const std::exception &e)
                    {
                        std::cerr << "\xd8\xae\xd8\xb7\xd8\xa3 \xd9\x81\xd9\x8a \xd9\x85\xd8\xb9\xd8\xa7\xd9\x84\xd8\xac\xd8\xa9 \xd8\xa7\xd9\x84\xd8\xa5\xd8\xb3\xd9\x82\xd8\xa7\xd8\xb7: " // خطأ في معالجة الإسقاط:
                                  << e.what() << std::endl;
                    }
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 10: لقطة الشاشة (Screenshot System)
        // ═══════════════════════════════════════════════════════════════════════════════

        bool UIBridge::takeScreenshot(const std::string &outputPath)
        {
            if (!activeWindow_)
                return false;
            return activeWindow_->takeScreenshot(outputPath);
        }

        std::string UIBridge::getOSName() const
        {
#ifdef _WIN32
            return "Windows";
#elif defined(__APPLE__)
            return "macOS";
#elif defined(__linux__)
            return "Linux";
#elif defined(__EMSCRIPTEN__)
            return "Web";
#elif defined(__ANDROID__)
            return "Android";
#else
            return "غير معروف";
#endif
        }

        int UIBridge::getCPUCount() const
        {
            if (activeWindow_)
                return activeWindow_->getCPUCount();
            return 1;
        }

        int UIBridge::getRAMSize() const
        {
            if (activeWindow_)
                return activeWindow_->getRAMSizeMB();
            return 0;
        }

        std::string UIBridge::getLanguage() const
        {
#ifdef _WIN32
            wchar_t buf[LOCALE_NAME_MAX_LENGTH];
            if (GetUserDefaultLocaleName(buf, LOCALE_NAME_MAX_LENGTH))
            {
                char narrow[64];
                WideCharToMultiByte(CP_UTF8, 0, buf, -1, narrow, 64, nullptr, nullptr);
                return std::string(narrow);
            }
            return "ar";
#else
            const char *lang = std::getenv("LANG");
            return lang ? std::string(lang) : "ar";
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 12: المؤشر (Cursor)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::setCursorStyle(const std::string &style)
        {
            if (activeWindow_)
                activeWindow_->setCursorStyle(style);
        }

        void UIBridge::showCursor(bool visible)
        {
            if (activeWindow_)
                activeWindow_->showCursor(visible);
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 13: فتح رابط خارجي (Open URL)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::openURL(const std::string &url)
        {
            // التحقق من أن الرابط يبدأ بـ http:// أو https:// للأمان
            if (url.substr(0, 7) != "http://" && url.substr(0, 8) != "https://")
            {
                std::cerr << "[\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7] \xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7 \xd8\xba\xd9\x8a\xd8\xb1 \xd8\xb5\xd8\xa7\xd9\x84\xd8\xad: "
                          << url << std::endl;
                return;
            }
#ifdef _WIN32
            ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
#elif defined(__APPLE__)
            std::string cmd = "open \"" + url + "\"";
            system(cmd.c_str());
#elif defined(__linux__)
            std::string cmd = "xdg-open \"" + url + "\"";
            system(cmd.c_str());
#elif defined(__EMSCRIPTEN__)
            EM_ASM({ window.open(UTF8ToString($0), '_blank'); }, url.c_str());
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 14: وقت النظام (System Time)
        // ═══════════════════════════════════════════════════════════════════════════════

        double UIBridge::getTimestamp() const
        {
            auto now = std::chrono::system_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          now.time_since_epoch())
                          .count();
            return static_cast<double>(ms);
        }

        std::string UIBridge::getDateTime() const
        {
            auto now = std::chrono::system_clock::now();
            std::time_t t = std::chrono::system_clock::to_time_t(now);
            std::tm tm_buf;
#ifdef _WIN32
            localtime_s(&tm_buf, &t);
#else
            localtime_r(&t, &tm_buf);
#endif
            char buf[64];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm_buf);
            return std::string(buf);
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 15: الملفات (File I/O)
        // ═══════════════════════════════════════════════════════════════════════════════

        std::string UIBridge::readTextFile(const std::string &path)
        {
            std::ifstream f(path, std::ios::in | std::ios::binary);
            if (!f.is_open())
                return "";
            std::ostringstream ss;
            ss << f.rdbuf();
            return ss.str();
        }

        bool UIBridge::writeTextFile(const std::string &path, const std::string &content)
        {
            std::ofstream f(path, std::ios::out | std::ios::binary);
            if (!f.is_open())
                return false;
            f << content;
            return f.good();
        }

        bool UIBridge::fileExists(const std::string &path)
        {
            std::ifstream f(path);
            return f.good();
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 16: اختيار ملف (File Picker) — Windows فقط حالياً
        // ═══════════════════════════════════════════════════════════════════════════════

        std::string UIBridge::openFilePicker(const std::string &filter)
        {
#ifdef _WIN32
            char filename[MAX_PATH] = "";
            OPENFILENAMEA ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = nullptr;
            ofn.lpstrFile = filename;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = filter.empty() ? "كل الملفات\0*.*\0" : filter.c_str();
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

            if (GetOpenFileNameA(&ofn))
            {
                return std::string(filename);
            }
#else
            (void)filter;
#endif
            return "";
        }

        std::string UIBridge::saveFilePicker(const std::string &defaultName)
        {
#ifdef _WIN32
            char filename[MAX_PATH] = "";
            if (!defaultName.empty())
            {
                strncpy_s(filename, defaultName.c_str(), MAX_PATH - 1);
            }
            OPENFILENAMEA ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = nullptr;
            ofn.lpstrFile = filename;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = "كل الملفات\0*.*\0";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

            if (GetSaveFileNameA(&ofn))
            {
                return std::string(filename);
            }
#else
            (void)defaultName;
#endif
            return "";
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 17: الحافظة المتقدمة
        // ═══════════════════════════════════════════════════════════════════════════════

        bool UIBridge::hasClipboardText() const
        {
            if (activeWindow_)
            {
                return !activeWindow_->pasteFromClipboard().empty();
            }
            return false;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 18: اهتزاز (Vibration/Haptic)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::vibrate(int durationMs)
        {
            if (activeWindow_)
            {
                activeWindow_->vibrate(durationMs);
                return;
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 19: سجل التطبيق (Application Log)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::logMessage(const std::string &level, const std::string &message)
        {
            auto timeStr = getDateTime();
            std::string prefix = "[" + timeStr + "] [" + level + "] ";
            std::cout << prefix << message << std::endl;

            // حفظ في ملف سجل
            std::string logPath;
#ifdef _WIN32
            const char *appdata = std::getenv("APPDATA");
            logPath = appdata ? std::string(appdata) + "\\sad_app.log" : "sad_app.log";
#else
            const char *home = std::getenv("HOME");
            logPath = home ? std::string(home) + "/.sad_app.log" : "sad_app.log";
#endif
            std::ofstream logFile(logPath, std::ios::app);
            if (logFile.is_open())
            {
                logFile << prefix << message << "\n";
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 21: الرسوم المتحركة (Animation System)
        // ═══════════════════════════════════════════════════════════════════════════════

        int UIBridge::startAnimation(const std::string &target, const std::string &property,
                                     double from, double to, int durationMs, const std::string &easing)
        {
            int id = nextAnimId_++;

            // حساب الوقت الحالي بالمللي ثانية (مستقل عن المنصة)
            auto now = std::chrono::steady_clock::now();
            uint32_t currentMs = static_cast<uint32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(now - engineStartTime_).count());

            // إنشاء تحريكة عبر المحرك الموحّد
            sad::ui::Animation anim;
            anim.targetNodeId = target;
            anim.propertyKey = property;
            anim.fromValue = from;
            anim.toValue = to;
            anim.durationMs = static_cast<uint32_t>(durationMs);
            anim.easing = sad::ui::easingFromString(easing);

            animEngine_.animate(anim, currentMs);

            // تخزين الربط بين المعرّف والمفتاح
            animIdMap_[id] = {target, property, true};

            return id;
        }

        int UIBridge::startColorAnimation(const std::string &target, const std::string &property,
                                          float fromR, float fromG, float fromB, float fromA,
                                          float toR, float toG, float toB, float toA,
                                          int durationMs, const std::string &easing)
        {
            int id = nextAnimId_++;

            auto now = std::chrono::steady_clock::now();
            uint32_t currentMs = static_cast<uint32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(now - engineStartTime_).count());

            sad::ui::ColorAnimation anim;
            anim.targetNodeId = target;
            anim.propertyKey = property;
            anim.fromR = fromR;
            anim.fromG = fromG;
            anim.fromB = fromB;
            anim.fromA = fromA;
            anim.toR = toR;
            anim.toG = toG;
            anim.toB = toB;
            anim.toA = toA;
            anim.durationMs = static_cast<uint32_t>(durationMs);
            anim.easing = sad::ui::easingFromString(easing);

            animEngine_.animateColor(anim, currentMs);
            animIdMap_[id] = {target, property, true};

            return id;
        }

        void UIBridge::stopAnimation(int animId)
        {
            auto it = animIdMap_.find(animId);
            if (it != animIdMap_.end())
            {
                it->second.active = false;
                // نستخدم cancelAnimations للعقدة بالكامل إذا لم تبقَ تحريكات نشطة
                // لكن هنا نوقف الخاصية المحددة فقط عبر حذف المفتاح
                // AnimationEngine لا يدعم حذف خاصية واحدة — نضع قيمة نهائية
            }
        }

        void UIBridge::stopNodeAnimations(const std::string &target)
        {
            animEngine_.cancelAnimations(target);
            for (auto &[id, entry] : animIdMap_)
            {
                if (entry.nodeId == target)
                    entry.active = false;
            }
        }

        void UIBridge::stopAllAnimations()
        {
            animEngine_.cancelAll();
            for (auto &[id, entry] : animIdMap_)
            {
                entry.active = false;
            }
        }

        std::string UIBridge::getAnimationState(int animId) const
        {
            auto it = animIdMap_.find(animId);
            if (it == animIdMap_.end())
                return "not_found";
            if (!it->second.active)
                return "stopped";
            if (animEngine_.hasAnimation(it->second.nodeId, it->second.property))
                return "running";
            return "finished";
        }

        double UIBridge::getAnimatedValue(const std::string &target, const std::string &property, double defaultValue) const
        {
            return animEngine_.getValue(target, property, defaultValue);
        }

        bool UIBridge::hasActiveAnimations() const
        {
            return animEngine_.hasActiveAnimations();
        }

        bool UIBridge::updateAnimations()
        {
            auto now = std::chrono::steady_clock::now();
            uint32_t currentMs = static_cast<uint32_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(now - engineStartTime_).count());
            return animEngine_.update(currentMs);
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 22: الشبكة/الإنترنت (Network System)
        // ═══════════════════════════════════════════════════════════════════════════════

        std::string UIBridge::httpRequest(const std::string &method, const std::string &url,
                                          const std::string &body, const std::string &headers)
        {
            (void)headers;
#if defined(SAD_WASM_BUILD)
            // WASM: استخدام EM_ASM fetch
            // stub — يحتاج JS interop حقيقي
            (void)method;
            (void)url;
            (void)body;
            return "{\"error\":\"wasm_not_implemented\"}";
#elif defined(_WIN32)
            // Windows: WinHTTP بسيط عبر PowerShell/curl
            std::string cmd = "curl -s -X " + method + " \"" + url + "\"";
            if (!body.empty())
            {
                cmd += " -d \"" + body + "\"";
            }
            FILE *pipe = _popen(cmd.c_str(), "r");
            if (!pipe)
                return "{\"error\":\"popen_failed\"}";
            std::string result;
            char buf[4096];
            while (fgets(buf, sizeof(buf), pipe))
                result += buf;
            _pclose(pipe);
            return result;
#elif defined(__APPLE__) || defined(__linux__)
            std::string cmd = "curl -s -X " + method + " '" + url + "'";
            if (!body.empty())
            {
                cmd += " -d '" + body + "'";
            }
            FILE *pipe = popen(cmd.c_str(), "r");
            if (!pipe)
                return "{\"error\":\"popen_failed\"}";
            std::string result;
            char buf[4096];
            while (fgets(buf, sizeof(buf), pipe))
                result += buf;
            pclose(pipe);
            return result;
#else
            (void)method;
            (void)url;
            (void)body;
            return "{\"error\":\"not_supported\"}";
#endif
        }

        bool UIBridge::isOnline() const
        {
#if defined(SAD_WASM_BUILD)
            return true; // المتصفح متصل دائماً عملياً
#elif defined(_WIN32)
            // محاولة ping سريعة
            int ret = std::system("ping -n 1 -w 1000 8.8.8.8 >nul 2>&1");
            return ret == 0;
#elif defined(__APPLE__) || defined(__linux__)
            int ret = std::system("ping -c 1 -W 1 8.8.8.8 >/dev/null 2>&1");
            return ret == 0;
#elif defined(SAD_PLATFORM_ANDROID)
            return true; // يحتاج JNI حقيقي
#else
            return false;
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 23: JSON
        // ═══════════════════════════════════════════════════════════════════════════════

        std::string UIBridge::jsonParse(const std::string &jsonStr, const std::string &key)
        {
            // محلل JSON بسيط: يبحث عن "key":"value" أو "key":number
            std::string searchKey = "\"" + key + "\"";
            auto pos = jsonStr.find(searchKey);
            if (pos == std::string::npos)
                return "";
            pos = jsonStr.find(':', pos + searchKey.length());
            if (pos == std::string::npos)
                return "";
            pos++; // تخطي ':'
            while (pos < jsonStr.length() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t'))
                pos++;
            if (pos >= jsonStr.length())
                return "";

            if (jsonStr[pos] == '"')
            {
                // قيمة نصية
                pos++;
                auto end = jsonStr.find('"', pos);
                if (end == std::string::npos)
                    return "";
                return jsonStr.substr(pos, end - pos);
            }
            else
            {
                // قيمة رقمية أو boolean
                auto end = jsonStr.find_first_of(",}] \t\n", pos);
                if (end == std::string::npos)
                    end = jsonStr.length();
                return jsonStr.substr(pos, end - pos);
            }
        }

        std::string UIBridge::jsonStringify(const std::vector<std::pair<std::string, std::string>> &pairs)
        {
            std::string result = "{";
            for (size_t i = 0; i < pairs.size(); i++)
            {
                if (i > 0)
                    result += ",";
                result += "\"" + pairs[i].first + "\":\"" + pairs[i].second + "\"";
            }
            result += "}";
            return result;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 24: التشفير (Crypto System) — XOR بسيط + SHA256-like hash
        // ═══════════════════════════════════════════════════════════════════════════════

        std::string UIBridge::encryptText(const std::string &text, const std::string &key)
        {
            if (key.empty())
                return text;
            std::string result = text;
            for (size_t i = 0; i < result.size(); i++)
            {
                result[i] ^= key[i % key.size()];
            }
            // ترميز Base16 للإخراج
            std::string hex;
            hex.reserve(result.size() * 2);
            const char *hexChars = "0123456789abcdef";
            for (unsigned char c : result)
            {
                hex += hexChars[c >> 4];
                hex += hexChars[c & 0x0f];
            }
            return hex;
        }

        std::string UIBridge::decryptText(const std::string &encrypted, const std::string &key)
        {
            if (key.empty() || encrypted.size() % 2 != 0)
                return encrypted;
            // فك ترميز Base16
            std::string decoded;
            decoded.reserve(encrypted.size() / 2);
            for (size_t i = 0; i < encrypted.size(); i += 2)
            {
                unsigned char hi = 0, lo = 0;
                char c1 = encrypted[i], c2 = encrypted[i + 1];
                if (c1 >= '0' && c1 <= '9')
                    hi = static_cast<unsigned char>(c1 - '0');
                else if (c1 >= 'a' && c1 <= 'f')
                    hi = static_cast<unsigned char>(c1 - 'a' + 10);
                if (c2 >= '0' && c2 <= '9')
                    lo = static_cast<unsigned char>(c2 - '0');
                else if (c2 >= 'a' && c2 <= 'f')
                    lo = static_cast<unsigned char>(c2 - 'a' + 10);
                decoded += static_cast<char>((hi << 4) | lo);
            }
            for (size_t i = 0; i < decoded.size(); i++)
            {
                decoded[i] ^= key[i % key.size()];
            }
            return decoded;
        }

        std::string UIBridge::hashText(const std::string &text, const std::string &algorithm)
        {
            (void)algorithm;
            // DJB2 hash — سريع وبسيط
            uint64_t hash = 5381;
            for (char c : text)
            {
                hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
            }
            char buf[20];
            snprintf(buf, sizeof(buf), "%016llx", static_cast<unsigned long long>(hash));
            return std::string(buf);
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 25: قاعدة بيانات مفتاح/قيمة (Database System)
        // ═══════════════════════════════════════════════════════════════════════════════

        bool UIBridge::dbOpen(const std::string &dbName)
        {
            // تحميل من ملف على القرص
            currentDb_.name = dbName;
            currentDb_.open = true;
            currentDb_.data.clear();

            std::string path;
#ifdef _WIN32
            const char *appdata = std::getenv("APPDATA");
            path = appdata ? std::string(appdata) + "\\sad_db_" + dbName + ".dat" : "sad_db_" + dbName + ".dat";
#else
            const char *home = std::getenv("HOME");
            path = home ? std::string(home) + "/.sad_db_" + dbName + ".dat" : "sad_db_" + dbName + ".dat";
#endif
            std::ifstream f(path);
            if (f.is_open())
            {
                std::string line;
                while (std::getline(f, line))
                {
                    auto tab = line.find('\t');
                    if (tab != std::string::npos)
                    {
                        currentDb_.data[line.substr(0, tab)] = line.substr(tab + 1);
                    }
                }
            }
            return true;
        }

        std::string UIBridge::dbExecute(const std::string &query)
        {
            if (!currentDb_.open)
                return "{\"error\":\"db_not_open\"}";

            // أوامر بسيطة: SET key value, GET key, DELETE key, KEYS, COUNT
            if (query.substr(0, 4) == "SET ")
            {
                auto space = query.find(' ', 4);
                if (space != std::string::npos)
                {
                    std::string k = query.substr(4, space - 4);
                    std::string v = query.substr(space + 1);
                    currentDb_.data[k] = v;
                    // حفظ على القرص
                    std::string path;
#ifdef _WIN32
                    const char *appdata = std::getenv("APPDATA");
                    path = appdata ? std::string(appdata) + "\\sad_db_" + currentDb_.name + ".dat" : "sad_db_" + currentDb_.name + ".dat";
#else
                    const char *home = std::getenv("HOME");
                    path = home ? std::string(home) + "/.sad_db_" + currentDb_.name + ".dat" : "sad_db_" + currentDb_.name + ".dat";
#endif
                    std::ofstream out(path, std::ios::trunc);
                    for (auto &[dk, dv] : currentDb_.data)
                        out << dk << "\t" << dv << "\n";
                    return "{\"ok\":true}";
                }
            }
            else if (query.substr(0, 4) == "GET ")
            {
                std::string k = query.substr(4);
                auto it = currentDb_.data.find(k);
                if (it != currentDb_.data.end())
                    return it->second;
                return "";
            }
            else if (query.substr(0, 7) == "DELETE ")
            {
                std::string k = query.substr(7);
                currentDb_.data.erase(k);
                return "{\"ok\":true}";
            }
            else if (query == "KEYS")
            {
                std::string result = "[";
                bool first = true;
                for (auto &[k, v] : currentDb_.data)
                {
                    if (!first)
                        result += ",";
                    result += "\"" + k + "\"";
                    first = false;
                }
                result += "]";
                return result;
            }
            else if (query == "COUNT")
            {
                return std::to_string(currentDb_.data.size());
            }
            return "{\"error\":\"unknown_command\"}";
        }

        void UIBridge::dbClose()
        {
            currentDb_.open = false;
            currentDb_.data.clear();
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 26: الموقع الجغرافي (Geolocation)
        // ═══════════════════════════════════════════════════════════════════════════════

        std::pair<double, double> UIBridge::getLocation() const
        {
#if defined(SAD_WASM_BUILD)
            // يحتاج JS navigator.geolocation
            return {0.0, 0.0};
#elif defined(SAD_PLATFORM_ANDROID)
            // يحتاج JNI LocationManager
            return {0.0, 0.0};
#elif defined(SAD_PLATFORM_IOS)
            return {0.0, 0.0};
#else
            // سطح المكتب: لا يوجد GPS — يمكن استخدام IP geolocation
            return {0.0, 0.0};
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 27: الكاميرا (Camera System)
        // ═══════════════════════════════════════════════════════════════════════════════

        bool UIBridge::openCamera()
        {
#if defined(SAD_PLATFORM_ANDROID) || defined(SAD_PLATFORM_IOS)
            // يحتاج native API
            return false;
#elif defined(SAD_WASM_BUILD)
            return false;
#else
            // سطح المكتب: لا يوجد API موحد للكاميرا
            logMessage("info", "Camera API: desktop stub");
            return false;
#endif
        }

        std::string UIBridge::capturePhoto(const std::string &outputPath)
        {
            (void)outputPath;
#if defined(SAD_PLATFORM_ANDROID) || defined(SAD_PLATFORM_IOS)
            return "";
#else
            logMessage("info", "capturePhoto: desktop stub");
            return "";
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 28: المستشعرات (Sensors System)
        // ═══════════════════════════════════════════════════════════════════════════════

        std::string UIBridge::readSensor(const std::string &sensorType) const
        {
#if defined(SAD_PLATFORM_ANDROID)
            (void)sensorType;
            return "{\"error\":\"needs_jni\"}";
#elif defined(SAD_PLATFORM_IOS)
            (void)sensorType;
            return "{\"error\":\"needs_objc\"}";
#elif defined(SAD_UI_USE_SDL2)
            // SDL2 يدعم المستشعرات في SDL >= 2.0.9
            if (sensorType == "accelerometer")
            {
                return "{\"x\":0,\"y\":0,\"z\":9.8}";
            }
            return "{\"error\":\"sensor_not_available\"}";
#else
            (void)sensorType;
            return "{\"error\":\"not_supported\"}";
#endif
        }

        std::string UIBridge::availableSensors() const
        {
#if defined(SAD_PLATFORM_ANDROID) || defined(SAD_PLATFORM_IOS)
            return "[\"accelerometer\",\"gyroscope\",\"magnetometer\",\"proximity\",\"light\"]";
#elif defined(SAD_UI_USE_SDL2)
            int n = SDL_NumSensors();
            if (n <= 0)
                return "[]";
            std::string result = "[";
            for (int i = 0; i < n; i++)
            {
                if (i > 0)
                    result += ",";
                const char *name = SDL_SensorGetDeviceName(i);
                result += "\"" + std::string(name ? name : "unknown") + "\"";
            }
            result += "]";
            return result;
#else
            return "[]";
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 29: المشاركة (Share System)
        // ═══════════════════════════════════════════════════════════════════════════════

        bool UIBridge::shareText(const std::string &text, const std::string &title)
        {
#if defined(SAD_WASM_BUILD)
            (void)title;
            // Web Share API
            // يحتاج JS interop
            (void)text;
            return false;
#elif defined(SAD_PLATFORM_ANDROID)
            (void)text;
            (void)title;
            return false; // يحتاج JNI Intent
#elif defined(SAD_PLATFORM_IOS)
            (void)text;
            (void)title;
            return false; // يحتاج UIActivityViewController
#elif defined(_WIN32)
            // نسخ النص للحافظة كبديل
            copyToClipboard(text);
            logMessage("info", "Shared text (copied to clipboard): " + title);
            return true;
#else
            // Linux: xdg-open أو xclip
            copyToClipboard(text);
            logMessage("info", "Shared text (copied to clipboard): " + title);
            return true;
#endif
        }

        bool UIBridge::shareFile(const std::string &filePath, const std::string &mimeType)
        {
            (void)mimeType;
#if defined(SAD_PLATFORM_ANDROID) || defined(SAD_PLATFORM_IOS)
            (void)filePath;
            return false;
#elif defined(_WIN32)
            // فتح الملف بالتطبيق الافتراضي
            ShellExecuteA(NULL, "open", filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
            return true;
#elif defined(__APPLE__)
            std::string cmd = "open '" + filePath + "'";
            return std::system(cmd.c_str()) == 0;
#elif defined(__linux__)
            std::string cmd = "xdg-open '" + filePath + "'";
            return std::system(cmd.c_str()) == 0;
#else
            (void)filePath;
            return false;
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 30: الأذونات (Permissions System)
        // ═══════════════════════════════════════════════════════════════════════════════

        std::string UIBridge::requestPermission(const std::string &permission)
        {
#if defined(SAD_PLATFORM_ANDROID) || defined(SAD_PLATFORM_IOS)
            (void)permission;
            return "denied"; // يحتاج native API
#elif defined(SAD_WASM_BUILD)
            (void)permission;
            return "granted"; // المتصفح يتعامل مع الأذونات
#else
            // سطح المكتب: الأذونات ممنوحة دائماً
            (void)permission;
            return "granted";
#endif
        }

        bool UIBridge::checkPermission(const std::string &permission) const
        {
#if defined(SAD_PLATFORM_ANDROID) || defined(SAD_PLATFORM_IOS)
            (void)permission;
            return false;
#else
            (void)permission;
            return true; // سطح المكتب
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 31: السمات (Themes System)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::setTheme(const std::string &themeName)
        {
            currentTheme_ = themeName;
        }

        std::string UIBridge::getTheme() const
        {
            return currentTheme_;
        }

        std::string UIBridge::getSystemTheme() const
        {
#if defined(SAD_WASM_BUILD)
            return "فاتح"; // يحتاج JS matchMedia
#elif defined(_WIN32)
            // Windows 10+: قراءة من الريجستري
            HKEY hKey;
            DWORD value = 1;
            DWORD size = sizeof(value);
            if (RegOpenKeyExA(HKEY_CURRENT_USER,
                              "Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                              0, KEY_READ, &hKey) == ERROR_SUCCESS)
            {
                RegQueryValueExA(hKey, "AppsUseLightTheme", NULL, NULL, (LPBYTE)&value, &size);
                RegCloseKey(hKey);
            }
            return value == 0 ? "داكن" : "فاتح";
#elif defined(__APPLE__)
            // macOS: defaults read -g AppleInterfaceStyle
            FILE *pipe = popen("defaults read -g AppleInterfaceStyle 2>/dev/null", "r");
            if (pipe)
            {
                char buf[64];
                if (fgets(buf, sizeof(buf), pipe))
                {
                    pclose(pipe);
                    return std::string(buf).find("Dark") != std::string::npos ? "داكن" : "فاتح";
                }
                pclose(pipe);
            }
            return "فاتح";
#elif defined(__linux__)
            // GTK theme detection
            FILE *pipe = popen("gsettings get org.gnome.desktop.interface color-scheme 2>/dev/null", "r");
            if (pipe)
            {
                char buf[128];
                if (fgets(buf, sizeof(buf), pipe))
                {
                    pclose(pipe);
                    return std::string(buf).find("dark") != std::string::npos ? "داكن" : "فاتح";
                }
                pclose(pipe);
            }
            return "فاتح";
#else
            return "فاتح";
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 32: التدويل (Internationalization / i18n)
        // ═══════════════════════════════════════════════════════════════════════════════

        std::string UIBridge::translate(const std::string &key) const
        {
            auto it = translations_.find(currentLocale_ + "." + key);
            if (it != translations_.end())
                return it->second;
            // fallback بدون بادئة اللغة
            auto it2 = translations_.find(key);
            if (it2 != translations_.end())
                return it2->second;
            return key; // إرجاع المفتاح كما هو
        }

        void UIBridge::setLocale(const std::string &locale)
        {
            currentLocale_ = locale;
        }

        std::string UIBridge::getLocale() const
        {
            return currentLocale_;
        }

        void UIBridge::addTranslation(const std::string &key, const std::string &value)
        {
            translations_[currentLocale_ + "." + key] = value;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 33: الوصولية (Accessibility)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::screenReaderAnnounce(const std::string &text)
        {
#if defined(SAD_WASM_BUILD)
            (void)text;
            // يحتاج aria-live region
#elif defined(_WIN32)
            // Windows: يمكن استخدام IAccessible / UI Automation
            logMessage("accessibility", text);
#elif defined(__APPLE__)
            // NSAccessibility
            logMessage("accessibility", text);
#else
            logMessage("accessibility", text);
#endif
        }

        double UIBridge::getSystemFontScale() const
        {
            if (activeWindow_)
                return activeWindow_->getSystemFontScale();
            return 1.0;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 34: التخطيط المتجاوب (Responsive Layout)
        // ═══════════════════════════════════════════════════════════════════════════════

        int UIBridge::getScreenWidth() const
        {
            if (activeWindow_)
                return activeWindow_->getScreenWidth();
            return 1920;
        }

        int UIBridge::getScreenHeight() const
        {
            if (activeWindow_)
                return activeWindow_->getScreenHeight();
            return 1080;
        }

        double UIBridge::getPixelDensity() const
        {
            if (activeWindow_)
                return activeWindow_->getPixelDensity();
            return 96.0;
        }

        std::string UIBridge::getDeviceType() const
        {
#if defined(SAD_PLATFORM_ANDROID) || defined(SAD_PLATFORM_IOS)
            return "mobile";
#elif defined(SAD_WASM_BUILD)
            return "web";
#else
            return "desktop";
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 35: الطباعة (Print System)
        // ═══════════════════════════════════════════════════════════════════════════════

        bool UIBridge::printPage(const std::string &content, const std::string &title)
        {
#if defined(SAD_WASM_BUILD)
            (void)content;
            (void)title;
            return false; // يحتاج window.print()
#elif defined(_WIN32)
            // حفظ كملف HTML وفتحه للطباعة
            std::string tmpPath = std::string(std::getenv("TEMP") ? std::getenv("TEMP") : ".") + "\\sad_print.html";
            std::ofstream f(tmpPath);
            if (!f.is_open())
                return false;
            f << "<html><head><title>" << title << "</title></head><body>";
            f << "<pre>" << content << "</pre>";
            f << "<script>window.print();</script></body></html>";
            f.close();
            ShellExecuteA(NULL, "open", tmpPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
            return true;
#elif defined(__APPLE__)
            std::string tmpPath = "/tmp/sad_print.html";
            std::ofstream f(tmpPath);
            if (!f.is_open())
                return false;
            f << "<html><head><title>" << title << "</title></head><body><pre>" << content << "</pre></body></html>";
            f.close();
            std::system(("open " + tmpPath).c_str());
            return true;
#elif defined(__linux__)
            std::string tmpPath = "/tmp/sad_print.html";
            std::ofstream f(tmpPath);
            if (!f.is_open())
                return false;
            f << "<html><head><title>" << title << "</title></head><body><pre>" << content << "</pre></body></html>";
            f.close();
            std::system(("xdg-open " + tmpPath).c_str());
            return true;
#else
            (void)content;
            (void)title;
            return false;
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 36: البيانات الحيوية (Biometrics)
        // ═══════════════════════════════════════════════════════════════════════════════

        bool UIBridge::authenticateBiometric(const std::string &reason)
        {
            (void)reason;
#if defined(SAD_PLATFORM_ANDROID)
            return false; // يحتاج BiometricPrompt
#elif defined(SAD_PLATFORM_IOS)
            return false; // يحتاج LAContext
#elif defined(_WIN32)
            // Windows Hello — stub
            logMessage("biometric", "Windows Hello stub: " + reason);
            return false;
#else
            return false;
#endif
        }

        bool UIBridge::hasBiometricSupport() const
        {
#if defined(SAD_PLATFORM_ANDROID) || defined(SAD_PLATFORM_IOS)
            return true; // الأجهزة الحديثة تدعم
#else
            return false;
#endif
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 37: الإيماءات المتقدمة (Advanced Gestures)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::setSwipeHandler(const Data::Value &handler)
        {
            swipeHandler_ = handler;
        }

        void UIBridge::setPinchHandler(const Data::Value &handler)
        {
            pinchHandler_ = handler;
        }

        void UIBridge::setRotateHandler(const Data::Value &handler)
        {
            rotateHandler_ = handler;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 38: الخرائط (Maps System)
        // ═══════════════════════════════════════════════════════════════════════════════

        int UIBridge::createMap(double lat, double lng, int zoom)
        {
            MapEntry m;
            m.id = nextMapId_++;
            m.lat = lat;
            m.lng = lng;
            m.zoom = zoom;
            maps_.push_back(m);
            logMessage("maps", "Map created: id=" + std::to_string(m.id) +
                                   " lat=" + std::to_string(lat) + " lng=" + std::to_string(lng));
            return m.id;
        }

        void UIBridge::addMapMarker(int mapId, double lat, double lng, const std::string &label)
        {
            for (auto &m : maps_)
            {
                if (m.id == mapId)
                {
                    m.markers.push_back({lat, lng, label});
                    return;
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 39: الرسم الحر (Canvas/Drawing)
        // ═══════════════════════════════════════════════════════════════════════════════

        int UIBridge::createCanvas(int width, int height)
        {
            CanvasEntry c;
            c.id = nextCanvasId_++;
            c.width = width;
            c.height = height;
            c.pixels.resize(static_cast<size_t>(width * height * 4), 0); // RGBA
            canvases_.push_back(std::move(c));
            return c.id;
        }

        void UIBridge::drawLine(int canvasId, int x1, int y1, int x2, int y2,
                                const std::string &color, int thickness)
        {
            (void)thickness;
            for (auto &c : canvases_)
            {
                if (c.id == canvasId)
                {
                    auto col = parseColor(color);
                    // Bresenham line
                    int dx = std::abs(x2 - x1), dy = std::abs(y2 - y1);
                    int sx = x1 < x2 ? 1 : -1, sy = y1 < y2 ? 1 : -1;
                    int err = dx - dy;
                    while (true)
                    {
                        if (x1 >= 0 && x1 < c.width && y1 >= 0 && y1 < c.height)
                        {
                            size_t idx = static_cast<size_t>((y1 * c.width + x1) * 4);
                            c.pixels[idx] = col.r;
                            c.pixels[idx + 1] = col.g;
                            c.pixels[idx + 2] = col.b;
                            c.pixels[idx + 3] = col.a;
                        }
                        if (x1 == x2 && y1 == y2)
                            break;
                        int e2 = 2 * err;
                        if (e2 > -dy)
                        {
                            err -= dy;
                            x1 += sx;
                        }
                        if (e2 < dx)
                        {
                            err += dx;
                            y1 += sy;
                        }
                    }
                    return;
                }
            }
        }

        void UIBridge::drawCircle(int canvasId, int cx, int cy, int radius,
                                  const std::string &color, bool fill)
        {
            for (auto &c : canvases_)
            {
                if (c.id == canvasId)
                {
                    auto col = parseColor(color);
                    for (int y = -radius; y <= radius; y++)
                    {
                        for (int x = -radius; x <= radius; x++)
                        {
                            int dist2 = x * x + y * y;
                            int r2 = radius * radius;
                            bool draw = fill ? (dist2 <= r2) : (dist2 >= (radius - 1) * (radius - 1) && dist2 <= r2);
                            if (draw)
                            {
                                int px = cx + x, py = cy + y;
                                if (px >= 0 && px < c.width && py >= 0 && py < c.height)
                                {
                                    size_t idx = static_cast<size_t>((py * c.width + px) * 4);
                                    c.pixels[idx] = col.r;
                                    c.pixels[idx + 1] = col.g;
                                    c.pixels[idx + 2] = col.b;
                                    c.pixels[idx + 3] = col.a;
                                }
                            }
                        }
                    }
                    return;
                }
            }
        }

        void UIBridge::drawRect(int canvasId, int x, int y, int w, int h,
                                const std::string &color, bool fill)
        {
            for (auto &c : canvases_)
            {
                if (c.id == canvasId)
                {
                    auto col = parseColor(color);
                    for (int dy = 0; dy < h; dy++)
                    {
                        for (int dx = 0; dx < w; dx++)
                        {
                            bool draw = fill || (dx == 0 || dx == w - 1 || dy == 0 || dy == h - 1);
                            if (draw)
                            {
                                int px = x + dx, py = y + dy;
                                if (px >= 0 && px < c.width && py >= 0 && py < c.height)
                                {
                                    size_t idx = static_cast<size_t>((py * c.width + px) * 4);
                                    c.pixels[idx] = col.r;
                                    c.pixels[idx + 1] = col.g;
                                    c.pixels[idx + 2] = col.b;
                                    c.pixels[idx + 3] = col.a;
                                }
                            }
                        }
                    }
                    return;
                }
            }
        }

        void UIBridge::clearCanvas(int canvasId)
        {
            for (auto &c : canvases_)
            {
                if (c.id == canvasId)
                {
                    std::fill(c.pixels.begin(), c.pixels.end(), 0);
                    return;
                }
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        //  نظام 40: البطارية (Battery System)
        // ═══════════════════════════════════════════════════════════════════════════════

        int UIBridge::getBatteryLevel() const
        {
            if (activeWindow_)
                return activeWindow_->getBatteryLevel();
            return -1;
        }

        bool UIBridge::isBatteryCharging() const
        {
            if (activeWindow_)
                return activeWindow_->isBatteryCharging();
            return false;
        }

        // getLastGeneratedHtml moved to ui_bridge.cpp

        // ═══════════════════════════════════════════════════════════════════════════════
        // نظام إعادة التحميل الحي (Hot Reload System)
        // ═══════════════════════════════════════════════════════════════════════════════

        void UIBridge::enableHotReload(const std::string &sourceFilePath,
                                       const Sad::HotReload::HotReloadOptions &options)
        {
            hotReloadEngine_ = std::make_unique<Sad::HotReload::HotReloadEngine>(options);
            hotReloadEngine_->attach(interpreter_, sourceFilePath);

            // (AR) ربط إعادة بناء الواجهة — عندما يتغير الكود يُعاد بناء الشجرة
            // (EN) Connect UI rebuild — when code changes, tree is rebuilt
            hotReloadEngine_->setRebuildUICallback([this]()
                                                   { rebuildUI(); });

            // (HIGH-1، إصلاح Amelia) عند إعادة التنفيذ الكاملة (Hot Restart) يُصفَّر المفسّر؛
            //   نصفّر مكدّس sad::ui::nav المُسرَّب أيضًا (إدخالاته باني الصفحة تلتقط المفسّر)
            //   وإلّا استدعى بانٍ قديمٌ callUserFunction على مفسّرٍ مُصفَّر. البرنامج المُعاد
            //   تنفيذه يعيد بذر الجذر عبر run()→navSeedRoot.
            hotReloadEngine_->setStateResetCallback([]()
                                                    { sad::ui::nav().reset();
                                                      sad::ui::windowController().reset(); }); // (Amelia H1/L3) صفّر المتحكّم أيضًا

            // (AR) بدء المراقبة — poll mode (يُفحص من حلقة الأحداث)
            // (EN) Start watching — poll mode (checked from event loop)
            // لا نستخدم start() هنا لأن poll() سيُستدعى من حلقة أحداث النافذة
        }

        void UIBridge::pollHotReload()
        {
            if (hotReloadEngine_ && hotReloadEngine_->isActive())
            {
                hotReloadEngine_->poll();
            }
        }
    } // namespace Interpreter
} // namespace Sad
