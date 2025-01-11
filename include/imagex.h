#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#if defined(_WIN32)
// For QueryPerformanceCounter/QueryPerformanceFrequency
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "tools.h"
#include "colorx.h"
#include "lodepng.h"
#include "fpng.h"

enum PNG_ENCODER {
    PE_UNKNOWN,
    PE_LODEPNG,
    PE_FPNG,
};

enum FPNG_FLAGS {
    // Enables computing custom Huffman tables for each file, instead of using the custom global tables. 
    // Results in roughly 6% smaller files on average, but compression is around 40% slower.
    FPNG_ENCODE_SLOWER = 1,
    // Only use raw Deflate blocks (no compression at all). Intended for testing.
    FPNG_FORCE_UNCOMPRESSED = 2,
};

class Image {
public:
    int width;
    int height;
    Color background = WHITE;
    std::vector<uint8_t> data;
    std::vector<unsigned char> output;

    std::vector<unsigned char>* toPNG(PNG_ENCODER encoder = PE_LODEPNG) {
        output.clear();
        if (encoder == PE_LODEPNG) {
            int error = lodepng::encode(output, data, width, height);
            if (error) {
                notifyf("Error encoding PNG: %s\n", lodepng_error_text(error));
                return nullptr;
            }
            return &output;
        }
        if (encoder == PE_FPNG) {
            // Initialize FPNG
            fpng::fpng_init();
            const void* ref = this->data.data();
            int channels = 4; // RGBA
            bool success = fpng::fpng_encode_image_to_memory(ref, width, height, channels, output, FPNG_ENCODE_SLOWER);
            if (!success) {
                notifyf("Error encoding PNG\n");
                return nullptr;
            }
            return &output;
        }
        notifyf("Unknown PNG encoder %d\n", encoder);
        return nullptr;
    }

    Image() {
        width = 1;
        height = 1;
        data.resize(4);
        clear(WHITE);
    }

    Image(int width, int height, Color color) {
        this->width = width;
        this->height = height;
        this->background = color;
        data.resize(width * height * 4);
        clear(color);
    }

    void clear(Color color) {
        for (int i = 0; i < width * height; i++) {
            int ix = i * 4;
            data[ix + 0] = color.r;
            data[ix + 1] = color.g;
            data[ix + 2] = color.b;
            data[ix + 3] = color.a;
        }
    }

    void resize(int width, int height, Color color) {
        if (width <= 0 || height <= 0) return;
        if (width != this->width || height != this->height) {
            this->width = width;
            this->height = height;
            data.resize(width * height * 4);
        }
        this->background = color;
        clear(color);
    }

    void drawPixel(int x, int y, Color color, bool inverted = false) {
        if (x < 0 || y < 0 || x >= width || y >= height) return;
        if (inverted) {
            int hue = color.getHue();
            int inversion = 255 - hue;
            invertPixel(x, y, inversion);
        } else {
            size_t idx = 4 * (y * width + x);
            data[idx] = color.r;
            data[idx + 1] = color.g;
            data[idx + 2] = color.b;
            data[idx + 3] = color.a;
        }
    }

    void invertPixel(int x, int y, uint8_t inversion) {
        if (x < 0 || y < 0 || x >= width || y >= height) return;
        size_t idx = 4 * (y * width + x);
        data[idx] = invert_color(data[idx], inversion);
        data[idx + 1] = invert_color(data[idx + 1], inversion);
        data[idx + 2] = invert_color(data[idx + 2], inversion);
    }

    Color getPixel(int x, int y) {
        if (x < 0 || y < 0 || x >= width || y >= height) return BLANK;
        size_t idx = 4 * (y * width + x);
        return Color{ data[idx], data[idx + 1], data[idx + 2], data[idx + 3] };
    }

    uint8_t getHue(int x, int y) {
        if (x < 0 || y < 0 || x >= width || y >= height) return 127;
        size_t idx = 4 * (y * width + x);
        return (data[idx] + data[idx + 1] + data[idx + 2]) / 3;
    }

