// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tools/sadinfo/src/dump_builtins.cpp
// (AR) إخراج كل الدوال المدمجة auto-loaded (~15 قانونية) والطرق المضمنة على
//      الأنواع (مصفوفات/نصوص/خرائط/قنوات) إلى JSON أو YAML بنفس فلسفة
//      dump_keywords.cpp: ترتيب حقول ثابت + تصفية + كاتب YAML اختياري عبر
//      yaml-cpp مع fallback يدوي.
//
//      الفلاتر المدعومة:
//        --filter category=printing|input|reflection|type_ctor|concurrency
//        --filter host=array|string|map|channel
//
// (EN) Dump all auto-loaded builtins (~15 canonical) and built-in methods on
//      host types into JSON or YAML, mirroring dump_keywords.cpp design:
//      stable field order, filtering, optional yaml-cpp with manual fallback.
// ═══════════════════════════════════════════════════════════════════════════════

#include "sadinfo.h"
#include "builtin_data.h"
#include "json_writer.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef SAD_HAS_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace Sad
{
    namespace Info
    {

        // ═══════════════════════════════════════════════════════════════════
        // (AR) قسم 1: أدوات مساعدة (escape، تطبيق فلاتر)
        // (EN) Section 1: helpers (escape, filter application)
        // ═══════════════════════════════════════════════════════════════════

        // (AR) هروب YAML بسيط — مطابق لما في dump_keywords.cpp
        static std::string yamlEscapeBuiltin(std::string_view s)
        {
            std::string out;
            out.reserve(s.size() + 2);
            out.push_back('"');
            for (char c : s)
            {
                if (c == '"' || c == '\\')
                    out.push_back('\\');
                out.push_back(c);
            }
            out.push_back('"');
            return out;
        }

        // (AR) يحلّل قيمة الفلتر "key=value" إلى زوج
        struct ParsedFilter
        {
            std::string key;
            std::string value;
            bool hasValue = false;
        };
        static ParsedFilter parseFilterExpr(const std::string &expr)
        {
            ParsedFilter f;
            auto eq = expr.find('=');
            if (eq == std::string::npos)
            {
                f.key = expr;
                return f;
            }
            f.key = expr.substr(0, eq);
            f.value = expr.substr(eq + 1);
            f.hasValue = true;
            return f;
        }

        // (AR) فلترة الدوال — تدعم category=
        static std::vector<BuiltinFunction>
        applyFunctionFilters(const std::vector<BuiltinFunction> &all,
                             const std::vector<std::string> &filters)
        {
            std::vector<BuiltinFunction> out;
            out.reserve(all.size());
            for (const auto &fn : all)
            {
                bool keep = true;
                for (const auto &raw : filters)
                {
                    auto pf = parseFilterExpr(raw);
                    if (pf.key == "category" && pf.hasValue)
                    {
                        if (categoryName(fn.category) != pf.value)
                        {
                            keep = false;
                            break;
                        }
                    }
                }
                if (keep)
                    out.push_back(fn);
            }
            return out;
        }

        // (AR) فلترة الطرق — تدعم host=
        static std::vector<BuiltinMethod>
        applyMethodFilters(const std::vector<BuiltinMethod> &all,
                           const std::vector<std::string> &filters)
        {
            std::vector<BuiltinMethod> out;
            out.reserve(all.size());
            for (const auto &m : all)
            {
                bool keep = true;
                for (const auto &raw : filters)
                {
                    auto pf = parseFilterExpr(raw);
                    if (pf.key == "host" && pf.hasValue)
                    {
                        if (hostTypeName(m.host) != pf.value)
                        {
                            keep = false;
                            break;
                        }
                    }
                }
                if (keep)
                    out.push_back(m);
            }
            return out;
        }

        // ═══════════════════════════════════════════════════════════════════
        // (AR) قسم 2: كتابة JSON
        // ═══════════════════════════════════════════════════════════════════

        static void emitParamJson(JsonWriter &w, const BuiltinParam &p,
                                  const DumpOptions &opts)
        {
            w.beginObject();
            w.kv("name", p.nameAr);
            if (opts.lang != "ar")
                w.kv("nameEn", p.nameEn);
            w.kv("type", p.typeAr);
            if (p.optional)
                w.kv("optional", true);
            if (p.variadic)
                w.kv("variadic", true);
            w.endObject();
        }

        static void emitFunctionJson(JsonWriter &w, const BuiltinFunction &fn,
                                     const DumpOptions &opts)
        {
            w.beginObject();
            w.kv("id", fn.id);
            w.kv("nameAr", fn.nameAr);
            if (opts.lang != "ar")
                w.kv("nameEn", fn.nameEn);
            w.kv("category", categoryName(fn.category));
            w.kv("returnType", fn.returnType);

            if (!opts.minimal)
            {
                w.kv("summaryAr", fn.summaryAr);
                if (opts.lang != "ar")
                    w.kv("summaryEn", fn.summaryEn);

                w.key("params");
                w.beginArray();
                for (const auto &p : fn.params)
                    emitParamJson(w, p, opts);
                w.endArray();

                if (!fn.example.empty())
                    w.kv("example", fn.example);
            }
            w.endObject();
        }

        static void emitMethodJson(JsonWriter &w, const BuiltinMethod &m,
                                   const DumpOptions &opts)
        {
            w.beginObject();
            w.kv("id", m.id);
            w.kv("host", hostTypeName(m.host));
            w.kv("nameAr", m.nameAr);
            if (opts.lang != "ar")
                w.kv("nameEn", m.nameEn);
            w.kv("returnType", m.returnType);

            if (!opts.minimal)
            {
                w.kv("summaryAr", m.summaryAr);
                if (opts.lang != "ar")
                    w.kv("summaryEn", m.summaryEn);
                w.key("params");
                w.beginArray();
                for (const auto &p : m.params)
                    emitParamJson(w, p, opts);
                w.endArray();
            }
            w.endObject();
        }

        static void emitBuiltinsJson(std::ostream &out,
                                     const std::vector<BuiltinFunction> &fns,
                                     const std::vector<BuiltinMethod> &methods,
                                     const DumpOptions &opts)
        {
            JsonWriter w(out, opts.pretty);
            w.beginObject();
            w.kv("schemaVersion", std::string_view(kSchemaVersion));
            w.kv("tool", std::string_view("sadinfo"));
            w.kv("toolVersion", std::string_view(kToolVersion));
            w.kv("functionCount", static_cast<long long>(fns.size()));
            w.kv("methodCount", static_cast<long long>(methods.size()));

            std::unordered_map<std::string, long long> categoryCounts;
            for (const auto &fn : fns)
                categoryCounts[std::string(categoryName(fn.category))]++;
            w.key("categoryCounts");
            w.beginObject();
            w.kv("printing", categoryCounts["printing"]);
            w.kv("input", categoryCounts["input"]);
            w.kv("reflection", categoryCounts["reflection"]);
            w.kv("type_ctor", categoryCounts["type_ctor"]);
            w.kv("concurrency", categoryCounts["concurrency"]);
            w.endObject();

            std::unordered_map<std::string, long long> hostCounts;
            for (const auto &m : methods)
                hostCounts[std::string(hostTypeName(m.host))]++;
            w.key("hostCounts");
            w.beginObject();
            w.kv("array", hostCounts["array"]);
            w.kv("string", hostCounts["string"]);
            w.kv("map", hostCounts["map"]);
            w.kv("channel", hostCounts["channel"]);
            w.endObject();

            w.key("functions");
            w.beginArray();
            for (const auto &fn : fns)
                emitFunctionJson(w, fn, opts);
            w.endArray();

            w.key("methods");
            w.beginArray();
            for (const auto &m : methods)
                emitMethodJson(w, m, opts);
            w.endArray();

            w.endObject();
            out << '\n';
        }

        // ═══════════════════════════════════════════════════════════════════
        // (AR) قسم 3: كتابة YAML — yaml-cpp إن وُجدت، وإلا fallback يدوي
        // ═══════════════════════════════════════════════════════════════════

        static void emitBuiltinsYaml(std::ostream &out,
                                     const std::vector<BuiltinFunction> &fns,
                                     const std::vector<BuiltinMethod> &methods,
                                     const DumpOptions &opts)
        {
#ifdef SAD_HAS_YAML_CPP
            YAML::Emitter em;
            em.SetIndent(2);
            em.SetMapFormat(YAML::Block);
            em.SetSeqFormat(YAML::Block);
            em.SetStringFormat(YAML::DoubleQuoted);

            std::unordered_map<std::string, long long> categoryCounts;
            for (const auto &fn : fns)
                categoryCounts[std::string(categoryName(fn.category))]++;
            std::unordered_map<std::string, long long> hostCounts;
            for (const auto &m : methods)
                hostCounts[std::string(hostTypeName(m.host))]++;

            auto emitParam = [&](const BuiltinParam &p) {
                em << YAML::BeginMap;
                em << YAML::Key << "name" << YAML::Value << std::string(p.nameAr);
                if (opts.lang != "ar")
                    em << YAML::Key << "nameEn" << YAML::Value << std::string(p.nameEn);
                em << YAML::Key << "type" << YAML::Value << std::string(p.typeAr);
                if (p.optional)
                    em << YAML::Key << "optional" << YAML::Value << true;
                if (p.variadic)
                    em << YAML::Key << "variadic" << YAML::Value << true;
                em << YAML::EndMap;
            };

            auto emitFn = [&](const BuiltinFunction &fn) {
                em << YAML::BeginMap;
                em << YAML::Key << "id" << YAML::Value << std::string(fn.id);
                em << YAML::Key << "nameAr" << YAML::Value << std::string(fn.nameAr);
                if (opts.lang != "ar")
                    em << YAML::Key << "nameEn" << YAML::Value << std::string(fn.nameEn);
                em << YAML::Key << "category" << YAML::Value
                   << std::string(categoryName(fn.category));
                em << YAML::Key << "returnType" << YAML::Value << std::string(fn.returnType);
                if (!opts.minimal) {
                    em << YAML::Key << "summaryAr" << YAML::Value << std::string(fn.summaryAr);
                    if (opts.lang != "ar")
                        em << YAML::Key << "summaryEn" << YAML::Value << std::string(fn.summaryEn);
                    em << YAML::Key << "params" << YAML::Value << YAML::BeginSeq;
                    for (const auto &p : fn.params) emitParam(p);
                    em << YAML::EndSeq;
                    if (!fn.example.empty())
                        em << YAML::Key << "example" << YAML::Value << std::string(fn.example);
                }
                em << YAML::EndMap;
            };

            auto emitM = [&](const BuiltinMethod &m) {
                em << YAML::BeginMap;
                em << YAML::Key << "id" << YAML::Value << std::string(m.id);
                em << YAML::Key << "host" << YAML::Value << std::string(hostTypeName(m.host));
                em << YAML::Key << "nameAr" << YAML::Value << std::string(m.nameAr);
                if (opts.lang != "ar")
                    em << YAML::Key << "nameEn" << YAML::Value << std::string(m.nameEn);
                em << YAML::Key << "returnType" << YAML::Value << std::string(m.returnType);
                if (!opts.minimal) {
                    em << YAML::Key << "summaryAr" << YAML::Value << std::string(m.summaryAr);
                    if (opts.lang != "ar")
                        em << YAML::Key << "summaryEn" << YAML::Value << std::string(m.summaryEn);
                    em << YAML::Key << "params" << YAML::Value << YAML::BeginSeq;
                    for (const auto &p : m.params) emitParam(p);
                    em << YAML::EndSeq;
                }
                em << YAML::EndMap;
            };

            em << YAML::BeginMap;
            em << YAML::Key << "schemaVersion" << YAML::Value << std::string(kSchemaVersion);
            em << YAML::Key << "tool" << YAML::Value << std::string("sadinfo");
            em << YAML::Key << "toolVersion" << YAML::Value << std::string(kToolVersion);
            em << YAML::Key << "functionCount" << YAML::Value
               << static_cast<long long>(fns.size());
            em << YAML::Key << "methodCount" << YAML::Value
               << static_cast<long long>(methods.size());

            em << YAML::Key << "categoryCounts" << YAML::Value << YAML::BeginMap;
            em << YAML::Key << "printing"    << YAML::Value << categoryCounts["printing"];
            em << YAML::Key << "input"       << YAML::Value << categoryCounts["input"];
            em << YAML::Key << "reflection"  << YAML::Value << categoryCounts["reflection"];
            em << YAML::Key << "type_ctor"   << YAML::Value << categoryCounts["type_ctor"];
            em << YAML::Key << "concurrency" << YAML::Value << categoryCounts["concurrency"];
            em << YAML::EndMap;

            em << YAML::Key << "hostCounts" << YAML::Value << YAML::BeginMap;
            em << YAML::Key << "array"   << YAML::Value << hostCounts["array"];
            em << YAML::Key << "string"  << YAML::Value << hostCounts["string"];
            em << YAML::Key << "map"     << YAML::Value << hostCounts["map"];
            em << YAML::Key << "channel" << YAML::Value << hostCounts["channel"];
            em << YAML::EndMap;

            em << YAML::Key << "functions" << YAML::Value << YAML::BeginSeq;
            for (const auto &fn : fns) emitFn(fn);
            em << YAML::EndSeq;

            em << YAML::Key << "methods" << YAML::Value << YAML::BeginSeq;
            for (const auto &m : methods) emitM(m);
            em << YAML::EndSeq;

            em << YAML::EndMap;
            out << em.c_str() << '\n';
            return;
#else
            // (AR) Fallback يدوي بسيط
            std::unordered_map<std::string, long long> categoryCounts;
            for (const auto &fn : fns)
                categoryCounts[std::string(categoryName(fn.category))]++;
            std::unordered_map<std::string, long long> hostCounts;
            for (const auto &m : methods)
                hostCounts[std::string(hostTypeName(m.host))]++;

            out << "schemaVersion: " << yamlEscapeBuiltin(kSchemaVersion) << "\n";
            out << "tool: \"sadinfo\"\n";
            out << "toolVersion: " << yamlEscapeBuiltin(kToolVersion) << "\n";
            out << "functionCount: " << fns.size() << "\n";
            out << "methodCount: " << methods.size() << "\n";

            out << "categoryCounts:\n";
            out << "  printing: "    << categoryCounts["printing"]    << "\n";
            out << "  input: "       << categoryCounts["input"]       << "\n";
            out << "  reflection: "  << categoryCounts["reflection"]  << "\n";
            out << "  type_ctor: "   << categoryCounts["type_ctor"]   << "\n";
            out << "  concurrency: " << categoryCounts["concurrency"] << "\n";

            out << "hostCounts:\n";
            out << "  array: "   << hostCounts["array"]   << "\n";
            out << "  string: "  << hostCounts["string"]  << "\n";
            out << "  map: "     << hostCounts["map"]     << "\n";
            out << "  channel: " << hostCounts["channel"] << "\n";

            auto emitParam = [&](const BuiltinParam &p, const std::string &ind) {
                out << ind << "- name: " << yamlEscapeBuiltin(p.nameAr) << "\n";
                if (opts.lang != "ar")
                    out << ind << "  nameEn: " << yamlEscapeBuiltin(p.nameEn) << "\n";
                out << ind << "  type: " << yamlEscapeBuiltin(p.typeAr) << "\n";
                if (p.optional)
                    out << ind << "  optional: true\n";
                if (p.variadic)
                    out << ind << "  variadic: true\n";
            };

            out << "functions:\n";
            for (const auto &fn : fns)
            {
                out << "  - id: " << yamlEscapeBuiltin(fn.id) << "\n";
                out << "    nameAr: " << yamlEscapeBuiltin(fn.nameAr) << "\n";
                if (opts.lang != "ar")
                    out << "    nameEn: " << yamlEscapeBuiltin(fn.nameEn) << "\n";
                out << "    category: " << yamlEscapeBuiltin(categoryName(fn.category)) << "\n";
                out << "    returnType: " << yamlEscapeBuiltin(fn.returnType) << "\n";
                if (!opts.minimal)
                {
                    out << "    summaryAr: " << yamlEscapeBuiltin(fn.summaryAr) << "\n";
                    if (opts.lang != "ar")
                        out << "    summaryEn: " << yamlEscapeBuiltin(fn.summaryEn) << "\n";
                    out << "    params:\n";
                    if (fn.params.empty())
                        out << "      []\n";
                    else
                        for (const auto &p : fn.params) emitParam(p, "      ");
                    if (!fn.example.empty())
                        out << "    example: " << yamlEscapeBuiltin(fn.example) << "\n";
                }
            }

            out << "methods:\n";
            for (const auto &m : methods)
            {
                out << "  - id: " << yamlEscapeBuiltin(m.id) << "\n";
                out << "    host: " << yamlEscapeBuiltin(hostTypeName(m.host)) << "\n";
                out << "    nameAr: " << yamlEscapeBuiltin(m.nameAr) << "\n";
                if (opts.lang != "ar")
                    out << "    nameEn: " << yamlEscapeBuiltin(m.nameEn) << "\n";
                out << "    returnType: " << yamlEscapeBuiltin(m.returnType) << "\n";
                if (!opts.minimal)
                {
                    out << "    summaryAr: " << yamlEscapeBuiltin(m.summaryAr) << "\n";
                    if (opts.lang != "ar")
                        out << "    summaryEn: " << yamlEscapeBuiltin(m.summaryEn) << "\n";
                    out << "    params:\n";
                    if (m.params.empty())
                        out << "      []\n";
                    else
                        for (const auto &p : m.params) emitParam(p, "      ");
                }
            }
#endif
        }

        // ═══════════════════════════════════════════════════════════════════
        // (AR) قسم 4: نقطة الدخول
        // ═══════════════════════════════════════════════════════════════════

        int dumpBuiltins(const DumpOptions &opts)
        {
            const auto &allFns = allBuiltinFunctions();
            const auto &allMethods = allBuiltinMethods();

            auto fns = applyFunctionFilters(allFns, opts.filters);
            auto methods = applyMethodFilters(allMethods, opts.filters);

            std::ofstream fileOut;
            std::ostream *outPtr = &std::cout;
            if (!opts.outputPath.empty())
            {
                fileOut.open(opts.outputPath, std::ios::binary);
                if (!fileOut.is_open())
                {
                    std::cerr << "(AR) خطأ: تعذّر فتح ملف الإخراج '"
                              << opts.outputPath << "'\n"
                              << "(EN) error: cannot open output file '"
                              << opts.outputPath << "'\n";
                    return 1;
                }
                outPtr = &fileOut;
            }

            if (opts.format == "yaml")
                emitBuiltinsYaml(*outPtr, fns, methods, opts);
            else
                emitBuiltinsJson(*outPtr, fns, methods, opts);

            return 0;
        }

    } // namespace Info
} // namespace Sad
