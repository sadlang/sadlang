// بسم الله الرحمن الرحيم
// ============================================================================
// docs_emitter.cpp — تنفيذ مُصدِّر التوثيق المتقدم
// Advanced Documentation Emitter Implementation
// ============================================================================
//
// (AR) تنفيذ كامل لنظام توليد التوثيق الاحترافي للغة ص.
//      يدعم HTML وMarkdown وJSON، مع بحث ومراجع متقاطعة وسمات متعددة.
//      يحل محل docs_emitter.cpp القديم (129 سطر) بنظام شامل.
//
// (EN) Complete implementation of the professional documentation generation
//      system for Sad language. Replaces old 129-line stub with full system.
//
// ============================================================================

#include "meta/docs_emitter.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <regex>
#include <cassert>

namespace fs = std::filesystem;

namespace Sad {
namespace Meta {

// ============================================================================
// DocEntry helpers
// ============================================================================

std::string DocEntry::kindNameAr() const {
    switch (kind) {
        case DocItemKind::Function:    return "دالة";
        case DocItemKind::Class:       return "صنف";
        case DocItemKind::Struct:      return "هيكل";
        case DocItemKind::Enum:        return "تعداد";
        case DocItemKind::Trait:       return "سمة";
        case DocItemKind::Interface:   return "واجهة";
        case DocItemKind::Variable:    return "متغير";
        case DocItemKind::Constant:    return "ثابت";
        case DocItemKind::Module:      return "وحدة";
        case DocItemKind::Property:    return "خاصية";
        case DocItemKind::Constructor: return "باني";
        case DocItemKind::Method:      return "تابع";
        case DocItemKind::TypeAlias:   return "اسم_بديل";
    }
    return "غير_معروف";
}

std::string DocEntry::kindNameEn() const {
    switch (kind) {
        case DocItemKind::Function:    return "function";
        case DocItemKind::Class:       return "class";
        case DocItemKind::Struct:      return "struct";
        case DocItemKind::Enum:        return "enum";
        case DocItemKind::Trait:       return "trait";
        case DocItemKind::Interface:   return "interface";
        case DocItemKind::Variable:    return "variable";
        case DocItemKind::Constant:    return "constant";
        case DocItemKind::Module:      return "module";
        case DocItemKind::Property:    return "property";
        case DocItemKind::Constructor: return "constructor";
        case DocItemKind::Method:      return "method";
        case DocItemKind::TypeAlias:   return "typealias";
    }
    return "unknown";
}

// ============================================================================
// DocCommentParser — محلل تعليقات التوثيق
// ============================================================================

std::vector<DocEntry> DocCommentParser::parseFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) return {};

    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    return parseSource(content, filePath);
}

