/*  BarcodeCode128.cpp
 *
 *  Copyright (C) 2025  J.Vovk <jozo132@gmail.com>
 *
 *  This file is part of glbarcode++.
 *
 *  glbarcode++ is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  glbarcode++ is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with glbarcode++.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "BarcodeCode128.h"
#include "barcode.h"

#include "glbarcode/Constants.h"
#include <stdint.h>

#include <cctype>
#include <algorithm>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

using namespace glbarcode;
using namespace Constants;


namespace BC_C128 {

	static const char* codeset [] = {
	"212222", "222122", "222221", "121223", "121322",  /*  0 -  4 */
	"131222", "122213", "122312", "132212", "221213",
	"221312", "231212", "112232", "122132", "122231",  /* 10 - 14 */
	"113222", "123122", "123221", "223211", "221132",
	"221231", "213212", "223112", "312131", "311222",  /* 20 - 24 */
	"321122", "321221", "312212", "322112", "322211",
	"212123", "212321", "232121", "111323", "131123",  /* 30 - 34 */
	"131321", "112313", "132113", "132311", "211313",
	"231113", "231311", "112133", "112331", "132131",  /* 40 - 44 */
	"113123", "113321", "133121", "313121", "211331",
	"231131", "213113", "213311", "213131", "311123",  /* 50 - 54 */
	"311321", "331121", "312113", "312311", "332111",
	"314111", "221411", "431111", "111224", "111422",  /* 60 - 64 */
	"121124", "121421", "141122", "141221", "112214",
	"112412", "122114", "122411", "142112", "142211",  /* 70 - 74 */
	"241211", "221114", "413111", "241112", "134111",
	"111242", "121142", "121241", "114212", "124112",  /* 80 - 84 */
	"124211", "411212", "421112", "421211", "212141",
	"214121", "412121", "111143", "111341", "131141",  /* 90 - 94 */
	"114113", "114311", "411113", "411311", "113141",
	"114131", "311141", "411131", "b1a4a2", "b1a2a4",  /* 100 - 104 */
	"b1a2c2", "b3c1a1b"
	};

	const uint8_t START_A = 103;
	const uint8_t START_B = 104;
	const uint8_t START_C = 105;
	const uint8_t STOP = 106;
	const uint8_t SHIFT = 98; /* only A and B */
	const uint8_t CODE_A = 101; /* only B and C */
	const uint8_t CODE_B = 100; /* only A and C */
	const uint8_t CODE_C = 99; /* only A and B */
	const uint8_t FUNC_1 = 102; /* all of them */
	const uint8_t FUNC_2 = 97; /* only A and B */
	const uint8_t FUNC_3 = 96; /* only A and B */
	/* FUNC_4 is CODE_A when in A and CODE_B when in B */

	const uint8_t SYMBOL_WID = 11; /* all of them are 11-bar wide */


	/*
		128A (Code Set A) – ASCII characters 00 to 95 (0–9, A–Z and control codes), special characters, and FNC 1–4
		128B (Code Set B) – ASCII characters 32 to 127 (0–9, A–Z, a–z), special characters, and FNC 1–4
		128C (Code Set C) – 00–99 (encodes two digits with a single code point) and FNC1
	*/

	int Barcode_128b_verify(char* text) {
		if (text[0] == '\0')
			return -1;
		while (*text && *text >= 32 && !(*text & 0x80))
			text++;
		if (*text)
			return -1; /* a non-ascii char */
		return 0; /* ok */
	}

	int Barcode_128b_encode(struct Barcode_Item* bc)
	{
		static char* text;
		static char* partial;  /* dynamic */
		static char* textinfo; /* dynamic */
		char* textptr;
		int i, code, textpos, checksum = 0;

		if (bc->partial)
			free(bc->partial);
		if (bc->textinfo)
			free(bc->textinfo);
		bc->partial = bc->textinfo = NULL; /* safe */

		if (!bc->encoding)
			bc->encoding = strdup("code 128-B");

		text = bc->ascii;
		if (!text) {
			bc->error = EINVAL;
			return -1;
		}
		/* the partial code is 6* (head + text + check + tail) + final + term. */
		partial = (char*) malloc((strlen(text) + 4) * 6 + 2);
		if (!partial) {
			bc->error = errno;
			return -1;
		}

		/* the text information is at most "nnn:fff:c " * strlen +term */
		textinfo = (char*) malloc(10 * strlen(text) + 2);
		if (!textinfo) {
			bc->error = errno;
			free(partial);
			return -1;
		}

		/* up to now, it was the same code as other encodings */

		strcpy(partial, "0"); /* the first space */
		strcat(partial, codeset[START_B]);
		checksum += START_B; /* the start char is counted in the checksum */
		textptr = textinfo;
		textpos = SYMBOL_WID;

		for (i = 0; i < strlen(text); i++) {
			if (text[i] < 32 || (text[i] & 0x80)) {
				bc->error = EINVAL; /* impossible if text is verified */
				free(partial);
				free(textinfo);
				return -1;
			}
			code = text[i] - 32;
			strcat(partial, codeset[code]);
			checksum += code * (i + 1); /* first * 1 + second * 2 + third * 3... */
			sprintf(textptr, "%i:12:%c ", textpos, text[i]);
			textptr += strlen(textptr);
			textpos += SYMBOL_WID; /* width of each code */
		}
		/* Add the checksum, independent of BARCODE_NO_CHECKSUM */
		checksum %= 103;
		strcat(partial, codeset[checksum]);
		/* and the end marker */
		strcat(partial, codeset[STOP]);

		bc->partial = partial;
		bc->textinfo = textinfo;

		return 0;
	}

	/*
	 * code 128-c is only digits, but two per symbol
	 */

	int Barcode_128c_verify(char* text) {
		if (text[0] == '\0')
			return -1;
		/* must be an even number of digits */
		if (strlen(text) % 2)
			return -1;
		/* and must be all digits */
		for (; *text; text++)
			if (!isdigit(*text))
				return -1;
		return 0;
	}

	int Barcode_128c_encode(struct Barcode_Item* bc) {
		static char* text;
		static char* partial;  /* dynamic */
		static char* textinfo; /* dynamic */
		char* textptr;
		int i, code, textpos, checksum = 0;

		if (bc->partial)
			free(bc->partial);
		if (bc->textinfo)
			free(bc->textinfo);
		bc->partial = bc->textinfo = NULL; /* safe */

		if (!bc->encoding)
			bc->encoding = strdup("code 128-C");

		text = bc->ascii;
		if (!text) {
			bc->error = EINVAL;
			return -1;
		}
		/* the partial code is 6* (head + text + check + tail) + final + term. */
		partial = (char*) malloc((strlen(text) + 3) * 6 + 2);
		if (!partial) {
			bc->error = errno;
			return -1;
		}

		/* the text information is at most "nnn.5:fff:c " * strlen +term */
		textinfo = (char*) malloc(12 * strlen(text) + 2);
		if (!textinfo) {
			bc->error = errno;
			free(partial);
			return -1;
		}

		strcpy(partial, "0"); /* the first space */
		strcat(partial, codeset[START_C]);
		checksum += START_C; /* the start char is counted in the checksum */
		textptr = textinfo;
		textpos = SYMBOL_WID;

		for (i = 0; text[i]; i += 2) {
			if (!isdigit(text[i]) || !isdigit(text[i + 1])) {
				bc->error = EINVAL; /* impossible if text is verified */
				free(partial);
				free(textinfo);
				return -1;
			}
			code = (text[i] - '0') * 10 + text[i + 1] - '0';
			strcat(partial, codeset[code]);
			checksum += code * (i / 2 + 1); /* first * 1 + second * 2 + third * 3... */

			/* print as "%s", because we have ".5" positions */
			sprintf(textptr, "%g:9:%c %g:9:%c ", (double) textpos, text[i],
				textpos + (double) SYMBOL_WID / 2, text[i + 1]);
			textptr += strlen(textptr);
			textpos += SYMBOL_WID; /* width of each code */
		}
		/* Add the checksum, independent of BARCODE_NO_CHECKSUM */
		checksum %= 103;
		strcat(partial, codeset[checksum]);
		/* and the end marker */
		strcat(partial, codeset[STOP]);

		bc->partial = partial;
		bc->textinfo = textinfo;

		return 0;
	}


	/*
	 * generic (full-featured) code128 implementation: it selects between
	 * A, B, C according to the data being encoded. F1, F2, F3, F4 are expressed
	 * using ascii chars 0xc1, 0xc2, 0xc3, 0xc4 (0301, 0302, 0303, 0304).
	 * Char '\0' is expressed by 0x80 (0200).
	 */

	int Barcode_128_verify(unsigned char* text) {
		if (text[0] == '\0')
			return -1;
		while (*text && (*text <= 0x80 || (*text >= 0xc1 && *text <= 0xc4)))
			text++;
		if (*text)
			return -1; /* unencodable character */
		return 0; /* ok */
	}


	/*
	 * These functions are extracted from Barcode_128_encode for clarity.
	 * It deals with choosing the symbols used to represent the text
	 * and returns a dynamic array of integers, terminated by -1.
	 *
	 * The algorithm used in choosing the codes comes from App 2 of
	 * "El Codigo Estandar EAN/UCC 128", courtesy of AECOC, Spain.
	 * Thanks to Dani Pardo for getting permission and giving me a copy
	 * of the document
	 */

