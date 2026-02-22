// compose_emitter.cpp — مصدّر Kotlin Compose
#include <string>
#include <sstream>

class ComposeEmitter {
public:
    std::string emit_composable(const std::string& name, const std::string& body) {
        std::ostringstream out;
        out << "@Composable\n";
        out << "fun " << name << "() {\n";
        out << body;
        out << "}\n";
        return out.str();
    }

    std::string emit_column(const std::string& children) {
        return "    Column(\n        modifier = Modifier.fillMaxSize()\n    ) {\n" + children + "    }\n";
    }

    std::string emit_text(const std::string& text, const std::string& style = "MaterialTheme.typography.bodyMedium") {
        return "        Text(text = \"" + text + "\", style = " + style + ")\n";
    }

    std::string emit_button(const std::string& label, const std::string& on_click = "{}") {
        return "        Button(onClick = " + on_click + ") {\n            Text(\"" + label + "\")\n        }\n";
    }

    std::string emit_row(const std::string& children) {
        return "    Row(\n        modifier = Modifier.fillMaxWidth(),\n        horizontalArrangement = Arrangement.SpaceBetween\n    ) {\n" + children + "    }\n";
    }
};

extern "C" {
    const char* compose_emit(const char* component_json) {
        static std::string result;
        ComposeEmitter emitter;
        result = emitter.emit_composable("MainScreen", emitter.emit_column(emitter.emit_text("مرحباً بالعربية") + emitter.emit_button("اضغط هنا")));
        return result.c_str();
    }
}
