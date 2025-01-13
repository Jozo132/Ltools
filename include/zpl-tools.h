#pragma once

#include "draw_utils.h"
#include "barcodex.h"
#include "imagex.h"
#include "stopwatch.h"




constexpr int ZPL_MAX_ELEMENTS = 1024 * 2;
constexpr int ZPL_MAX_STRING = 1024 * 2;

/*
^XA

^FX Demo VDA4902 Label Template

^FX A5 landscape border
^FO10,10^GB1200,792,3^FS

^FX VDA 4902 frame
^FX Horizontal Dividers
^FO10,110^GB1200,1,1^FS
^FO10,250^GB1200,1,1^FS
^FO600,180^GB610,1,1^FS
^FO10,400^GB1200,1,1^FS
^FO10,560^GB590,1,1^FS
^FO600,470^GB610,1,1^FS
^FO600,620^GB610,1,1^FS
^FO10,670^GB1200,1,1^FS

^FX Vertical Dividers
^FO600,10^GB1,240,1^FS
^FO800,180^GB1,70,1^FS
^FO1000,180^GB1,70,1^FS
^FO600,400^GB1,400,1^FS
^FO840,620^GB1,50,1^FS

^FX Set default font
^CF0,16

^FX Section 1: Receiver
^FO20,20^FD(1) Receiver^FS
^CF0,32
^FO20,45^FDXYZ Motors Inc.^FS
^FO20,80^FD1234 Industrial Drive, Detroit, 48201^FS

^XZ
*/


enum ZPL_CMD {
    UNKNOWN = 0,
    XA, // Start Label
    XZ, // End Label
    CC, // Change Caret
    CD, // Change Delimiter
    CF, // Change Font
    PW, // Print Width in Dots
    LL, // Label Length in Dots
    PQ, // Print Quantity (Number of Copies)
    SN, // Serial Number (Incrementing with each subsequent copy using PQ)
    FO, // Field Origin
    FR, // Invert
    GB, // Graphic Box
    FD, // Field Data
    FS, // End Field
    BY, // Barcode Field Default
    B3, // Barcode 39
    BC, // Barcode 128
    FX, // Comment
    GF, // Graphic Field
};

// Use macro to generate the enum strings to make it easier to print the enum
#define ZPL_CMD_STRINGS \
    "UNKNOWN", \
    "XA", \
    "XZ", \
    "CC", \
    "CD", \
    "CF", \
    "PW", \
    "LL", \
    "PQ", \
    "SN", \
    "FO", \
    "FR", \
    "GB", \
    "FD", \
    "FS", \
    "BY", \
    "B3", \
    "FX", \
    "GF"

const char* ZPL_CMD_NAMES [] = { ZPL_CMD_STRINGS };




Image* temp_img = nullptr;



struct ZPL_element {
    const char* str = nullptr;
    int len = 0;
    ZPL_CMD type = UNKNOWN;
    int index = -1;
    char character = '\0';
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int radius = 0; // 0 - 8 (0 = square corners, 8 = round corners)
    int inset = 0;
    char color = 'B'; // B = Black, W = White
    std::string text;
    int font_type = 0;
    int font_size = 0;
    bool inverted = false;
    int increment = 1;
    bool padding = false;
    bool barcode = false;
    bool show_text = false;
    bool checksum = false;
    int barcode_width = 2;
    int barcode_height = 10;
    int barcode_wn_ratio = 3;
    char orientation = 'N';
    char check = 'N';
    char mode = 'N';
    char interpretation = 'N';
    char interpretation_above = 'N';
    uint8_t* halfbytes = nullptr;
    void print() {
        if (type == UNKNOWN) {
            printf("        Unknown: %.*s\n", len, str);
            return;
        }
        switch (type) {
            case XA: printf("        Start Label\n"); break;
            case XZ: printf("        End Label\n"); break;
            case CC: printf("        Change Caret: %c\n", character); break;
            case CD: printf("        Change Delimiter: %c\n", character); break;
            case CF: printf("        Font: %d, %d\n", font_type, font_size); break;
            case PW: printf("        Print Width: %d\n", width); break;
            case LL: printf("        Label Length: %d\n", height); break;
            case PQ: printf("        Print Quantity: %d\n", x); break;
            case SN: printf("        Serial Number: %s\n", text.c_str()); break;
            case FO: printf("        Field Origin: %d, %d\n", x, y); break;
            case FX: printf("        Comment: %s\n", text.c_str()); break;
            case FD: printf("        Text %d,%d: %s\n", font_type, font_size, text.c_str()); break;
            case GB: printf("        Rect: %d, %d, %d, %d, %d, %c, %d\n", x, y, width, height, inset, color, radius); break;
            case FR: printf("        Invert\n"); break;
            case FS: printf("        End Field\n"); break;
            case BY: printf("        Barcode Field Default %d, %d, %d, %d, %c, %c, %c, %c\n", x, y, width, height, orientation, check, interpretation, interpretation_above); break;
            case B3: printf("        Barcode [%d,%d] Code 39 %c,%c,%d,%c,%c -> %s\n", x, y, orientation, check, barcode_height, interpretation, interpretation_above, text.c_str()); break;
            case BC: printf("        Barcode [%d,%d] Code 128 %c,%c,%d,%c,%c -> %s\n", x, y, orientation, check, barcode_height, interpretation, interpretation_above, text.c_str()); break;
            case GF: printf("        Graphic Field\n"); break;

            default: printf("        Other: %d\n", type); break;
        }
    }

