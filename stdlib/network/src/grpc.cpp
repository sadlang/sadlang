// ===================================================================
// gRPC_عربي - ربط gRPC بالعربية
// grpc.cpp - Arabic gRPC Bindings
// ===================================================================
// يوفر واجهة عربية لبروتوكول gRPC:
// - تعريف خدمات بالعربية
// - خادم وعميل gRPC
// - Streaming أحادي وثنائي الاتجاه
// ===================================================================

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <optional>

namespace sad {
namespace stdlib {
namespace network {

/// حالة gRPC
enum class GrpcStatus {
    OK = 0,                  // نجاح
    CANCELLED = 1,            // ملغى
    UNKNOWN = 2,              // غير معروف
    INVALID_ARGUMENT = 3,     // وسيط غير صالح
    DEADLINE_EXCEEDED = 4,    // انتهت المهلة
    NOT_FOUND = 5,            // غير موجود
    ALREADY_EXISTS = 6,       // موجود مسبقاً
    PERMISSION_DENIED = 7,    // مرفوض
    UNAUTHENTICATED = 16,     // غير مُصادق
    UNAVAILABLE = 14,         // غير متاح
    INTERNAL = 13,            // خطأ داخلي
};

/// تحويل حالة gRPC إلى رسالة عربية
inline std::string grpcStatusToArabic(GrpcStatus status) {
    switch (status) {
        case GrpcStatus::OK: return "نجاح";
        case GrpcStatus::CANCELLED: return "تم الإلغاء";
        case GrpcStatus::UNKNOWN: return "خطأ غير معروف";
        case GrpcStatus::INVALID_ARGUMENT: return "وسيط غير صالح";
        case GrpcStatus::DEADLINE_EXCEEDED: return "انتهت المهلة الزمنية";
        case GrpcStatus::NOT_FOUND: return "غير موجود";
        case GrpcStatus::ALREADY_EXISTS: return "موجود مسبقاً";
        case GrpcStatus::PERMISSION_DENIED: return "الوصول مرفوض";
        case GrpcStatus::UNAUTHENTICATED: return "غير مُصادق عليه";
        case GrpcStatus::UNAVAILABLE: return "الخدمة غير متاحة";
        case GrpcStatus::INTERNAL: return "خطأ داخلي في الخادم";
    }
    return "خطأ غير محدد";
}

/// نتيجة استدعاء gRPC
template<typename T>
struct GrpcResult {
    GrpcStatus status;
    std::optional<T> value;
    std::string message_ar;
    
    bool isOk() const { return status == GrpcStatus::OK; }
    
    static GrpcResult ok(T val) {
        return {GrpcStatus::OK, std::move(val), "نجاح"};
    }
    
    static GrpcResult error(GrpcStatus s, const std::string& msg = "") {
        return {s, std::nullopt, msg.empty() ? grpcStatusToArabic(s) : msg};
    }
};

/// تعريف طريقة gRPC
struct GrpcMethodDef {
    std::string name;           // اسم الطريقة
    std::string name_ar;        // الاسم العربي
    bool client_streaming;      // هل العميل يبث؟
    bool server_streaming;      // هل الخادم يبث؟
    std::string input_type;     // نوع الإدخال
    std::string output_type;    // نوع الإخراج
};

/// تعريف خدمة gRPC
class GrpcServiceDef {
public:
    /// إنشاء تعريف خدمة
    GrpcServiceDef(const std::string& name, const std::string& name_ar)
        : name_(name), name_ar_(name_ar) {}
    
    /// إضافة طريقة أحادية (Unary)
    GrpcServiceDef& طريقة(const std::string& name, 
                           const std::string& input, 
                           const std::string& output) {
        methods_.push_back({name, name, false, false, input, output});
        return *this;
    }
    
    /// إضافة طريقة بث من الخادم
    GrpcServiceDef& بث_خادم(const std::string& name,
                              const std::string& input,
                              const std::string& output) {
        methods_.push_back({name, name, false, true, input, output});
        return *this;
    }
    
    /// إضافة طريقة بث من العميل
    GrpcServiceDef& بث_عميل(const std::string& name,
                              const std::string& input,
                              const std::string& output) {
        methods_.push_back({name, name, true, false, input, output});
        return *this;
    }
    
    /// إضافة طريقة بث ثنائي
    GrpcServiceDef& بث_ثنائي(const std::string& name,
                               const std::string& input,
                               const std::string& output) {
        methods_.push_back({name, name, true, true, input, output});
        return *this;
    }
    
    const std::string& name() const { return name_; }
    const std::string& nameAr() const { return name_ar_; }
    const std::vector<GrpcMethodDef>& methods() const { return methods_; }

private:
    std::string name_;
    std::string name_ar_;
    std::vector<GrpcMethodDef> methods_;
};

/// خادم gRPC
class GrpcServer {
public:
    /// إنشاء خادم
    explicit GrpcServer(int port = 50051) : port_(port) {}
    
    /// إضافة خدمة
    GrpcServer& أضف_خدمة(const GrpcServiceDef& service) {
        services_.push_back(service);
        return *this;
    }
    
    /// تحديد المنفذ
    GrpcServer& منفذ(int port) {
        port_ = port;
        return *this;
    }
    
    /// تفعيل TLS
    GrpcServer& tls(const std::string& cert_path, const std::string& key_path) {
        tls_cert_ = cert_path;
        tls_key_ = key_path;
        return *this;
    }
    
    /// تحديد أقصى عدد اتصالات
    GrpcServer& أقصى_اتصالات(int max) {
        max_connections_ = max;
        return *this;
    }
    
    /// بدء الخادم
    void ابدأ() {
        running_ = true;
        // التنفيذ الفعلي مع مكتبة gRPC
    }
    
    /// إيقاف الخادم
    void أوقف() {
        running_ = false;
    }
    
    bool isRunning() const { return running_; }
    int port() const { return port_; }

private:
    int port_;
    int max_connections_ = 100;
    bool running_ = false;
    std::string tls_cert_;
    std::string tls_key_;
    std::vector<GrpcServiceDef> services_;
};

/// عميل gRPC
class GrpcClient {
public:
    /// إنشاء عميل
    explicit GrpcClient(const std::string& address)
        : address_(address) {}
    
    /// الاتصال بالخادم
    bool اتصل() {
        connected_ = true;
        return true;
    }
    
    /// قطع الاتصال
    void اقطع() {
        connected_ = false;
    }
    
    /// هل متصل؟
    bool متصل() const { return connected_; }
    
    /// تحديد مهلة الاستدعاء
    GrpcClient& مهلة(std::chrono::milliseconds ms) {
        timeout_ = ms;
        return *this;
    }
    
    /// تفعيل TLS
    GrpcClient& tls(const std::string& ca_cert) {
        ca_cert_ = ca_cert;
        return *this;
    }

private:
    std::string address_;
    bool connected_ = false;
    std::chrono::milliseconds timeout_{30000};
    std::string ca_cert_;
};

/// إنشاء تعريف خدمة
inline GrpcServiceDef خدمة_grpc(const std::string& name, 
                                  const std::string& name_ar = "") {
    return GrpcServiceDef(name, name_ar.empty() ? name : name_ar);
}

/// إنشاء خادم
inline GrpcServer خادم_grpc(int port = 50051) {
    return GrpcServer(port);
}

/// إنشاء عميل
inline GrpcClient عميل_grpc(const std::string& address) {
    return GrpcClient(address);
}

} // namespace network
} // namespace stdlib
} // namespace sad
