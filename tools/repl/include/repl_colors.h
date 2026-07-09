// بسم الله الرحمن الرحيم
/**
 * @file repl_colors.h
 * @brief (AR) ثوابت ألوان ANSI مشتركة لأداة REPL — مصدر واحد يمنع تكرار التعريف
 *        عبر repl_engine.cpp و repl_commands.cpp (كان مكرَّرًا — م-2 من التدقيق).
 *        (EN) Shared ANSI color constants for the REPL — a single source that
 *        removes the duplicate definitions across repl_engine.cpp and
 *        repl_commands.cpp (previously duplicated — audit item م-2).
 *
 * @author Sad Language Team
 */

#pragma once

namespace Sad {
namespace REPL {
namespace Colors {

inline constexpr const char *RESET = "\033[0m";
inline constexpr const char *BOLD = "\033[1m";
inline constexpr const char *RED = "\033[31m";
inline constexpr const char *GREEN = "\033[32m";
inline constexpr const char *YELLOW = "\033[33m";
inline constexpr const char *BLUE = "\033[34m";
inline constexpr const char *MAGENTA = "\033[35m";
inline constexpr const char *CYAN = "\033[36m";

} // namespace Colors
} // namespace REPL
} // namespace Sad