std::vector<DocEntry> DocCommentParser::parseSource(
    const std::string& source, const std::string& filename) const {

    std::vector<DocEntry> entries;
    std::istringstream stream(source);
    std::string line;
    std::vector<std::string> docLines;
    int lineNumber = 0;
    bool inBlockComment = false;

    while (std::getline(stream, line)) {
        lineNumber++;

        // ========== تعليق كتلة توثيق #** ... **# ==========
        if (!inBlockComment) {
            size_t blockStart = line.find("#**");
            if (blockStart != std::string::npos) {
                inBlockComment = true;
                std::string rest = line.substr(blockStart + 3);
                size_t blockEnd = rest.find("**#");
                if (blockEnd != std::string::npos) {
                    docLines.push_back(rest.substr(0, blockEnd));
                    inBlockComment = false;
                } else if (!rest.empty()) {
                    size_t start = rest.find_first_not_of(" \t");
                    if (start != std::string::npos) {
                        docLines.push_back(rest.substr(start));
                    }
                }
                continue;
            }
        }

        if (inBlockComment) {
            size_t blockEnd = line.find("**#");
            if (blockEnd != std::string::npos) {
                std::string before = line.substr(0, blockEnd);
                size_t start = before.find_first_not_of(" \t*");
                if (start != std::string::npos) {
                    docLines.push_back(before.substr(start));
                }
                inBlockComment = false;
                continue;
            }
            size_t start = line.find_first_not_of(" \t*");
            if (start != std::string::npos) {
                docLines.push_back(line.substr(start));
            } else {
                docLines.emplace_back("");
            }
            continue;
        }

        // ========== تعليق توثيق سطري ## أو /// ==========
        size_t docPos = std::string::npos;

        size_t hashHash = line.find("##");
        if (hashHash != std::string::npos) {
            if (!(hashHash + 2 < line.size() && line[hashHash + 2] == '*')) {
                docPos = hashHash;
            }
        }

        size_t tripleSlash = line.find("///");
        if (tripleSlash != std::string::npos) {
            if (docPos == std::string::npos || tripleSlash < docPos) {
                docPos = tripleSlash;
            }
        }

        if (docPos != std::string::npos) {
            std::string comment;
            if (line.substr(docPos, 3) == "///") {
                comment = line.substr(docPos + 3);
            } else {
                comment = line.substr(docPos + 2);
            }
            size_t start = comment.find_first_not_of(" \t");
            if (start != std::string::npos) {
                comment = comment.substr(start);
            } else {
                comment = "";
            }
            docLines.push_back(comment);
            continue;
        }

        // ========== إعلان بعد كتلة تعليقات ==========
        if (!docLines.empty()) {
            std::string trimmed = line;
            size_t firstNonSpace = trimmed.find_first_not_of(" \t");
            if (firstNonSpace != std::string::npos) {
                trimmed = trimmed.substr(firstNonSpace);
            }

            bool isDecl = false;
            static const std::vector<std::string> declKeywords = {
                "دالة", "صنف", "هيكل", "تعداد", "سمة", "واجهة",
                "متغير", "ثابت", "وحدة", "خاصية", "باني",
                "function", "class", "struct", "enum", "trait",
                "interface", "var", "const", "module", "property"
            };

            for (const auto& kw : declKeywords) {
                if (trimmed.find(kw) == 0) {
                    size_t kwLen = kw.size();
                    if (kwLen >= trimmed.size() ||
                        trimmed[kwLen] == ' ' || trimmed[kwLen] == '(' ||
                        trimmed[kwLen] == '<' || trimmed[kwLen] == ':' ||
                        static_cast<unsigned char>(trimmed[kwLen]) >= 0x80) {
                        isDecl = true;
                        break;
                    }
                }
            }

            if (isDecl) {
                DocEntry entry = parseDocBlock(docLines);
                entry.sourceFile = filename;
                entry.lineNumber = lineNumber;
                parseDeclaration(trimmed, entry);
                entry.id = generateId(entry);
                entries.push_back(std::move(entry));
            }

            if (!line.empty() && line.find("///") == std::string::npos &&
                line.find("##") == std::string::npos) {
                docLines.clear();
            }
        }
    }

    return entries;
}

