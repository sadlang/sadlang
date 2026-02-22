// ===================================================================
// تطبيق الطرفية لنظام صدOS
// المرحلة 52: طرفية تدعم العربية بشكل أصيل
// ===================================================================

#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <deque>

namespace sad::os::ui {

/// لون خلية الطرفية
struct TermColor {
    uint8_t r, g, b;
    static TermColor defaultFg() { return {220, 220, 220}; }
    static TermColor defaultBg() { return {30, 30, 46}; }
};

/// سمات خلية واحدة في الطرفية
struct CellAttributes {
    TermColor fg;          // لون النص
    TermColor bg;          // لون الخلفية
    bool bold;             // عريض
    bool italic;           // مائل
    bool underline;        // تحت خط
    bool blink;            // وميض
    bool rtl;              // نص عربي RTL
};

/// خلية واحدة في شبكة الطرفية
struct TerminalCell {
    char32_t character;        // الحرف (يونيكود كامل)
    CellAttributes attrs;      // السمات البصرية
    uint8_t width;             // عرض الحرف (1 أو 2)
};

/// سطر في الطرفية مع دعم ثنائي الاتجاه
class TerminalLine {
    std::vector<TerminalCell> cells_;   // خلايا السطر
    bool is_rtl_;                       // هل السطر عربي؟

public:
    explicit TerminalLine(uint32_t cols, bool rtl = false)
        : cells_(cols), is_rtl_(rtl) {
        CellAttributes default_attrs{
            TermColor::defaultFg(), TermColor::defaultBg(),
            false, false, false, false, rtl
        };
        for (auto& cell : cells_) {
            cell = {U' ', default_attrs, 1};
        }
    }

    /// كتابة حرف في موقع محدد
    void set_char(uint32_t col, char32_t ch, const CellAttributes& attrs) {
        if (col < cells_.size()) {
            cells_[col] = {ch, attrs, 1};
        }
    }

    const TerminalCell& at(uint32_t col) const { return cells_[col]; }
    size_t size() const { return cells_.size(); }
    bool is_rtl() const { return is_rtl_; }
};

/// محاكي الطرفية الافتراضية (VT100/xterm)
class TerminalEmulator {
    std::vector<TerminalLine> screen_;     // شبكة الشاشة
    std::deque<TerminalLine> scrollback_;  // سجل التمرير
    uint32_t cursor_row_, cursor_col_;     // موقع المؤشر
    uint32_t rows_, cols_;                 // أبعاد الشاشة
    CellAttributes current_attrs_;         // السمات الحالية
    size_t max_scrollback_;                // أقصى سجل تمرير

    /// حالة محلل تسلسلات الهروب
    enum class ParseState { NORMAL, ESCAPE, CSI };
    ParseState parse_state_;
    std::string csi_params_;               // معاملات CSI

public:
    TerminalEmulator(uint32_t rows = 24, uint32_t cols = 80)
        : cursor_row_(0), cursor_col_(0), rows_(rows), cols_(cols),
          max_scrollback_(10000), parse_state_(ParseState::NORMAL) {
        current_attrs_ = {TermColor::defaultFg(), TermColor::defaultBg(),
                         false, false, false, false, false};
        // تهيئة شبكة الشاشة
        for (uint32_t i = 0; i < rows; i++) {
            screen_.emplace_back(cols);
        }
    }

    /// معالجة بايتات الخرج من العملية الفرعية
    void process_output(const char* data, size_t len) {
        for (size_t i = 0; i < len; i++) {
            char ch = data[i];
            switch (parse_state_) {
                case ParseState::NORMAL:
                    if (ch == '\x1b') {
                        parse_state_ = ParseState::ESCAPE;
                    } else if (ch == '\n') {
                        new_line();
                    } else if (ch == '\r') {
                        cursor_col_ = 0;
                    } else {
                        put_char(static_cast<char32_t>(ch));
                    }
                    break;
                case ParseState::ESCAPE:
                    if (ch == '[') {
                        parse_state_ = ParseState::CSI;
                        csi_params_.clear();
                    } else {
                        parse_state_ = ParseState::NORMAL;
                    }
                    break;
                case ParseState::CSI:
                    if (ch >= 0x40 && ch <= 0x7E) {
                        execute_csi(ch);
                        parse_state_ = ParseState::NORMAL;
                    } else {
                        csi_params_ += ch;
                    }
                    break;
            }
        }
    }

    /// تغيير حجم الطرفية
    void resize(uint32_t new_rows, uint32_t new_cols) {
        rows_ = new_rows;
        cols_ = new_cols;
        screen_.clear();
        for (uint32_t i = 0; i < rows_; i++) {
            screen_.emplace_back(cols_);
        }
        if (cursor_row_ >= rows_) cursor_row_ = rows_ - 1;
        if (cursor_col_ >= cols_) cursor_col_ = cols_ - 1;
    }

private:
    /// كتابة حرف في الموقع الحالي
    void put_char(char32_t ch) {
        if (cursor_col_ >= cols_) {
            new_line();
        }
        screen_[cursor_row_].set_char(cursor_col_, ch, current_attrs_);
        cursor_col_++;
    }

    /// الانتقال لسطر جديد مع التمرير
    void new_line() {
        cursor_col_ = 0;
        if (cursor_row_ + 1 < rows_) {
            cursor_row_++;
        } else {
            // تمرير: نقل السطر الأول للسجل
            scrollback_.push_back(std::move(screen_[0]));
            if (scrollback_.size() > max_scrollback_) {
                scrollback_.pop_front();
            }
            screen_.erase(screen_.begin());
            screen_.emplace_back(cols_);
        }
    }

    /// تنفيذ تسلسل CSI (أوامر التحكم)
    void execute_csi(char final_ch) {
        // معالجة أوامر مثل: تحريك المؤشر، مسح، تلوين
        (void)final_ch;
    }
};

/// تطبيق الطرفية الكامل
class TerminalApp {
    TerminalEmulator emulator_;    // المحاكي
    std::string shell_path_;       // مسار الصدفة

public:
    TerminalApp() : shell_path_("/bin/sad-shell") {}

    /// تشغيل الطرفية مع صدفة النظام
    void launch() {
        // إنشاء عملية فرعية وربطها بالمحاكي عبر PTY
    }
};

} // namespace sad::os::ui
