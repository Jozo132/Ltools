#pragma once

#include "tools.h"


uint8_t hexToNum(char hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    return 0;
}

// Color encoder that parses a char array like "0xFFF" for red, green and blue and returns a 32-bit color
uint32_t toColorARGB(const char* hex) {
    uint16_t red = 0;
    uint16_t green = 0;
    uint16_t blue = 0;
    uint16_t alpha = 0;
    int len = strlen(hex);
    if (!len) return 0;
    // Handle 'f', '#f', '0xf', 'fff', '#fff', '0xfff', 'ffffff', '#ffffff' and '0xffffff' formats
    bool hasHash = len > 0 && hex[0] == '#';
    bool hasHex = len > 1 && hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X');
    if (hasHash) {
        hex++;
        len--;
    }
    if (hasHex) {
        hex += 2;
        len -= 2;
    }

    for (int i = 0; i < len; i++) {
        char c = hex[i];
        if (c >= '0' && c <= '9') continue;
        if (c >= 'a' && c <= 'f') continue;
        if (c >= 'A' && c <= 'F') continue;
        return 0;
    }

    if (len == 3) {
        red = hexToNum(hex[0]) * 16 + hexToNum(hex[0]);
        green = hexToNum(hex[1]) * 16 + hexToNum(hex[1]);
        blue = hexToNum(hex[2]) * 16 + hexToNum(hex[2]);
        alpha = 255;
    } else if (len == 4) {
        red = hexToNum(hex[0]) * 16 + hexToNum(hex[0]);
        green = hexToNum(hex[1]) * 16 + hexToNum(hex[1]);
        blue = hexToNum(hex[2]) * 16 + hexToNum(hex[2]);
        alpha = hexToNum(hex[3]) * 16 + hexToNum(hex[3]);
    } else if (len == 6) {
        red = hexToNum(hex[0]) * 16 + hexToNum(hex[1]);
        green = hexToNum(hex[2]) * 16 + hexToNum(hex[3]);
        blue = hexToNum(hex[4]) * 16 + hexToNum(hex[5]);
        alpha = 255;
    } else if (len == 8) {
        red = hexToNum(hex[0]) * 16 + hexToNum(hex[1]);
        green = hexToNum(hex[2]) * 16 + hexToNum(hex[3]);
        blue = hexToNum(hex[4]) * 16 + hexToNum(hex[5]);
        alpha = hexToNum(hex[6]) * 16 + hexToNum(hex[7]);
    }
    uint32_t color = (alpha << 24) | (red << 16) | (green << 8) | blue;
    return color;
}

uint16_t toColor565(const char* hex) {
    uint32_t color = toColorARGB(hex);
    uint8_t red = (color >> 16) & 0xFF;
    uint8_t green = (color >> 8) & 0xFF;
    uint8_t blue = color & 0xFF;
    // Red max = 0b11111 = 31, Green max = 0b111111 = 63, Blue max = 0b11111 = 31
    red = 0b11111 * red / 255;
    green = 0b111111 * green / 255;
    blue = 0b11111 * blue / 255;
    uint16_t output = red << 11 | green << 5 | blue;
    return output;
}

Color colorFrom(const char* hex) {
    uint32_t color = toColorARGB(hex);
    uint8_t red = (color >> 16) & 0xFF;
    uint8_t green = (color >> 8) & 0xFF;
    uint8_t blue = color & 0xFF;
    uint8_t alpha = (color >> 24) & 0xFF;
    return (Color) { .r = red, .g = green, .b = blue, .a = alpha };
}