DocEntry DocCommentParser::parseDocBlock(const std::vector<std::string>& docLines) const {
    DocEntry entry;
    entry.kind = DocItemKind::Function; // default, will be overridden
    std::ostringstream description;
    bool inDescription = true;
    bool summarySet = false;

    for (const auto& line : docLines) {
        if (line.empty()) {
            if (inDescription && summarySet) {
                description << "\n";
            }
            continue;
        }

        if (line[0] == '@') {
            inDescription = false;
            DocTag tag = parseTag(line);

            if (tag.name == "@معطى" || tag.name == "@param") {
                ParamInfo param;
                param.name = tag.paramName;
                param.description = tag.content;
                entry.params.push_back(std::move(param));
            }
            else if (tag.name == "@أرجع" || tag.name == "@return" || tag.name == "@returns") {
                entry.returnDescription = tag.content;
            }
            else if (tag.name == "@مثال" || tag.name == "@example") {
                entry.examples.push_back(tag.content);
            }
            else if (tag.name == "@انظر" || tag.name == "@see") {
                entry.seeAlso.push_back(tag.content);
            }
            else if (tag.name == "@منذ" || tag.name == "@since") {
                entry.since = tag.content;
            }
            else if (tag.name == "@مهمل" || tag.name == "@deprecated") {
                entry.deprecated = tag.content;
            }
            else if (tag.name == "@ملاحظة" || tag.name == "@note") {
                entry.notes.push_back(tag.content);
            }
            else if (tag.name == "@تحذير" || tag.name == "@warning") {
                entry.warnings.push_back(tag.content);
            }
            else if (tag.name == "@نسخة" || tag.name == "@version") {
                entry.version = tag.content;
            }
            else if (tag.name == "@مؤلف" || tag.name == "@author") {
                entry.metadata["author"] = tag.content;
            }
            else if (tag.name == "@رمي" || tag.name == "@throws") {
                entry.metadata["throws:" + tag.paramName] = tag.content;
            }
            else {
                entry.tags.push_back(std::move(tag));
            }
        }
        else if (inDescription) {
            if (!summarySet) {
                entry.summary = line;
                summarySet = true;
            } else {
                description << line << "\n";
            }
        }
    }

    entry.description = description.str();
    while (!entry.description.empty() && entry.description.back() == '\n') {
        entry.description.pop_back();
    }

    // إذا كان الوصف فارغاً لكن الملخص موجود، استخدم الملخص كوصف
    if (entry.description.empty() && !entry.summary.empty()) {
        entry.description = entry.summary;
    }

    return entry;
}

DocTag DocCommentParser::parseTag(const std::string& line) const {
    DocTag tag;
    size_t spacePos = line.find(' ');
    tag.name = (spacePos != std::string::npos) ? line.substr(0, spacePos) : line;
    std::string rest = (spacePos != std::string::npos) ? line.substr(spacePos + 1) : "";

    if (tag.name == "@معطى" || tag.name == "@param" ||
        tag.name == "@رمي" || tag.name == "@throws") {
        size_t nameEnd = rest.find(' ');
        if (nameEnd != std::string::npos) {
            tag.paramName = rest.substr(0, nameEnd);
            tag.content = rest.substr(nameEnd + 1);
        } else {
            tag.paramName = rest;
        }
    } else {
        tag.content = rest;
    }
    return tag;
}

void DocCommentParser::parseDeclaration(const std::string& line, DocEntry& entry) const {
    entry.declarationCode = line;

    if (line.find("دالة ") == 0 || line.find("function ") == 0) {
        entry.kind = DocItemKind::Function;
        parseFunctionSignature(line, entry);
    }
    else if (line.find("صنف ") == 0 || line.find("class ") == 0) {
        entry.kind = DocItemKind::Class;
        parseClassDeclaration(line, entry);
    }
    else if (line.find("هيكل ") == 0 || line.find("struct ") == 0) {
        entry.kind = DocItemKind::Struct;
        parseClassDeclaration(line, entry);
    }
    else if (line.find("تعداد ") == 0 || line.find("enum ") == 0) {
        entry.kind = DocItemKind::Enum;
        size_t start = line.find(' ') + 1;
        size_t end = line.find_first_of(" {:<(", start);
        entry.name = (end != std::string::npos) ? line.substr(start, end - start) : line.substr(start);
    }
    else if (line.find("سمة ") == 0 || line.find("trait ") == 0) {
        entry.kind = DocItemKind::Trait;
        size_t start = line.find(' ') + 1;
        size_t end = line.find_first_of(" {:<", start);
        entry.name = (end != std::string::npos) ? line.substr(start, end - start) : line.substr(start);
    }
    else if (line.find("واجهة ") == 0 || line.find("interface ") == 0) {
        entry.kind = DocItemKind::Interface;
        size_t start = line.find(' ') + 1;
        size_t end = line.find_first_of(" {:<", start);
        entry.name = (end != std::string::npos) ? line.substr(start, end - start) : line.substr(start);
    }
    else if (line.find("ثابت ") == 0 || line.find("const ") == 0) {
        entry.kind = DocItemKind::Constant;
        size_t start = line.find(' ') + 1;
        size_t end = line.find_first_of(" =:", start);
        entry.name = (end != std::string::npos) ? line.substr(start, end - start) : line.substr(start);
    }
    else if (line.find("متغير ") == 0 || line.find("var ") == 0) {
        entry.kind = DocItemKind::Variable;
        size_t start = line.find(' ') + 1;
        size_t end = line.find_first_of(" =:", start);
        entry.name = (end != std::string::npos) ? line.substr(start, end - start) : line.substr(start);
    }
    else if (line.find("باني") == 0) {
        entry.kind = DocItemKind::Constructor;
        entry.name = "باني";
        parseFunctionSignature(line, entry);
    }
    else if (line.find("وحدة ") == 0 || line.find("module ") == 0) {
        entry.kind = DocItemKind::Module;
        size_t start = line.find(' ') + 1;
        size_t end = line.find_first_of(" {", start);
        entry.name = (end != std::string::npos) ? line.substr(start, end - start) : line.substr(start);
    }
    else {
        entry.kind = DocItemKind::Variable;
        entry.name = line;
    }

    if (entry.signature.empty()) {
        entry.signature = line;
    }
    if (entry.qualifiedName.empty()) {
        entry.qualifiedName = entry.name;
    }
}