    void draw(Image* image) {
        if (!image) return;
        switch (type) {
            case GB: {
                float roundness = (float) (radius < 0 ? 0 : radius > 8 ? 8 : radius) / 8.0f;
                float shortHalf = width < height ? width / 2 : height / 2;
                bool full = inset >= shortHalf;
                const Color stroke = color == 'W' ? WHITE : BLACK;
                    float ix = x;
                    float iy = y;
                    float iw = width;
                    float ih = height;
                if (full) {
                    // ImageDrawRectangleRounded(image, (Rectangle) { ix, iy, iw, ih }, roundness, 128, stroke);
                    image->drawRoundedRectangle(ix, iy, iw, ih, roundness, 0, BLANK, stroke, inverted);
                } else {
                    // ImageDrawRectangleRoundedLinesEx(image, (Rectangle) { ix, iy, iw, ih }, roundness, 128, inset, stroke);
                    image->drawRoundedRectangle(ix, iy, iw, ih, roundness, inset, stroke, BLANK, inverted);
                }
            } break;

            case FD: {
                float ix = x;
                float iy = y;
                const Color stroke = color == 'W' ? WHITE : BLACK;
                bool font_found = true;
                std::string font_name = "Helvetica";
                switch (font_type) {
                    case 0: font_name = "Helvetica"; break;
                    case 1: font_name = "OCR-A"; break;
                    case 2: font_name = "OCR-B"; break;
                    case 3: font_name = "Roboto-Regular"; break;
                    default: {
                        notifyf("Unknown font type %d\n", font_type);
                        font_found = false;
                        break;
                    }
                }
                if (!font_found) return;
                image->drawText(ix, iy, font_size, text.c_str(), font_name.c_str(), stroke, inverted);
            } break;

            case GF: {
                // Draw custom graphic field
                // A halfbyte represents 4x1 pixels in the image
                float scaling_factor = 1;
                for (float iy = 0; iy < height; iy++) {
                    for (float ix = 0; ix < width; ix++) {
                        int halfbyte = halfbytes[((int) iy) * width + ((int) ix)];
                        for (int i = 0; i < 4; i++) {
                            int bit = (halfbyte >> (3 - i)) & 1;
                            // if (bit) DrawPixel(x + ix * 4.0 * scaling_factor + i, y + iy * scaling_factor, BLACK);
                            if (bit) image->drawPixel(x + ix * 4.0 * scaling_factor + i, y + iy * scaling_factor, BLACK, inverted);
                        }
                    }
                }
            } break;

            case B3: {
                // orientation
                bool checksum = check == 'Y';
                bool show = interpretation == 'Y';
                int h = barcode_height;
                int w = barcode_width;
                // interpretation_above
                ImageDrawBarcode_Code39(image, text.c_str(), x, y, h, w, show, checksum, inverted);
            } break;

            case BC: {
                // orientation
                // bool checksum = check == 'Y'; // Unused
                bool show = interpretation == 'Y';
                int h = barcode_height;
                int w = barcode_width;
                // interpretation_above
                ImageDrawBarcode_Code128(image, text.c_str(), x, y, h, w, show, inverted);
            } break;

            default: {

            } break;
        }
    }
};

