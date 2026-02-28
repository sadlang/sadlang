// ═══════════════════════════════════════════════════════════════════════════════
// sad_framebuffer_binding.cpp — تنفيذ ربط الإطار المخزني
// OS Framebuffer Binding Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_framebuffer_binding.h"
#include <map>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <cmath>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// هياكل داخلية / Internal Structures
// ═══════════════════════════════════════════════════════════════

struct PixelRGB {
    uint8_t r = 0, g = 0, b = 0;
};

struct FramebufferData {
    int id = 0;
    int width = 0;
    int height = 0;
    std::vector<PixelRGB> frontBuffer;
    std::vector<PixelRGB> backBuffer;
    bool doubleBuffered = false;
    
    PixelRGB* writeBuffer() {
        return doubleBuffered ? backBuffer.data() : frontBuffer.data();
    }
    
    const PixelRGB* readBuffer() const {
        return frontBuffer.data();
    }
    
    bool inBounds(int x, int y) const {
        return x >= 0 && x < width && y >= 0 && y < height;
    }
};

static std::map<int, FramebufferData> g_framebuffers;
static int g_nextFbId = 1;

// ═══════════════════════════════════════════════════════════════
// تنفيذ / Implementation
// ═══════════════════════════════════════════════════════════════

int framebuffer_create_impl(int width, int height) {
    if (width <= 0 || height <= 0 || width > 8192 || height > 8192) return -1;
    int id = g_nextFbId++;
    auto& fb = g_framebuffers[id];
    fb.id = id;
    fb.width = width;
    fb.height = height;
    fb.frontBuffer.resize(width * height, {0, 0, 0});
    return id;
}

void framebuffer_destroy_impl(int fbId) {
    g_framebuffers.erase(fbId);
}

int framebuffer_width_impl(int fbId) {
    auto it = g_framebuffers.find(fbId);
    return it != g_framebuffers.end() ? it->second.width : 0;
}

int framebuffer_height_impl(int fbId) {
    auto it = g_framebuffers.find(fbId);
    return it != g_framebuffers.end() ? it->second.height : 0;
}

void framebuffer_clear_impl(int fbId, int r, int g, int b) {
    auto it = g_framebuffers.find(fbId);
    if (it == g_framebuffers.end()) return;
    PixelRGB c = {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)};
    auto* buf = it->second.writeBuffer();
    for (int i = 0; i < it->second.width * it->second.height; i++) {
        buf[i] = c;
    }
}

void framebuffer_set_pixel_impl(int fbId, int x, int y, int r, int g, int b) {
    auto it = g_framebuffers.find(fbId);
    if (it == g_framebuffers.end() || !it->second.inBounds(x, y)) return;
    auto* buf = it->second.writeBuffer();
    buf[y * it->second.width + x] = {static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b)};
}

int framebuffer_get_pixel_impl(int fbId, int x, int y) {
    auto it = g_framebuffers.find(fbId);
    if (it == g_framebuffers.end() || !it->second.inBounds(x, y)) return 0;
    const auto& p = it->second.readBuffer()[y * it->second.width + x];
    return (p.r << 16) | (p.g << 8) | p.b;
}

