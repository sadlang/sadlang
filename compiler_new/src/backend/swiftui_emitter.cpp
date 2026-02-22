// swiftui_emitter.cpp — مصدّر SwiftUI
#include <string>
#include <sstream>

class SwiftUiEmitter {
public:
    std::string emit_view(const std::string& name, const std::string& body) {
        std::ostringstream out;
        out << "struct " << name << ": View {\n";
        out << "    var body: some View {\n";
        out << body;
        out << "    }\n";
        out << "}\n";
        return out.str();
    }

    std::string emit_vstack(const std::string& children) {
        return "        VStack {\n" + children + "        }\n";
    }

    std::string emit_hstack(const std::string& children) {
        return "        HStack {\n" + children + "        }\n";
    }

    std::string emit_text(const std::string& text) {
        return "            Text(\"" + text + "\")\n                .environment(\\.layoutDirection, .rightToLeft)\n";
    }

    std::string emit_button(const std::string& label, const std::string& action = "{}") {
        return "            Button(\"" + label + "\") " + action + "\n";
    }
};

extern "C" {
    const char* swiftui_emit(const char* component_json) {
        static std::string result;
        SwiftUiEmitter emitter;
        result = emitter.emit_view("MainView", emitter.emit_vstack(emitter.emit_text("مرحباً بالعربية") + emitter.emit_button("اضغط هنا")));
        return result.c_str();
    }
}
