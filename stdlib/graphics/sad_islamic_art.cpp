// ═══════════════════════════════════════════════════════════════════════════════
// تطبيق نظام الزخارف الإسلامية — لغة ص
// Islamic Geometric Art System Implementation — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يحتوي على التنفيذ الكامل لجميع دوال الرسم الهندسي الإسلامي
// يتم استخدام مكتبة SDL2 لعمليات الرسم الأساسية (خطوط، نقاط، مستطيلات)
// جميع الأشكال الهندسية تُحسب رياضياً باستخدام حساب المثلثات
//
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_islamic_art.h"
#include <SDL.h>
#include <cmath>
#include <vector>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace sad { namespace islamic_art {

// ═══════════════════════════════════════════════════════════════════
// دوال مساعدة داخلية — لا تُصدَّر خارج هذا الملف
// ═══════════════════════════════════════════════════════════════════

// تحويل المؤشر العام إلى مؤشر SDL — يُستخدم في كل دالة رسم
static SDL_Renderer* asSDL(void* renderer) {
    return static_cast<SDL_Renderer*>(renderer);
}

// ضبط لون الرسم الحالي على SDL_Renderer
static void setColor(SDL_Renderer* r, const ArtColor& c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
}

// رسم خط بسماكة محددة — يرسم خطوطاً متوازية لمحاكاة السماكة
static void drawThickLine(SDL_Renderer* r, int x1, int y1, int x2, int y2, int thickness) {
    // إذا كان السمك 1، نرسم خطاً واحداً فقط
    if (thickness <= 1) {
        SDL_RenderDrawLine(r, x1, y1, x2, y2);
        return;
    }
    // حساب المتجه العمودي على الخط لإزاحة الخطوط المتوازية
    double dx = static_cast<double>(x2 - x1);
    double dy = static_cast<double>(y2 - y1);
    double len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.001) return; // تجاهل الخطوط ذات الطول صفر
    double nx = -dy / len; // المركبة الأفقية للمتجه العمودي
    double ny =  dx / len; // المركبة الرأسية للمتجه العمودي
    int half = thickness / 2;
    for (int i = -half; i <= half; ++i) {
        int ox = static_cast<int>(nx * i);
        int oy = static_cast<int>(ny * i);
        SDL_RenderDrawLine(r, x1 + ox, y1 + oy, x2 + ox, y2 + oy);
    }
}

// رسم دائرة باستخدام خوارزمية بريسنهام — تُستخدم في الزخارف النباتية
static void drawCircle(SDL_Renderer* r, int cx, int cy, int radius) {
    int x = radius, y = 0;
    int d = 1 - radius;
    while (x >= y) {
        // رسم النقاط الثمانية المتناظرة
        SDL_RenderDrawPoint(r, cx + x, cy + y);
        SDL_RenderDrawPoint(r, cx - x, cy + y);
        SDL_RenderDrawPoint(r, cx + x, cy - y);
        SDL_RenderDrawPoint(r, cx - x, cy - y);
        SDL_RenderDrawPoint(r, cx + y, cy + x);
        SDL_RenderDrawPoint(r, cx - y, cy + x);
        SDL_RenderDrawPoint(r, cx + y, cy - x);
        SDL_RenderDrawPoint(r, cx - y, cy - x);
        y++;
        if (d <= 0) {
            d += 2 * y + 1;
        } else {
            x--;
            d += 2 * (y - x) + 1;
        }
    }
}

// رسم دائرة مملوءة — تُستخدم كأوراق في الزخارف النباتية
static void drawFilledCircle(SDL_Renderer* r, int cx, int cy, int radius) {
    for (int dy = -radius; dy <= radius; ++dy) {
        int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy));
        SDL_RenderDrawLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

// ملء مضلع بتقنية خطوط المسح (Scanline Fill)
// تُستخدم لملء النجوم والأشكال الهندسية الأخرى
static void scanlineFillPolygon(SDL_Renderer* r,
                                const std::vector<int>& vx,
                                const std::vector<int>& vy,
                                const ArtColor& color) {
    if (vx.size() < 3) return;
    int n = static_cast<int>(vx.size());

    // إيجاد الحدود العليا والسفلى للمضلع
    int yMin = *std::min_element(vy.begin(), vy.end());
    int yMax = *std::max_element(vy.begin(), vy.end());

    setColor(r, color);

    // لكل سطر أفقي ضمن حدود المضلع
    for (int y = yMin; y <= yMax; ++y) {
        std::vector<int> intersections;
        // إيجاد نقاط تقاطع السطر مع أضلاع المضلع
        for (int i = 0; i < n; ++i) {
            int j = (i + 1) % n;
            int yi = vy[i], yj = vy[j];
            if ((yi <= y && yj > y) || (yj <= y && yi > y)) {
                int xi = vx[i], xj = vx[j];
                int xIntersect = xi + (y - yi) * (xj - xi) / (yj - yi);
                intersections.push_back(xIntersect);
            }
        }
        // ترتيب نقاط التقاطع ورسم الخطوط الأفقية بينها
        std::sort(intersections.begin(), intersections.end());
        for (size_t k = 0; k + 1 < intersections.size(); k += 2) {
            SDL_RenderDrawLine(r, intersections[k], y, intersections[k + 1], y);
        }
    }
}

