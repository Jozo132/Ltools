/*
 * barcode.h -- definitions for libbarcode
 *
 * Copyright (c) 1999 Alessandro Rubini (rubini@gnu.org)
 * Copyright (c) 1999 Prosa Srl. (prosa@prosa.it)
 * Copyright (c) 2010 Giuseppe Scrivano <gscrivano@gnu.org>
 * Copyright (c) 2010, 2011 Giuseppe Scrivano (gscrivano@gnu.org)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

 // #include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Convert variable name to text
#define STRINGIFY(x) #x

#define _(X) STRINGIFY (X)
#define _N(X) (X)

/*
 * The object
 */
struct Barcode_Item {
    int flags;         /* type of encoding and decoding */
    char* ascii;       /* malloced */
    char* partial;     /* malloced too */
    char* textinfo;    /* information about text positioning */
    char* text;        /* text to bo printed */
    char* encoding;    /* code name, filled by encoding engine */
    int width, height; /* output units */
    int xoff, yoff;    /* output units */
    int margin;        /* output units */
    double scalef;     /* requested scaling for barcode */
    int error;         /* an errno-like value, in case of failure */
};


char* strdup(const char* s) {
    char* p = (char*) malloc(strlen(s) + 1);
    if (p) {
        strcpy(p, s);
    }
    return p;
}

/*
 * The flags field
 */
#define BARCODE_DEFAULT_FLAGS 0x00000000

#define BARCODE_ENCODING_MASK 0x000000ff   /* 256 possibilites... */
#define BARCODE_NO_ASCII      0x00000100   /* avoid text in output */
#define BARCODE_NO_CHECKSUM   0x00000200   /* avoid checksum in output */

#define BARCODE_OUTPUT_MASK   0x000ff000   /* 256 output types */
#define BARCODE_OUT_EPS       0x00001000
#define BARCODE_OUT_PS        0x00002000
#define BARCODE_OUT_PCL       0x00004000   /* by Andrea Scopece */
 /*                  PCL_III   0x00008000   */
#define BARCODE_OUT_PCL_III   0x0000C000
#define BARCODE_OUT_SVG       0x00010000
#define BARCODE_OUT_NOHEADERS 0x00100000   /* no header nor footer */

enum {
    BARCODE_ANY = 0,     /* choose best-fit */
    BARCODE_EAN,
    BARCODE_UPC,        /* upc == 12-digit ean */
    BARCODE_ISBN,       /* isbn numbers (still EAN13) */
    BARCODE_39,         /* code 39 */
    BARCODE_128,        /* code 128 (a,b,c: autoselection) */
    BARCODE_128C,       /* code 128 (compact form for digits) */
    BARCODE_128B,       /* code 128, full printable ascii */
    BARCODE_I25,        /* interleaved 2 of 5 (only digits) */
    BARCODE_128RAW,     /* Raw code 128 (by Leonid A. Broukhis) */
    BARCODE_CBR,        /* Codabar (by Leonid A. Broukhis) */
    BARCODE_MSI,        /* MSI (by Leonid A. Broukhis) */
    BARCODE_PLS,        /* Plessey (by Leonid A. Broukhis) */
    BARCODE_93,         /* code 93 (by Nathan D. Holmes) */
    BARCODE_11,         /* code 11 (USD-8) */
    BARCODE_39EXT       /* code 39 extended (by Ian Ward) */
};

#define BARCODE_DEFAULT_MARGIN 10