struct ZPL_state {
    bool reading = false; // XA and XZ
    int line = 0;
    int column = 0;
    char caret = '^';
    char delimiter = ',';
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int radius = 0;
    int inset = 0;
    char color = 'B';
    const char* text = nullptr;
    int font_type = 0;
    int font_size = 0;
    bool inverted = false;
    int barcode_width = 2;
    int barcode_wn_ratio = 3;
    int barcode_height = 10;
    void reset() {
        x = 0;
        y = 0;
        width = 0;
        height = 0;
        radius = 0;
        inset = 0;
        color = 'B';
        text = nullptr;
        inverted = false;
    }
};

struct ZPL_parsing_error {
    int parsed;
    int error;
    char message[256];
    int line;
    int column;
};


int hexCharToNum(char hex) {
    if (hex >= '0' && hex <= '9') return hex - '0';
    if (hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
    if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
    return -1;
}

struct ZPL_RLE_parser {
    const char* str;
    int len;
    int idx = 0;
    int pixel_count = 0;
    uint8_t* half_byte_pixels = nullptr;
    int width = 0; // 1/4 in size of the actual pixed count width
    int height = 0; // 1/1 in size of the actual pixed count height
    int error = 0;
    const char* message = nullptr;
    void _push(uint8_t halfbyte, int repeat = 1) {
        if ((idx + repeat) > pixel_count) {
            // error = 1;
            // message = "Too many pixels";
            return;
        }
        for (int i = 0; i < repeat; i++) {
            half_byte_pixels[idx++] = halfbyte;
        }
    }
    void _copy_previous_row() { // Symbol ':'
        // Override current row with previous row and set index to the start of the next row
        int row = idx / width;
        int previous = row > 0 ? (row - 1) * width : 0;
        int start = row * width;
        if (row > 0) {
            for (int i = 0; i < width; i++) {
                half_byte_pixels[start + i] = half_byte_pixels[previous + i];
            }
        }
        idx = start + width;
    }
    void _fill_remaining_row() { // Symbol '!'
        // Fill the rest of the row with the 0xF half bytes until the end of the row
        int row = idx / width;
        int start = row * width;
        for (int i = idx; i < start + width; i++) {
            half_byte_pixels[i] = 0xF;
        }
        idx = start + width;
    }
    void _fill_empty_row() { // Symbol ','
        // Skip the rest of the row and set index to the start of the next row
        int row = idx / width;
        int start = row * width;
        idx = start + width;
    }
    int getRepeat(char c) {
        if (!c) return 0;
        if (c >= 'G' && c < 'Z') return c - 'G' + 1;
        if (c >= 'g' && c <= 'z') return (c - 'g' + 1) * 20;
        if (c >= 'G') return 1;
        return 0;
    }
    bool parse(int byte_count, int column_count, const char* str, int length, char caret) { // A,4096,4096,32,,:::::::::::::hY03........
        if (half_byte_pixels) free(half_byte_pixels);
        this->str = str;
        this->len = length;
        this->idx = 0;
        this->error = 0;
        this->message = nullptr;

        width = column_count * 2;
        height = (byte_count * 2) / width;
        pixel_count = width * height;

        half_byte_pixels = new uint8_t[pixel_count];
        for (int i = 0; i < pixel_count; i++) {
            half_byte_pixels[i] = 0;
        }

        if (len < 2) {
            error = 1;
            message = "Empty RLE string";
            return false;
        }
        int idx = 1;
        while (idx < len) {
            char c = str[idx++];
            if (c == caret) break;
            if (c == ',') {
                _fill_empty_row();
                continue;
            }
            if (c == '!') {
                _fill_remaining_row();
                continue;
            }
            if (c == ':') {
                _copy_previous_row();
                continue;
            }
            int value = hexCharToNum(c);
            if (value >= 0) {
                _push(value, 1);
                continue;
            }
            int repeat = getRepeat(c);
            char next = str[idx++];
            if (next == caret) break;
            while (getRepeat(next) > 0) {
                repeat += getRepeat(next);
                next = str[idx++];
            }
            if (next == caret) break;
            value = hexCharToNum(next);
            if (value != -1) {
                _push(value, repeat);
            } else {
                error = 1;
                message = "Invalid RLE character";
                return false;
            }
        }
        return true;
    }
} rle_parser;

class ZPL_label {
public:
    ZPL_parsing_error errorObj;
    int error = 0;
    const char* message = nullptr;
    int line = 0;
    int column = 0;
    int idx = 0;

    int label_width_parm = 0;
    int label_height_parm = 0;
    int copies = 1;

    ZPL_state state;
    ZPL_element elements[ZPL_MAX_ELEMENTS];
    int length = 0;
    int barcode_awaiting_text = -1;
    Image image;

    ZPL_element* nextElement() {
        if (length >= ZPL_MAX_ELEMENTS) return nullptr;
        return &elements[length++];
    }

    void clear() {
        error = 0;
        message = nullptr;
        line = 0;
        column = 0;
        idx = 0;
        state.reading = false;
        // if (state.text) free((void*) state.text);
        // for (int i = 0; i < length; i++) {
        //     if (elements[i].text) free((void*) elements[i].text);
        //     if (elements[i].halfbytes) free(elements[i].halfbytes);
        // }
        state.reset();
        length = 0;
        barcode_awaiting_text = -1;
    }

    void print() {
        printf("    Label with %d elements\n", length);
        for (int i = 0; i < length; i++) {
            ZPL_element& element = elements[i];
            element.print();
        }
    }

    Image* draw(int width = 0, int height = 0) {
        if (width == 0) width = label_width_parm;
        if (height == 0) height = label_height_parm;
        if (width <= 0 || height <= 0) return &image;
        if (width != image.width || height != image.height) {
            image.resize(width, height, WHITE);
        }
        for (int i = 0; i < length; i++) {
            ZPL_element& element = elements[i];
            element.draw(&image);
        }
        return &image;
    }

    void draw(Image& im) {
        if (label_width_parm > 0 && label_height_parm > 0) {
            im.resize(label_width_parm, label_height_parm, WHITE);
        }
        if (im.width <= 0 || im.height <= 0) return;
        for (int i = 0; i < length; i++) {
            ZPL_element& element = elements[i];
            element.draw(&im);
        }
    }
};



bool startsWith(const char* str, const char* prefix) {
    while (*prefix) {
        if (*prefix++ != *str++) return false;
    }
    return true;
}


ZPL_CMD nextCommand(const char* str, int len) {
    while (len > 0 && (str[0] == ' ' || str[0] == '\t' || str[0] == '\r' || str[0] == '\n' || str[0] == '^')) {
        str++;
        len--;
    }
    if (len < 2) return UNKNOWN;
    if (startsWith(str, "XA")) return XA;
    if (startsWith(str, "XZ")) return XZ;
    if (startsWith(str, "CC")) return CC;
    if (startsWith(str, "CD")) return CD;
    if (startsWith(str, "CF")) return CF;
    if (startsWith(str, "PW")) return PW;
    if (startsWith(str, "LL")) return LL;
    if (startsWith(str, "PQ")) return PQ;
    if (startsWith(str, "SN")) return SN;
    if (startsWith(str, "FO")) return FO;
    if (startsWith(str, "FR")) return FR;
    if (startsWith(str, "GB")) return GB;
    if (startsWith(str, "FD")) return FD;
    if (startsWith(str, "FX")) return FX;
    if (startsWith(str, "FS")) return FS;
    if (startsWith(str, "BY")) return BY;
    if (startsWith(str, "B3")) return B3;
    if (startsWith(str, "BC")) return BC;
    if (startsWith(str, "GF")) return GF;
    return UNKNOWN;
}

ZPL_parsing_error parseNumber(char caret, char delimiter, const char* str, int len, int& number, bool required = false) {
    int n = 0;
    int count = 0;
    int skipDelimiter = 0;
    if (len == 0) return (ZPL_parsing_error) { 0, 1, "Empty number", 0, 0 };
    if (str[0] == delimiter || str[0] == caret) {
        if (required) return (ZPL_parsing_error) { 0, 1, "Missing required number", 0, 0 };
        if (str[0] == delimiter) skipDelimiter = 1;
        return (ZPL_parsing_error) { skipDelimiter, 0, "", 0, 0 };
    }
    for (int i = 0; i < len; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            n = n * 10 + (str[i] - '0');
            count++;
        } else {
            if (str[i] == delimiter) {
                skipDelimiter = 1;
                break;
            }
            if (str[i] == caret) break;
            if (str[i] == '\t') continue;
            if (str[i] == '\r') break;
            if (str[i] == '\n') break;
            if (str[i] == ' ') continue;
            return (ZPL_parsing_error) { i, 1, "Invalid number", 0, i };
        }
    }
    if (count > 0) number = n; // Only update if we found a number
    else {
        if (required) return (ZPL_parsing_error) { 0, 1, "Missing required number", 0, 0 };
    }
    return (ZPL_parsing_error) { count + skipDelimiter, 0, "", 0, 0 };
}


