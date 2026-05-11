// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: tools/sadinfo/src/dump_keywords.cpp
// (AR) إخراج كل كلمات اللغة (JSON أو YAML) مع دعم: اشتقاق id مستقر،
//      كشف deprecated، فلترة، تجميع بالفئة، نمط minimal، اختيار اللغة.
//      المصدر: shared/lexer/generated/keywords_generated.cpp المولَّد من
//      data/language/keywords.yaml.
//
// (AR) ترتيب الحقول الثابت داخل كل entry:
//        id, word, english, category, tokenType, emittedByLexer, deprecated?,
//        aliases, roles
//      english يُحذف عند --lang=ar
//      deprecated يظهر فقط عندما e.deprecated=true
//      aliases/roles تُحذف عند الفراغ (لتقليل الضوضاء)
//
// (AR) التدفق:
//        getAllProcessed() → applyFilters() → [groupByCategory()] →
//        [applyMinimal()] → emit
//
// (EN) Dump all keywords (JSON or YAML) with: stable id derivation, deprecated
//      detection, filtering, grouping by category, minimal mode, language
//      selection. Source: generated table.
// ═══════════════════════════════════════════════════════════════════════════════

#include "sadinfo.h"
#include "json_writer.h"
#include "keyword_transforms.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#ifdef SAD_HAS_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace Sad
{
    namespace Info
    {

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) كتابة entry واحد إلى JSON بالترتيب الثابت
        // (EN) Emit single entry to JSON in fixed field order
        // ───────────────────────────────────────────────────────────────────────────────
        static void emitEntryJson(JsonWriter &w, const ProcessedKeyword &e,
                                  const DumpOptions &opts)
        {
            w.beginObject();
            // (AR) الترتيب الثابت — لا تُغيّره (يكسر الـ schema المُعتمَد)
            // (EN) Fixed order — do NOT change (would break consumed schema)
            w.kv("id", std::string_view(e.id));
            w.kv("word", std::string_view(e.word));

            if (!opts.minimal && opts.lang != "ar")
            {
                w.kv("english", std::string_view(e.english));
            }

            w.kv("category", std::string_view(e.category));
            w.kv("tokenType", static_cast<long long>(e.tokenType));

            if (!opts.minimal)
            {
                w.kv("emittedByLexer", e.emittedByLexer);

                // (AR) deprecated يظهر فقط حين يكون true (تقليل الضوضاء)
                // (EN) deprecated emitted only when true (reduce noise)
                if (e.deprecated)
                    w.kv("deprecated", true);

                if (!e.aliases.empty())
                    w.kvStringArray("aliases", e.aliases);
                if (!e.roles.empty())
                    w.kvStringArray("roles", e.roles);
            }
            w.endObject();
        }

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) كتابة entry واحد إلى YAML (تنفيذ يدوي بسيط — يُستخدم كاحتياطي
        //      عند غياب yaml-cpp). نُبقيها مُترجَمة دائماً للحفاظ على بساطة الـ ifdef.
        // (EN) Emit single entry to YAML (manual writer — fallback when yaml-cpp
        //      is unavailable). Always compiled to keep ifdef simple.
        // ───────────────────────────────────────────────────────────────────────────────
        [[maybe_unused]] static std::string yamlEscape(std::string_view s)
        {
            // (AR) دائماً نضع علامات اقتباس مزدوجة لأن النص قد يحوي عربية أو
            //      رموزاً خاصة. نهرب " و \\ فقط.
            // (EN) Always quote — text may contain Arabic or special chars.
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

        [[maybe_unused]] static void emitEntryYaml(std::ostream &out, const ProcessedKeyword &e,
                                  const DumpOptions &opts, const std::string &indent)
        {
            // (AR) أول حقل يبدأ بـ "- " ثم البقية بمسافات بادئة
            out << indent << "- id: " << yamlEscape(e.id) << "\n";
            out << indent << "  word: " << yamlEscape(e.word) << "\n";
            if (!opts.minimal && opts.lang != "ar")
                out << indent << "  english: " << yamlEscape(e.english) << "\n";
            out << indent << "  category: " << yamlEscape(e.category) << "\n";
            out << indent << "  tokenType: " << e.tokenType << "\n";
            if (!opts.minimal)
            {
                out << indent << "  emittedByLexer: " << (e.emittedByLexer ? "true" : "false") << "\n";
                if (e.deprecated)
                    out << indent << "  deprecated: true\n";
                if (!e.aliases.empty())
                {
                    out << indent << "  aliases:\n";
                    for (const auto &a : e.aliases)
                        out << indent << "    - " << yamlEscape(a) << "\n";
                }
                if (!e.roles.empty())
                {
                    out << indent << "  roles:\n";
                    for (const auto &r : e.roles)
                        out << indent << "    - " << yamlEscape(r) << "\n";
                }
            }
        }

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) إخراج JSON كامل
        // (EN) Full JSON emission
        // ───────────────────────────────────────────────────────────────────────────────
        static void emitJson(std::ostream &out, const std::vector<ProcessedKeyword> &filtered,
                             const DumpOptions &opts)
        {
            JsonWriter w(out, opts.pretty);
            w.beginObject();
            w.kv("schemaVersion", std::string_view(kSchemaVersion));
            w.kv("tool", std::string_view("sadinfo"));
            w.kv("toolVersion", std::string_view(kToolVersion));
            w.kv("totalCount", static_cast<long long>(filtered.size()));

            // ── إحصاءات الفئات (دائماً بناءً على المُرشَّح) ──
            std::unordered_map<std::string, long long> counts;
            for (const auto &e : filtered)
                counts[e.category]++;
            w.key("categoryCounts");
            w.beginObject();
            w.kv("reserved", counts["reserved"]);
            w.kv("operator", counts["operator"]);
            w.kv("contextual", counts["contextual"]);
            w.kv("builtin_type", counts["builtin_type"]);
            w.endObject();

            if (opts.byCategory)
            {
                // ── تجميع تحت categories.{reserved,operator,contextual,builtin_type} ──
                auto g = groupByCategory(filtered);
                w.key("categories");
                w.beginObject();
                w.key("reserved");
                w.beginArray();
                for (const auto &e : g.reserved) emitEntryJson(w, e, opts);
                w.endArray();
                w.key("operator");
                w.beginArray();
                for (const auto &e : g.op) emitEntryJson(w, e, opts);
                w.endArray();
                w.key("contextual");
                w.beginArray();
                for (const auto &e : g.contextual) emitEntryJson(w, e, opts);
                w.endArray();
                w.key("builtin_type");
                w.beginArray();
                for (const auto &e : g.builtinType) emitEntryJson(w, e, opts);
                w.endArray();
                w.endObject();
            }
            else
            {
                w.key("keywords");
                w.beginArray();
                for (const auto &e : filtered)
                    emitEntryJson(w, e, opts);
                w.endArray();
            }

            w.endObject();
            out << '\n';
        }

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) إخراج YAML كامل
        // (EN) Full YAML emission
        //   - مع SAD_HAS_YAML_CPP: نستخدم YAML::Emitter (آمن، canonical)
        //   - بدونه: نسقط إلى الكاتب اليدوي البسيط (fallback)
        // ───────────────────────────────────────────────────────────────────────────────
        static void emitYaml(std::ostream &out, const std::vector<ProcessedKeyword> &filtered,
                             const DumpOptions &opts)
        {
#ifdef SAD_HAS_YAML_CPP
            // ── المسار المُفضَّل: yaml-cpp ──
            YAML::Emitter em;
            em.SetIndent(2);
            em.SetMapFormat(YAML::Block);
            em.SetSeqFormat(YAML::Block);
            em.SetStringFormat(YAML::DoubleQuoted);

            std::unordered_map<std::string, long long> counts;
            for (const auto &e : filtered) counts[e.category]++;

            auto emitEntryCpp = [&](const ProcessedKeyword &e) {
                em << YAML::BeginMap;
                em << YAML::Key << "id" << YAML::Value << e.id;
                em << YAML::Key << "word" << YAML::Value << e.word;
                if (!opts.minimal && opts.lang != "ar")
                    em << YAML::Key << "english" << YAML::Value << e.english;
                em << YAML::Key << "category" << YAML::Value << e.category;
                em << YAML::Key << "tokenType" << YAML::Value
                   << static_cast<long long>(e.tokenType);
                if (!opts.minimal) {
                    em << YAML::Key << "emittedByLexer" << YAML::Value << e.emittedByLexer;
                    if (e.deprecated)
                        em << YAML::Key << "deprecated" << YAML::Value << true;
                    if (!e.aliases.empty()) {
                        em << YAML::Key << "aliases" << YAML::Value << YAML::BeginSeq;
                        for (const auto &a : e.aliases) em << a;
                        em << YAML::EndSeq;
                    }
                    if (!e.roles.empty()) {
                        em << YAML::Key << "roles" << YAML::Value << YAML::BeginSeq;
                        for (const auto &r : e.roles) em << r;
                        em << YAML::EndSeq;
                    }
                }
                em << YAML::EndMap;
            };

            em << YAML::BeginMap;
            em << YAML::Key << "schemaVersion" << YAML::Value << std::string(kSchemaVersion);
            em << YAML::Key << "tool" << YAML::Value << std::string("sadinfo");
            em << YAML::Key << "toolVersion" << YAML::Value << std::string(kToolVersion);
            em << YAML::Key << "totalCount" << YAML::Value
               << static_cast<long long>(filtered.size());
            em << YAML::Key << "categoryCounts" << YAML::Value << YAML::BeginMap;
            em << YAML::Key << "reserved"     << YAML::Value << counts["reserved"];
            em << YAML::Key << "operator"     << YAML::Value << counts["operator"];
            em << YAML::Key << "contextual"   << YAML::Value << counts["contextual"];
            em << YAML::Key << "builtin_type" << YAML::Value << counts["builtin_type"];
            em << YAML::EndMap;

            if (opts.byCategory) {
                auto g = groupByCategory(filtered);
                em << YAML::Key << "categories" << YAML::Value << YAML::BeginMap;
                em << YAML::Key << "reserved" << YAML::Value << YAML::BeginSeq;
                for (const auto &e : g.reserved) emitEntryCpp(e);
                em << YAML::EndSeq;
                em << YAML::Key << "operator" << YAML::Value << YAML::BeginSeq;
                for (const auto &e : g.op) emitEntryCpp(e);
                em << YAML::EndSeq;
                em << YAML::Key << "contextual" << YAML::Value << YAML::BeginSeq;
                for (const auto &e : g.contextual) emitEntryCpp(e);
                em << YAML::EndSeq;
                em << YAML::Key << "builtin_type" << YAML::Value << YAML::BeginSeq;
                for (const auto &e : g.builtinType) emitEntryCpp(e);
                em << YAML::EndSeq;
                em << YAML::EndMap;
            } else {
                em << YAML::Key << "keywords" << YAML::Value << YAML::BeginSeq;
                for (const auto &e : filtered) emitEntryCpp(e);
                em << YAML::EndSeq;
            }
            em << YAML::EndMap;

            out << em.c_str() << '\n';
            return;
#else
            // ── المسار الاحتياطي: كاتب يدوي ──
            out << "schemaVersion: " << yamlEscape(kSchemaVersion) << "\n";
            out << "tool: \"sadinfo\"\n";
            out << "toolVersion: " << yamlEscape(kToolVersion) << "\n";
            out << "totalCount: " << filtered.size() << "\n";

            std::unordered_map<std::string, long long> counts;
            for (const auto &e : filtered) counts[e.category]++;
            out << "categoryCounts:\n";
            out << "  reserved: " << counts["reserved"] << "\n";
            out << "  operator: " << counts["operator"] << "\n";
            out << "  contextual: " << counts["contextual"] << "\n";
            out << "  builtin_type: " << counts["builtin_type"] << "\n";

            if (opts.byCategory)
            {
                auto g = groupByCategory(filtered);
                out << "categories:\n";
                out << "  reserved:\n";
                for (const auto &e : g.reserved) emitEntryYaml(out, e, opts, "    ");
                out << "  operator:\n";
                for (const auto &e : g.op) emitEntryYaml(out, e, opts, "    ");
                out << "  contextual:\n";
                for (const auto &e : g.contextual) emitEntryYaml(out, e, opts, "    ");
                out << "  builtin_type:\n";
                for (const auto &e : g.builtinType) emitEntryYaml(out, e, opts, "    ");
            }
            else
            {
                out << "keywords:\n";
                for (const auto &e : filtered)
                    emitEntryYaml(out, e, opts, "  ");
            }
#endif // SAD_HAS_YAML_CPP
        }

        // ───────────────────────────────────────────────────────────────────────────────
        // (AR) واجهة dumpKeywords الموحَّدة
        // (EN) Unified dumpKeywords entry point
        // ───────────────────────────────────────────────────────────────────────────────
        int dumpKeywords(const DumpOptions &opts)
        {
            // ── 1) تحليل الفلاتر / Parse filters ──
            std::vector<ParsedFilter> parsedFilters;
            parsedFilters.reserve(opts.filters.size());
            for (const auto &raw : opts.filters)
            {
                ParsedFilter pf;
                std::string err;
                if (!parseFilter(raw, pf, err))
                {
                    std::cerr << err << "\n";
                    return 2;
                }
                parsedFilters.push_back(std::move(pf));
            }

            // ── 2) جلب كل الإدخالات المعالَجة (91) / Get all processed (91) ──
            auto all = getAllProcessed();

            // ── 3) تطبيق الفلاتر + استبعاد المُهمَل افتراضياً ──
            const bool excludeDeprecated = !opts.includeDeprecated;
            auto filtered = applyFilters(all, parsedFilters, excludeDeprecated);

            // ── 4) اختيار التيار / Choose stream ──
            auto run = [&](std::ostream &out)
            {
                if (opts.format == "yaml")
                    emitYaml(out, filtered, opts);
                else
                    emitJson(out, filtered, opts);
            };

            if (opts.outputPath.empty())
            {
                run(std::cout);
                return 0;
            }

            std::ofstream f(opts.outputPath, std::ios::out | std::ios::binary);
            if (!f.is_open())
            {
                std::cerr << "(AR) خطأ: تعذر فتح الملف للكتابة: " << opts.outputPath << "\n"
                          << "(EN) error: cannot open file for writing: " << opts.outputPath << "\n";
                return 1;
            }
            run(f);
            return 0;
        }

    } // namespace Info
} // namespace Sad