// حساب رؤوس نجمة — النواة الرياضية لكل الأنماط النجمية
// تُرجع متجهين: أحدهما للإحداثيات الأفقية والآخر للرأسية
static void computeStarVertices(int points, int cx, int cy,
                                int outerR, int innerR,
                                std::vector<int>& outX,
                                std::vector<int>& outY) {
    int totalVerts = points * 2; // لكل رأس خارجي يوجد رأس داخلي
    double angleStep = M_PI / points; // الزاوية بين كل رأسين متتاليين
    double startAngle = -M_PI / 2.0; // البدء من الأعلى

    outX.resize(totalVerts);
    outY.resize(totalVerts);

    for (int i = 0; i < totalVerts; ++i) {
        double angle = startAngle + i * angleStep;
        int radius = (i % 2 == 0) ? outerR : innerR;
        outX[i] = cx + static_cast<int>(radius * std::cos(angle));
        outY[i] = cy + static_cast<int>(radius * std::sin(angle));
    }
}

// ═══════════════════════════════════════════════════════════════════
// تنفيذ دالة الحصول على ألوان نظام اللون
// الألوان مستوحاة من الفن الإسلامي التاريخي الحقيقي
// ═══════════════════════════════════════════════════════════════════

void getSchemeColors(ColorScheme scheme,
                     ArtColor& primary, ArtColor& secondary,
                     ArtColor& background, ArtColor& accent) {
    switch (scheme) {
        case ColorScheme::OttomanBlue:
            // الأزرق العثماني — مستوحى من بلاط مسجد السلطان أحمد (الجامع الأزرق)
            primary    = {28, 69, 135, 255};   // أزرق كوبالت عميق
            secondary  = {212, 175, 55, 255};  // ذهبي دافئ
            background = {245, 240, 230, 255}; // أبيض عاجي
            accent     = {65, 105, 170, 255};  // أزرق فاتح مكمل
            break;
        case ColorScheme::MoorishGreen:
            // الأخضر الأندلسي — مستوحى من قصر الحمراء في غرناطة
            primary    = {0, 100, 60, 255};    // أخضر زمردي غامق
            secondary  = {245, 240, 230, 255}; // أبيض كريمي
            background = {35, 40, 30, 255};    // أخضر داكن جداً (قريب من الأسود)
            accent     = {180, 155, 50, 255};  // ذهبي زيتوني
            break;
        case ColorScheme::PersianRose:
            // الوردي الفارسي — مستوحى من مسجد نصير الملك في شيراز
            primary    = {180, 60, 100, 255};  // وردي فارسي عتيق
            secondary  = {60, 170, 170, 255};  // فيروزي فاتح
            background = {255, 245, 238, 255}; // أبيض مائل للوردي
            accent     = {200, 160, 60, 255};  // ذهبي باهت
            break;
        case ColorScheme::MarrakechGold:
            // الذهبي المراكشي — مستوحى من الزخارف المغربية التقليدية
            primary    = {185, 140, 50, 255};  // ذهبي مراكشي
            secondary  = {120, 60, 30, 255};   // بني محروق (لون التراب)
            background = {250, 235, 200, 255}; // كريمي دافئ
            accent     = {160, 50, 30, 255};   // أحمر تراكوتا
            break;
        case ColorScheme::IznikCeramic:
            // سيراميك إزنيك — مستوحى من بلاط إزنيك التركي القرن السادس عشر
            primary    = {255, 252, 245, 255}; // أبيض خزفي
            secondary  = {20, 60, 140, 255};   // أزرق كوبالت إزنيكي
            background = {230, 238, 245, 255}; // أبيض مائل للأزرق
            accent     = {180, 35, 30, 255};   // أحمر أرمني (بولي أحمر)
            break;
        case ColorScheme::MamlukRed:
            // الأحمر المملوكي — مستوحى من المخطوطات والعمارة المملوكية في مصر
            primary    = {160, 30, 20, 255};   // أحمر مملوكي عميق
            secondary  = {200, 165, 55, 255};  // ذهبي ملكي
            background = {25, 20, 18, 255};    // أسود مائل للبني
            accent     = {240, 230, 210, 255}; // عاجي فاتح
            break;
        case ColorScheme::Custom:
        default:
            // الألوان المخصصة — افتراضي: أبيض وأسود
            primary    = {0, 0, 0, 255};
            secondary  = {128, 128, 128, 255};
            background = {255, 255, 255, 255};
            accent     = {64, 64, 64, 255};
            break;
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم نجمة واحدة — الدالة الأساسية لجميع الأنماط النجمية
// تستخدم حساب المثلثات لحساب الرؤوس ثم ملء المضلع وتحديده
// ═══════════════════════════════════════════════════════════════════

void drawSingleStar(void* renderer, int points,
                    int cx, int cy, int outerR, int innerR,
                    ArtColor fillColor, ArtColor lineColor,
                    int lineWidth) {
    SDL_Renderer* r = asSDL(renderer);

    // حساب رؤوس النجمة (خارجية وداخلية بالتناوب)
    std::vector<int> vx, vy;
    computeStarVertices(points, cx, cy, outerR, innerR, vx, vy);

    // ملء النجمة بلون التعبئة باستخدام تقنية خطوط المسح
    scanlineFillPolygon(r, vx, vy, fillColor);

    // رسم حواف النجمة بلون الخط المحدد
    setColor(r, lineColor);
    int n = static_cast<int>(vx.size());
    for (int i = 0; i < n; ++i) {
        int j = (i + 1) % n;
        drawThickLine(r, vx[i], vy[i], vx[j], vy[j], lineWidth);
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم نمط نجمي مع الهندسة المتصلة
// هذه الدالة ترسم النجمة الأساسية ثم تضيف خطوطاً هندسية مكملة
// ═══════════════════════════════════════════════════════════════════

void drawStarPattern(void* renderer, StarPattern type,
                     int cx, int cy, int radius,
                     ColorScheme scheme, int lineWidth) {
    SDL_Renderer* r = asSDL(renderer);

    // الحصول على الألوان من نظام اللون المحدد
    ArtColor primary, secondary, bg, accent;
    getSchemeColors(scheme, primary, secondary, bg, accent);

    int points = 8; // افتراضي
    switch (type) {
        case StarPattern::SixPointed:    points = 6;  break;
        case StarPattern::EightPointed:  points = 8;  break;
        case StarPattern::TenPointed:    points = 10; break;
        case StarPattern::TwelvePointed: points = 12; break;
    }

    // حساب نصف القطر الداخلي — يختلف حسب عدد الرؤوس لإنتاج شكل متناسق
    int innerR = static_cast<int>(radius * 0.45);

    // رسم النجمة الأساسية
    drawSingleStar(renderer, points, cx, cy, radius, innerR, secondary, primary, lineWidth);

    // رسم الهندسة المتصلة — خطوط تربط الرؤوس الخارجية لتكوين المضلع المحيط
    setColor(r, accent);
    double angleStep = 2.0 * M_PI / points;
    double startAngle = -M_PI / 2.0;
    for (int i = 0; i < points; ++i) {
        double a1 = startAngle + i * angleStep;
        double a2 = startAngle + ((i + 2) % points) * angleStep;
        int x1 = cx + static_cast<int>(radius * 0.85 * std::cos(a1));
        int y1 = cy + static_cast<int>(radius * 0.85 * std::sin(a1));
        int x2 = cx + static_cast<int>(radius * 0.85 * std::cos(a2));
        int y2 = cy + static_cast<int>(radius * 0.85 * std::sin(a2));
        drawThickLine(r, x1, y1, x2, y2, lineWidth);
    }

    // رسم دائرة مركزية صغيرة كعنصر زخرفي
    setColor(r, primary);
    drawFilledCircle(r, cx, cy, radius / 6);
}

// ═══════════════════════════════════════════════════════════════════
// رسم شبكة من الأنماط النجمية — تبليط المنطقة المحددة
// تُنشئ فسيفساء متكررة من النجوم عبر كامل المساحة
// ═══════════════════════════════════════════════════════════════════

void drawStarGrid(void* renderer, StarPattern type,
                  int x, int y, int width, int height,
                  int cellSize, ColorScheme scheme, int lineWidth) {
    // حساب نصف قطر النجمة من حجم الخلية — نترك هامشاً صغيراً
    int starRadius = cellSize / 2 - 2;
    if (starRadius < 4) return; // الخلية صغيرة جداً للرسم

    // التبليط: تكرار النمط أفقياً ورأسياً
    for (int row = y + cellSize / 2; row < y + height; row += cellSize) {
        for (int col = x + cellSize / 2; col < x + width; col += cellSize) {
            drawStarPattern(renderer, type, col, row, starRadius, scheme, lineWidth);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// دوال الفسيفساء الهندسية — أربعة أنماط تقليدية
// ═══════════════════════════════════════════════════════════════════

// رسم مسدس (سداسي) واحد
static void drawHexagon(SDL_Renderer* r, int cx, int cy, int radius,
                        const ArtColor& fill, const ArtColor& line, int lw) {
    std::vector<int> vx(6), vy(6);
    for (int i = 0; i < 6; ++i) {
        double angle = M_PI / 6.0 + i * M_PI / 3.0; // بدء من 30 درجة لتسطيح القاعدة
        vx[i] = cx + static_cast<int>(radius * std::cos(angle));
        vy[i] = cy + static_cast<int>(radius * std::sin(angle));
    }
    scanlineFillPolygon(r, vx, vy, fill);
    setColor(r, line);
    for (int i = 0; i < 6; ++i) {
        int j = (i + 1) % 6;
        drawThickLine(r, vx[i], vy[i], vx[j], vy[j], lw);
    }
}

// رسم مثمن (ثماني) واحد
static void drawOctagon(SDL_Renderer* r, int cx, int cy, int radius,
                        const ArtColor& fill, const ArtColor& line, int lw) {
    std::vector<int> vx(8), vy(8);
    for (int i = 0; i < 8; ++i) {
        double angle = M_PI / 8.0 + i * M_PI / 4.0;
        vx[i] = cx + static_cast<int>(radius * std::cos(angle));
        vy[i] = cy + static_cast<int>(radius * std::sin(angle));
    }
    scanlineFillPolygon(r, vx, vy, fill);
    setColor(r, line);
    for (int i = 0; i < 8; ++i) {
        int j = (i + 1) % 8;
        drawThickLine(r, vx[i], vy[i], vx[j], vy[j], lw);
    }
}

// رسم فسيفساء سداسية — شبكة من المسدسات المتراصة
static void tessellateHexagonal(SDL_Renderer* r, int x, int y, int w, int h,
                                int cellSize, const ArtColor& p, const ArtColor& s,
                                const ArtColor& bg, int lw) {
    int radius = cellSize / 2;
    // المسافات الأفقية والرأسية بين مراكز المسدسات
    double hSpacing = radius * 1.5;
    double vSpacing = radius * std::sqrt(3.0);
    int row = 0;
    for (double cy = y + radius; cy < y + h; cy += vSpacing, ++row) {
        double xOffset = (row % 2 == 0) ? 0.0 : hSpacing * 0.5;  // إزاحة الصفوف الزوجية
        // تناوب الألوان بين صفوف المسدسات لإثراء التصميم
        for (double cx = x + radius + xOffset; cx < x + w; cx += hSpacing) {
            bool alt = (static_cast<int>((cx - x) / hSpacing) + row) % 2 == 0;
            drawHexagon(r, static_cast<int>(cx), static_cast<int>(cy),
                        radius - 1, alt ? p : s, bg, lw);
        }
    }
}

// رسم فسيفساء ثمانية — مثمنات ومربعات متناوبة
static void tessellateOctagonal(SDL_Renderer* r, int x, int y, int w, int h,
                                int cellSize, const ArtColor& p, const ArtColor& s,
                                const ArtColor& accent, int lw) {
    int octR = cellSize / 2;
    // حساب جانب المثمن لإيجاد حجم المربع الملائم
    double step = cellSize * (1.0 + 1.0 / std::sqrt(2.0)) * 0.5;
    double gap = step - octR; // حجم المربع بين المثمنات

    for (double cy = y + octR; cy < y + h; cy += step) {
        for (double cx = x + octR; cx < x + w; cx += step) {
            // رسم المثمن
            drawOctagon(r, static_cast<int>(cx), static_cast<int>(cy),
                        octR - 1, p, accent, lw);
            // رسم المربع الصغير في الزاوية (جنوب شرق المثمن)
            int sx = static_cast<int>(cx + step / 2 - gap / 2);
            int sy = static_cast<int>(cy + step / 2 - gap / 2);
            int gs = static_cast<int>(gap);
            SDL_Rect sq = {sx, sy, gs, gs};
            setColor(r, s);
            SDL_RenderFillRect(r, &sq);
            setColor(r, accent);
            SDL_RenderDrawRect(r, &sq);
        }
    }
}

// رسم فسيفساء كريه — أنماط عشارية مبسطة مستوحاة من بلاط دارب إمام
static void tessellateGirih(SDL_Renderer* r, int x, int y, int w, int h,
                            int cellSize, const ArtColor& p, const ArtColor& s,
                            const ArtColor& accent, int lw) {
    int decR = cellSize / 2;
    // الإزاحات المحسوبة للتبليط العشاري — تقريب هندسي
    double hStep = cellSize * 1.618; // النسبة الذهبية — خاصية أساسية في بلاط كريه
    double vStep = cellSize * 1.4;
    int row = 0;
    for (double cy = y + decR; cy < y + h; cy += vStep, ++row) {
        double xOff = (row % 2 == 0) ? 0.0 : hStep * 0.5;
        for (double cx = x + decR + xOff; cx < x + w; cx += hStep) {
            // رسم عشاري في كل موقع
            std::vector<int> vx(10), vy(10);
            for (int i = 0; i < 10; ++i) {
                double angle = i * M_PI / 5.0 - M_PI / 2.0;
                vx[i] = static_cast<int>(cx + (decR - 1) * std::cos(angle));
                vy[i] = static_cast<int>(cy + (decR - 1) * std::sin(angle));
            }
            scanlineFillPolygon(r, vx, vy, p);
            setColor(r, accent);
            for (int i = 0; i < 10; ++i) {
                int j = (i + 1) % 10;
                drawThickLine(r, vx[i], vy[i], vx[j], vy[j], lw);
            }
            // رسم خطوط كريه الداخلية — خطوط تربط كل رأس بالرأس المقابل
            setColor(r, s);
            for (int i = 0; i < 5; ++i) {
                drawThickLine(r, vx[i], vy[i], vx[i + 5], vy[i + 5], 1);
            }
        }
    }
}

// رسم مقرنصات مبسطة — تمثيل ثنائي الأبعاد للمقرنصات المعمارية
// المقرنصات هي عناصر معمارية إسلامية على شكل خلايا نحل معلقة
static void tessellateMuqarnas(SDL_Renderer* r, int x, int y, int w, int h,
                               int cellSize, const ArtColor& p, const ArtColor& s,
                               const ArtColor& accent, int lw) {
    int half = cellSize / 2;
    int row = 0;
    for (int cy = y; cy < y + h; cy += half, ++row) {
        int xOff = (row % 2 == 0) ? 0 : half;
        for (int cx = x + xOff; cx < x + w; cx += cellSize) {
            // رسم شكل ماسي (معين) — الوحدة الأساسية للمقرنصات ثنائية الأبعاد
            std::vector<int> dx = {cx + half, cx + cellSize, cx + half, cx};
            std::vector<int> dy = {cy,        cy + half,     cy + cellSize, cy + half};
            // تدرج لوني بسيط حسب الصف لمحاكاة العمق
            ArtColor fill = (row % 3 == 0) ? p : (row % 3 == 1) ? s : accent;
            scanlineFillPolygon(r, dx, dy, fill);
            setColor(r, {0, 0, 0, 100}); // خطوط شفافة داكنة لإضافة بُعد
            for (int i = 0; i < 4; ++i) {
                int j = (i + 1) % 4;
                drawThickLine(r, dx[i], dy[i], dx[j], dy[j], lw);
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// الدالة الرئيسية لرسم الفسيفساء — توزع العمل حسب النوع
// ═══════════════════════════════════════════════════════════════════

void drawTessellation(void* renderer, TessellationType type,
                      int x, int y, int width, int height,
                      int cellSize, ColorScheme scheme, int lineWidth) {
    SDL_Renderer* r = asSDL(renderer);
    ArtColor primary, secondary, bg, accent;
    getSchemeColors(scheme, primary, secondary, bg, accent);

    // ملء الخلفية أولاً
    setColor(r, bg);
    SDL_Rect bgRect = {x, y, width, height};
    SDL_RenderFillRect(r, &bgRect);

    // اختيار نوع الفسيفساء المطلوب
    switch (type) {
        case TessellationType::Hexagonal:
            tessellateHexagonal(r, x, y, width, height, cellSize, primary, secondary, bg, lineWidth);
            break;
        case TessellationType::Octagonal:
            tessellateOctagonal(r, x, y, width, height, cellSize, primary, secondary, accent, lineWidth);
            break;
        case TessellationType::Girih:
            tessellateGirih(r, x, y, width, height, cellSize, primary, secondary, accent, lineWidth);
            break;
        case TessellationType::Muqarnas:
            tessellateMuqarnas(r, x, y, width, height, cellSize, primary, secondary, accent, lineWidth);
            break;
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم الإطارات الزخرفية — أربعة أنماط
// ═══════════════════════════════════════════════════════════════════

// شريط هندسي متكرر — معينات (أشكال ماسية) متكررة على طول الحواف
static void borderGeometricBand(SDL_Renderer* r, int x, int y, int w, int h,
                                int bw, const ArtColor& p, const ArtColor& s,
                                const ArtColor& accent, int lw) {
    int diamondSize = bw;
    // الحافة العلوية
    for (int cx = x; cx < x + w; cx += diamondSize) {
        std::vector<int> dx = {cx + diamondSize / 2, cx + diamondSize, cx + diamondSize / 2, cx};
        std::vector<int> dy = {y, y + bw / 2, y + bw, y + bw / 2};
        scanlineFillPolygon(r, dx, dy, ((cx / diamondSize) % 2 == 0) ? p : s);
        setColor(r, accent);
        for (int i = 0; i < 4; ++i) {
            int j = (i + 1) % 4;
            drawThickLine(r, dx[i], dy[i], dx[j], dy[j], lw);
        }
    }
    // الحافة السفلية
    for (int cx = x; cx < x + w; cx += diamondSize) {
        int by = y + h - bw;
        std::vector<int> dx = {cx + diamondSize / 2, cx + diamondSize, cx + diamondSize / 2, cx};
        std::vector<int> dy = {by, by + bw / 2, by + bw, by + bw / 2};
        scanlineFillPolygon(r, dx, dy, ((cx / diamondSize) % 2 == 0) ? s : p);
        setColor(r, accent);
        for (int i = 0; i < 4; ++i) {
            int j = (i + 1) % 4;
            drawThickLine(r, dx[i], dy[i], dx[j], dy[j], lw);
        }
    }
    // الحافة اليسرى
    for (int cy = y + bw; cy < y + h - bw; cy += diamondSize) {
        std::vector<int> dx = {x + bw / 2, x + bw, x + bw / 2, x};
        std::vector<int> dy = {cy, cy + diamondSize / 2, cy + diamondSize, cy + diamondSize / 2};
        scanlineFillPolygon(r, dx, dy, ((cy / diamondSize) % 2 == 0) ? p : s);
        setColor(r, accent);
        for (int i = 0; i < 4; ++i) {
            int j = (i + 1) % 4;
            drawThickLine(r, dx[i], dy[i], dx[j], dy[j], lw);
        }
    }
    // الحافة اليمنى
    for (int cy = y + bw; cy < y + h - bw; cy += diamondSize) {
        int rx = x + w - bw;
        std::vector<int> dx = {rx + bw / 2, rx + bw, rx + bw / 2, rx};
        std::vector<int> dy = {cy, cy + diamondSize / 2, cy + diamondSize, cy + diamondSize / 2};
        scanlineFillPolygon(r, dx, dy, ((cy / diamondSize) % 2 == 0) ? s : p);
        setColor(r, accent);
        for (int i = 0; i < 4; ++i) {
            int j = (i + 1) % 4;
            drawThickLine(r, dx[i], dy[i], dx[j], dy[j], lw);
        }
    }
}

// شريط متشابك — نسيج هندسي يشبه العقدة الكلتية
// يُحاكي التشابك بتقاطع خطوط قطرية مع تبادل الألوان
static void borderInterlaced(SDL_Renderer* r, int x, int y, int w, int h,
                             int bw, const ArtColor& p, const ArtColor& s,
                             const ArtColor& accent, int lw) {
    int segLen = bw * 2; // طول كل قطعة من النسيج

    // رسم الشرائط المتقاطعة على الحواف الأربع
    auto drawInterlaceSegment = [&](int sx, int sy, int ex, int ey, bool over) {
        setColor(r, over ? p : s);
        drawThickLine(r, sx, sy, ex, ey, lw + 2);
        setColor(r, accent);
        drawThickLine(r, sx, sy, ex, ey, lw);
    };

    // الحافة العلوية — خطوط قطرية متشابكة
    for (int cx = x; cx < x + w; cx += segLen) {
        bool over = ((cx / segLen) % 2 == 0);
        drawInterlaceSegment(cx, y, cx + segLen / 2, y + bw, over);
        drawInterlaceSegment(cx + segLen / 2, y + bw, cx + segLen, y, !over);
    }
    // الحافة السفلية
    int by = y + h - bw;
    for (int cx = x; cx < x + w; cx += segLen) {
        bool over = ((cx / segLen) % 2 == 0);
        drawInterlaceSegment(cx, by + bw, cx + segLen / 2, by, over);
        drawInterlaceSegment(cx + segLen / 2, by, cx + segLen, by + bw, !over);
    }
    // الحافة اليسرى
    for (int cy = y; cy < y + h; cy += segLen) {
        bool over = ((cy / segLen) % 2 == 0);
        drawInterlaceSegment(x, cy, x + bw, cy + segLen / 2, over);
        drawInterlaceSegment(x + bw, cy + segLen / 2, x, cy + segLen, !over);
    }
    // الحافة اليمنى
    int rx = x + w - bw;
    for (int cy = y; cy < y + h; cy += segLen) {
        bool over = ((cy / segLen) % 2 == 0);
        drawInterlaceSegment(rx + bw, cy, rx, cy + segLen / 2, over);
        drawInterlaceSegment(rx, cy + segLen / 2, rx + bw, cy + segLen, !over);
    }
}

// كتابة كوفية هندسية مبسطة — محاكاة بكتل مستطيلة
// الخط الكوفي هو أقدم خط عربي، ويتميز بزواياه المستقيمة
static void borderKufic(SDL_Renderer* r, int x, int y, int w, int h,
                        int bw, const ArtColor& p, const ArtColor& s,
                        const ArtColor& /*accent*/, int lw) {
    // أنماط الكتل: 1 = مملوء، 0 = فارغ — يحاكي أشكال الحروف الكوفية
    const int pattern[] = {1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1};
    const int patLen = 16;
    int blockW = bw / 2;
    if (blockW < 2) blockW = 2;

    // الحافة العلوية — كتل رأسية متفاوتة الارتفاع
    int idx = 0;
    for (int cx = x; cx < x + w; cx += blockW, ++idx) {
        if (pattern[idx % patLen]) {
            int bh = (pattern[(idx + 1) % patLen]) ? bw : bw * 2 / 3; // ارتفاع متغير
            SDL_Rect block = {cx, y, blockW - 1, bh};
            setColor(r, p);
            SDL_RenderFillRect(r, &block);
            setColor(r, s);
            SDL_RenderDrawRect(r, &block);
        }
    }
    // الحافة السفلية
    idx = 0;
    for (int cx = x; cx < x + w; cx += blockW, ++idx) {
        if (pattern[(idx + 3) % patLen]) {
            int bh = (pattern[(idx + 2) % patLen]) ? bw : bw * 2 / 3;
            SDL_Rect block = {cx, y + h - bh, blockW - 1, bh};
            setColor(r, p);
            SDL_RenderFillRect(r, &block);
            setColor(r, s);
            SDL_RenderDrawRect(r, &block);
        }
    }
    // الحافة اليسرى
    idx = 0;
    for (int cy = y + bw; cy < y + h - bw; cy += blockW, ++idx) {
        if (pattern[(idx + 5) % patLen]) {
            int blkW = (pattern[(idx + 1) % patLen]) ? bw : bw * 2 / 3;
            SDL_Rect block = {x, cy, blkW, blockW - 1};
            setColor(r, p);
            SDL_RenderFillRect(r, &block);
            setColor(r, s);
            SDL_RenderDrawRect(r, &block);
        }
    }
    // الحافة اليمنى
    idx = 0;
    for (int cy = y + bw; cy < y + h - bw; cy += blockW, ++idx) {
        if (pattern[(idx + 7) % patLen]) {
            int blkW = (pattern[(idx + 2) % patLen]) ? bw : bw * 2 / 3;
            SDL_Rect block = {x + w - blkW, cy, blkW, blockW - 1};
            setColor(r, p);
            SDL_RenderFillRect(r, &block);
            setColor(r, s);
            SDL_RenderDrawRect(r, &block);
        }
    }
    // خطوط الإطار الخارجي لتوحيد المظهر
    (void)lw;
    setColor(r, p);
    SDL_Rect outer = {x, y, w, h};
    SDL_RenderDrawRect(r, &outer);
}

// زخرفة نباتية حدودية — لفائف مع دوائر تمثل الأزهار
static void borderFloral(SDL_Renderer* r, int x, int y, int w, int h,
                         int bw, const ArtColor& p, const ArtColor& s,
                         const ArtColor& accent, int lw) {
    int flowerR = bw / 4; // نصف قطر الزهرة
    if (flowerR < 2) flowerR = 2;
    int vineY = bw / 2;   // الخط المركزي للكرمة
    double freq = M_PI / (bw * 1.5); // تردد الموجة الجيبية

    // الحافة العلوية — كرمة جيبية مع أزهار
    setColor(r, p);
    for (int cx = x; cx < x + w - 1; ++cx) {
        double t1 = (cx - x) * freq;
        double t2 = (cx + 1 - x) * freq;
        int y1 = y + vineY + static_cast<int>((bw / 3) * std::sin(t1));
        int y2 = y + vineY + static_cast<int>((bw / 3) * std::sin(t2));
        drawThickLine(r, cx, y1, cx + 1, y2, lw);
    }
    // أزهار على الحافة العلوية عند قمم وقيعان الموجة
    for (int cx = x; cx < x + w; cx += bw) {
        double t = (cx - x) * freq;
        int fy = y + vineY + static_cast<int>((bw / 3) * std::sin(t));
        setColor(r, s);
        drawFilledCircle(r, cx, fy, flowerR);
        setColor(r, accent);
        drawCircle(r, cx, fy, flowerR);
    }

    // الحافة السفلية
    int baseY = y + h - bw;
    setColor(r, p);
    for (int cx = x; cx < x + w - 1; ++cx) {
        double t1 = (cx - x) * freq;
        double t2 = (cx + 1 - x) * freq;
        int y1 = baseY + vineY + static_cast<int>((bw / 3) * std::sin(t1));
        int y2 = baseY + vineY + static_cast<int>((bw / 3) * std::sin(t2));
        drawThickLine(r, cx, y1, cx + 1, y2, lw);
    }
    for (int cx = x; cx < x + w; cx += bw) {
        double t = (cx - x) * freq;
        int fy = baseY + vineY + static_cast<int>((bw / 3) * std::sin(t));
        setColor(r, s);
        drawFilledCircle(r, cx, fy, flowerR);
        setColor(r, accent);
        drawCircle(r, cx, fy, flowerR);
    }

    // الحافة اليسرى
    setColor(r, p);
    for (int cy = y + bw; cy < y + h - bw - 1; ++cy) {
        double t1 = (cy - y) * freq;
        double t2 = (cy + 1 - y) * freq;
        int x1 = x + vineY + static_cast<int>((bw / 3) * std::sin(t1));
        int x2 = x + vineY + static_cast<int>((bw / 3) * std::sin(t2));
        drawThickLine(r, x1, cy, x2, cy + 1, lw);
    }
    for (int cy = y + bw; cy < y + h - bw; cy += bw) {
        double t = (cy - y) * freq;
        int fx = x + vineY + static_cast<int>((bw / 3) * std::sin(t));
        setColor(r, s);
        drawFilledCircle(r, fx, cy, flowerR);
        setColor(r, accent);
        drawCircle(r, fx, cy, flowerR);
    }

    // الحافة اليمنى
    int baseX = x + w - bw;
    setColor(r, p);
    for (int cy = y + bw; cy < y + h - bw - 1; ++cy) {
        double t1 = (cy - y) * freq;
        double t2 = (cy + 1 - y) * freq;
        int x1 = baseX + vineY + static_cast<int>((bw / 3) * std::sin(t1));
        int x2 = baseX + vineY + static_cast<int>((bw / 3) * std::sin(t2));
        drawThickLine(r, x1, cy, x2, cy + 1, lw);
    }
    for (int cy = y + bw; cy < y + h - bw; cy += bw) {
        double t = (cy - y) * freq;
        int fx = baseX + vineY + static_cast<int>((bw / 3) * std::sin(t));
        setColor(r, s);
        drawFilledCircle(r, fx, cy, flowerR);
        setColor(r, accent);
        drawCircle(r, fx, cy, flowerR);
    }
}

// ═══════════════════════════════════════════════════════════════════
// دالة رسم الإطار الزخرفي الرئيسية — توزع العمل حسب النوع
// ═══════════════════════════════════════════════════════════════════

void drawBorder(void* renderer, BorderType type,
                int x, int y, int width, int height,
                int borderWidth, ColorScheme scheme, int lineWidth) {
    SDL_Renderer* r = asSDL(renderer);
    ArtColor primary, secondary, bg, accent;
    getSchemeColors(scheme, primary, secondary, bg, accent);

    switch (type) {
        case BorderType::GeometricBand:
            borderGeometricBand(r, x, y, width, height, borderWidth, primary, secondary, accent, lineWidth);
            break;
        case BorderType::Interlaced:
            borderInterlaced(r, x, y, width, height, borderWidth, primary, secondary, accent, lineWidth);
            break;
        case BorderType::Kufic:
            borderKufic(r, x, y, width, height, borderWidth, primary, secondary, accent, lineWidth);
            break;
        case BorderType::Floral:
            borderFloral(r, x, y, width, height, borderWidth, primary, secondary, accent, lineWidth);
            break;
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم زخرفة نباتية (أرابيسك)
// تُنشئ كرمات جيبية متموجة مع أوراق دائرية — التعقيد يتحكم بعدد الطبقات
// ═══════════════════════════════════════════════════════════════════

void drawArabesque(void* renderer,
                   int x, int y, int width, int height,
                   ColorScheme scheme, int complexity) {
    SDL_Renderer* r = asSDL(renderer);
    ArtColor primary, secondary, bg, accent;
    getSchemeColors(scheme, primary, secondary, bg, accent);

    // ملء الخلفية
    setColor(r, bg);
    SDL_Rect bgRect = {x, y, width, height};
    SDL_RenderFillRect(r, &bgRect);

    // ضبط مستوى التعقيد ضمن الحدود المسموحة (1-5)
    if (complexity < 1) complexity = 1;
    if (complexity > 5) complexity = 5;

    int centerY = y + height / 2;
    int leafR = std::max(3, width / (complexity * 20)); // نصف قطر الورقة

    // رسم طبقات الكرمة — كل طبقة بتردد وسعة مختلفين
    for (int layer = 0; layer < complexity; ++layer) {
        double freq = (layer + 1) * M_PI / (width * 0.4);   // تردد الموجة
        int amplitude = height / (3 + layer);                 // سعة الموجة تتناقص مع الطبقات
        int yOffset = (layer - complexity / 2) * (height / (complexity + 2)); // إزاحة رأسية

        // رسم الكرمة الجيبية — خط متموج
        setColor(r, (layer % 2 == 0) ? primary : accent);
        for (int px = x; px < x + width - 1; ++px) {
            double t1 = (px - x) * freq;
            double t2 = (px + 1 - x) * freq;
            int py1 = centerY + yOffset + static_cast<int>(amplitude * std::sin(t1));
            int py2 = centerY + yOffset + static_cast<int>(amplitude * std::sin(t2));
            drawThickLine(r, px, py1, px + 1, py2, 2);
        }

        // رسم الأوراق — دوائر صغيرة عند القمم والقيعان
        int leafSpacing = std::max(20, width / (complexity * 4));
        for (int px = x + leafSpacing / 2; px < x + width; px += leafSpacing) {
            double t = (px - x) * freq;
            int py = centerY + yOffset + static_cast<int>(amplitude * std::sin(t));
            // ورقة علوية
            setColor(r, secondary);
            drawFilledCircle(r, px, py - leafR, leafR);
            setColor(r, primary);
            drawCircle(r, px, py - leafR, leafR);
            // ورقة سفلية أصغر
            int smallR = leafR * 2 / 3;
            if (smallR >= 2) {
                setColor(r, accent);
                drawFilledCircle(r, px + leafR, py, smallR);
                setColor(r, primary);
                drawCircle(r, px + leafR, py, smallR);
            }
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// رسم خلفية إسلامية مزخرفة كاملة
// تملأ الخلفية بلون أساسي ثم تضع شبكة نجمية فوقها
// ═══════════════════════════════════════════════════════════════════

void drawIslamicBackground(void* renderer,
                           int width, int height,
                           ColorScheme scheme, int patternSize) {
    SDL_Renderer* r = asSDL(renderer);
    ArtColor primary, secondary, bg, accent;
    getSchemeColors(scheme, primary, secondary, bg, accent);

    // الخطوة الأولى: ملء الخلفية بالكامل
    setColor(r, bg);
    SDL_Rect fullBg = {0, 0, width, height};
    SDL_RenderFillRect(r, &fullBg);

    // الخطوة الثانية: رسم شبكة نجمية ثمانية كخلفية مزخرفة
    // النجمة الثمانية هي الأكثر شيوعاً في الزخارف الإسلامية
    drawStarGrid(renderer, StarPattern::EightPointed,
                 0, 0, width, height, patternSize, scheme, 1);
}

}} // إغلاق فضاء الأسماء — namespace sad::islamic_art