ZPL_parsing_error parseString(char caret, char delimiter, const char* str, int len, char* text, bool required = false, bool ignoreDelimiter = false) {
    int count = 0;
    int skipDelimiter = 0;
    if (len == 0) return (ZPL_parsing_error) { 0, 1, "Empty string", 0, 0 };
    if (str[0] == delimiter || str[0] == caret) {
        if (required) return (ZPL_parsing_error) { 0, 1, "Missing required string", 0, 0 };
        if (str[0] == delimiter) skipDelimiter = 1;
        return (ZPL_parsing_error) { skipDelimiter, 0, "", 0, 0 };
    }
    for (int i = 0; i < len; i++) {
        if (str[i] == delimiter && !ignoreDelimiter) {
            skipDelimiter = 1;
            break;
        }
        if (str[i] == caret) break;
        if (str[i] == '\t') continue;
        if (str[i] == '\r') continue;
        if (str[i] == '\n') continue;
        text[count++] = str[i];
    }
    text[count] = '\0';
    if (count == 0) {
        if (required) return (ZPL_parsing_error) { 0, 1, "Missing required string", 0, 0 };
    }
    return (ZPL_parsing_error) { count + skipDelimiter, 0, "", 0, 0 };
}

ZPL_parsing_error parseChar(char caret, char delimiter, const char* str, int len, char& character, bool required = false) {
    if (len == 0) return (ZPL_parsing_error) { 0, 1, "Empty character", 0, 0 };
    int skipDelimiter = 0;
    if (str[0] == delimiter || str[0] == caret) {
        if (required) return (ZPL_parsing_error) { 0, 1, "Missing required character", 0, 0 };
        if (str[0] == delimiter) skipDelimiter = 1;
        return (ZPL_parsing_error) { skipDelimiter, 0, "", 0, 0 };
    }
    character = str[0];
    if (str[1] == delimiter) skipDelimiter = 1;
    return (ZPL_parsing_error) { 1 + skipDelimiter, 0, "", 0, 0 };
}

