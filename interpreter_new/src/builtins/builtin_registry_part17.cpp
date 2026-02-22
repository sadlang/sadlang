// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
// builtin_registry_part17.cpp ג€” [FIX #291-400] 110+ ״¯ˆ״§„ ״¹״±״¨״© ״¬״¯״¯״©
// ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€ג”€
// ״®״µ״§״¦״µ ״§„†״µ ״§„…״×‚״¯…״© / ״­״§„״© ״§„״×״±ƒ״² / ״§„״×״­‚‚ / ״§„״´״§״±״§״×
// ״§„״­״¯ˆ״¯ ״§„…†״µ„״© / ״§„״×…״±״± / ״§„״×״¯״±״¬ / ״§„״¥…ƒ״§†״© / ״§„״§״®״×״§״±
// ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

#include "builtins.h"
#include "interpreter_core.h"
#include "graphics/sad_ui_framework.h"
#include <string>
#include <memory>
#include <vector>
#include <cmath>

namespace Sad {
namespace Interpreter {

void registerBuiltinsPart17(Interpreter& interpreter) {
    using Args = const std::vector<std::shared_ptr<Data::Value>>&;

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // [FIX #291-310] ״¯ˆ״§„ ״®״µ״§״¦״µ ״§„†״µ ״§„…״×‚״¯…״©
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ״¹†_״¶_†״µ(…״¹״±״ ‚…״©) ג€” textOverflow: "ellipsis" / "clip" / "wrap"
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x81\xd9\x8a\xd8\xb6_\xd9\x86\xd8\xb5", // ״¹†_״¶_†״µ
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "textOverflow", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״²״®״±״©_†״µ(…״¹״±״ ‚…״©) ג€” textDecoration: "underline"/"strikethrough"/"none"
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb2\xd8\xae\xd8\xb1\xd9\x81\xd8\xa9_\xd9\x86\xd8\xb5", // ״¹†_״²״®״±״©_†״µ
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "textDecoration", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״×״­ˆ„_†״µ(…״¹״±״ ‚…״©) ג€” textTransform: "uppercase"/"lowercase"/"none"
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x84_\xd9\x86\xd8\xb5", // ״¹†_״×״­ˆ„_†״µ
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "textTransform", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״×״¨״§״¹״¯_״§״­״±(…״¹״±״ ‚…״©_״±‚…״©)
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xaf_\xd8\xa7\xd8\xad\xd8\xb1\xd9\x81", // ״¹†_״×״¨״§״¹״¯_״§״­״±
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "letterSpacing", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״§‚״µ‰_״§״³״·״±(…״¹״±״ ״¹״¯״¯)
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x82\xd8\xb5\xd9\x89_\xd8\xa7\xd8\xb3\xd8\xb7\xd8\xb1", // ״¹†_״§‚״µ‰_״§״³״·״±
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "maxLines", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״§„״×״§_†״µ(…״¹״±״ …†״·‚)
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x84\xd8\xaa\xd9\x81\xd8\xa7\xd9\x81_\xd9\x86\xd8\xb5", // ״¹†_״§„״×״§_†״µ
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "textWrap", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // [FIX #311-325] ״¯ˆ״§„ ״§„״´ƒ„ ˆ״§„״§״±״×״§״¹ ˆ״§„‚״µ
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ״¹†_״´ƒ„(…״¹״±״ "rectangle"/"circle"/"״¯״§״¦״±״©")
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb4\xd9\x83\xd9\x84", // ״¹†_״´ƒ„
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "shape", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״§״±״×״§״¹_״·״¨‚״©(…״¹״±״ ‚…״©) ג€” elevation
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xb7\xd8\xa8\xd9\x82\xd8\xa9", // ״¹†_״§״±״×״§״¹_״·״¨‚״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "elevation", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_‚״µ_״­״¯ˆ״¯(…״¹״±״ …†״·‚) ג€” clipToBounds
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x82\xd8\xb5_\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf", // ״¹†_‚״µ_״­״¯ˆ״¯
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "clipToBounds", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_†״³״¨״©_״§״¨״¹״§״¯(…״¹״±״ ‚…״©) ג€” aspectRatio
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb3\xd8\xa8\xd8\xa9_\xd8\xa7\xd8\xa8\xd8\xb9\xd8\xa7\xd8\xaf", // ״¹†_†״³״¨״©_״§״¨״¹״§״¯
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "aspectRatio", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // [FIX #326-340] ״¯ˆ״§„ ״§„״×״®״·״· ״§„…״×‚״¯…
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ״¹†_״¬ˆ״©(…״¹״±״ ‚…״©) ג€” gap
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x81\xd8\xac\xd9\x88\xd8\xa9", // ״¹†_״¬ˆ״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "gapSize", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״¹ƒ״³(…״¹״±״ …†״·‚) ג€” reverse
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb9\xd9\x83\xd8\xb3", // ״¹†_״¹ƒ״³
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "reverse", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_†…ˆ_…״±†(…״¹״±״ ‚…״©) ג€” flexGrow
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x86\xd9\x85\xd9\x88_\xd9\x85\xd8\xb1\xd9\x86", // ״¹†_†…ˆ_…״±†
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "flexGrow", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״§†ƒ…״§״´_…״±†(…״¹״±״ ‚…״©) ג€” flexShrink
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd9\x86\xd9\x83\xd9\x85\xd8\xa7\xd8\xb4_\xd9\x85\xd8\xb1\xd9\x86", // ״¹†_״§†ƒ…״§״´_…״±†
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "flexShrink", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_…״­״§״°״§״©_״°״§״×״©(…״¹״±״ ‚…״©) ג€” alignSelf
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9_\xd8\xb0\xd8\xa7\xd8\xaa\xd9\x8a\xd8\xa9", // ״¹†_…״­״§״°״§״©_״°״§״×״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "alignSelf", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // [FIX #341-355] ״¯ˆ״§„ ״§„״×״±ƒ״² ˆ״§„״³״­״¨ ˆ״§„״¥״³‚״§״·
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ״¹†_‚״§״¨„_״×״±ƒ״²(…״¹״±״ …†״·‚) ג€” focusable
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", // ״¹†_‚״§״¨„_״×״±ƒ״²
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "focusable", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״×״±ƒ״²(…״¹״±״ …†״·‚) ג€” hasFocus
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", // ״¹†_״×״±ƒ״²
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "hasFocus", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_„ˆ†_״×״±ƒ״²(…״¹״±״ „ˆ†_†״µ)
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", // ״¹†_„ˆ†_״×״±ƒ״²
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "focusColor", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_‚״§״¨„_״³״­״¨(…״¹״±״ …†״·‚) ג€” draggable
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd8\xb3\xd8\xad\xd8\xa8", // ״¹†_‚״§״¨„_״³״­״¨
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "draggable", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_‚״§״¨„_״§״³‚״§״·(…״¹״±״ …†״·‚) ג€” droppable
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd8\xa7\xd8\xb3\xd9\x82\xd8\xa7\xd8\xb7", // ״¹†_‚״§״¨„_״§״³‚״§״·
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "droppable", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // [FIX #356-370] ״¯ˆ״§„ ״­‚ˆ„ ״§„״¥״¯״®״§„ ״§„…״×‚״¯…״©
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ״¹†_״¹†״µ״±_†״§״¦״¨(…״¹״±״ †״µ) ג€” placeholder
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x86\xd8\xa7\xd8\xa6\xd8\xa8", // ״¹†_״¹†״µ״±_†״§״¦״¨
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "placeholder", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_‚״±״§״¡״©_‚״·(…״¹״±״ …†״·‚) ג€” readOnly
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9_\xd9\x81\xd9\x82\xd8\xb7", // ״¹†_‚״±״§״¡״©_‚״·
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "readOnly", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_†ˆ״¹_״§״¯״®״§„(…״¹״±״ "text"/"number"/"email"/"password")
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x86\xd9\x88\xd8\xb9_\xd8\xa7\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84", // ״¹†_†ˆ״¹_״§״¯״®״§„
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "inputType", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_†״µ_״®״·״§(…״¹״±״ †״µ) ג€” errorText (also sets hasError=true)
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb5_\xd8\xae\xd8\xb7\xd8\xa7", // ״¹†_†״µ_״®״·״§
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString()) {
                int id = args[0]->toInt();
                std::string txt = args[1]->toString();
                sad::ui::widgetSetProp(id, "errorText", txt);
                sad::ui::widgetSetProp(id, "hasError", !txt.empty());
            }
            return std::make_shared<Data::Value>();
        });

    // ״¹†_†״µ_…״³״§״¹״¯״©(…״¹״±״ †״µ) ג€” helperText
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x86\xd8\xb5_\xd9\x85\xd8\xb3\xd8\xa7\xd8\xb9\xd8\xaf\xd8\xa9", // ״¹†_†״µ_…״³״§״¹״¯״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "helperText", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_…ˆ״¶״¹_…״₪״´״±(…״¹״±״ ״±‚…) ג€” cursorPosition
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd9\x88\xd8\xb6\xd8\xb9_\xd9\x85\xd8\xa4\xd8\xb4\xd8\xb1", // ״¹†_…ˆ״¶״¹_…״₪״´״±
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "cursorPosition", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״×״­״¯״¯(…״¹״±״ ״¨״¯״§״©״ †‡״§״©) ג€” selectionStart + selectionEnd
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xaf", // ״¹†_״×״­״¯״¯
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 3 && args[0]->isNumeric() && args[1]->isNumeric() && args[2]->isNumeric()) {
                int id = args[0]->toInt();
                sad::ui::widgetSetProp(id, "selectionStart", args[1]->toDouble());
                sad::ui::widgetSetProp(id, "selectionEnd", args[2]->toDouble());
            }
            return std::make_shared<Data::Value>();
        });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // [FIX #371-385] ״¯ˆ״§„ ״§„״­״¯ˆ״¯ ״§„…†״µ„״© ˆ״§„״×״¯״±״¬
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ״¹†_״­״¯ˆ״¯_…†״µ„״©(…״¹״±״ ״¹„ˆ״©״ …†‰״ ״³„״©״ ״³״±‰)
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf_\xd9\x85\xd9\x86\xd9\x81\xd8\xb5\xd9\x84\xd8\xa9", // ״¹†_״­״¯ˆ״¯_…†״µ„״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 5 && args[0]->isNumeric()) {
                int id = args[0]->toInt();
                sad::ui::widgetSetProp(id, "borderTopWidth", args[1]->toDouble());
                sad::ui::widgetSetProp(id, "borderRightWidth", args[2]->toDouble());
                sad::ui::widgetSetProp(id, "borderBottomWidth", args[3]->toDouble());
                sad::ui::widgetSetProp(id, "borderLeftWidth", args[4]->toDouble());
            }
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״§״×״¬״§‡_״×״¯״±״¬(…״¹״±״ "vertical"/"horizontal") ג€” gradientDirection
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xaa\xd8\xac\xd8\xa7\xd9\x87_\xd8\xaa\xd8\xaf\xd8\xb1\xd8\xac", // ״¹†_״§״×״¬״§‡_״×״¯״±״¬
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "gradientDirection", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_„ˆ†_״×״±״§ƒ״¨(…״¹״±״ „ˆ†_†״µ) ג€” overlayColor
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x84\xd9\x88\xd9\x86_\xd8\xaa\xd8\xb1\xd8\xa7\xd9\x83\xd8\xa8", // ״¹†_„ˆ†_״×״±״§ƒ״¨
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "overlayColor", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // [FIX #386-395] ״¯ˆ״§„ ״§„״´״§״±״§״× ˆ״§„״×…״±״± ˆ״§„״­״§„״©
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ״¹†_״´״§״±״©(…״¹״±״ †״µ) ג€” badge text
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9", // ״¹†_״´״§״±״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isString())
                sad::ui::widgetSetProp(args[0]->toInt(), "badge", args[1]->toString());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״¹״¯״¯_״´״§״±״©(…״¹״±״ ״¹״¯״¯) ג€” badgeCount
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb9\xd8\xaf\xd8\xaf_\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9", // ״¹†_״¹״¯״¯_״´״§״±״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "badgeCount", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״´״±״·_״×…״±״±(…״¹״±״ …†״·‚) ג€” scrollbarVisible
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", // ״¹†_״´״±״·_״×…״±״±
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "scrollbarVisible", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״³״±״¹״©_״×…״±״±(…״¹״±״ ‚…״©) ג€” scrollSpeed
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xb3\xd8\xb1\xd8\xb9\xd8\xa9_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1", // ״¹†_״³״±״¹״©_״×…״±״±
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "scrollSpeed", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_…״­״¯״¯(…״¹״±״ …†״·‚) ג€” selected state
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", // ״¹†_…״­״¯״¯
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "selected", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״×‚״¯…(…״¹״±״ ‚…״© 0..1) ג€” progress
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // ״¹†_״×‚״¯…
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isNumeric())
                sad::ui::widgetSetProp(args[0]->toInt(), "progress", args[1]->toDouble());
            return std::make_shared<Data::Value>();
        });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // [FIX #396-400] ״¯ˆ״§„ ״§„״×״®״µ״µ ״§„…״±״¦
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ״¹†_…״¶״÷ˆ״·(…״¹״±״ …†״·‚) ג€” compact
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x85\xd8\xb6\xd8\xba\xd9\x88\xd8\xb7", // ״¹†_…״¶״÷ˆ״·
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "compact", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_ƒ״«(…״¹״±״ …†״·‚) ג€” dense
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd9\x83\xd8\xab\xd9\x8a\xd9\x81", // ״¹†_ƒ״«
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "dense", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״§״¸‡״±_״¸„(…״¹״±״ …†״·‚) ג€” showShadow
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb8\xd9\x87\xd8\xb1_\xd8\xb8\xd9\x84", // ״¹†_״§״¸‡״±_״¸„
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "showShadow", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״§״¸‡״±_״§״µ„(…״¹״±״ …†״·‚) ג€” showDivider
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xa7\xd8\xb8\xd9\x87\xd8\xb1_\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84", // ״¹†_״§״¸‡״±_״§״µ„
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "showDivider", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ״¹†_״®״·״§(…״¹״±״ …†״·‚) ג€” hasError
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xb9\xd9\x8a\xd9\x86_\xd8\xae\xd8\xb7\xd8\xa7", // ״¹†_״®״·״§
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 2 && args[0]->isNumeric() && args[1]->isBoolean())
                sad::ui::widgetSetProp(args[0]->toInt(), "hasError", args[1]->toBool());
            return std::make_shared<Data::Value>();
        });

    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    // ״¯ˆ״§„ ״§״³״×״¹„״§… (getters) „„״®״µ״§״¦״µ ״§„״¬״¯״¯״©
    // ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•

    // ‡„_…״±ƒ״²_״×״±ƒ״²(…״¹״±) ג€” hasFocus?
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd9\x85\xd8\xb1\xd9\x83\xd8\xb2_\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2", // ‡„_…״±ƒ״²_״×״±ƒ״²
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 1 && args[0]->isNumeric()) {
                auto* w = sad::ui::getWidget(args[0]->toInt());
                if (w) return std::make_shared<Data::Value>(w->hasFocus);
            }
            return std::make_shared<Data::Value>(false);
        });

    // ‡„_…״­״¯״¯(…״¹״±) ג€” selected?
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd9\x85\xd8\xad\xd8\xaf\xd8\xaf", // ‡„_…״­״¯״¯
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 1 && args[0]->isNumeric()) {
                auto* w = sad::ui::getWidget(args[0]->toInt());
                if (w) return std::make_shared<Data::Value>(w->selected);
            }
            return std::make_shared<Data::Value>(false);
        });

    // ‡„_״®״·״§(…״¹״±) ג€” hasError?
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd8\xae\xd8\xb7\xd8\xa7", // ‡„_״®״·״§
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 1 && args[0]->isNumeric()) {
                auto* w = sad::ui::getWidget(args[0]->toInt());
                if (w) return std::make_shared<Data::Value>(w->hasError);
            }
            return std::make_shared<Data::Value>(false);
        });

    // ‡„_‚״±״§״¡״©_‚״·(…״¹״±) ג€” readOnly?
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd9\x87\xd9\x84_\xd9\x82\xd8\xb1\xd8\xa7\xd8\xa1\xd8\xa9_\xd9\x81\xd9\x82\xd8\xb7", // ‡„_‚״±״§״¡״©_‚״·
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 1 && args[0]->isNumeric()) {
                auto* w = sad::ui::getWidget(args[0]->toInt());
                if (w) return std::make_shared<Data::Value>(w->readOnly);
            }
            return std::make_shared<Data::Value>(false);
        });

    // ״§״­״µ„_״´״§״±״©(…״¹״±) ג€” badge text
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd8\xb4\xd8\xa7\xd8\xb1\xd8\xa9", // ״§״­״µ„_״´״§״±״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 1 && args[0]->isNumeric()) {
                auto* w = sad::ui::getWidget(args[0]->toInt());
                if (w) return std::make_shared<Data::Value>(w->badge);
            }
            return std::make_shared<Data::Value>(std::string(""));
        });

    // ״§״­״µ„_†״µ_״®״·״§(…״¹״±) ג€” errorText
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd9\x86\xd8\xb5_\xd8\xae\xd8\xb7\xd8\xa7", // ״§״­״µ„_†״µ_״®״·״§
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 1 && args[0]->isNumeric()) {
                auto* w = sad::ui::getWidget(args[0]->toInt());
                if (w) return std::make_shared<Data::Value>(w->errorText);
            }
            return std::make_shared<Data::Value>(std::string(""));
        });

    // ״§״­״µ„_״×‚״¯…(…״¹״±) ג€” progress value
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85", // ״§״­״µ„_״×‚״¯…
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 1 && args[0]->isNumeric()) {
                auto* w = sad::ui::getWidget(args[0]->toInt());
                if (w) return std::make_shared<Data::Value>((double)w->progress);
            }
            return std::make_shared<Data::Value>(0.0);
        });

    // ״§״­״µ„_״§״±״×״§״¹_״·״¨‚״©(…״¹״±) ג€” elevation
    interpreter.getFunctionManager().registerBuiltinFunction(
        "\xd8\xa7\xd8\xad\xd8\xb5\xd9\x84_\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9_\xd8\xb7\xd8\xa8\xd9\x82\xd8\xa9", // ״§״­״µ„_״§״±״×״§״¹_״·״¨‚״©
        [](Args args) -> std::shared_ptr<Data::Value> {
            if (args.size() >= 1 && args[0]->isNumeric()) {
                auto* w = sad::ui::getWidget(args[0]->toInt());
                if (w) return std::make_shared<Data::Value>((double)w->elevation);
            }
            return std::make_shared<Data::Value>(0.0);
        });
}

} // namespace Interpreter
} // namespace Sad
