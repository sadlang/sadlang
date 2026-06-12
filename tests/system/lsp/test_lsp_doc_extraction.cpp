/**
 * اختبار استخراج التعليقات التوثيقية وتنسيقها بـ Markdown — يحاكي ما يفعله LSP
 * Test doc comment extraction + Markdown formatting — simulates LSP behavior
 * 
 * يمكن ترجمته وتشغيله مباشرة:
 *   cl /EHsc /utf-8 /std:c++17 /I shared/ast/include tests\test_lsp_doc_extraction.cpp /Fe:build\test_doc.exe
 *   .\test_doc.exe
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include "doc_comment.h"

// ═══════════════════════════════════════════════════════════════════════════════
// تحويل النص الخام إلى Markdown غني (نفس المنطق في lsp_server.cpp)
// ═══════════════════════════════════════════════════════════════════════════════
std::string format_doc_as_markdown(const std::string& raw_text) {
    if (raw_text.empty()) return "";
    
    auto doc = Sad::AST::DocCommentParser::parse(raw_text);
    if (doc.isEmpty()) return raw_text;
    
    std::string md = doc.toMarkdown();
    while (!md.empty() && (md.back() == '\n' || md.back() == ' ')) {
        md.pop_back();
    }
    return md.empty() ? raw_text : md;
}

// ═══════════════════════════════════════════════════════════════════════════════
// نسخة مستقلة من extract_doc_comment (نفس المنطق في lsp_server.cpp)
// ═══════════════════════════════════════════════════════════════════════════════
std::string extract_doc_comment(const std::vector<std::string>& lines, size_t symbol_line) {
    if (symbol_line == 0) return "";
    
    int scan_line = static_cast<int>(symbol_line) - 1;
    
    // تخطي الأسطر الفارغة
    while (scan_line >= 0) {
        std::string trimmed = lines[scan_line];
        size_t start = trimmed.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            scan_line--;
            break;
        }
        break;
    }
    
    if (scan_line < 0) return "";
    
    // التحقق من تعليق كتلة #** ... **#
    {
        std::string trimmed = lines[scan_line];
        size_t start = trimmed.find_first_not_of(" \t");
        if (start != std::string::npos) {
            trimmed = trimmed.substr(start);
        }
        
        if (trimmed.find("**#") != std::string::npos) {
            std::string block_text;
            int block_start = scan_line;
            
            for (int j = scan_line; j >= 0; j--) {
                if (lines[j].find("#**") != std::string::npos) {
                    block_start = j;
                    break;
                }
            }
            
            for (int j = block_start; j <= scan_line; j++) {
                std::string l = lines[j];
                size_t pos = l.find("#**");
                if (pos != std::string::npos) {
                    l = l.substr(pos + std::string("#**").length());
                }
                pos = l.find("**#");
                if (pos != std::string::npos) {
                    l = l.substr(0, pos);
                }
                size_t s = l.find_first_not_of(" \t");
                if (s != std::string::npos && l[s] == '*') {
                    l = l.substr(s + 1);
                }
                s = l.find_first_not_of(" \t");
                if (s != std::string::npos) {
                    l = l.substr(s);
                }
                if (!l.empty()) {
                    if (!block_text.empty()) block_text += "\n";
                    block_text += l;
                }
            }
            
            return format_doc_as_markdown(block_text);
        }
    }
    
    // تعليقات ## سطرية
    {
        std::vector<std::string> doc_lines;
        
        for (int j = scan_line; j >= 0; j--) {
            std::string trimmed = lines[j];
            size_t start = trimmed.find_first_not_of(" \t");
            if (start == std::string::npos) break;
            trimmed = trimmed.substr(start);
            
            if (trimmed.length() >= 2 && trimmed[0] == '#' && trimmed[1] == '#') {
                std::string text = trimmed.substr(2);
                size_t s = text.find_first_not_of(" \t");
                if (s != std::string::npos) {
                    text = text.substr(s);
                }
                doc_lines.push_back(text);
            } else {
                break;
            }
        }
        
        if (!doc_lines.empty()) {
            std::reverse(doc_lines.begin(), doc_lines.end());
            std::string result;
            for (size_t k = 0; k < doc_lines.size(); k++) {
                if (k > 0) result += "\n";
                result += doc_lines[k];
            }
            return format_doc_as_markdown(result);
        }
    }
    
    return "";
}

// ═══════════════════════════════════════════════════════════════════════════════
// تقسيم النص إلى أسطر
// ═══════════════════════════════════════════════════════════════════════════════
std::vector<std::string> split_lines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    return lines;
}

// ═══════════════════════════════════════════════════════════════════════════════
int main() {
    // قراءة ملف الاختبار
    // (AR) نستخدم wstring للتعامل مع أسماء الملفات العربية على Windows
    std::wstring filepath = L"examples\\test_lsp_hover.\x0635"; // .ص
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        filepath = L"c:\\s_lang\\s-programming-language\\examples\\test_lsp_hover.\x0635";
        file.open(filepath, std::ios::binary);
    }
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open test file\n";
        return 1;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();
    
    auto lines = split_lines(content);
    
    std::cout << "=== Test LSP Doc Comment Extraction ===" << std::endl;
    std::cout << "Total lines: " << lines.size() << std::endl;
    std::cout << std::endl;
    
    // البحث عن الرموز باستخدام regex
    // (AR) نفس regex المستخدم في parse_symbols
    std::regex func_regex(R"(\xd8\xaf\xd8\xa7\xd9\x84\xd8\xa9\s+(\S+)\s*\()"); // دالة
    std::regex class_regex(R"(\xd8\xb5\xd9\x86\xd9\x81\s+(\S+))"); // صنف  
    std::regex const_regex(R"(\xd8\xab\xd8\xa7\xd8\xa8\xd8\xaa\s+(\S+)\s*=)"); // ثابت
    
    int pass = 0, fail = 0;
    
    for (size_t i = 0; i < lines.size(); ++i) {
        const auto& line = lines[i];
        std::smatch match;
        
        std::string symbol_name;
        std::string symbol_type;
        
        if (std::regex_search(line, match, func_regex)) {
            symbol_name = match[1].str();
            symbol_type = "FUNCTION";
        } else if (std::regex_search(line, match, class_regex)) {
            symbol_name = match[1].str();
            symbol_type = "CLASS";
        } else if (std::regex_search(line, match, const_regex)) {
            symbol_name = match[1].str();
            symbol_type = "CONST";
        } else {
            continue;
        }
        
        std::string doc = extract_doc_comment(lines, i);
        
        std::cout << "--- Line " << (i+1) << ": " << symbol_type << " [" << symbol_name << "] ---" << std::endl;
        if (!doc.empty()) {
            std::cout << "  DOC: \"" << doc << "\"" << std::endl;
            pass++;
        } else {
            std::cout << "  DOC: (none)" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // التحقق من النتائج المتوقعة
    std::cout << "=== Results ===" << std::endl;
    std::cout << "Symbols with docs: " << pass << std::endl;
    
    // تحقق إضافي: التعليقات التي تحتوي @معطى يجب أن تُنسق
    bool has_rich = false;
    for (size_t i = 0; i < lines.size(); ++i) {
        const auto& line = lines[i];
        std::smatch match;
        if (std::regex_search(line, match, func_regex)) {
            std::string doc = extract_doc_comment(lines, i);
            // التحقق من أن @معطى تحولت إلى **المعطيات:**
            if (doc.find("**") != std::string::npos) {
                has_rich = true;
                std::cout << "RICH FORMAT detected for: " << match[1].str() << std::endl;
            }
        }
    }
    
    if (pass >= 5 && has_rich) {
        std::cout << "PASS: All documented symbols extracted with rich Markdown!" << std::endl;
        return 0;
    } else if (pass >= 5) {
        std::cout << "PARTIAL: Docs extracted but no rich formatting detected" << std::endl;
        return 1;
    } else {
        std::cout << "FAIL: Expected at least 5 documented symbols, got " << pass << std::endl;
        return 1;
    }
}