char* substring(const char* str, int start, int end) {
    int len = end - start;
    if (len <= 0) return nullptr;
    char* result = (char*) malloc(len + 1);
    for (int i = 0; i < len; i++) {
        result[i] = str[start + i];
    }
    result[len] = '\0';
    return result;
}

int indexOf(const char* str, int len, char c) {
    for (int i = 0; i < len; i++) {
        if (str[i] == c) return i;
    }
    return -1;
}

int string_length(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

int string_length(char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

char line_msg[1024];
char* lineAt(const char* str, int length, int index, int* offset = nullptr, int* row = nullptr) {
    if (index < 0) return nullptr;
    if (index >= length) return nullptr;
    int start = index;
    int _offset = 0;
    if (row) {
        int _row = 1;
        for (int i = 0; i < index; i++) {
            if (str[i] == '\n') _row++;
        }
        *row = _row;
    }
    while (start > 0 && str[start] != '\0' && str[start] != '\n' && start > (index - 40)) {
        start--;
        _offset++;
    }
    if (str[start] == '\n') start++;
    if (offset) *offset = _offset;
    int end = index;
    while (end < length && str[end] != '\0' && str[end] != '\n' && end < (index + 40)) end++;
    // return substring(str, start, end);
    for (int i = 0; i < end - start; i++) {
        line_msg[i] = str[start + i];
    }
    line_msg[end - start] = '\0';
    return line_msg;
}

#define ZPL_NEXT(count) { idx += count; c += count; parsed += count; }
#define ZPL_THROW(cmp, ...) { if (cmp) {label.errorObj = __VA_ARGS__; label.error = label.errorObj.error; label.message = label.errorObj.message; label.line = label.errorObj.line; label.column = label.errorObj.column; return &label; } else { ZPL_NEXT(err.parsed); } }
#define ZPL_VEFIRY_ELEMENT(element) { if (!element) { label.error = 1; label.message = "Too many elements"; return &label; } }

#define Z_REQUIRED true
#define Z_OPTIONAL false

#define WITH_DELIMITER true
#define WITHOUT_DELIMITER false

#define ZPL_PARSE_NUMBER(number, required) { err = parseNumber(caret, delimiter, c, zpl_len - idx, number, required); ZPL_THROW(err.error, err); }
#define ZPL_PARSE_STRING(text, required, ignoreDelimiter) { err = parseString(caret, delimiter, c, zpl_len - idx, text, required, ignoreDelimiter); ZPL_THROW(err.error, err); }
#define ZPL_PARSE_CHAR(character, required) { err = parseChar(caret, delimiter, c, zpl_len - idx, character, required); ZPL_THROW(err.error, err); }


ZPL_parsing_error skipDelimiter(char delimiter, const char* str, bool required = false) {
    if (str[0] == '\0') return (ZPL_parsing_error) { 0, 1, "End of string", 0, 0 };
    if (str[0] == delimiter) return (ZPL_parsing_error) { 1, 0, "", 0, 0 };
    if (required) return (ZPL_parsing_error) { 0, 1, "Missing required delimiter", 0, 0 };
    return (ZPL_parsing_error) { 0, 0, "", 0, 0 };
}

ZPL_label label;
ZPL_label* parse_zpl(const char* zpl_text, int zpl_len) {
    label.clear();
    auto& idx = label.idx;
    auto& state = label.state;
    auto& caret = state.caret;
    auto& delimiter = state.delimiter;
    auto& reading = state.reading;
    auto& x = state.x;
    auto& y = state.y;
    auto& width = state.width;
    auto& height = state.height;
    auto& inset = state.inset;
    auto& radius = state.radius;
    auto& color = state.color;
    auto& inverted = state.inverted;


    // Parse ZPL text
    idx = 0;
    char* c = (char*) zpl_text;

    char temp[ZPL_MAX_STRING];

    ZPL_CMD cmd = UNKNOWN;
    ZPL_parsing_error err;
    while (idx < zpl_len) {
        int parsed = 0;
        if (*c != caret) {
            c++;
            idx++;
            continue;
        }
        const char* c0 = c;
        ZPL_NEXT(1);
        cmd = nextCommand(c, zpl_len - idx);
        if (cmd == UNKNOWN) {
            char cmd_str[4] = { c0[0], c0[1], c0[2], '\0' };
            sprintf(err.message, "Unknown command: %s", cmd_str);
            err.error = 1;
            err.parsed = 2;
        } else {
            err.message[0] = '\0';
            err.error = 0;
            err.parsed = 2;
        }
        ZPL_THROW(err.error, err);

        switch (cmd) {
            case XA: {
                reading = true;
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
            } break;
            case XZ: {
                reading = false;
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
            } break;
            case CC: {
                // ^CC/
                char character = c[0];
                caret = character;
                ZPL_NEXT(1);
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->character = character;
            } break;
            case CD: {
                // ^CD;
                char character = c[0];
                delimiter = character;
                ZPL_NEXT(1);
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->character = character;
            } break;

            case CF: {
                // ^CF0,16
                char font = '0';
                ZPL_PARSE_CHAR(font, Z_OPTIONAL);
                ZPL_PARSE_NUMBER(state.font_size, Z_OPTIONAL);
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                if (font >= '0' && font <= '3') state.font_type = font - '0';
                if (font >= 'A') state.font_type = font - 'A' + 1;
                element->font_type = state.font_type;
                element->font_size = state.font_size;
            } break;

            case PW: {
                // ^PW800
                ZPL_PARSE_NUMBER(width, Z_REQUIRED);
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->width = width;
                label.label_width_parm = width;
            } break;

            case LL: {
                // ^LL800
                ZPL_PARSE_NUMBER(height, Z_REQUIRED);
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->height = height;
                label.label_height_parm = height;
            } break;

            case PQ: {
                // ^PQ3
                int copies = 1;
                ZPL_PARSE_NUMBER(copies, Z_REQUIRED);
                if (copies < 1) copies = 1;
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->x = copies;
                label.copies = copies;
            } break;

            case SN: {
                // ^SN0001,1,Y
                ZPL_PARSE_STRING(temp, Z_REQUIRED, WITHOUT_DELIMITER);
                int increment = 1;
                ZPL_PARSE_NUMBER(increment, Z_OPTIONAL);
                char* temp2 = (char*) malloc(16);
                ZPL_PARSE_CHAR(temp2[0], Z_OPTIONAL);
                bool pad = temp2[0] == 'Y';
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->text = temp;
                element->increment = increment;
                element->padding = pad;
            } break;

            case FO: {
                // ^FO10,10
                ZPL_PARSE_NUMBER(x, Z_REQUIRED);
                ZPL_PARSE_NUMBER(y, Z_REQUIRED);
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->x = x;
                element->y = y;
            } break;
            case FR: {
                // ^FR
                inverted = true;
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->inverted = inverted;
            } break;

            case GB: {
                // ^GB1200,792,3,B,3  (width, height, inset=3, color=W, radius=3)
                // ^GB1200,792,3,,3  (width, height, inset=3, default=B, radius=3)
                // ^GB1200,792,3  (width, height, inset=3, default=B, default=0)
                ZPL_PARSE_NUMBER(width, Z_REQUIRED);
                ZPL_PARSE_NUMBER(height, Z_REQUIRED);
                ZPL_PARSE_NUMBER(inset, Z_OPTIONAL);
                ZPL_PARSE_STRING(temp, Z_OPTIONAL, WITHOUT_DELIMITER);
                if (temp[0] == 'B' || temp[0] == 'W') color = temp[0];
                ZPL_PARSE_NUMBER(radius, Z_OPTIONAL);
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->x = x;
                element->y = y;
                element->width = width;
                element->height = height;
                element->inset = inset;
                element->radius = radius;
                element->color = color;
                element->inverted = inverted;
                state.reset();
            } break;

            case FD: {
                // ^FDHello, World^FS
                ZPL_PARSE_STRING(temp, Z_REQUIRED, WITH_DELIMITER);
                if (label.barcode_awaiting_text >= 0) {
                    ZPL_element& bc = label.elements[label.barcode_awaiting_text];
                    bc.text = temp;
                    bc.x = x;
                    bc.y = y;
                    bc.inverted = inverted;
                    // bc.color = color;
                    label.barcode_awaiting_text = -1;
                } else {
                    ZPL_element* element = label.nextElement();
                    ZPL_VEFIRY_ELEMENT(element);
                    element->str = substring(c0, 0, parsed);
                    element->len = parsed;
                    element->type = cmd;
                    element->x = x;
                    element->y = y;
                    element->text = temp;
                    element->color = color;
                    element->inverted = inverted;
                    element->font_type = state.font_type;
                    element->font_size = state.font_size;
                }
                state.reset();
            } break;
            case FX: {
                // ^FX Demo VDA4902 Label Template
                // Only store the text
                ZPL_PARSE_STRING(temp, Z_OPTIONAL, WITH_DELIMITER);
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->text = temp;
            }
            case FS: {
                // ^FS
                state.reset();
                temp[0] = '\0'; // Reset the temporary string
            } break;

            case BY: {
                // ^BY3
                ZPL_PARSE_NUMBER(state.barcode_width, Z_OPTIONAL);
                ZPL_PARSE_NUMBER(state.barcode_wn_ratio, Z_OPTIONAL);
                ZPL_PARSE_NUMBER(state.barcode_height, Z_OPTIONAL);
            } break;

            case B3: {
                // ^B3N,N,70,N,N^FD852934^FS
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);

                element->barcode_width = state.barcode_width;
                element->barcode_height = state.barcode_height;
                element->barcode_wn_ratio = state.barcode_wn_ratio;

                element->orientation = 'N';
                element->check = 'N';
                element->interpretation = 'N';
                element->interpretation_above = 'N';

                ZPL_PARSE_CHAR(element->orientation, Z_OPTIONAL); // N = normal, R = rotated 90 degrees clockwise, I = inverted 180 degrees, B = bottom up 180 degrees
                ZPL_PARSE_CHAR(element->check, Z_OPTIONAL); // N = no check digit, Y = check digit
                ZPL_PARSE_NUMBER(element->barcode_height, Z_OPTIONAL); // Height of the barcode in dots
                ZPL_PARSE_CHAR(element->interpretation, Z_OPTIONAL); // N = no interpretation line, Y = interpretation line
                ZPL_PARSE_CHAR(element->interpretation_above, Z_OPTIONAL); // N = no interpretation line above the barcode, Y = interpretation line above the barcode

                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->barcode = true;
                label.barcode_awaiting_text = label.length - 1;
            } break;

            case BC: {
                // ^BCN,70,N,N^FD852934^FS
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);

                element->barcode_width = state.barcode_width;
                element->barcode_height = state.barcode_height;
                element->barcode_wn_ratio = state.barcode_wn_ratio;

                element->orientation = 'N';
                element->check = 'N';
                element->interpretation = 'N';
                element->interpretation_above = 'N';
                element->mode = 'N';

                ZPL_PARSE_CHAR(element->orientation, Z_OPTIONAL); // N = normal, R = rotated 90 degrees clockwise, I = inverted 180 degrees, B = bottom up 180 degrees
                ZPL_PARSE_NUMBER(element->barcode_height, Z_OPTIONAL); // Height of the barcode in dots
                ZPL_PARSE_CHAR(element->interpretation, Z_OPTIONAL); // N = no interpretation line, Y = interpretation line
                ZPL_PARSE_CHAR(element->interpretation_above, Z_OPTIONAL); // N = no interpretation line above the barcode, Y = interpretation line above the barcode
                ZPL_PARSE_CHAR(element->check, Z_OPTIONAL); // N = no check digit, Y = check digit
                ZPL_PARSE_CHAR(element->mode, Z_OPTIONAL); // To be implemented

                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->barcode = true;
                label.barcode_awaiting_text = label.length - 1;
            } break;

            case GF: {
                // ^GFA,1024,1024,4,::::........
                char type = c[0];
                if (type != 'A') {
                    label.error = 1;
                    label.message = "Only type A is supported for graphic fields";
                    return &label;
                }
                ZPL_NEXT(2);
                int temp = 0;
                ZPL_PARSE_NUMBER(temp, Z_REQUIRED);
                int byte_count = 0;
                ZPL_PARSE_NUMBER(byte_count, Z_REQUIRED);
                int column_count = 0;
                ZPL_PARSE_NUMBER(column_count, Z_REQUIRED);
                int num_of_characters = indexOf(c, zpl_len - idx, '^');
                if (num_of_characters < 0) num_of_characters = zpl_len - idx;
                rle_parser.parse(byte_count, column_count, c, num_of_characters, caret);
                if (rle_parser.error) {
                    label.error = 1;
                    label.message = rle_parser.message;
                    label.idx = idx + rle_parser.idx;
                    return &label;
                }
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                // element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
                element->x = x;
                element->y = y;
                element->width = rle_parser.width;
                element->height = rle_parser.height;
                element->halfbytes = rle_parser.half_byte_pixels;
            } break;

            default: {
                ZPL_element* element = label.nextElement();
                ZPL_VEFIRY_ELEMENT(element);
                element->str = substring(c0, 0, parsed);
                element->len = parsed;
                element->type = cmd;
            } break;
        }
    }

    return &label;
}







