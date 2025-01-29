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
#include "fontx.h"

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

    void drawText(int x, int y, int font_size, const char* text, const char* font, Color color, bool inverted = false) {
        if (font_size <= 0) return;
        if (x < 0 || y < 0 || x >= width || y >= height) return;

        int offset = font_size * 2 / 3;
        int x_pos = x;
        int length = strlen(text);

        int error = FontLib.setFont(font, font_size);
        if (error) {
            notifyf("Failed to set font %s at size %d\n", font, font_size);
            return;
        }

        for (int i = 0; i < length; i++) {
            char c = text[i];
            FT_GlyphSlot* glyph = FontLib.getChar(c, font, font_size);
            if (glyph) {
                FT_GlyphSlot& g = glyph[0];
                FT_Bitmap& bitmap = g->bitmap;
                int iw = bitmap.width;
                int ih = bitmap.rows;
                int offsetX = x_pos + g->bitmap_left;
                int offsetY = offset - g->bitmap_top;
                for (int iy = 0; iy < ih; iy++) {
                    int y_px = y + iy + offsetY;
                    if (y_px < 0 || y_px >= height) continue;
                    for (int ix = 0; ix < iw; ix++) {
                        int x_px = offsetX + ix;
                        if (x_px < 0 || x_px >= width) continue;
                        uint8_t greyscale = bitmap.buffer[((int) iy) * iw + ((int) ix)]; // Single 8 bit value
                        // if (greyscale > 0) {
                            // greyscale = inverted ? greyscale : (255 - greyscale);
                            // Color color = { greyscale , greyscale, greyscale,  0xFF };
                            // image->drawPixel(x_px, y_px, color, inverted);
                        // }
                        if (greyscale > 0) {
                            drawPixel(x_px, y_px, color, inverted);
                        }
                    }
                }
                x_pos += g->advance.x >> 6;
            } else {
                notifyf("Glyph '%c' not found\n", c);
            }
        }
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

    void drawDiagonalZPL(int x, int y, int w, int h, char direction, int stroke_width, Color stroke, bool inverted = false) {
        // The diagonal line is either 'L' (left or '\' line) or 'R' (right or '/' line)
        // The stroke is always drawn on the right side of the line
        int hue = inverted ? stroke.getHue() : 255;
        int inversion = 255 - hue;
        double slope = (double) h / w;
        if (direction == 'L') { // Left diagonal line ('\') drawn from top row to bottom row
            for (int iy = y; iy < y + h; iy++) {
                // get the x position of the diagonal for the current row
                int ix = x + (int) ((double)(iy - y) / slope);
                for (int i = 0; i < stroke_width; i++, ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue; // Skip out of image bounds
                    if (inverted) {
                        invertPixel(ix, iy, inversion);
                    } else {
                        size_t idx = 4 * (iy * width + ix);
                        data[idx] = stroke.r;
                        data[idx + 1] = stroke.g;
                        data[idx + 2] = stroke.b;
                        data[idx + 3] = stroke.a;
                    }
                }
            }
        } else if (direction == 'R') { // Right diagonal line ('/') drawn from top row to bottom row
            for (int iy = y; iy < y + h; iy++) {
                // get the x position of the diagonal for the current row
                int ix = x + w - (int) ((double)(iy - y) / slope);
                for (int i = 0; i < stroke_width; i++, ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue; // Skip out of image bounds
                    if (inverted) {
                        invertPixel(ix, iy, inversion);
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

    void drawRect(int x, int y, int w, int h, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        int fill_hue = inverted ? fill.getHue() : 255;
        int fill_inversion = 255 - fill_hue;
        int stroke_hue = inverted ? stroke.getHue() : 255;
        int stroke_inversion = 255 - stroke_hue;
        int x1 = x;
        int x2 = x + w;
        int y1 = y;
        int y2 = y + h;
        if (!fill.isBlank()) {
            for (int iy = y1; iy < y2; iy++) {
                for (int ix = x1; ix < x2; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue; // Skip out of image bounds
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
            int pad_left = x1 + stroke_width;
            int pad_right = x2 - stroke_width;
            int pad_top = y1 + stroke_width;
            int pad_bottom = y2 - stroke_width;
            for (int iy = y1; iy < y2; iy++) {
                for (int ix = x1; ix < x2; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue; // Skip out of image bounds

                    if (ix >= pad_left && ix < pad_right && iy >= pad_top && iy < pad_bottom) continue; // Skip inside of the stroke

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
        int r2_outer = radius * radius;
        int r2_inner = (radius - stroke_width) * (radius - stroke_width);
        if (!fill.isBlank()) {
            for (int iy = y - radius; iy < y + radius; iy++) {
                for (int ix = x - radius; ix < x + radius; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                    int r2 = (ix - x) * (ix - x) + (iy - y) * (iy - y) - 1;
                    if (r2_outer >= r2) {
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
                    int r2 = (ix - x) * (ix - x) + (iy - y) * (iy - y) - 1;
                    if (r2_outer >= r2 && r2 >= r2_inner) {
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

    void drawEllipse(int x, int y, int w, int h, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        int fill_hue = inverted ? fill.getHue() : 255;
        int fill_inversion = 255 - fill_hue;
        int stroke_hue = inverted ? stroke.getHue() : 255;
        int stroke_inversion = 255 - stroke_hue;
        int rw = w / 2;
        int rh = h / 2;
        int inner_rw = rw - stroke_width > 0 ? rw - stroke_width : 0;
        int inner_rh = rh - stroke_width > 0 ? rh - stroke_width : 0;
        int rw2 = rw * rw;
        int rh2 = rh * rh;
        int inner_rw2 = inner_rw * inner_rw;
        int inner_rh2 = inner_rh * inner_rh;
        bool fully_filled = stroke_width >= std::min(w, h);
        for (int iy = y - rh; iy < y + rh; iy++) {
            for (int ix = x - rw; ix < x + rw; ix++) {
                if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                float outer_eq = ((ix - x) * (ix - x)) / (float) rw2 + ((iy - y) * (iy - y)) / (float) rh2;
                float inner_eq = ((ix - x) * (ix - x)) / (float) inner_rw2 + ((iy - y) * (iy - y)) / (float) inner_rh2;
                if (outer_eq <= 1) {
                    if (inner_eq > 1 || fully_filled) {
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
    }

    void drawArc(Vector2 center, int radius, int startAngle, int endAngle, int stroke_width, Color stroke, Color fill, bool inverted = false) {
        int fill_hue = inverted ? fill.getHue() : 255;
        int fill_inversion = 255 - fill_hue;
        int stroke_hue = inverted ? stroke.getHue() : 255;
        int stroke_inversion = 255 - stroke_hue;
        int r2_outer = radius * radius;
        int r2_inner = (radius - stroke_width) * (radius - stroke_width);
        if (!fill.isBlank()) {
            for (int iy = center.y - radius; iy < center.y + radius; iy++) {
                for (int ix = center.x - radius; ix < center.x + radius; ix++) {
                    if (ix < 0 || iy < 0 || ix >= width || iy >= height) continue;
                    float angle = atan2f(iy - center.y, ix - center.x) * RAD2DEG;
                    if (angle < startAngle || angle > endAngle) continue;
                    int r2 = (ix - center.x) * (ix - center.x) + (iy - center.y) * (iy - center.y) - 1;
                    if (r2_outer >= r2) {
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
                    int r2 = (ix - center.x) * (ix - center.x) + (iy - center.y) * (iy - center.y) - 1;
                    if (r2_outer >= r2 && r2 >= r2_inner) {
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
        if (roundness <= 0) return drawRect(x, y, w, h, stroke_width, stroke, fill, inverted);
        int fill_hue = inverted ? fill.getHue() : 255;
        int fill_inversion = 255 - fill_hue;
        int stroke_hue = inverted ? stroke.getHue() : 255;
        int stroke_inversion = 255 - stroke_hue;
        if (roundness > 1) roundness = 1;
        int radius = roundness * std::min(w, h) / 2;
        int rx1 = x + radius; // Center of the top-left rounded corner (x)
        int ry1 = y + radius; // Center of the top-left rounded corner (y)
        int rx2 = x + w - radius; // Center of the top-right rounded corner (x)
        int ry2 = ry1; // Center of the top-right rounded corner (y)
        int rx3 = rx2; // Center of the bottom-right rounded corner (x)
        int ry3 = y + h - radius; // Center of the bottom-right rounded corner (y)
        int rx4 = rx1; // Center of the bottom-left rounded corner (x)
        int ry4 = ry3; // Center of the bottom-left rounded corner (y)
        float r = radius;
        // Draw the top left quarter of the rounded rectangle and mirror the pixels to the other three quarters
        int x_odd = w % 2;
        int y_odd = h % 2;

        int xL = x; // Left
        int xM = x + w / 2 + x_odd; // Middle
        int xR = x + w - 1; // Right

        int yT = y; // Top
        int yM = y + h / 2 + y_odd; // Middle
        int yB = y + h - 1; // Bottom

        int x_pos = 0;
        int y_pos = 0;

        bool doInfill = !fill.isBlank();
        bool doStroke = !stroke.isBlank();

        if (doInfill) {
            for (int iy = yT, y_pos = 0; iy < yM; iy++, y_pos++) {
                bool visible = 0;
                bool y_last = iy == yM - 1;
                for (int ix = xL, x_pos = 0; ix < xM; ix++, x_pos++) {
                    bool x_last = ix == xM - 1;
                    bool top_left = (ix < rx1 && iy < ry1);
                    bool in_rect = !top_left;

                    if (top_left) {
                        // Get the distance from the center of the rounded corner
                        float dx = ix - rx1;
                        float dy = iy - ry1;
                        float dx2 = dx * dx;
                        float dy2 = dy * dy;
                        float dr = sqrtf(dx2 + dy2);
                        float delta = dr - r;
                        visible = delta <= 0;
                    }
                    if (in_rect) visible = true;

                    // Test
                    // visible = top_left || top_right || bottom_right || bottom_left;

                    if (!visible) continue;

                    int ix_mirror = xR - x_pos;
                    int iy_mirror = yB - y_pos;
                    for (int i = 0; i < 4; i++) {
                        int x = ix;
                        int y = iy;

                        if (i == 1) { // Top right
                            if (x_last && x_odd) continue;
                            x = ix_mirror;
                        }
                        if (i == 2) { // Bottom right
                            if (x_last && x_odd) continue;
                            if (y_last && y_odd) continue;
                            x = ix_mirror;
                            y = iy_mirror;
                        }
                        if (i == 3) { // Bottom left
                            if (y_last && y_odd) continue;
                            y = iy_mirror;
                        }

                        if (inverted) {
                            invertPixel(x, y, fill_inversion);
                        } else {
                            size_t idx = 4 * (y * width + x);
                            data[idx] = fill.r;
                            data[idx + 1] = fill.g;
                            data[idx + 2] = fill.b;
                            data[idx + 3] = fill.a;
                        }
                    }
                }
            }
            return;
        }
        if (doStroke) {
            for (int iy = yT, y_pos = 0; iy < yM; iy++, y_pos++) {
                bool visible = 0;
                bool y_last = iy == yM - 1;
                for (int ix = xL, x_pos = 0; ix < xM; ix++, x_pos++) {
                    bool x_last = ix == xM - 1;
                    bool top_left = (ix < rx1 && iy < ry1);
                    bool in_rect = !top_left;

                    if (top_left) {
                        // Get the distance from the center of the rounded corner
                        float dx = ix - rx1;
                        float dy = iy - ry1;
                        float dx2 = dx * dx;
                        float dy2 = dy * dy;
                        float dr = sqrtf(dx2 + dy2);
                        float delta = dr - r;
                        visible = delta <= 0 && delta >= -stroke_width;
                    }
                    if (in_rect) {
                        // Draw edge lines with a stroke width
                        bool left = ix < (xL + stroke_width);
                        bool top = iy < (yT + stroke_width);
                        visible = left || top;
                    }

                    if (!visible) continue;

                    int ix_mirror = xR - x_pos;
                    int iy_mirror = yB - y_pos;
                    for (int i = 0; i < 4; i++) {
                        int x = ix;
                        int y = iy;

                        if (i == 1) { // Top right
                            if (x_last && x_odd) continue;
                            x = ix_mirror;
                        }
                        if (i == 2) { // Bottom right
                            if (x_last && x_odd) continue;
                            if (y_last && y_odd) continue;
                            x = ix_mirror;
                            y = iy_mirror;
                        }
                        if (i == 3) { // Bottom left
                            if (y_last && y_odd) continue;
                            y = iy_mirror;
                        }

                        if (inverted) {
                            invertPixel(x, y, stroke_inversion);
                        } else {
                            size_t idx = 4 * (y * width + x);
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

};

