// ============================================================================
// (AR) ملف: sir_builder_builtins_network.cpp
// (EN) File: sir_builder_builtins_network.cpp
// المكون: Sad::Compiler::SIR
// المسار: compiler/src/frontend/sir_builder_builtins_network.cpp
// ============================================================================
// (AR) بناء تعليمات SIR للدوال المضمنة الخاصة بوحدة الشبكة.
//      يشمل: مقابس TCP/UDP، عميل HTTP، خادم HTTP، أدوات الشبكة، العناوين.
//      كل دالة عربية تُربط بـ SIROpcode مقابل (BUILTIN_NET_*) الذي يُترجم
//      لاحقاً في LLVM codegen إلى استدعاء دالة C API من features/network/core.
//      الأسماء العربية مأخوذة من shared/builtins/include/builtin_registry.h
//      (فضاء Sad::Builtins::Names — أقسام Sockets, HttpClient, HttpServer, NetworkUtils).
//      النمط المتبع مطابق لـ sir_builder_builtins_core.cpp و sir_builder_builtins_system.cpp:
//      - فحص اسم الدالة بالعربية
//      - بناء SIRInstruction بالـ opcode المناسب
//      - تمرير المعاملات كـ SIROperand
//      - إرجاع BuildResult بنوع القيمة المُرجعة
//
// (EN) Builds SIR instructions for network module builtin functions.
//      Covers: TCP/UDP sockets, HTTP client, HTTP server, network utilities, addresses.
//      Each Arabic function name maps to a BUILTIN_NET_* SIROpcode which is later
//      translated in LLVM codegen to a C API call from features/network/core.
//      Arabic names come from shared/builtins/include/builtin_registry.h
//      (Sad::Builtins::Names — Sockets, HttpClient, HttpServer, NetworkUtils sections).
//      Pattern follows sir_builder_builtins_core.cpp and sir_builder_builtins_system.cpp:
//      - Check Arabic function name
//      - Build SIRInstruction with appropriate opcode
//      - Pass arguments as SIROperand
//      - Return BuildResult with correct return type
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_builder.h"
#include "builtin_registry.h"
#include "builders/builtin_arity_check.h"
#include <optional>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // (AR) أسماء مختصرة لفضاءات الأسماء — لتسهيل القراءة
            // (EN) Short namespace aliases — for readability
            // ============================================================================
            namespace Bsk = Sad::Builtins::Names::Sockets;
            namespace Bhc = Sad::Builtins::Names::HttpClient;
            namespace Bhs = Sad::Builtins::Names::HttpServer;
            namespace Bnu = Sad::Builtins::Names::NetworkUtils;
            namespace Bwsc = Sad::Builtins::Names::WebSocketClient;
            namespace Bwss = Sad::Builtins::Names::WebSocketServer;

            // (AR) رتبةُ المدمجِ من حقلِ `arity` في مصدرِ الحقيقةِ — ثابتٌ مُولَّدٌ لا
            //      رقمٌ يُكتَب. وكانت أذرعُ هذا الملفِّ تمرّرُ `argOperands` جملةً بلا
            //      فحصِ رتبةٍ البتّة، فالنداءُ الناقصُ يُترجَمُ ويقرأُ خانةً غيرَ
            //      موجودة. ولا أساسَ ههنا يُشتقُّ منه العقد — فاشتُقَّ من المفسّرِ
            //      حيث الشرطُ مكتوبٌ صراحةً، ثمّ بُذِر في مصدرِ الحقيقة.
            namespace Ar = Sad::Builtins::Arity;

            // ============================================================================
            // (AR) دالة مساعدة: بناء تعليمة SIR بسيطة لدالة شبكة
            //      تُنشئ SIRInstruction بالـ opcode المحدد، تضيف المعاملات،
            //      وتُرجع BuildResult بالنوع المحدد.
            //      تُقلل التكرار بين 68 دالة مضمنة.
            // (EN) Helper: build a simple SIR instruction for a network function.
            //      Creates SIRInstruction with given opcode, adds operands,
            //      and returns BuildResult with specified return type.
            //      Reduces repetition across 68 builtin functions.
            // ============================================================================
            BuildResult BuiltinBuilder::buildNetworkBuiltinInstruction(
                SIROpcode opcode,
                std::vector<SIROperand> &argOperands,
                SadTypeKind returnType,
                const char *comment)
            {
                std::string resultReg = b_.newTempRegister();
                SIRInstruction inst(opcode);
                inst.result = SIROperand::Register(resultReg, returnType);

                for (auto &arg : argOperands)
                {
                    inst.operands.push_back(arg);
                }

                inst.comment = comment;

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(inst);
                }

                return BuildResult(resultReg, returnType);
            }

            // ============================================================================
            // (AR) الدالة الرئيسية: معالجة استدعاء دالة مدمجة للشبكة
            //      تُستدعى من sir_builder_calls.cpp بعد b_.buildBuiltinCallSystem.
            //      تتحقق من اسم الدالة مقابل أسماء builtin_registry.h وتُصدر
            //      تعليمة SIR المناسبة.
            // (EN) Main function: handle network builtin function call.
            //      Called from sir_builder_calls.cpp after b_.buildBuiltinCallSystem.
            //      Checks function name against builtin_registry.h names and emits
            //      the appropriate SIR instruction.
            // ============================================================================
            std::optional<BuildResult> BuiltinBuilder::buildBuiltinCallNetwork(
                const std::string &funcName,
                bool isUserDefinedFunction,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // (AR) إذا كانت الدالة معرّفة من المستخدم، لا تعامل كدالة مضمنة
                // (EN) If function is user-defined, skip all builtins
                if (isUserDefinedFunction)
                    return std::nullopt;

                // ================================================================
                // --- (AR) القسم أ: مقابس TCP / (EN) Section A: TCP Sockets ---
                // ================================================================

                // (AR) أنشئ_وصلة() — إنشاء مقبس TCP جديد
                // (EN) Create new TCP socket — returns handle (i64)
                if (funcName == Bsk::TCP_NEW)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_NEW, argOperands,
                        SadTypeKind::Integer, "أنشئ_وصلة / tcp_socket_new");
                }

                // (AR) اتصل_بـ(وصلة، عنوان، منفذ) — الاتصال بخادم
                // (EN) Connect TCP socket to server — returns i64 (0=ok, -1=error)
                if (funcName == Bsk::TCP_CONNECT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_CONNECT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_CONNECT, argOperands,
                        SadTypeKind::Boolean, "اتصل_بـ / tcp_connect");
                }

                // (AR) أرسل_عبر_وصلة(وصلة، بيانات) — إرسال بيانات
                // (EN) Send data through TCP socket — returns bytes sent (i64)
                if (funcName == Bsk::TCP_SEND)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_SEND, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_SEND, argOperands,
                        SadTypeKind::Integer, "أرسل_عبر_وصلة / tcp_send");
                }

                // (AR) استقبل_عبر_وصلة(وصلة، حجم) — استقبال بيانات
                // (EN) Receive data from TCP socket — returns string
                if (funcName == Bsk::TCP_RECV)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_RECV, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_RECV, argOperands,
                        SadTypeKind::String, "استقبل_عبر_وصلة / tcp_recv");
                }

                // (AR) أغلق_وصلة(وصلة) — إغلاق المقبس
                // (EN) Close TCP socket
                if (funcName == Bsk::TCP_CLOSE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_CLOSE, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_CLOSE, argOperands,
                        SadTypeKind::Boolean, "أغلق_وصلة / tcp_close");
                }

                // (AR) خصص_منفذ(وصلة، عنوان، منفذ) — ربط المقبس بمنفذ
                // (EN) Bind TCP socket to address and port
                if (funcName == Bsk::TCP_BIND)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_BIND, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_BIND, argOperands,
                        SadTypeKind::Boolean, "خصص_منفذ / tcp_bind");
                }

                // (AR) انتظر_اتصالات(وصلة، حد) — بدء الاستماع
                // (EN) Start listening for connections
                if (funcName == Bsk::TCP_LISTEN)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_LISTEN, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_LISTEN, argOperands,
                        SadTypeKind::Boolean, "انتظر_اتصالات / tcp_listen");
                }

                // (AR) اقبل_متصل(وصلة) — قبول اتصال وارد
                // (EN) Accept incoming connection — returns new socket handle
                if (funcName == Bsk::TCP_ACCEPT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_ACCEPT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_ACCEPT, argOperands,
                        SadTypeKind::Integer, "اقبل_متصل / tcp_accept");
                }

                // (AR) هل_متصلة(وصلة) — هل المقبس متصل
                // (EN) Is TCP socket connected — returns boolean (i64: 0/1)
                if (funcName == Bsk::TCP_CONNECTED)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_CONNECTED, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_CONNECTED, argOperands,
                        SadTypeKind::Boolean, "هل_متصلة / tcp_is_connected");
                }

                // (AR) عنوان_الطرف_الآخر(وصلة) — الحصول على عنوان الطرف المتصل
                // (EN) Get remote peer address — returns string
                if (funcName == Bsk::TCP_REMOTE_ADDR)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::TCP_REMOTE_ADDR, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_TCP_REMOTE_ADDR, argOperands,
                        SadTypeKind::String, "عنوان_الطرف_الآخر / tcp_remote_addr");
                }

                // ================================================================
                // --- (AR) القسم ب: مقابس UDP / (EN) Section B: UDP Sockets ---
                // ================================================================

                if (funcName == Bsk::UDP_NEW)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_UDP_NEW, argOperands,
                        SadTypeKind::Integer, "أنشئ_مرسال / udp_socket_new");
                }

                if (funcName == Bsk::UDP_BIND)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::UDP_BIND, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_UDP_BIND, argOperands,
                        SadTypeKind::Boolean, "خصص_منفذ_رسائل / udp_bind");
                }

                if (funcName == Bsk::UDP_SEND)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::UDP_SEND, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_UDP_SEND, argOperands,
                        SadTypeKind::Integer, "أرسل_رسالة / udp_send_to");
                }

                if (funcName == Bsk::UDP_RECV)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::UDP_RECV, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_UDP_RECV, argOperands,
                        SadTypeKind::String, "استقبل_رسالة / udp_recv_from");
                }

                if (funcName == Bsk::UDP_CLOSE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::UDP_CLOSE, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_UDP_CLOSE, argOperands,
                        SadTypeKind::Boolean, "أغلق_مرسال / udp_close");
                }

                // ================================================================
                // --- (AR) القسم ج: إعدادات المقابس / (EN) Section C: Socket Settings ---
                // ================================================================

                if (funcName == Bsk::RECV_TIMEOUT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::RECV_TIMEOUT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RECV_TIMEOUT, argOperands,
                        SadTypeKind::Boolean, "حدد_انتظار_الاستقبال / set_recv_timeout");
                }

                if (funcName == Bsk::SEND_TIMEOUT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::SEND_TIMEOUT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SEND_TIMEOUT, argOperands,
                        SadTypeKind::Boolean, "حدد_انتظار_الإرسال / set_send_timeout");
                }

                if (funcName == Bsk::NO_DELAY)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::NO_DELAY, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_NO_DELAY, argOperands,
                        SadTypeKind::Boolean, "أرسل_فوراً / set_nodelay");
                }

                if (funcName == Bsk::LOCAL_PORT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::Sockets::LOCAL_PORT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_LOCAL_PORT, argOperands,
                        SadTypeKind::Integer, "منفذ_محلي / local_port");
                }

                // ================================================================
                // --- (AR) القسم د: عميل HTTP / (EN) Section D: HTTP Client ---
                // ================================================================

                if (funcName == Bhc::NEW_CLIENT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_NEW_CLIENT, argOperands,
                        SadTypeKind::Integer, "أنشئ_متصفح / http_client_new");
                }

                if (funcName == Bhc::FREE_CLIENT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::FREE_CLIENT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_FREE_CLIENT, argOperands,
                        SadTypeKind::Boolean, "أغلق_متصفح / http_client_free");
                }

                if (funcName == Bhc::GET)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::GET, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_GET, argOperands,
                        SadTypeKind::Integer, "اجلب / http_get");
                }

                if (funcName == Bhc::POST)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::POST, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_POST, argOperands,
                        SadTypeKind::Integer, "أرسل / http_post");
                }

                if (funcName == Bhc::PUT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::PUT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_PUT, argOperands,
                        SadTypeKind::Integer, "استبدل / http_put");
                }

                if (funcName == Bhc::DELETE_REQ)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::DELETE_REQ, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_DELETE, argOperands,
                        SadTypeKind::Integer, "احذف_مورد / http_delete");
                }

                if (funcName == Bhc::PATCH)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::PATCH, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_PATCH, argOperands,
                        SadTypeKind::Integer, "عدّل_مورد / http_patch");
                }

                if (funcName == Bhc::SET_BASE_URL)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::SET_BASE_URL, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_SET_BASE, argOperands,
                        SadTypeKind::Boolean, "حدد_الموقع / set_base_url");
                }

                if (funcName == Bhc::SET_HEADER)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::SET_HEADER, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_SET_HEADER, argOperands,
                        SadTypeKind::Boolean, "أضف_ترويسة / set_header");
                }

                if (funcName == Bhc::SET_TIMEOUT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::SET_TIMEOUT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_SET_TIMEOUT, argOperands,
                        SadTypeKind::Boolean, "حدد_الانتظار / set_timeout");
                }

                if (funcName == Bhc::SET_BEARER)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::SET_BEARER, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_SET_BEARER, argOperands,
                        SadTypeKind::Boolean, "سجّل_دخول_برمز / set_bearer_token");
                }

                if (funcName == Bhc::LAST_ERROR)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::LAST_ERROR, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_LAST_ERROR, argOperands,
                        SadTypeKind::String, "سبب_الفشل / last_error");
                }

                if (funcName == Bhc::IS_OK)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::IS_OK, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_HTTP_IS_OK, argOperands,
                        SadTypeKind::Boolean, "هل_نجح / is_ok");
                }

                // ================================================================
                // --- (AR) القسم هـ: استجابة HTTP / (EN) Section E: HTTP Response ---
                // ================================================================

                if (funcName == Bhc::RESP_FREE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::RESP_FREE, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_FREE, argOperands,
                        SadTypeKind::Boolean, "تجاهل_الرد / response_free");
                }

                if (funcName == Bhc::RESP_STATUS)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::RESP_STATUS, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_STATUS, argOperands,
                        SadTypeKind::Integer, "رمز_الحالة / response_status");
                }

                if (funcName == Bhc::RESP_BODY)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::RESP_BODY, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_BODY, argOperands,
                        SadTypeKind::String, "نص_الرد / response_body");
                }

                if (funcName == Bhc::RESP_HEADER)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::RESP_HEADER, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_HEADER, argOperands,
                        SadTypeKind::String, "معلومة_الرد / response_header");
                }

                if (funcName == Bhc::RESP_SUCCESS)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpClient::RESP_SUCCESS, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_SUCCESS, argOperands,
                        SadTypeKind::Boolean, "هل_الرد_ناجح / response_is_success");
                }

                // ================================================================
                // --- (AR) القسم و: خادم HTTP / (EN) Section F: HTTP Server ---
                // ================================================================

                if (funcName == Bhs::NEW_SERVER)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::NEW_SERVER, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_NEW, argOperands,
                        SadTypeKind::Integer, "أنشئ_خادم / http_server_new");
                }

                if (funcName == Bhs::FREE_SERVER)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::FREE_SERVER, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_FREE, argOperands,
                        SadTypeKind::Boolean, "أزل_خادم / http_server_free");
                }

                if (funcName == Bhs::ON_GET)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::ON_GET, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_ON_GET, argOperands,
                        SadTypeKind::Boolean, "عند_طلب_جلب / server_get_cb");
                }

                if (funcName == Bhs::ON_POST)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_ON_POST, argOperands,
                        SadTypeKind::Boolean, "عند_طلب_إرسال / server_post_cb");
                }

                if (funcName == Bhs::ON_PUT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_ON_PUT, argOperands,
                        SadTypeKind::Boolean, "عند_طلب_استبدال / server_put_cb");
                }

                if (funcName == Bhs::ON_DELETE)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_ON_DELETE, argOperands,
                        SadTypeKind::Boolean, "عند_طلب_حذف / server_delete_cb");
                }

                if (funcName == Bhs::LISTEN)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::LISTEN, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_LISTEN, argOperands,
                        SadTypeKind::Boolean, "ابدأ_الاستماع / server_listen");
                }

                if (funcName == Bhs::STOP)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::STOP, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_STOP, argOperands,
                        SadTypeKind::Boolean, "أوقف_الخادم / server_stop");
                }

                if (funcName == Bhs::ENABLE_CORS)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::ENABLE_CORS, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_SRV_CORS, argOperands,
                        SadTypeKind::Boolean, "اسمح_بالوصول_الخارجي / server_enable_cors");
                }

                // ================================================================
                // --- (AR) القسم ز: بيانات الطلب / (EN) Section G: Request Data ---
                // ================================================================

                if (funcName == Bhs::REQ_METHOD)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_REQ_METHOD, argOperands,
                        SadTypeKind::String, "نوع_الطلب / request_method");
                }

                if (funcName == Bhs::REQ_PATH)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_REQ_PATH, argOperands,
                        SadTypeKind::String, "وجهة_الطلب / request_path");
                }

                if (funcName == Bhs::REQ_BODY)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_REQ_BODY, argOperands,
                        SadTypeKind::String, "بيانات_الطلب / request_body");
                }

                if (funcName == Bhs::REQ_HEADER)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::REQ_HEADER, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_REQ_HEADER, argOperands,
                        SadTypeKind::String, "معلومة_الطلب / request_header");
                }

                if (funcName == Bhs::REQ_QUERY)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::REQ_QUERY, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_REQ_QUERY, argOperands,
                        SadTypeKind::String, "قيمة_من_الرابط / request_query_param");
                }

                // ================================================================
                // --- (AR) القسم ح: بناء الاستجابة / (EN) Section H: Response Building ---
                // ================================================================

                if (funcName == Bhs::RESP_SET_STATUS)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::RESP_SET_STATUS, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_SET_STATUS, argOperands,
                        SadTypeKind::Boolean, "عيّن_حالة_الرد / response_set_status");
                }

                if (funcName == Bhs::RESP_SET_BODY)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::RESP_SET_BODY, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_SET_BODY, argOperands,
                        SadTypeKind::Boolean, "عيّن_نص_الرد / response_set_body");
                }

                if (funcName == Bhs::RESP_SET_JSON)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::RESP_SET_JSON, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_SET_JSON, argOperands,
                        SadTypeKind::Boolean, "عيّن_رد_جيسون / response_set_json");
                }

                if (funcName == Bhs::RESP_SET_HTML)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::RESP_SET_HTML, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_SET_HTML, argOperands,
                        SadTypeKind::Boolean, "عيّن_رد_صفحة / response_set_html");
                }

                if (funcName == Bhs::RESP_SET_HEADER)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::HttpServer::RESP_SET_HEADER, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_RESP_SET_HEADER, argOperands,
                        SadTypeKind::Boolean, "عيّن_ترويسة_الرد / response_set_header");
                }

                // ================================================================
                // --- (AR) القسم ط: أدوات الشبكة / (EN) Section I: Network Utilities ---
                // ================================================================

                if (funcName == Bnu::INIT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_INIT, argOperands,
                        SadTypeKind::Boolean, "جهّز_الشبكة / network_init");
                }

                if (funcName == Bnu::CLEANUP)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_CLEANUP, argOperands,
                        SadTypeKind::Boolean, "نظّف_الشبكة / network_cleanup");
                }

                if (funcName == Bnu::LAST_ERROR_CODE)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ERROR_CODE, argOperands,
                        SadTypeKind::Integer, "رمز_آخر_خطأ / last_error_code");
                }

                if (funcName == Bnu::LAST_ERROR_MSG)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ERROR_MSG, argOperands,
                        SadTypeKind::String, "وصف_آخر_خطأ / last_error_message");
                }

                // ================================================================
                // --- (AR) القسم ي: العناوين / (EN) Section J: Addresses ---
                // ================================================================

                if (funcName == Bnu::ADDR_NEW)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::NetworkUtils::ADDR_NEW, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ADDR_NEW, argOperands,
                        SadTypeKind::Integer, "عنوان / address_new");
                }

                if (funcName == Bnu::ADDR_NEW_V6)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::NetworkUtils::ADDR_NEW_V6, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ADDR_NEW_V6, argOperands,
                        SadTypeKind::Integer, "عنوان_حديث / address_new_v6");
                }

                if (funcName == Bnu::ADDR_FREE)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::NetworkUtils::ADDR_FREE, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ADDR_FREE, argOperands,
                        SadTypeKind::Boolean, "حرر_عنوان / address_free");
                }

                if (funcName == Bnu::ADDR_IP)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::NetworkUtils::ADDR_IP, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ADDR_IP, argOperands,
                        SadTypeKind::String, "رقم_الجهاز / address_ip");
                }

                if (funcName == Bnu::ADDR_PORT)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::NetworkUtils::ADDR_PORT, argOperands.size()))
                        return BuildResult("", SadTypeKind::Integer);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ADDR_PORT, argOperands,
                        SadTypeKind::Integer, "رقم_المنفذ / address_port");
                }

                if (funcName == Bnu::ADDR_IS_V4)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::NetworkUtils::ADDR_IS_V4, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ADDR_IS_V4, argOperands,
                        SadTypeKind::Boolean, "هل_عنوان_قديم / address_is_v4");
                }

                if (funcName == Bnu::ADDR_IS_V6)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::NetworkUtils::ADDR_IS_V6, argOperands.size()))
                        return BuildResult("", SadTypeKind::Boolean);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ADDR_IS_V6, argOperands,
                        SadTypeKind::Boolean, "هل_عنوان_حديث / address_is_v6");
                }

                if (funcName == Bnu::ADDR_STR)
                {
                    if (!checkBuiltinArity(b_.errors_, funcName,
                                           Ar::NetworkUtils::ADDR_STR, argOperands.size()))
                        return BuildResult("", SadTypeKind::String);
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_ADDR_STR, argOperands,
                        SadTypeKind::String, "العنوان_كنص / address_to_string");
                }

                // ================================================================
                // --- (AR) القسم ك: عميل WebSocket / (EN) Section K: WebSocket Client ---
                // ================================================================

                if (funcName == Bwsc::NEW_CLIENT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_NEW, argOperands,
                        SadTypeKind::Integer, "أنشئ_عميل_ويبسوكت / ws_client_new");
                }
                if (funcName == Bwsc::FREE_CLIENT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_FREE, argOperands,
                        SadTypeKind::Boolean, "أزل_عميل_ويبسوكت / ws_client_free");
                }
                if (funcName == Bwsc::CONNECT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_CONNECT, argOperands,
                        SadTypeKind::Boolean, "اتصل_بويبسوكت / ws_client_connect");
                }
                if (funcName == Bwsc::CLOSE)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_CLOSE, argOperands,
                        SadTypeKind::Boolean, "أغلق_ويبسوكت / ws_client_close");
                }
                if (funcName == Bwsc::IS_CONNECTED)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_IS_CONNECTED, argOperands,
                        SadTypeKind::Boolean, "هل_متصل_ويبسوكت / ws_client_is_connected");
                }
                if (funcName == Bwsc::SEND)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_SEND, argOperands,
                        SadTypeKind::Boolean, "أرسل_ويبسوكت / ws_client_send");
                }
                if (funcName == Bwsc::PING)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_PING, argOperands,
                        SadTypeKind::Boolean, "نبض_ويبسوكت / ws_client_ping");
                }
                if (funcName == Bwsc::RECEIVE)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_RECEIVE, argOperands,
                        SadTypeKind::String, "استقبل_ويبسوكت / ws_client_receive");
                }
                if (funcName == Bwsc::HAS_MESSAGE)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_HAS_MESSAGE, argOperands,
                        SadTypeKind::Boolean, "يوجد_رسالة_ويبسوكت / ws_client_has_message");
                }
                if (funcName == Bwsc::GET_URL)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_GET_URL, argOperands,
                        SadTypeKind::String, "رابط_ويبسوكت / ws_client_get_url");
                }
                if (funcName == Bwsc::LAST_ERROR)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_LAST_ERROR, argOperands,
                        SadTypeKind::String, "خطأ_ويبسوكت / ws_client_last_error");
                }
                if (funcName == Bwsc::SET_RECV_TIMEOUT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_RECV_TIMEOUT, argOperands,
                        SadTypeKind::Boolean, "حدد_مهلة_استقبال_ويبسوكت / ws_client_set_recv_timeout");
                }
                if (funcName == Bwsc::SET_SEND_TIMEOUT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_CLIENT_SEND_TIMEOUT, argOperands,
                        SadTypeKind::Boolean, "حدد_مهلة_إرسال_ويبسوكت / ws_client_set_send_timeout");
                }

                // ================================================================
                // --- (AR) القسم ل: خادم WebSocket / (EN) Section L: WebSocket Server ---
                // ================================================================

                if (funcName == Bwss::NEW_SERVER)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_NEW, argOperands,
                        SadTypeKind::Integer, "أنشئ_خادم_ويبسوكت / ws_server_new");
                }
                if (funcName == Bwss::FREE_SERVER)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_FREE, argOperands,
                        SadTypeKind::Boolean, "أزل_خادم_ويبسوكت / ws_server_free");
                }
                if (funcName == Bwss::START)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_START, argOperands,
                        SadTypeKind::Boolean, "ابدأ_خادم_ويبسوكت / ws_server_start");
                }
                if (funcName == Bwss::STOP)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_STOP, argOperands,
                        SadTypeKind::Boolean, "أوقف_خادم_ويبسوكت / ws_server_stop");
                }
                if (funcName == Bwss::IS_RUNNING)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_IS_RUNNING, argOperands,
                        SadTypeKind::Boolean, "هل_يعمل_خادم_ويبسوكت / ws_server_is_running");
                }
                if (funcName == Bwss::CONN_COUNT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_CONN_COUNT, argOperands,
                        SadTypeKind::Integer, "عدد_اتصالات_ويبسوكت / ws_server_connection_count");
                }
                if (funcName == Bwss::GET_PORT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_GET_PORT, argOperands,
                        SadTypeKind::Integer, "منفذ_خادم_ويبسوكت / ws_server_get_port");
                }
                if (funcName == Bwss::BROADCAST)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_BROADCAST, argOperands,
                        SadTypeKind::Integer, "بث_ويبسوكت / ws_server_broadcast");
                }
                if (funcName == Bwss::BROADCAST_ROOM)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_BROADCAST_ROOM, argOperands,
                        SadTypeKind::Integer, "بث_لغرفة_ويبسوكت / ws_server_broadcast_to_room");
                }
                if (funcName == Bwss::ROOM_COUNT)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_ROOM_COUNT, argOperands,
                        SadTypeKind::Integer, "عدد_غرف_ويبسوكت / ws_server_room_count");
                }
                if (funcName == Bwss::ROOM_SIZE)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_ROOM_SIZE, argOperands,
                        SadTypeKind::Integer, "حجم_غرفة_ويبسوكت / ws_server_room_size");
                }
                if (funcName == Bwss::CLOSE_ALL)
                {
                    return b_.buildNetworkBuiltinInstruction(
                        SIROpcode::BUILTIN_NET_WS_SERVER_CLOSE_ALL, argOperands,
                        SadTypeKind::Boolean, "أغلق_كل_اتصالات_ويبسوكت / ws_server_close_all");
                }

                // (AR) لم يُطابق أي اسم — الدالة ليست من وحدة الشبكة
                // (EN) No match — function is not from the network module
                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