#define NEED_CODE_A(c) ((c)<32 || (c)==0x80) 
#define NEED_CODE_B(c) ((c)>=96 && (c)<128)

	static int Barcode_a_or_b(char* text) {
		for (; *text; text++) {
			if (NEED_CODE_A(*text))
				return 'A';
			if (NEED_CODE_B(*text))
				return 'B';
		}
		return 0; /* any */
	}

	/* code is either 'A' or 'B', and value must be valid */
	static int Barcode_encode_as(int code, int value)
	{

		/* first check the special chars */
		if (value == 0xC1) return FUNC_1;
		if (value == 0xC2) return FUNC_2;
		if (value == 0xC3) return FUNC_3;
		if (value == 0xC4) { /* F4 */
			if (code == 'A') return CODE_A;
			return CODE_B;
		}

		/* then check ascii values */
		if (value >= 0x20 && value <= 0x5F)
			return value - 0x20; /* both codes */
		if (value == 0x80) return 64; /* code A */
		if (value < 0x20) return value + 64; /* code A */
		if (value >= 0x60) return value - 0x20; /* code B */
		/* can't happen */
		return -1;
	}

	static int* Barcode_128_make_array(struct Barcode_Item* bc, int* lptr)
	{
		int len, * codes;
		char* s;
		int i = 0, j, code, checksum;

		/* allocate twice the text length + 5, as this is the worst case */
		len = 2 * strlen(bc->ascii) + 5;
		codes = (int*) malloc(len * sizeof(int));
		s = bc->ascii;

		if (bc->encoding) {
			if (!strcmp(bc->encoding, "code 128-A")) {
				code = 'A';
			} else if (!strcmp(bc->encoding, "code 128-B")) {
				code = 'B';
			} else if (!strcmp(bc->encoding, "code 128-C")) {
				code = 'C';
			} else {
				code = Barcode_a_or_b(s);
				if (!code) code = 'B'; /* default */
			}
		} else {
			code = Barcode_a_or_b(s);
			if (!code) code = 'B'; /* default */
		}

		/* choose the starting code */
		// if (s[2] == '\0' && isdigit(s[0]) && isdigit(s[1])) {
		// 	code = 'C';
		// } else if (isdigit(s[0]) && isdigit(s[1]) && isdigit(s[2]) && isdigit(s[3])) {
		// 	code = 'C';
		// } else {
		// 	code = Barcode_a_or_b(s);
		// 	if (!code) code = 'B'; /* default */
		// }
		codes[i++] = START_A + code - 'A';

		for (s = bc->ascii; *s; /* increments are in the loop */) {
			switch (code) {

				case 'C':
					if (s[0] == 0xC1) { /* F1 is valid */
						codes[i++] = FUNC_1;
						s++;
					} else if (isdigit(s[0]) && isdigit(s[1])) {
						/* encode two digits */
						codes[i++] = (s[0] - '0') * 10 + s[1] - '0';
						s += 2;
					} else {
						/* change code */
						code = Barcode_a_or_b(s);
						if (!code) code = 'B';
						codes[i++] = (code == 'A') ? CODE_A : CODE_B;
					}
					break;

				case 'B':
				case 'A':
					// NO
					// for (j = 0; isdigit(s[j]); j++)
					// 	;
					// if (j >= 4) { /* if there are 4 or more digits, turn to C */
					// 	if (j & 1) {
					// 		/* odd number: encode one first */
					// 		codes[i++] = *(s++) - ' ';
					// 	}
					// 	codes[i++] = CODE_C;
					// 	code = 'C';
					// } else 
					if (code == 'A' && NEED_CODE_B(*s)) {
						/* check whether we should use SHIFT or change code */
						j = Barcode_a_or_b(s + 1);
						if (j == 'B') {
							codes[i++] = CODE_B;
							code = 'B';
						} else {
							codes[i++] = SHIFT;
							codes[i++] = Barcode_encode_as('B', *s);
							s++;
						}
					} else if (code == 'B' && NEED_CODE_A(*s)) {
						/* check whether we should use SHIFT or change code */
						j = Barcode_a_or_b(s + 1);
						if (j == 'A') {
							codes[i++] = CODE_A;
							code = 'A';
						} else {
							codes[i++] = SHIFT;
							codes[i++] = Barcode_encode_as('A', *s);
							s++;
						}
					} else {
						codes[i++] = Barcode_encode_as(code, *s);
						s++;
					}
					break;
			}
		}
		/* add the checksum */
		checksum = codes[0];
		for (j = 1; j < i; j++)
			checksum += j * codes[j];
		checksum %= 103;
		codes[i++] = checksum;
		codes[i++] = STOP;
		if (i > len) {
			fprintf(stderr, "impossible: length is > allocated len\n");
		}
		*lptr = i;
		return codes;
	}

	/*
	 * Encoding is tricky, in that we cannot print the ascii
	 * representation in an easy way. Therefore, just skip non-printable
	 * chars and try to fit the rest somehow.
	 */
	int Barcode_128_encode(struct Barcode_Item* bc)
	{
		static char* text;
		static char* partial;  /* dynamic */
		static char* textinfo; /* dynamic */
		char* textptr;
		int* codes; /* dynamic */
		int i, c, len;
		double textpos, size, step;

		if (bc->partial)
			free(bc->partial);
		if (bc->textinfo)
			free(bc->textinfo);
		bc->partial = bc->textinfo = NULL; /* safe */

		if (!bc->encoding)
			bc->encoding = strdup("code 128");

		text = bc->ascii;
		if (!text) {
			bc->error = EINVAL;
			return -1;
		}

		codes = Barcode_128_make_array(bc, &len);
		if (!codes) return -1;

		/* the partial code is 6*codelen + ini + term (+margin) */
		partial = (char*) malloc(6 * len + 4);
		if (!partial) {
			bc->error = errno;
			free(codes);
			return -1;
		}

		/* the text information is at most "nnn.n:ff.f:c " * strlen +term */
		textptr = textinfo = (char*) malloc(13 * strlen(text) + 2);
		if (!textinfo) {
			bc->error = errno;
			free(partial);
			free(codes);
			return -1;
		}

		/* up to now, it was almost the same code as other encodings */

		strcpy(partial, "0"); /* the first space */
		for (i = 0; i < len; i++) /* the symbols */
			strcat(partial, codeset[codes[i]]);

		/* avoid bars that fall lower than other bars */
		for (i = 0; partial[i]; i++)
			if (isalpha(partial[i]))
				partial[i] += '1' - 'a';

		/*
		 * the accompanying text: reserve a space for every char.
		 * F[1-4] are rendered as spaces (separators), other unprintable chars
		 * are rendered as underscoress (placeholders)
		 */

		 /*
		  * A size of 9 is good for code-C (used above), so 18 for each symbol,
		  * but we place an upper limit of 12 to avoid overlapping on the bars.
		  */
		size = (int) (180.0 * (len - 3) / strlen(text) + .5) / 10.0;
		if (size > 12.0) size = 12.0;

		/* align the text to the left, using its natural size */
		step = (int) (10 * (size / 18.0 * SYMBOL_WID) + .5) / 10.0;
		textpos = SYMBOL_WID;

		for (i = 0; i < strlen(text); i++) {
			c = text[i];
			if (c < 32 || c == 0x80) c = '_'; /* not printable */
			if (c > 0xc0) c = ' '; /* F code */
			sprintf(textptr, "%g:%g:%c ", textpos, size, c);
			textptr += strlen(textptr);
			textpos += step;
		}

		bc->partial = partial;
		bc->textinfo = textinfo;
		free(codes);
		return 0;
	}

	/*
	 * A raw 128 code is given as a sequence of space separated numbers
	 * from 0 to 105, starting from the start code to be (Leonid)
	 */

	int Barcode_128raw_verify(char* text)
	{
		int n;
		unsigned val;

		if (!strlen(text))
			return -1;
		while (*text) {
			if (sscanf(text, "%u%n", &val, &n) < 1)
				return -1;
			if (val > 105)
				return -1;
			text += n;
		}
		return 0;
	}

	int Barcode_128raw_encode(struct Barcode_Item* bc)
	{
		static char* text;
		static char* partial;  /* dynamic */
		static char* textinfo; /* dynamic */
		char* textptr;
		int i, n, count, code, textpos, checksum = 0;

		if (bc->partial)
			free(bc->partial);
		if (bc->textinfo)
			free(bc->textinfo);
		bc->partial = bc->textinfo = NULL; /* safe */

		if (!bc->encoding)
			bc->encoding = strdup("128raw");

		text = bc->ascii;
		if (!text) {
			bc->error = EINVAL;
			return -1;
		}
		/*
		 * length of partial code is unknown in advance, but it is
		 * at most  6* (1+text/2 + check + tail) + final + terminator
		 */
		partial = (char*) malloc((3 + strlen(text) / 2) * 6 + 2);
		if (!partial) {
			bc->error = errno;
			return -1;
		}

		/* the text information is at most "nnn.5:fff:c " * 1+strlen/2 +term */
		textinfo = (char*) malloc(12 * (1 + strlen(text) / 2) + 2);
		if (!textinfo) {
			bc->error = errno;
			free(partial);
			return -1;
		}

		strcpy(partial, "0"); /* the first space */
		textptr = textinfo;
		textpos = 0;

		for (i = 0, count = 0; i < strlen(text); count++) {
			if (sscanf(text + i, "%u%n", &code, &n) < 1) {
				bc->error = EINVAL; /* impossible if text is verified */
				free(partial);
				free(textinfo);
				return -1;
			}
			strcat(partial, codeset[code]);

			/*
			 * since the start code is part of the "raw" input, it is
			 * counted in the checksum by itself
			 */
			if (!count) checksum += code; /* the start code */
			else        checksum += code * count; /* first * 1 + second * 2 ... */

			/*
			 * print as "%s", because we have ".5" positions
			 * also, use a size of 9 like codeC above, as each symbol is
			 * represented by two chars
			 */
			sprintf(textptr, "%g:9:%c %g:9:%c ", (double) textpos,
				code >= 100 ? 'A' : code / 10 + '0',
				textpos + (double) SYMBOL_WID / 2, code % 10 + '0');
			textptr += strlen(textptr);
			textpos += SYMBOL_WID; /* width of each code */
			i += n;
		}
		/* Add the checksum, independent of BARCODE_NO_CHECKSUM */
		checksum %= 103;
		strcat(partial, codeset[checksum]);
		/* and the end marker */
		strcat(partial, codeset[STOP]);

		bc->partial = partial;
		bc->textinfo = textinfo;

		return 0;
	}
	struct Barcode_Item bc;
}