    void fillPolygon(std::vector<Vector2> points, Color color, bool inverted = false) {
        auto isInsidePolygon = [&points](int x, int y) {
            bool inside = false;
            for (size_t i = 0, j = points.size() - 1; i < points.size(); j = i++) {
                if (((points[i].y > y) != (points[j].y > y)) &&
                    (x < (points[j].x - points[i].x) * (y - points[i].y) / (points[j].y - points[i].y) + points[i].x)) {
                    inside = !inside;
                }
            }
            return inside;
            };

        int hue = inverted ? color.getHue() : 255;
        int inversion = 255 - hue;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (isInsidePolygon(x, y)) {
                    if (inverted) {
                        invertPixel(x, y, inversion);
                    } else {
                        size_t idx = 4 * (y * width + x);
                        data[idx] = color.r;
                        data[idx + 1] = color.g;
                        data[idx + 2] = color.b;
                        data[idx + 3] = color.a;
                    }
                }
            }
        }
    }

    void drawLine(int x0, int y0, int x1, int y1, int stroke_width, Color color, bool inverted = false) {
        int dx = abs(x1 - x0);
        int sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0);
        int sy = y0 < y1 ? 1 : -1;
        int err = dx + dy;
        int hue = inverted ? color.getHue() : 255;
        int inversion = 255 - hue;
        while (true) {
            for (int i = -stroke_width / 2; i < stroke_width / 2; i++) {
                int ix = x0 + i;
                int iy = y0 + i;
                if (ix >= 0 && iy >= 0 && ix < width && iy < height) {
                    if (inverted) {
                        invertPixel(ix, iy, inversion);
                    } else {
                        size_t idx = 4 * (iy * width + ix);
                        data[idx] = color.r;
                        data[idx + 1] = color.g;
                        data[idx + 2] = color.b;
                        data[idx + 3] = color.a;
                    }
                }
            }
            if (x0 == x1 && y0 == y1) break;
            int e2 = 2 * err;
            if (e2 >= dy) {
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx) {
                err += dx;
                y0 += sy;
            }
        }
    }


    void drawRect(int x, int y, int w, int h, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        int fill_hue = inverted ? fill.getHue() : 255;
        int fill_inversion = 255 - fill_hue;
        int stroke_hue = inverted ? stroke.getHue() : 255;
        int stroke_inversion = 255 - stroke_hue;
        if (!fill.isBlank()) {
            for (int iy = y; iy < y + h; iy++) {
                for (int ix = x; ix < x + w; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                    if (inverted) {
                        invertPixel(ix, iy, fill_inversion);
                    } else {
                        size_t idx = 4 * (iy * width + ix);
                        data[idx] = fill.r;
                        data[idx + 1] = fill.g;
                        data[idx + 2] = fill.b;
                        data[idx + 3] = fill.a;
                    }
                }
            }
        }
        if (!stroke.isBlank()) {
            for (int iy = y; iy < y + h; iy++) {
                for (int ix = x; ix < x + w; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                    if (ix < x + stroke_width || ix >= x + w - stroke_width || iy < y + stroke_width || iy >= y + h - stroke_width) {
                        if (inverted) {
                            invertPixel(ix, iy, stroke_inversion);
                        } else {
                            size_t idx = 4 * (iy * width + ix);
                            data[idx] = stroke.r;
                            data[idx + 1] = stroke.g;
                            data[idx + 2] = stroke.b;
                            data[idx + 3] = stroke.a;
                        }
                    }
                }
            }
        }
    }

    float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    void fillTriangle(Vector2 p1, Vector2 p2, Vector2 p3, Color color, bool inverted = false) {
        // Sort the vertices by y-coordinate ascending to simplify
        if (p1.y > p2.y) std::swap(p1, p2);
        if (p1.y > p3.y) std::swap(p1, p3);
        if (p2.y > p3.y) std::swap(p2, p3);
        int hue = inverted ? color.getHue() : 255;
        int inversion = 255 - hue;
        // Manually compute the scanline
        for (int y = p1.y; y <= p3.y; y++) {
            float t = (y - p1.y) / (p3.y - p1.y);
            float xl = lerp(p1.x, p3.x, t);
            float xr = lerp(p1.x, p2.x, (y - p1.y) / (p2.y - p1.y));
            if (xl > xr) std::swap(xl, xr);
            for (int x = xl; x <= xr; x++) {
                if (x < 0 || y < 0 || x >= width || y >= height) continue;
                if (inverted) {
                    invertPixel(x, y, inversion);
                } else {
                    size_t idx = 4 * (y * width + x);
                    data[idx] = color.r;
                    data[idx + 1] = color.g;
                    data[idx + 2] = color.b;
                    data[idx + 3] = color.a;
                }
            }
        }
    }

    void drawTriangle(Vector2 p1, Vector2 p2, Vector2 p3, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        if (!fill.isBlank()) {
            fillTriangle(p1, p2, p3, fill, inverted);
        }
        if (!stroke.isBlank()) {
            drawLine(p1.x, p1.y, p2.x, p2.y, stroke_width, stroke, inverted);
            drawLine(p2.x, p2.y, p3.x, p3.y, stroke_width, stroke, inverted);
            drawLine(p3.x, p3.y, p1.x, p1.y, stroke_width, stroke, inverted);
        }
    }

    void drawQuad(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        if (!fill.isBlank()) {
            fillTriangle(p1, p2, p3, fill, inverted);
            fillTriangle(p1, p3, p4, fill, inverted);
        }
        if (!stroke.isBlank()) {
            drawLine(p1.x, p1.y, p2.x, p2.y, stroke_width, stroke, inverted);
            drawLine(p2.x, p2.y, p3.x, p3.y, stroke_width, stroke, inverted);
            drawLine(p3.x, p3.y, p4.x, p4.y, stroke_width, stroke, inverted);
            drawLine(p4.x, p4.y, p1.x, p1.y, stroke_width, stroke, inverted);
        }
    }

    void drawCircle(int x, int y, int radius, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        int fill_hue = inverted ? fill.getHue() : 255;
        int fill_inversion = 255 - fill_hue;
        int stroke_hue = inverted ? stroke.getHue() : 255;
        int stroke_inversion = 255 - stroke_hue;
        if (!fill.isBlank()) {
            for (int iy = y - radius; iy < y + radius; iy++) {
                for (int ix = x - radius; ix < x + radius; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                    if ((ix - x) * (ix - x) + (iy - y) * (iy - y) < radius * radius) {
                        if (inverted) {
                            invertPixel(ix, iy, fill_inversion);
                        } else {
                            size_t idx = 4 * (iy * width + ix);
                            data[idx] = fill.r;
                            data[idx + 1] = fill.g;
                            data[idx + 2] = fill.b;
                            data[idx + 3] = fill.a;
                        }
                    }
                }
            }
        }
        if (!stroke.isBlank()) {
            for (int iy = y - radius; iy < y + radius; iy++) {
                for (int ix = x - radius; ix < x + radius; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                    if ((ix - x) * (ix - x) + (iy - y) * (iy - y) < radius * radius + stroke_width * stroke_width / 4) {
                        if (inverted) {
                            invertPixel(ix, iy, stroke_inversion);
                        } else {
                            size_t idx = 4 * (iy * width + ix);
                            data[idx] = stroke.r;
                            data[idx + 1] = stroke.g;
                            data[idx + 2] = stroke.b;
                            data[idx + 3] = stroke.a;
                        }
                    }
                }
            }
        }
    }

    void drawArc(Vector2 center, int radius, int startAngle, int endAngle, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        int fill_hue = inverted ? fill.getHue() : 255;
        int fill_inversion = 255 - fill_hue;
        int stroke_hue = inverted ? stroke.getHue() : 255;
        int stroke_inversion = 255 - stroke_hue;
        if (!fill.isBlank()) {
            for (int iy = center.y - radius; iy < center.y + radius; iy++) {
                for (int ix = center.x - radius; ix < center.x + radius; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                    float angle = atan2f(iy - center.y, ix - center.x) * RAD2DEG;
                    if (angle < startAngle || angle > endAngle) continue;
                    if ((ix - center.x) * (ix - center.x) + (iy - center.y) * (iy - center.y) < radius * radius) {
                        if (inverted) {
                            invertPixel(ix, iy, fill_inversion);
                        } else {
                            size_t idx = 4 * (iy * width + ix);
                            data[idx] = fill.r;
                            data[idx + 1] = fill.g;
                            data[idx + 2] = fill.b;
                            data[idx + 3] = fill.a;
                        }
                    }
                }
            }
        }
        if (!stroke.isBlank()) {
            for (int iy = center.y - radius; iy < center.y + radius; iy++) {
                for (int ix = center.x - radius; ix < center.x + radius; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                    float angle = atan2f(iy - center.y, ix - center.x) * RAD2DEG;
                    if (angle < startAngle || angle > endAngle) continue;
                    if ((ix - center.x) * (ix - center.x) + (iy - center.y) * (iy - center.y) < radius * radius + stroke_width * stroke_width / 4) {
                        if (inverted) {
                            invertPixel(ix, iy, stroke_inversion);
                        } else {
                            size_t idx = 4 * (iy * width + ix);
                            data[idx] = stroke.r;
                            data[idx + 1] = stroke.g;
                            data[idx + 2] = stroke.b;
                            data[idx + 3] = stroke.a;
                        }
                    }
                }
            }
        }
    }

    void drawRoundedRectangle(int x, int y, int w, int h, float roundness, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        if (roundness <= 0) {
            return drawRect(x, y, w, h, stroke_width, stroke, fill, inverted);
        }
        int fill_hue = inverted ? fill.getHue() : 255;
        int fill_inversion = 255 - fill_hue;
        int stroke_hue = inverted ? stroke.getHue() : 255;
        int stroke_inversion = 255 - stroke_hue;
        if (roundness > 1) roundness = 1;
        int radius = roundness * std::min(w, h) / 2;
        if (!fill.isBlank()) {
            for (int iy = y; iy < y + h; iy++) {
                for (int ix = x; ix < x + w; ix++) {
                    if (ix < x + radius || ix >= x + w - radius || iy < y + radius || iy >= y + h - radius) {
                        if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                        if (inverted) {
                            invertPixel(ix, iy, fill_inversion);
                        } else {
                            size_t idx = 4 * (iy * width + ix);
                            data[idx] = fill.r;
                            data[idx + 1] = fill.g;
                            data[idx + 2] = fill.b;
                            data[idx + 3] = fill.a;
                        }
                    }
                }
            }
        }
        if (!stroke.isBlank()) {
            for (int ix = x + radius; ix < x + w - radius; ix++) {
                if (ix < 0 || y < 0 || ix >= width || y >= height) continue;
                if (inverted) {
                    invertPixel(ix, y, stroke_inversion);
                } else {
                    size_t idx = 4 * (y * width + ix);
                    data[idx] = stroke.r;
                    data[idx + 1] = stroke.g;
                    data[idx + 2] = stroke.b;
                    data[idx + 3] = stroke.a;
                }
            }
        }

        drawArc(Vector2{ (float) (x + radius), (float) (y + radius) }, radius, 180, 270, stroke_width, stroke, fill, inverted);
        drawArc(Vector2{ (float) (x + w - radius), (float) (y + radius) }, radius, 270, 360, stroke_width, stroke, fill, inverted);
        drawArc(Vector2{ (float) (x + w - radius), (float) (y + h - radius) }, radius, 0, 90, stroke_width, stroke, fill, inverted);
        drawArc(Vector2{ (float) (x + radius), (float) (y + h - radius) }, radius, 90, 180, stroke_width, stroke, fill, inverted);
    }

};

