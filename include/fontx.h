#include "tools.h"
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H


struct ttf_font_t {
    const char* name;
    int length;
    const uint8_t* data;
};

const struct ttf_font_t ttf_fonts [] = {
    #include "../resources/precompiled_fonts.h"
};

const int ttf_fonts_count = sizeof(ttf_fonts) / sizeof(ttf_fonts[0]);


class FontLib_t {
private:
    struct Font {
        std::string name;
        std::vector<unsigned char> data;
        FT_Face face;
    };

    FT_Library ftLibrary;
    std::map<std::string, Font> fontTable;
    static constexpr size_t maxFonts = 100;

    Font* selectedFont = nullptr;
    int fontSize = 12;
    bool monochrome = false; // Doesn't work correctly

public:
    FontLib_t() {
        if (FT_Init_FreeType(&ftLibrary)) {
            throw std::runtime_error("Failed to initialize FreeType library");
        }
    }

    ~FontLib_t() {
        for (auto& font : fontTable) {
            FT_Done_Face(font.second.face);
        }
        FT_Done_FreeType(ftLibrary);
    }

    int loadFont(const char* name, const char* data, size_t length) {
        if (fontTable.size() >= maxFonts) {
            notifyf("Maximum number of fonts loaded\n");
            return -1; // Maximum number of fonts loaded
        }

        if (fontTable.find(name) != fontTable.end()) {
            notifyf("Font already loaded\n");
            return -2; // Font already loaded
        }

        Font font;
        font.name = name;
        font.data.assign(data, data + length);

        if (FT_New_Memory_Face(ftLibrary, font.data.data(), length, 0, &font.face)) {
            notifyf("Failed to load font\n");
            return -3; // Failed to load font
        }

        fontTable[name] = std::move(font);
        if (selectedFont == nullptr) {
            selectedFont = &fontTable[name];
        }
        return 0; // Success
    }

    int setFont(const char* name, int font_size = 0) {
        if (font_size > 0) fontSize = font_size;
        auto it = fontTable.find(name);
        if (it == fontTable.end()) {
            // Search for the font in the precompiled fonts
            for (int i = 0; i < ttf_fonts_count; i++) {
                if (strcmp(name, ttf_fonts[i].name) == 0) {
                    int error = loadFont(name, (const char*) ttf_fonts[i].data, ttf_fonts[i].length);
                    if (error) {
                        notifyf("Failed to load font\n");
                        return -1; // Failed to load font
                    }
                    it = fontTable.find(name);
                    break;
                }
            }
            return -1; // Font not found
        }
        selectedFont = &it->second;
        return 0; // Success
    }

    FT_GlyphSlot* getChar(char c, const char* name = nullptr, int font_size = 0) {
        if (name != nullptr) {
            setFont(name, font_size);
        }

        if (selectedFont == nullptr) {
            notifyf("No font selected\n");
            return nullptr;
        }

        if (FT_Set_Pixel_Sizes(selectedFont->face, 0, fontSize)) {
            notifyf("Failed to set font size %d\n", fontSize);
            return nullptr;
        }

        if (FT_Load_Char(selectedFont->face, c, monochrome ? FT_LOAD_RENDER|FT_LOAD_MONOCHROME : FT_LOAD_RENDER)) {
            notifyf("Failed to load character %c\n", c);
            return nullptr;
        }

        return &selectedFont->face->glyph;
    }

    int getWidth(const char* name, const char* text, int length) {
        auto it = fontTable.find(name);
        if (it == fontTable.end()) {
            return -1; // Font not found
        }

        Font& font = it->second;

        if (FT_Set_Pixel_Sizes(font.face, 0, 64)) {
            return -2; // Failed to set font size
        }

        int width = 0;
        for (int i = 0; i < length; ++i) {
            if (FT_Load_Char(font.face, text[i], FT_LOAD_RENDER)) {
                return -3; // Failed to load character
            }

            width += font.face->glyph->advance.x >> 6;
        }

        return width;
    }
};


FontLib_t FontLib;