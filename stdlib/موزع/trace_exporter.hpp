/**
 * =============================================================================
 * ملف: trace_exporter.hpp
 * الوصف: تصدير البصمات للتحليل
 * المهمة: T1131 - Phase 110
 * =============================================================================
 * 
 * 📚 دليل المبتدئ لتصدير البصمات
 * ═══════════════════════════════════════
 * 
 * ما هو التصدير؟
 * ───────────────
 * 
 * تحويل بيانات التتبع لتنسيقات مختلفة
 * 
 *     ┌──────────────────────────────────────────────────────────────┐
 *     │                                                              │
 *     │    📤 تصدير البصمات                                          │
 *     │    ══════════════════                                        │
 *     │                                                              │
 *     │    البصمة ─────────► تنسيقات متعددة:                        │
 *     │                                                              │
 *     │    ┌────────────────────────────────────────┐               │
 *     │    │                                        │               │
 *     │    │  JSON     → تحليل مخصص                │               │
 *     │    │  Jaeger   → واجهة Jaeger UI           │               │
 *     │    │  Zipkin   → واجهة Zipkin              │               │
 *     │    │  OpenTelemetry → معيار موحد          │               │
 *     │    │  CSV      → تحليل في Excel           │               │
 *     │    │                                        │               │
 *     │    └────────────────────────────────────────┘               │
 *     │                                                              │
 *     │    الاستخدام:                                                │
 *     │    ┌────────────────────────────────────────┐               │
 *     │    │ مصدر.صدر_jaeger(بصمة, ملف)            │               │
 *     │    │ مصدر.صدر_json(بصمة, ملف)              │               │
 *     │    └────────────────────────────────────────┘               │
 *     │                                                              │
 *     └──────────────────────────────────────────────────────────────┘
 * 
 * =============================================================================
 */

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace sad {
namespace موزع {

// Forward declaration
struct بصمة_كونية;
struct نقطة_مسار;

// ═══════════════════════════════════════════════════════════════════════════════
//                              مُصدِّر البصمات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📤 مُصدِّر البصمات
 */
class مصدر_بصمات {
public:
    /**
     * تصدير إلى JSON
     */
    static std::string صدر_json(const بصمة_كونية& بصمة) {
        std::ostringstream json;
        json << "{\n";
        json << "  \"traceId\": \"" << بصمة.المعرف << "\",\n";
        json << "  \"spans\": [\n";
        
        for (size_t i = 0; i < بصمة.المسار.size(); i++) {
            const auto& نقطة = بصمة.المسار[i];
            json << "    {\n";
            json << "      \"service\": \"" << نقطة.الخدمة << "\",\n";
            json << "      \"operation\": \"" << نقطة.العملية << "\",\n";
            json << "      \"duration_ms\": " << نقطة.المدة_ms() << ",\n";
            json << "      \"status\": \"" << نقطة.الحالة << "\"\n";
            json << "    }";
            if (i < بصمة.المسار.size() - 1) json << ",";
            json << "\n";
        }
        
        json << "  ]\n";
        json << "}";
        
        return json.str();
    }
    
    /**
     * تصدير إلى تنسيق Jaeger
     */
    static std::string صدر_jaeger(const بصمة_كونية& بصمة) {
        std::ostringstream json;
        json << "{\n";
        json << "  \"data\": [{\n";
        json << "    \"traceID\": \"" << بصمة.المعرف << "\",\n";
        json << "    \"spans\": [\n";
        
        for (size_t i = 0; i < بصمة.المسار.size(); i++) {
            const auto& نقطة = بصمة.المسار[i];
            json << "      {\n";
            json << "        \"traceID\": \"" << بصمة.المعرف << "\",\n";
            json << "        \"spanID\": \"span" << i << "\",\n";
            json << "        \"operationName\": \"" << نقطة.العملية << "\",\n";
            json << "        \"serviceName\": \"" << نقطة.الخدمة << "\",\n";
            json << "        \"duration\": " << static_cast<int>(نقطة.المدة_ms() * 1000) << ",\n";
            json << "        \"tags\": [\n";
            json << "          {\"key\": \"status\", \"value\": \"" << نقطة.الحالة << "\"}\n";
            json << "        ]\n";
            json << "      }";
            if (i < بصمة.المسار.size() - 1) json << ",";
            json << "\n";
        }
        
        json << "    ],\n";
        json << "    \"processes\": {}\n";
        json << "  }]\n";
        json << "}";
        
        return json.str();
    }
    
    /**
     * تصدير إلى تنسيق Zipkin
     */
    static std::string صدر_zipkin(const بصمة_كونية& بصمة) {
        std::ostringstream json;
        json << "[\n";
        
        for (size_t i = 0; i < بصمة.المسار.size(); i++) {
            const auto& نقطة = بصمة.المسار[i];
            json << "  {\n";
            json << "    \"traceId\": \"" << بصمة.المعرف << "\",\n";
            json << "    \"id\": \"" << i << "\",\n";
            json << "    \"name\": \"" << نقطة.العملية << "\",\n";
            json << "    \"duration\": " << static_cast<int>(نقطة.المدة_ms() * 1000) << ",\n";
            json << "    \"localEndpoint\": {\n";
            json << "      \"serviceName\": \"" << نقطة.الخدمة << "\"\n";
            json << "    }\n";
            json << "  }";
            if (i < بصمة.المسار.size() - 1) json << ",";
            json << "\n";
        }
        
        json << "]";
        return json.str();
    }
    
    /**
     * تصدير إلى تنسيق OpenTelemetry
     */
    static std::string صدر_otlp(const بصمة_كونية& بصمة) {
        std::ostringstream json;
        json << "{\n";
        json << "  \"resourceSpans\": [{\n";
        json << "    \"resource\": {\n";
        json << "      \"attributes\": []\n";
        json << "    },\n";
        json << "    \"scopeSpans\": [{\n";
        json << "      \"scope\": {\"name\": \"sad-tracer\"},\n";
        json << "      \"spans\": [\n";
        
        for (size_t i = 0; i < بصمة.المسار.size(); i++) {
            const auto& نقطة = بصمة.المسار[i];
            json << "        {\n";
            json << "          \"traceId\": \"" << بصمة.المعرف << "\",\n";
            json << "          \"spanId\": \"span" << i << "\",\n";
            json << "          \"name\": \"" << نقطة.العملية << "\",\n";
            json << "          \"kind\": 1,\n";  // INTERNAL
            json << "          \"attributes\": [\n";
            json << "            {\"key\": \"service\", \"value\": {\"stringValue\": \"" << نقطة.الخدمة << "\"}},\n";
            json << "            {\"key\": \"status\", \"value\": {\"stringValue\": \"" << نقطة.الحالة << "\"}}\n";
            json << "          ]\n";
            json << "        }";
            if (i < بصمة.المسار.size() - 1) json << ",";
            json << "\n";
        }
        
        json << "      ]\n";
        json << "    }]\n";
        json << "  }]\n";
        json << "}";
        
        return json.str();
    }
    
    /**
     * تصدير إلى CSV
     */
    static std::string صدر_csv(const بصمة_كونية& بصمة) {
        std::ostringstream csv;
        csv << "trace_id,span_index,service,operation,duration_ms,status\n";
        
        for (size_t i = 0; i < بصمة.المسار.size(); i++) {
            const auto& نقطة = بصمة.المسار[i];
            csv << بصمة.المعرف << ","
                << i << ","
                << نقطة.الخدمة << ","
                << نقطة.العملية << ","
                << نقطة.المدة_ms() << ","
                << نقطة.الحالة << "\n";
        }
        
        return csv.str();
    }
    
    /**
     * حفظ في ملف
     */
    static bool احفظ(const std::string& محتوى, const std::string& مسار) {
        std::ofstream ملف(مسار);
        if (!ملف.is_open()) {
            return false;
        }
        ملف << محتوى;
        return true;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                              مُرسل البصمات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📡 مُرسل البصمات لأنظمة المراقبة
 */
class مرسل_بصمات {
public:
    /**
     * إرسال إلى Jaeger
     */
    static bool أرسل_jaeger(const بصمة_كونية& بصمة, 
                             const std::string& خادم = "http://localhost:14268") {
        std::string json = مصدر_بصمات::صدر_jaeger(بصمة);
        // TODO: HTTP POST إلى خادم/api/traces
        return true;
    }
    
    /**
     * إرسال إلى Zipkin
     */
    static bool أرسل_zipkin(const بصمة_كونية& بصمة,
                             const std::string& خادم = "http://localhost:9411") {
        std::string json = مصدر_بصمات::صدر_zipkin(بصمة);
        // TODO: HTTP POST إلى خادم/api/v2/spans
        return true;
    }
    
    /**
     * إرسال إلى OTLP Collector
     */
    static bool أرسل_otlp(const بصمة_كونية& بصمة,
                           const std::string& خادم = "http://localhost:4318") {
        std::string json = مصدر_بصمات::صدر_otlp(بصمة);
        // TODO: HTTP POST إلى خادم/v1/traces
        return true;
    }
};

} // namespace موزع
} // namespace sad
