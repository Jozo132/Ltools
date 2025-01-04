#pragma once

#include "tools.h"


#define MAX_NUM_OF_FONTS 32
#define MAX_FONT_NAME_LENGTH 16


struct ttf_font_t {
    const char* name;
    int length;
    const uint8_t* data;
};

const struct ttf_font_t ttf_fonts [] = {
    #include "../resources/precompiled_fonts.h"
};

const int ttf_fonts_count = sizeof(ttf_fonts) / sizeof(ttf_fonts[0]);

ttf_font_t* find_font(const char* name) {
    if (name == nullptr) return nullptr;
    for (int i = 0; i < ttf_fonts_count; i++) {
        auto& font = ttf_fonts[i];
        int input_len = 0;
        while (name[input_len] != 0) input_len++;
        int name_len = 0;
        while (font.name[name_len] != 0) name_len++;
        if (input_len != name_len) continue;
        bool match = true;
        for (int j = 0; j < input_len; j++) {
            if (name[j] != font.name[j]) {
                match = false;
                break;
            }
        }
        if (match) return (ttf_font_t*) &font;
    }
    return nullptr;
}


bool string_match(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return false;
        a++;
        b++;
    }
    return *a == *b;
}

void string_copy(char* dest, const char* src, int len) {
    while (*src && len > 0) {
        *dest = *src;
        dest++;
        src++;
        len--;
    }
    *dest = '\0';
}

// struct FontX_t {
//     struct MyFont {
//         Font font;
//         char name[MAX_FONT_NAME_LENGTH];
//         int size;
//     };

//     int count;
//     MyFont* fonts = nullptr;
//     Font* loadFont(const char* name, int size) {
//         //// Load font from file into ImageFont
//         // char temp[64];
//         // sprintf(temp, "resources/%s.ttf", name);
//         // Font font = LoadFontEx(temp, size, 0, 250);
        
//         // Load font from memory
//         ttf_font_t* font = find_font(name);
//         if (font == nullptr) {
//             printf("Font not found: %s\n", name);
//             // return nullptr;
//             if (ttf_fonts_count == 0) return nullptr;
//             font = (ttf_font_t*) &ttf_fonts[0];
//         }
//         Font f = LoadFontFromMemory(".ttf", (unsigned char*) font->data, font->length, size, 0, 250);
//         return new Font(f);
//     }
//     Font* getFont(const char* name, int size) {
//         if (fonts == nullptr) {
//             fonts = new MyFont[MAX_NUM_OF_FONTS];
//             count = 0;
//         }
//         for (int i = 0; i < count; i++) {
//             if (string_match(name, fonts[i].name) && fonts[i].size == size) {
//                 return &fonts[i].font;
//             }
//         }
//         if (count >= MAX_NUM_OF_FONTS) return nullptr;
//         Font* f = loadFont(name, size);
//         if (f == nullptr) return nullptr;
//         fonts[count].font = *f;
//         string_copy(fonts[count].name, name, MAX_FONT_NAME_LENGTH);
//         fonts[count].size = size;
//         count++;
//         return &fonts[count - 1].font;
//     }
// } fontx;