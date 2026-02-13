// docs_emitter.cpp — مُصدّر التوثيق (sadc meta --docs)
#include <string>
#include <vector>
#include <sstream>
#include <map>

struct DocEntry {
    std::string name;
    std::string kind; // "function", "struct", "enum", "trait", "variable"
    std::string signature;
    std::string description;
    std::vector<std::pair<std::string, std::string>> params; // name, description
    std::string return_type;
    std::string return_description;
    std::vector<std::string> examples;
    std::string source_file;
    int line_number;
};

class DocsEmitter {
private:
    std::vector<DocEntry> entries;

public:
    void add_entry(const DocEntry& entry) {
        entries.push_back(entry);
    }

    // Emit as JSON for IDE consumption
    std::string emit_json() const {
        std::ostringstream out;
        out << "[\n";
        for (size_t i = 0; i < entries.size(); ++i) {
            const auto& e = entries[i];
            out << "  {\n";
            out << "    \"name\": \"" << escape_json(e.name) << "\",\n";
            out << "    \"kind\": \"" << e.kind << "\",\n";
            out << "    \"signature\": \"" << escape_json(e.signature) << "\",\n";
            out << "    \"description\": \"" << escape_json(e.description) << "\",\n";
            out << "    \"params\": [";
            for (size_t j = 0; j < e.params.size(); ++j) {
                out << "{\"name\":\"" << escape_json(e.params[j].first)
                    << "\",\"desc\":\"" << escape_json(e.params[j].second) << "\"}";
                if (j < e.params.size() - 1) out << ",";
            }
            out << "],\n";
            out << "    \"return_type\": \"" << escape_json(e.return_type) << "\",\n";
            out << "    \"return_description\": \"" << escape_json(e.return_description) << "\",\n";
            out << "    \"examples\": [";
            for (size_t j = 0; j < e.examples.size(); ++j) {
                out << "\"" << escape_json(e.examples[j]) << "\"";
                if (j < e.examples.size() - 1) out << ",";
            }
            out << "],\n";
            out << "    \"source_file\": \"" << escape_json(e.source_file) << "\",\n";
            out << "    \"line_number\": " << e.line_number << "\n";
            out << "  }";
            if (i < entries.size() - 1) out << ",";
            out << "\n";
        }
        out << "]\n";
        return out.str();
    }

    // Emit as Markdown
    std::string emit_markdown() const {
        std::ostringstream out;
        out << "# \xD8\xAA\xD9\x88\xD8\xAB\xD9\x8A\xD9\x82 \xD9\x84\xD8\xBA\xD8\xA9 \xD8\xB5\n\n"; // توثيق لغة ص
        std::map<std::string, std::vector<const DocEntry*>> by_kind;
        for (const auto& e : entries) by_kind[e.kind].push_back(&e);
        for (const auto& [kind, items] : by_kind) {
            out << "## " << kind << "\n\n";
            for (const auto* e : items) {
                out << "### `" << e->name << "`\n\n";
                if (!e->signature.empty()) out << "```\n" << e->signature << "\n```\n\n";
                if (!e->description.empty()) out << e->description << "\n\n";
                if (!e->params.empty()) {
                    out << "**\xD9\x85\xD8\xB9\xD8\xA7\xD9\x85\xD9\x84\xD8\xA7\xD8\xAA:**\n\n"; // معاملات
                    for (const auto& [name, desc] : e->params) {
                        out << "- `" << name << "`: " << desc << "\n";
                    }
                    out << "\n";
                }
                if (!e->return_type.empty()) {
                    out << "**\xD8\xA5\xD8\xB1\xD8\xAC\xD8\xA7\xD8\xB9:** `" << e->return_type << "`"; // إرجاع
                    if (!e->return_description.empty()) out << " \u2014 " << e->return_description;
                    out << "\n\n";
                }
                for (const auto& example : e->examples) {
                    out << "**\xD9\x85\xD8\xAB\xD8\xA7\xD9\x84:**\n```\xD8\xB5\n" << example << "\n```\n\n"; // مثال + ص
                }
            }
        }
        return out.str();
    }

    static std::string escape_json(const std::string& s) {
        std::string result;
        for (char c : s) {
            switch (c) {
                case '"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default: result += c;
            }
        }
        return result;
    }
};

// Entry point for sadc meta --docs
extern "C" {
    const char* sadc_emit_docs(const char* source_path, const char* format) {
        DocsEmitter emitter;
        // TODO: Parse AST from source_path and extract doc comments
        // For now, return empty array
        std::string fmt(format ? format : "json");
        static std::string result;
        if (fmt == "markdown") {
            result = emitter.emit_markdown();
        } else {
            result = emitter.emit_json();
        }
        return result.c_str();
    }
}