void framebuffer_line_impl(int fbId, int x0, int y0, int x1, int y1, int r, int g, int b) {
    auto it = g_framebuffers.find(fbId);
    if (it == g_framebuffers.end()) return;
    // خوارزمية Bresenham / Bresenham's algorithm
    int dx = std::abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -std::abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        framebuffer_set_pixel_impl(fbId, x0, y0, r, g, b);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void framebuffer_rect_impl(int fbId, int x, int y, int w, int h, int r, int g, int b, bool filled) {
    if (filled) {
        for (int iy = y; iy < y + h; iy++) {
            for (int ix = x; ix < x + w; ix++) {
                framebuffer_set_pixel_impl(fbId, ix, iy, r, g, b);
            }
        }
    } else {
        framebuffer_line_impl(fbId, x, y, x + w - 1, y, r, g, b);
        framebuffer_line_impl(fbId, x, y + h - 1, x + w - 1, y + h - 1, r, g, b);
        framebuffer_line_impl(fbId, x, y, x, y + h - 1, r, g, b);
        framebuffer_line_impl(fbId, x + w - 1, y, x + w - 1, y + h - 1, r, g, b);
    }
}

void framebuffer_circle_impl(int fbId, int cx, int cy, int radius, int r, int g, int b, bool filled) {
    if (filled) {
        for (int iy = -radius; iy <= radius; iy++) {
            for (int ix = -radius; ix <= radius; ix++) {
                if (ix * ix + iy * iy <= radius * radius) {
                    framebuffer_set_pixel_impl(fbId, cx + ix, cy + iy, r, g, b);
                }
            }
        }
    } else {
        // Midpoint circle algorithm
        int px = 0, py = radius;
        int d = 1 - radius;
        auto plot8 = [&](int px, int py) {
            framebuffer_set_pixel_impl(fbId, cx + px, cy + py, r, g, b);
            framebuffer_set_pixel_impl(fbId, cx - px, cy + py, r, g, b);
            framebuffer_set_pixel_impl(fbId, cx + px, cy - py, r, g, b);
            framebuffer_set_pixel_impl(fbId, cx - px, cy - py, r, g, b);
            framebuffer_set_pixel_impl(fbId, cx + py, cy + px, r, g, b);
            framebuffer_set_pixel_impl(fbId, cx - py, cy + px, r, g, b);
            framebuffer_set_pixel_impl(fbId, cx + py, cy - px, r, g, b);
            framebuffer_set_pixel_impl(fbId, cx - py, cy - px, r, g, b);
        };
        while (px <= py) {
            plot8(px, py);
            px++;
            if (d < 0) {
                d += 2 * px + 1;
            } else {
                py--;
                d += 2 * (px - py) + 1;
            }
        }
    }
}

void framebuffer_swap_impl(int fbId) {
    auto it = g_framebuffers.find(fbId);
    if (it == g_framebuffers.end() || !it->second.doubleBuffered) return;
    std::swap(it->second.frontBuffer, it->second.backBuffer);
}

void framebuffer_double_buffer_impl(int fbId, bool enable) {
    auto it = g_framebuffers.find(fbId);
    if (it == g_framebuffers.end()) return;
    it->second.doubleBuffered = enable;
    if (enable && it->second.backBuffer.empty()) {
        it->second.backBuffer.resize(it->second.width * it->second.height, {0, 0, 0});
    }
}

int framebuffer_pixel_count_impl(int fbId) {
    auto it = g_framebuffers.find(fbId);
    return it != g_framebuffers.end() ? it->second.width * it->second.height : 0;
}

void framebuffer_fill_region_impl(int fbId, int x, int y, int w, int h, int r, int g, int b) {
    framebuffer_rect_impl(fbId, x, y, w, h, r, g, b, true);
}

void framebuffer_blit_impl(int srcId, int dstId, int sx, int sy, int sw, int sh, int dx, int dy) {
    auto sit = g_framebuffers.find(srcId);
    auto dit = g_framebuffers.find(dstId);
    if (sit == g_framebuffers.end() || dit == g_framebuffers.end()) return;
    for (int iy = 0; iy < sh; iy++) {
        for (int ix = 0; ix < sw; ix++) {
            int srcX = sx + ix, srcY = sy + iy;
            int dstX = dx + ix, dstY = dy + iy;
            if (sit->second.inBounds(srcX, srcY) && dit->second.inBounds(dstX, dstY)) {
                const auto& p = sit->second.readBuffer()[srcY * sit->second.width + srcX];
                auto* buf = dit->second.writeBuffer();
                buf[dstY * dit->second.width + dstX] = p;
            }
        }
    }
}

} // namespace graphics
} // namespace stdlib
} // namespace sad