namespace glbarcode {

	/* Static Code128 barcode creation method */
	Barcode* BarcodeCode128::create(void) { return new BarcodeCode128(); }

	/* Code128 data validation, implements Barcode1dBase::validate() */
	bool BarcodeCode128::validate(const std::string& rawData) {
		unsigned char* text = (unsigned char*) rawData.c_str();
		int result = BC_C128::Barcode_128_verify(text);
		if (result == -1) return false;
		return true;
	}

	/* Code128 data encoding, implements Barcode1dBase::encode() */
	std::string BarcodeCode128::encode(const std::string& cookedData) {
		std::string code;
		std::string encoding = "code 128-B";
		BC_C128::bc.encoding = (char*) encoding.c_str();
		BC_C128::bc.ascii = (char*) cookedData.c_str();
		BC_C128::Barcode_128_encode(&BC_C128::bc);
		code = BC_C128::bc.partial;
		return code;
	}

	/* Code128 prepare text for display, implements Barcode1dBase::prepareText() */
	std::string BarcodeCode128::prepareText(const std::string& rawData) {
		std::string displayText;
		// for (unsigned int i = 0; i < rawData.size(); i++) displayText += rawData[i];
		displayText = BC_C128::bc.textinfo;
		return displayText;
	}

	/* Code128 vectorization, implements Barcode1dBase::vectorize() */
	void BarcodeCode128::vectorize(const std::string& codedData, const std::string& displayText, const std::string& cookedData, double& w, double& h) {
		// Use addLine(x, y, w, h) to draw the barcode

		int length = 0;
		if (BC_C128::bc.error == 0) {
			length = strlen(BC_C128::bc.partial);
		}
		std::string data = BC_C128::bc.partial;

		// printf("Vectorizing Code128 with length %d\n", length);
		// for (int i = 0; i < length; i++) {
		// 	printf("%d ", data[i]);
		// }
		// printf("\n");

		double xQuiet = 10;
		double textSize = 8;
		double hTextArea = 10;
		double width = w;
		double height = h;

		double x = 0;
		double y = 0;

		for (int i = 0; i < length; i++) {
			int bar_width = data[i] - 48;
			bool odd = i % 2;
			if (odd) addLine(x, y, bar_width, height);
			x += bar_width;
		}

		if (showText()) {
			// The textinfo
			// X:Y:C X:Y:C X:Y:C...
			// Parse x, y and c with : as delimiter and space as separator between each character
			std::string text = displayText;
			std::string delimiter = ":";
			std::string space = " ";
			size_t pos = 0;
			std::string token;
			double x = 0;
			double y = 0;
			std::string c;
			
			// "%f:%f:%c" -> x, y, c
			while ((pos = text.find(delimiter)) != std::string::npos) {
				token = text.substr(0, pos);
				x = std::stod(token);
				text.erase(0, pos + delimiter.length());
				pos = text.find(delimiter);
				token = text.substr(0, pos);
				y = std::stod(token);
				text.erase(0, pos + delimiter.length());
				pos = text.find(space);
				token = text.substr(0, pos);
				c = token;
				text.erase(0, pos + space.length());
				addText(x + 21, y + height + 6, textSize, c);
			}
		}
		w = width + 2 * xQuiet;
		h = showText() ? height + hTextArea : height;
	}

}