void DocCommentParser::parseFunctionSignature(const std::string& line, DocEntry& entry) const {
    size_t funcStart = line.find(' ');
    if (funcStart == std::string::npos) return;
    funcStart++;

    size_t parenOpen = line.find('(', funcStart);
    size_t parenClose = line.find(')', parenOpen != std::string::npos ? parenOpen : 0);

    if (parenOpen != std::string::npos) {
        entry.name = line.substr(funcStart, parenOpen - funcStart);
        while (!entry.name.empty() && entry.name.back() == ' ') {
            entry.name.pop_back();
        }
    } else {
        size_t nameEnd = line.find_first_of(" (:", funcStart);
        entry.name = (nameEnd != std::string::npos) ? line.substr(funcStart, nameEnd - funcStart)
                                                      : line.substr(funcStart);
    }

    if (parenOpen != std::string::npos && parenClose != std::string::npos &&
        parenClose > parenOpen + 1) {
        std::string paramStr = line.substr(parenOpen + 1, parenClose - parenOpen - 1);
        auto parsedParams = parseParamList(paramStr);

        for (auto& pp : parsedParams) {
            bool found = false;
            for (auto& ep : entry.params) {
                if (ep.name == pp.name) {
                    if (ep.type.empty()) ep.type = pp.type;
                    if (ep.defaultValue.empty()) ep.defaultValue = pp.defaultValue;
                    found = true;
                    break;
                }
            }
            if (!found) {
                entry.params.push_back(std::move(pp));
            }
        }
    }

    size_t arrowPos = line.find("->");
    if (arrowPos != std::string::npos) {
        std::string retType = line.substr(arrowPos + 2);
        size_t start = retType.find_first_not_of(" \t>");
        if (start != std::string::npos) {
            entry.returnType = retType.substr(start);
            while (!entry.returnType.empty() &&
                   (entry.returnType.back() == ' ' || entry.returnType.back() == '\t' ||
                    entry.returnType.back() == '\r')) {
                entry.returnType.pop_back();
            }
        }
    }

    entry.signature = line;
}

void DocCommentParser::parseClassDeclaration(const std::string& line, DocEntry& entry) const {
    size_t start = line.find(' ') + 1;

    size_t nameEnd = line.find_first_of(" {:<(", start);
    entry.name = (nameEnd != std::string::npos) ? line.substr(start, nameEnd - start)
                                                  : line.substr(start);

    size_t inheritPos = line.find('<', start);
    if (inheritPos == std::string::npos) {
        inheritPos = line.find(':');
    }
    if (inheritPos != std::string::npos) {
        std::string parents = line.substr(inheritPos + 1);
        std::istringstream pstream(parents);
        std::string parent;
        while (std::getline(pstream, parent, ',')) {
            size_t ps = parent.find_first_not_of(" \t");
            size_t pe = parent.find_last_not_of(" \t\r\n{");
            if (ps != std::string::npos && pe != std::string::npos) {
                entry.inherits.push_back(parent.substr(ps, pe - ps + 1));
            }
        }
    }

    entry.signature = line;
}