Image temp_image = Image(0, 0, WHITE);
int zpl2png(std::string zpl_text, std::vector<uint8_t>& png_data, int width, int height, int dpi, PNG_ENCODER compression, bool debug = false) {
    if (zpl_text.empty()) {
        notifyf("Empty ZPL text\n");
        return 1;
    }
    // if (debug) timer.start("zpl2png total");
    if (debug) timer.start("Parse ZPL");
    ZPL_label* label = parse_zpl(zpl_text.c_str(), zpl_text.length()); // Decode ZPL
    if (debug) timer.log("Parse ZPL");
    if (!label) {
        notifyf("Error parsing ZPL\n");
        return 2;
    }
    if (label->error) {
        notifyf("Error reading ZPL: %s\n", label->message);
        return 3;
    }
    if (debug) timer.start("Render ZPL to image");
    temp_image.resize(width, height, WHITE);
    label->draw(temp_image); // Render ZPL to image
    if (debug) timer.log("Render ZPL to image");
    if (debug) timer.start("Compress image to PNG");
    // std::vector<unsigned char>* png = image.toPNG(PE_LODEPNG); // Slower but better compression
    // std::vector<unsigned char>* png = image.toPNG(PE_FPNG); // Faster but less compression
    std::vector<unsigned char>* png = temp_image.toPNG(compression); // Compress the image to PNG
    if (png == nullptr) {
        notifyf("Error converting image to PNG\n");
        return 4;
    }
    if (png->empty()) {
        notifyf("Empty PNG data\n");
        return 5;
    }
    if (debug) timer.log("Compress image to PNG");
    png_data.clear();
    png_data.insert(png_data.end(), png->begin(), png->end()); // Copy the PNG data to the output vector
    // if (debug) timer.log("zpl2png total");
    return 0;
}