// html_emitter.cpp — مصدّر HTML/CSS
#include <string>
#include <sstream>

class HtmlEmitter {
public:
    std::string emit_page(const std::string& title, const std::string& body) {
        std::ostringstream out;
        out << "<!DOCTYPE html>\n<html lang=\"ar\" dir=\"rtl\">\n<head>\n";
        out << "  <meta charset=\"UTF-8\">\n";
        out << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        out << "  <title>" << title << "</title>\n";
        out << "  <style>\n" << default_css() << "  </style>\n";
        out << "</head>\n<body>\n" << body << "</body>\n</html>\n";
        return out.str();
    }

    std::string emit_div(const std::string& class_name, const std::string& children) {
        return "  <div class=\"" + class_name + "\">\n" + children + "  </div>\n";
    }

    std::string emit_text(const std::string& text, const std::string& tag = "p") {
        return "    <" + tag + ">" + text + "</" + tag + ">\n";
    }

    std::string emit_button(const std::string& label, const std::string& on_click = "") {
        return "    <button" + (on_click.empty() ? "" : " onclick=\"" + on_click + "\"") + ">" + label + "</button>\n";
    }

    std::string emit_input(const std::string& type = "text", const std::string& placeholder = "") {
        return "    <input type=\"" + type + "\" placeholder=\"" + placeholder + "\">\n";
    }

private:
    std::string default_css() {
        return "    * { box-sizing: border-box; margin: 0; padding: 0; }\n"
               "    body { font-family: 'Segoe UI', Tahoma, sans-serif; direction: rtl; padding: 1rem; }\n"
               "    button { padding: 0.5rem 1rem; cursor: pointer; border-radius: 4px; border: 1px solid #ccc; }\n"
               "    input { padding: 0.5rem; border: 1px solid #ccc; border-radius: 4px; direction: rtl; }\n";
    }
};

extern "C" {
    const char* html_emit(const char* component_json) {
        // (AR) thread_local لأمان الخيوط / (EN) thread_local for thread safety
        thread_local std::string result;
        HtmlEmitter emitter;
        result = emitter.emit_page("\xD8\xAA\xD8\xB7\xD8\xA8\xD9\x8A\xD9\x82 \xD8\xB5", // تطبيق ص
            emitter.emit_div("container", emitter.emit_text("\xD9\x85\xD8\xB1\xD8\xAD\xD8\xA8\xD8\xA7\xD9\x8B", "h1") + emitter.emit_button("\xD8\xA7\xD8\xB6\xD8\xBA\xD8\xB7 \xD9\x87\xD9\x86\xD8\xA7")));
        return result.c_str();
    }
}