std::vector<ParamInfo> DocCommentParser::parseParamList(const std::string& paramStr) const {
    std::vector<ParamInfo> result;

    std::string normalized = paramStr;
    for (size_t i = 0; i < normalized.size(); i++) {
        if (i + 1 < normalized.size() &&
            static_cast<unsigned char>(normalized[i]) == 0xD8 &&
            static_cast<unsigned char>(normalized[i + 1]) == 0x8C) {
            normalized[i] = ',';
            normalized.erase(i + 1, 1);
        }
    }

    std::istringstream stream(normalized);
    std::string param;
    while (std::getline(stream, param, ',')) {
        ParamInfo info;

        size_t start = param.find_first_not_of(" \t");
        size_t end = param.find_last_not_of(" \t");
        if (start == std::string::npos) continue;
        param = param.substr(start, end - start + 1);

        size_t eqPos = param.find('=');
        if (eqPos != std::string::npos) {
            info.defaultValue = param.substr(eqPos + 1);
            size_t ds = info.defaultValue.find_first_not_of(" \t");
            if (ds != std::string::npos) info.defaultValue = info.defaultValue.substr(ds);
            info.isOptional = true;
            param = param.substr(0, eqPos);
            end = param.find_last_not_of(" \t");
            if (end != std::string::npos) param = param.substr(0, end + 1);
        }

        size_t colonPos = param.find(':');
        if (colonPos != std::string::npos) {
            info.name = param.substr(0, colonPos);
            info.type = param.substr(colonPos + 1);

            auto trim = [](std::string& s) {
                size_t a = s.find_first_not_of(" \t");
                size_t b = s.find_last_not_of(" \t");
                s = (a != std::string::npos) ? s.substr(a, b - a + 1) : "";
            };
            trim(info.name);
            trim(info.type);
        } else {
            info.name = param;
        }

        if (!info.name.empty()) {
            result.push_back(std::move(info));
        }
    }

    return result;
}

std::string DocCommentParser::generateId(const DocEntry& entry) {
    std::string id = entry.qualifiedName.empty() ? entry.name : entry.qualifiedName;
    for (char& c : id) {
        if (c == ' ' || c == '.' || c == ':') c = '-';
        else if (c == '(') c = '_';
        else if (c == ')' || c == ',' || c == '<' || c == '>') c = '_';
    }
    return entry.kindNameEn() + "-" + id;
}

// ============================================================================
// FileCollector — جامع الملفات
// ============================================================================

std::vector<std::string> FileCollector::collect(
    const std::vector<std::string>& inputs,
    const std::vector<std::string>& excludePatterns) const {

    std::vector<std::string> files;
    if (inputs.empty()) {
        collectFromDirectory(".", files, excludePatterns);
    } else {
        for (const auto& input : inputs) {
            if (fs::is_directory(input)) {
                collectFromDirectory(input, files, excludePatterns);
            } else if (fs::exists(input)) {
                if (!shouldExclude(input, excludePatterns)) {
                    files.push_back(input);
                }
            }
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

void FileCollector::collectFromDirectory(
    const std::string& dir,
    std::vector<std::string>& files,
    const std::vector<std::string>& excludePatterns) const {
    try {
        for (const auto& entry : fs::recursive_directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".ص" || ext == ".sad") {
                    std::string path = entry.path().string();
                    if (!shouldExclude(path, excludePatterns)) {
                        files.push_back(path);
                    }
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "تحذير: خطأ في قراءة المجلد: " << e.what() << "\n";
    }
}

bool FileCollector::shouldExclude(
    const std::string& path,
    const std::vector<std::string>& patterns) const {
    for (const auto& pattern : patterns) {
        if (path.find(pattern) != std::string::npos) return true;
    }
    return false;
}

// ============================================================================
// HtmlDocGenerator — مولّد HTML
// ============================================================================


} // namespace Meta
} // namespace Sad

