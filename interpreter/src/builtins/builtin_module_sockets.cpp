/**
 * @file builtin_module_sockets.cpp
 * @brief (AR) وحدة المقابس — اتصالات TCP/UDP
 * @brief (EN) Sockets module — TCP/UDP socket connections
 *
 * @details
 * (AR) الأقسام:
 *   1. إنشاء مقابس TCP/UDP
 *   2. الإرسال والاستقبال
 *   3. الاستماع والقبول
 *
 * @note يتطلب: builtin_common.h (الـ headers المشتركة)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "value.h"
#include "builtin_registry.h"

#include <atomic>
#include <cstring>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
// يجب أن يكون خارج كتلة #ifdef _WIN32/#else لأن النص المُجمَّع يستخدم
// SecurityNS::SafeArithmetic على كل المنصات.
#include "safe_arithmetic.h"

// ═══════════════════════════════════════════════════════════════════════
// (AR) واجهة نظام التشغيل للمقابس — WinSock2 أو POSIX
// (EN) OS-level socket interface — WinSock2 or POSIX
// ═══════════════════════════════════════════════════════════════════════
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using socket_handle_t = SOCKET;
#define SAD_INVALID_SOCKET INVALID_SOCKET
#define SAD_SOCKET_ERROR SOCKET_ERROR
#define SAD_CLOSE_SOCKET closesocket
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
using socket_handle_t = int;
#define SAD_INVALID_SOCKET (-1)
#define SAD_SOCKET_ERROR (-1)
#define SAD_CLOSE_SOCKET ::close
// (AR) ‏`<netinet/tcp.h>` يعرّف `TCP_LISTEN` و`TCP_CLOSE` **ماكروَين** (حالتا
//      اتّصالٍ رقميّتان)، وهما اسما ثابتَين في `Bsk` المُولَّد من مصدر الحقيقة.
//      فيستبدل المعالجُ الأوّليّ الاسمَ برقمٍ داخل `Bsk::…` فينكسر الترجمة.
//      ولا يقع على ويندوز لأنّ الفرعَ الآخر لا يشمل هذه الترويسة — فبقي
//      كامنًا. والماكرو يُلغى لأنّ هذا الملفّ لا يستعمل حالاتِ الاتّصال أصلًا.
// (EN) <netinet/tcp.h> defines TCP_LISTEN/TCP_CLOSE as macros, colliding with the
//      SoT-generated constant names in Bsk. Latent on Windows (header not pulled).
#ifdef TCP_LISTEN
#undef TCP_LISTEN
#endif
#ifdef TCP_CLOSE
#undef TCP_CLOSE
#endif
#endif

namespace Sad
{
    namespace Interpreter
    {

        // (AR) اختصار لفضاء أسماء ثوابت المقابس
        namespace Bsk = Sad::Builtins::Names::Sockets;

        // (AR) اختصار للطبقة الأمنية المشتركة — يتجنب مشكلة dependent name
        //      في lambda المتداخلة داخل namespace Sad في MSVC strict parser.
        // (EN) Alias for shared security primitives — sidesteps MSVC dependent
        //      name issue inside nested lambdas within namespace Sad.
        namespace SecurityNS = ::Sad::Security;

// ═══════════════════════════════════════════════════════════════════════
// (AR) تهيئة WinSock — تلقائية عند أول استخدام
// (EN) WinSock initialization — automatic on first use
// ═══════════════════════════════════════════════════════════════════════
#ifdef _WIN32
        static struct WinSockInit
        {
            WinSockInit()
            {
                WSADATA wsaData;
                WSAStartup(MAKEWORD(2, 2), &wsaData);
            }
            ~WinSockInit()
            {
                WSACleanup();
            }
        } g_winsock_init;
#endif

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) إدارة المقابس — خريطة عالمية بمعرّفات رقمية
        //      تستخدم mutex للأمان مع الخيوط المتعددة
        // (EN) Socket management — global map with numeric IDs
        //      Uses mutex for thread safety
        // ═══════════════════════════════════════════════════════════════════════

        // (AR) نوع المقبس — TCP أو UDP
        enum class SocketProto
        {
            TCP,
            UDP
        };

        // (AR) مدخل مقبس يحتوي المقبس الحقيقي ونوعه
        struct SocketEntry
        {
            socket_handle_t handle;
            SocketProto proto;
            bool connected;

            SocketEntry(socket_handle_t h, SocketProto p)
                : handle(h), proto(p), connected(false) {}
            ~SocketEntry()
            {
                if (handle != SAD_INVALID_SOCKET)
                {
                    SAD_CLOSE_SOCKET(handle);
                }
            }
        };

        static std::unordered_map<int64_t, std::unique_ptr<SocketEntry>> g_sockets;
        static std::mutex g_socket_mutex;
        static std::atomic<int64_t> g_next_socket_id{1};

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) دوال مساعدة داخلية
        // ═══════════════════════════════════════════════════════════════════════

        static SocketEntry *getSocket(int64_t id)
        {
            std::lock_guard<std::mutex> lock(g_socket_mutex);
            auto it = g_sockets.find(id);
            if (it == g_sockets.end())
                return nullptr;
            return it->second.get();
        }

        static int64_t storeSocket(socket_handle_t handle, SocketProto proto)
        {
            int64_t id = g_next_socket_id.fetch_add(1);
            auto entry = std::make_unique<SocketEntry>(handle, proto);
            std::lock_guard<std::mutex> lock(g_socket_mutex);
            g_sockets[id] = std::move(entry);
            return id;
        }

        static bool removeSocket(int64_t id)
        {
            std::lock_guard<std::mutex> lock(g_socket_mutex);
            return g_sockets.erase(id) > 0;
        }

        // (AR) تحويل اسم المضيف إلى عنوان IP (يدعم DNS)
        static bool resolveHost(const std::string &host, uint16_t port, sockaddr_in &addr)
        {
            std::memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);

            if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) == 1)
            {
                return true;
            }
            struct addrinfo hints, *result = nullptr;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            if (getaddrinfo(host.c_str(), nullptr, &hints, &result) == 0 && result)
            {
                auto *ipv4 = reinterpret_cast<sockaddr_in *>(result->ai_addr);
                addr.sin_addr = ipv4->sin_addr;
                freeaddrinfo(result);
                return true;
            }
            if (result)
                freeaddrinfo(result);
            return false;
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) تسجيل جميع دوال المقابس في المفسر
        // ═══════════════════════════════════════════════════════════════════════

        void registerBuiltinsSockets(Interpreter &interpreter)
        {
            auto &fm = interpreter.getFunctionManager();

            // ═════════════════════════════════════════════════════════════════
            //  القسم 1: مقابس TCP — بروتوكول تدفق موثوق
            // ═════════════════════════════════════════════════════════════════

            // ─── 1. مقبس_tcp_جديد / tcp_new ─────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    socket_handle_t s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                    if (s == SAD_INVALID_SOCKET)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    int64_t id = storeSocket(s, SocketProto::TCP);
                    return std::make_shared<Data::Value>(static_cast<double>(id));
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_NEW), f); // مقبس_tcp_جديد
            }

            // ─── 2. مقبس_اتصال / tcp_connect ────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 3)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    std::string host = args[1]->toString();
                    uint16_t port = static_cast<uint16_t>(args[2]->toDouble());

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    sockaddr_in addr;
                    if (!resolveHost(host, port, addr))
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int result = ::connect(entry->handle, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
                    if (result == SAD_SOCKET_ERROR)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    entry->connected = true;
                    return std::make_shared<Data::Value>(true);
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_CONNECT), f); // مقبس_اتصال
            }

            // ─── 3. مقبس_ارسال / tcp_send ───────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    std::string data = args[1]->toString();

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    const int dataLen = SecurityNS::SafeArithmetic::assertSafeCast<int>(data.size(), "builtin_module_sockets_tcp_send_size");
                    int sent = ::send(entry->handle, data.c_str(), dataLen, 0);
                    return std::make_shared<Data::Value>(static_cast<double>(sent));
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_SEND), f); // مقبس_ارسال
            }

            // ─── 4. مقبس_استقبال / tcp_recv ─────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    int maxSize = 4096;
                    if (args.size() >= 2)
                    {
                        maxSize = static_cast<int>(args[1]->toDouble());
                        if (maxSize <= 0 || maxSize > 1048576)
                            maxSize = 4096;
                    }

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }
                    std::vector<char> buffer(maxSize);
                    int received = ::recv(entry->handle, buffer.data(), maxSize, 0);
                    if (received <= 0)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }
                    return std::make_shared<Data::Value>(std::string(buffer.data(), received));
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_RECV), f); // مقبس_استقبال
            }

            // ─── 5. مقبس_ربط / tcp_bind ─────────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    uint16_t port = static_cast<uint16_t>(args[1]->toDouble());

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    // (AR) تفعيل إعادة استخدام العنوان لتجنب "Address already in use"
                    int opt = 1;
                    setsockopt(entry->handle, SOL_SOCKET, SO_REUSEADDR,
                               reinterpret_cast<const char *>(&opt), sizeof(opt));

                    sockaddr_in addr;
                    std::memset(&addr, 0, sizeof(addr));
                    addr.sin_family = AF_INET;
                    addr.sin_addr.s_addr = INADDR_ANY;
                    addr.sin_port = htons(port);

                    int result = ::bind(entry->handle, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
                    return std::make_shared<Data::Value>(result != SAD_SOCKET_ERROR);
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_BIND), f); // مقبس_ربط
            }

            // ─── 6. مقبس_استماع / tcp_listen ────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    int backlog = 10;
                    if (args.size() >= 2)
                    {
                        backlog = static_cast<int>(args[1]->toDouble());
                    }

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int result = ::listen(entry->handle, backlog);
                    return std::make_shared<Data::Value>(result != SAD_SOCKET_ERROR);
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_LISTEN), f); // مقبس_استماع
            }

            // ─── 7. مقبس_قبول / tcp_accept ──────────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }

                    sockaddr_in clientAddr;
#ifdef _WIN32
                    int addrLen = sizeof(clientAddr);
                    socket_handle_t clientSock = ::accept(entry->handle,
                                                          reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
#else
                    socklen_t addrLen = sizeof(clientAddr);
                    socket_handle_t clientSock = ::accept(entry->handle,
                                                          reinterpret_cast<sockaddr *>(&clientAddr), &addrLen);
#endif
                    if (clientSock == SAD_INVALID_SOCKET)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    int64_t clientId = storeSocket(clientSock, SocketProto::TCP);
                    auto *clientEntry = getSocket(clientId);
                    if (clientEntry)
                        clientEntry->connected = true;
                    return std::make_shared<Data::Value>(static_cast<double>(clientId));
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_ACCEPT), f); // مقبس_قبول
            }

            // ─── 8. مقبس_اغلاق / socket_close ──────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    return std::make_shared<Data::Value>(removeSocket(id));
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_CLOSE), f); // مقبس_اغلاق
            }

            // ─── 9. مقبس_متصل / tcp_is_connected ───────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    return std::make_shared<Data::Value>(entry->connected);
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_CONNECTED), f); // مقبس_متصل
            }

            // ─── 10. مقبس_عنوان_بعيد / tcp_peer_address ────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }
                    sockaddr_in peerAddr;
#ifdef _WIN32
                    int addrLen = sizeof(peerAddr);
#else
                    socklen_t addrLen = sizeof(peerAddr);
#endif
                    if (getpeername(entry->handle, reinterpret_cast<sockaddr *>(&peerAddr), &addrLen) != 0)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }

                    char ipStr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &peerAddr.sin_addr, ipStr, sizeof(ipStr));
                    std::ostringstream oss;
                    oss << ipStr << ":" << ntohs(peerAddr.sin_port);
                    return std::make_shared<Data::Value>(oss.str());
                };
                fm.registerBuiltinFunction(std::string(Bsk::TCP_REMOTE_ADDR), f); // مقبس_عنوان_بعيد
            }

            // ═════════════════════════════════════════════════════════════════
            //  القسم 2: مقابس UDP — بروتوكول رسائل بدون اتصال
            // ═════════════════════════════════════════════════════════════════

            // ─── 11. مقبس_udp_جديد / udp_new ───────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    (void)args;
                    socket_handle_t s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
                    if (s == SAD_INVALID_SOCKET)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    int64_t id = storeSocket(s, SocketProto::UDP);
                    return std::make_shared<Data::Value>(static_cast<double>(id));
                };
                fm.registerBuiltinFunction(std::string(Bsk::UDP_NEW), f); // مقبس_udp_جديد
            }

            // ─── 12. مقبس_udp_ربط / udp_bind ───────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    uint16_t port = static_cast<uint16_t>(args[1]->toDouble());

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::UDP)
                    {
                        return std::make_shared<Data::Value>(false);
                    }

                    sockaddr_in addr;
                    std::memset(&addr, 0, sizeof(addr));
                    addr.sin_family = AF_INET;
                    addr.sin_addr.s_addr = INADDR_ANY;
                    addr.sin_port = htons(port);

                    int result = ::bind(entry->handle, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
                    return std::make_shared<Data::Value>(result != SAD_SOCKET_ERROR);
                };
                fm.registerBuiltinFunction(std::string(Bsk::UDP_BIND), f); // مقبس_udp_ربط
            }

            // ─── 13. مقبس_udp_ارسال / udp_send_to ──────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 4)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    std::string data = args[1]->toString();
                    std::string host = args[2]->toString();
                    uint16_t port = static_cast<uint16_t>(args[3]->toDouble());

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::UDP)
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }

                    sockaddr_in destAddr;
                    if (!resolveHost(host, port, destAddr))
                    {
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    }
                    const int dataLen = SecurityNS::SafeArithmetic::assertSafeCast<int>(data.size(), "builtin_module_sockets_udp_send_size");
                    int sent = ::sendto(entry->handle, data.c_str(), dataLen, 0,
                                        reinterpret_cast<sockaddr *>(&destAddr), sizeof(destAddr));
                    return std::make_shared<Data::Value>(static_cast<double>(sent));
                };
                fm.registerBuiltinFunction(std::string(Bsk::UDP_SEND), f); // مقبس_udp_ارسال
            }

            // ─── 14. مقبس_udp_استقبال / udp_recv_from ──────────────────────
            // (AR) يُرجع خريطة: "بيانات"/"data"، "عنوان"/"address"، "منفذ"/"port"
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    int maxSize = 4096;
                    if (args.size() >= 2)
                    {
                        maxSize = static_cast<int>(args[1]->toDouble());
                        if (maxSize <= 0 || maxSize > 65536)
                            maxSize = 4096;
                    }

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::UDP)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }

                    std::vector<char> buffer(maxSize);
                    sockaddr_in senderAddr;
#ifdef _WIN32
                    int addrLen = sizeof(senderAddr);
#else
                    socklen_t addrLen = sizeof(senderAddr);
#endif
                    int received = ::recvfrom(entry->handle, buffer.data(), maxSize, 0,
                                              reinterpret_cast<sockaddr *>(&senderAddr), &addrLen);
                    if (received <= 0)
                    {
                        return std::make_shared<Data::Value>(std::string(""));
                    }

                    char ipStr[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &senderAddr.sin_addr, ipStr, sizeof(ipStr));

                    Data::Value::MapType map;
                    map["\xd8\xa8\xd9\x8a\xd8\xa7\xd9\x86\xd8\xa7\xd8\xaa"] = // بيانات
                        Data::Value(std::string(buffer.data(), received));
                    map["data"] =
                        Data::Value(std::string(buffer.data(), received));
                    map["\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"] = // عنوان
                        Data::Value(std::string(ipStr));
                    map["address"] =
                        Data::Value(std::string(ipStr));
                    map["\xd9\x85\xd9\x86\xd9\x81\xd8\xb0"] = // منفذ
                        Data::Value(static_cast<double>(ntohs(senderAddr.sin_port)));
                    map["port"] =
                        Data::Value(static_cast<double>(ntohs(senderAddr.sin_port)));

                    return std::make_shared<Data::Value>(std::move(map));
                };
                fm.registerBuiltinFunction(std::string(Bsk::UDP_RECV), f); // مقبس_udp_استقبال
            }

            // ─── 15. مقبس_udp_اغلاق / udp_close ────────────────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    return std::make_shared<Data::Value>(removeSocket(id));
                };
                fm.registerBuiltinFunction(std::string(Bsk::UDP_CLOSE), f); // مقبس_udp_اغلاق
            }

            // ═════════════════════════════════════════════════════════════════
            //  القسم 3: إعدادات عامة للمقابس
            // ═════════════════════════════════════════════════════════════════

            // ─── 16. مقبس_مهلة_استقبال / socket_recv_timeout ───────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    int ms = static_cast<int>(args[1]->toDouble());

                    auto *entry = getSocket(id);
                    if (!entry)
                        return std::make_shared<Data::Value>(false);

#ifdef _WIN32
                    DWORD timeout = static_cast<DWORD>(ms);
                    int result = setsockopt(entry->handle, SOL_SOCKET, SO_RCVTIMEO,
                                            reinterpret_cast<const char *>(&timeout), sizeof(timeout));
#else
                    struct timeval tv;
                    tv.tv_sec = ms / 1000;
                    tv.tv_usec = (ms % 1000) * 1000;
                    int result = setsockopt(entry->handle, SOL_SOCKET, SO_RCVTIMEO,
                                            &tv, sizeof(tv));
#endif
                    return std::make_shared<Data::Value>(result != SAD_SOCKET_ERROR);
                };
                fm.registerBuiltinFunction(std::string(Bsk::RECV_TIMEOUT), f); // مقبس_مهلة_استقبال
            }

            // ─── 17. مقبس_مهلة_ارسال / socket_send_timeout ─────────────────
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    int ms = static_cast<int>(args[1]->toDouble());

                    auto *entry = getSocket(id);
                    if (!entry)
                        return std::make_shared<Data::Value>(false);

#ifdef _WIN32
                    DWORD timeout = static_cast<DWORD>(ms);
                    int result = setsockopt(entry->handle, SOL_SOCKET, SO_SNDTIMEO,
                                            reinterpret_cast<const char *>(&timeout), sizeof(timeout));
#else
                    struct timeval tv;
                    tv.tv_sec = ms / 1000;
                    tv.tv_usec = (ms % 1000) * 1000;
                    int result = setsockopt(entry->handle, SOL_SOCKET, SO_SNDTIMEO,
                                            &tv, sizeof(tv));
#endif
                    return std::make_shared<Data::Value>(result != SAD_SOCKET_ERROR);
                };
                fm.registerBuiltinFunction(std::string(Bsk::SEND_TIMEOUT), f); // مقبس_مهلة_ارسال
            }

            // ─── 18. مقبس_بدون_تأخير / socket_nodelay ──────────────────────
            // (AR) تعطيل/تفعيل خوارزمية Nagle — TCP فقط
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 2)
                    {
                        return std::make_shared<Data::Value>(false);
                    }
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    bool enable = args[1]->toBool();

                    auto *entry = getSocket(id);
                    if (!entry || entry->proto != SocketProto::TCP)
                    {
                        return std::make_shared<Data::Value>(false);
                    }

                    int flag = enable ? 1 : 0;
                    int result = setsockopt(entry->handle, IPPROTO_TCP, TCP_NODELAY,
                                            reinterpret_cast<const char *>(&flag), sizeof(flag));
                    return std::make_shared<Data::Value>(result != SAD_SOCKET_ERROR);
                };
                fm.registerBuiltinFunction(std::string(Bsk::NO_DELAY), f); // مقبس_بدون_تأخير
            }

            // ─── 19. منفذ_محلي / local_port ─────────────────────────────────
            // (AR) الحصول على رقم المنفذ المحلي الفعلي للمقبس بعد الربط بـ 0
            // (EN) Get the actual local port number after binding with port 0 (ephemeral)
            {
                auto f = [](Sad::Interpreter::BuiltinContext &ctx)
                    -> std::shared_ptr<Data::Value>
                {
                const auto &args = ctx.args(); (void)args;
                    if (args.size() < 1)
                        return std::make_shared<Data::Value>(static_cast<double>(-1));
                    int64_t id = static_cast<int64_t>(args[0]->toDouble());
                    auto *entry = getSocket(id);
                    if (!entry)
                        return std::make_shared<Data::Value>(static_cast<double>(-1));

                    sockaddr_in localAddr;
                    std::memset(&localAddr, 0, sizeof(localAddr));
#ifdef _WIN32
                    int addrLen = sizeof(localAddr);
#else
                    socklen_t addrLen = sizeof(localAddr);
#endif
                    if (getsockname(entry->handle,
                                    reinterpret_cast<sockaddr *>(&localAddr),
                                    &addrLen) == SAD_SOCKET_ERROR)
                        return std::make_shared<Data::Value>(static_cast<double>(-1));

                    return std::make_shared<Data::Value>(
                        static_cast<double>(ntohs(localAddr.sin_port)));
                };
                fm.registerBuiltinFunction(std::string(Bsk::LOCAL_PORT), f); // منفذ_محلي
            }

        } // registerBuiltinsSockets

    } // namespace Interpreter
} // namespace Sad
