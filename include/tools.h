#pragma once


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <chrono>
#include <algorithm>


#ifndef RAD2DEG
#define RAD2DEG 57.295779513082320876798154814105
#endif

#ifndef DEG2RAD
#define DEG2RAD 0.017453292519943295769236907684886
#endif

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
	bool isBlank() {
		return a == 0;
	}
};

const Color WHITE = { 255, 255, 255, 255 };
const Color BLACK = { 0, 0, 0, 255 };

const Color BLANK = { 0, 0, 0, 0 };


int invert_color(int color, int inversion) {
    // Flip the color about the midpoint by the inversion value (0 = no inversion, 255 = full inversion)
    int fully_inverted = 255 - color;
    int diff = fully_inverted - color;
    int change = diff * inversion / 255;
    return color + change;
}

struct Vector2 {
	float x;
	float y;
};

struct Rectangle {
	float x;
	float y;
	float width;
	float height;
};

typedef uint8_t u8;
typedef int8_t i8;

typedef uint16_t u16;
typedef int16_t i16;

typedef uint32_t u32;
typedef int32_t i32;

#define READ_BE16(mem) ((((u8*)(mem))[0] << 8) | (((u8*)(mem))[1]))
#define READ_BE32(mem) ((((u8*)(mem))[0] << 24) | (((u8*)(mem))[1] << 16) | (((u8*)(mem))[2] << 8) | (((u8*)(mem))[3]))
#define P_MOVE(mem, a) ((mem) += (a))

#define READ_BE16_MOVE(mem) (READ_BE16((mem))); (P_MOVE((mem), 2))
#define READ_BE32_MOVE(mem) (READ_BE32((mem))); (P_MOVE((mem), 4))

typedef struct {
	u32	scaler_type;
	u16	numTables;
	u16	searchRange;
	u16	entrySelector;
	u16	rangeShift;
} offset_subtable;

typedef struct {
	union {
		char tag_c[4];
		u32	tag;
	};
	u32	checkSum;
	u32	offset;
	u32	length;
} table_directory;


typedef struct  {
	offset_subtable off_sub;
	table_directory* tbl_dir;
} font_directory;

void read_offset_subtable(char** mem, offset_subtable* off_sub) {
	char* m = *mem;
	off_sub->scaler_type = READ_BE32_MOVE(m);
	off_sub->numTables = READ_BE16_MOVE(m);
	off_sub->searchRange = READ_BE16_MOVE(m);
	off_sub->entrySelector = READ_BE16_MOVE(m);
	off_sub->rangeShift = READ_BE16_MOVE(m);
	*mem = m;
}

void read_table_directory(char** mem, table_directory** tbl_dir, int tbl_size) {
	char* m = *mem;
	*tbl_dir = (table_directory*)calloc(1, sizeof(table_directory)*tbl_size);

	for(int i = 0; i < tbl_size; ++i) {
		table_directory* t = *tbl_dir + i;
		t->tag = READ_BE32_MOVE(m);
		t->checkSum = READ_BE32_MOVE(m);
		t->offset = READ_BE32_MOVE(m);
		t->length = READ_BE32_MOVE(m);
	}

	*mem = m;
}

void read_font_directory(char** mem, font_directory* ft) {
	read_offset_subtable(mem, &ft->off_sub); 
	read_table_directory(mem, &ft->tbl_dir, ft->off_sub.numTables); 
}

void print_table_directory(table_directory* tbl_dir, int tbl_size) {
	printf("#)\ttag\tlen\toffset\n");
	for(int i = 0; i < tbl_size; ++i) {
		table_directory* t = tbl_dir + i;
		printf("%d)\t%c%c%c%c\t%d\t%d\n", i+1,
				t->tag_c[3], t->tag_c[2],
				t->tag_c[1], t->tag_c[0],
				t->length, t->offset);
	}
}

typedef struct {
	u16 platformID;
	u16 platformSpecificID;
	u32 offset;
} cmap_encoding_subtable;

typedef struct {
	u16 version;
	u16 numberSubtables;
	cmap_encoding_subtable* subtables;
} cmap;


char* loadFile(const char* fileName) {
    char* text = NULL;
    FILE* file = fopen(fileName, "rt");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        int size = ftell(file);
        rewind(file);
        if (size > 0) {
            text = (char*) malloc(sizeof(char) * (size + 1));
            int count = fread(text, sizeof(char), size, file);
            text[count] = '\0';
        }
        fclose(file);
    }
    return text;
}


int saveFile(const char* fileName, const char* data, int size) {
    if (size <= 0) return -1;
    FILE* file = fopen(fileName, "wt");
    if (file != NULL) {
        fwrite(data, sizeof(char), size, file);
        fclose(file);
        return 0;
    }
    return -1;
}

double GetTime() {
	const auto now = std::chrono::system_clock::now();
	const auto duration = now.time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
}