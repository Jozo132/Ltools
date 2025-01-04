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

#include "BarcodeCode128.h"

#include "Constants.h"
#include <stdint.h>

#include <cctype>
#include <algorithm>


using namespace glbarcode;
using namespace Constants;


namespace BC_C128 {

	/* Vectorization constants */
	const double MIN_X = (0.01 * PTS_PER_INCH);
	const double N = 2.5;
	const double MIN_I = MIN_X;
	const double MIN_HEIGHT = (0.25 * PTS_PER_INCH);
	const double MIN_QUIET = (0.10 * PTS_PER_INCH);

	const double MIN_TEXT_AREA_HEIGHT = 14.0;
	const double MIN_TEXT_SIZE = 10.0;

	/* Code 128 character set A */
	const std::string setA = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_";

	/* Code 128 character set B */
	const std::string setB = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

	/* Code 128 character set C */
	// Sum each pair of digits to encode as a single character

	/* Code 128 start character */
	const char start = 0x7B;

	/* Code 128 stop character */
	const char stop = 0x7B;

	/* Code 128 shift character */
	const char shift = 0x7C;

	/* Code 128 checksum character */
	const char checksum = 0x7D;

	/* Code 128 symbols. Position must match position in values array */
	const uint16_t symbols [] = {
		/* 00 */  0b11011001100,
		/* 01 */  0b11001101100,
		/* 02 */  0b11001100110,
		/* 03 */  0b10010011000,
		/* 04 */  0b10010001100,
		/* 05 */  0b10001001100,
		/* 06 */  0b10011001000,
		/* 07 */  0b10011000100,
		/* 08 */  0b10001100100,
		/* 09 */  0b11001001000,
		/* 0A */  0b11001000100,
		/* 0B */  0b11000100100,
		/* 0C */  0b10110011100,
		/* 0D */  0b10011011100,
		/* 0E */  0b10011001110,
		/* 0F */  0b10111001100,
		/* 10 */  0b10011101100,
		/* 11 */  0b10011100110,
		/* 12 */  0b11001110010,
		/* 13 */  0b11001011100,
		/* 14 */  0b11001001110,
		/* 15 */  0b11011100100,
		/* 16 */  0b11001110100,
		/* 17 */  0b11101101110,
		/* 18 */  0b11101001100,
		/* 19 */  0b11100101100,
		/* 1A */  0b11100100110,
		/* 1B */  0b11101100100,
		/* 1C */  0b11100110100,
		/* 1D */  0b11100110010,
		/* 1E */  0b11011011000,
		/* 1F */  0b11011000110,
		/* 20 */  0b11000110110,
		/* 21 */  0b10100011000,
		/* 22 */  0b10001011000,
		/* 23 */  0b10001000110,
		/* 24 */  0b10110001000,
		/* 25 */  0b10001101000,
		/* 26 */  0b10001100010,
		/* 27 */  0b11010001000,
		/* 28 */  0b11000101000,
		/* 29 */  0b11000100010,
		/* 2A */  0b10110111000,
		/* 2B */  0b10110001110,
		/* 2C */  0b10001101110,
		/* 2D */  0b10111011000,
		/* 2E */  0b10111000110,
		/* 2F */  0b10001110110,
		/* 30 */  0b11101110110,
		/* 31 */  0b11010001110,
		/* 32 */  0b11000101110,
		/* 33 */  0b11011101000,
		/* 34 */  0b11011100010,
		/* 35 */  0b11011101110,
		/* 36 */  0b11101011000,
		/* 37 */  0b11101000110,
		/* 38 */  0b11100010110,
		/* 39 */  0b11101101000,
		/* 3A */  0b11101100010,
		/* 3B */  0b11100011010,
		/* 3C */  0b11101111010,
		/* 3D */  0b11001000010,
		/* 3E */  0b11110001010,
		/* 3F */  0b10100110000,
		/* 40 */  0b10100001100,
		/* 41 */  0b10010110000,
		/* 42 */  0b10010000110,
		/* 43 */  0b10000101100,
		/* 44 */  0b10000100110,
		/* 45 */  0b10110010000,
		/* 46 */  0b10110000100,
		/* 47 */  0b10011010000,
		/* 48 */  0b10011000010,
		/* 49 */  0b10000110100,
		/* 4A */  0b10000110010,
		/* 4B */  0b11000010010,
		/* 4C */  0b11001010000,
		/* 4D */  0b11110111010,
		/* 4E */  0b11000010100,
		/* 4F */  0b10001111010,
		/* 50 */  0b10100111100,
		/* 51 */  0b10010111100,
		/* 52 */  0b10010011110,
		/* 53 */  0b10111100100,
		/* 54 */  0b10011110100,
		/* 55 */  0b10011110010,
		/* 56 */  0b11110100100,
		/* 57 */  0b11110010100,
		/* 58 */  0b11110010010,
		/* 59 */  0b11011011110,
		/* 5A */  0b11011110110,
		/* 5B */  0b11110110110,
		/* 5C */  0b10101111000,
		/* 5D */  0b10100011110,
		/* 5E */  0b10001011110,
		/* 5F */  0b10111101000,
		/* 60 */  0b10111100010,
		/* 61 */  0b11110101000,
		/* 62 */  0b11110100010,
		/* 63 */  0b10111011110,
		/* 64 */  0b10111101110,
		/* 65 */  0b11101011110,
		/* 66 */  0b11110101110,
		/* 67 */  0b11010000100,
		/* 68 */  0b11010010000,
		/* 69 */  0b11010011100,
		/* 6A */  0b11000111010,
	};
	int widths_value[6] = { 0, 0, 0, 0, 0, 0 };

	int* getWidths(uint16_t symbol) {
		// Form right to left
		// Example: 0b11011001100 => [2,1,1,2,2,2]
		int i = 0;
		int cnt = 6;
		int state = 0;
		while (symbol > 0) {
			int bit = symbol & 1;
			if (i == 0) {
				state = bit;
				i++;
				symbol >>= 1;
				continue;
			}
			if (bit != state) {
				cnt--;
				if (cnt < 0) break;
				widths_value[cnt] = i;
				state = bit;
				i = 1;
			} else {
				i++;
			}
			symbol >>= 1;
			i++;
		}
		return widths_value;
	}

	/*
		128A (Code Set A) – ASCII characters 00 to 95 (0–9, A–Z and control codes), special characters, and FNC 1–4
		128B (Code Set B) – ASCII characters 32 to 127 (0–9, A–Z, a–z), special characters, and FNC 1–4
		128C (Code Set C) – 00–99 (encodes two digits with a single code point) and FNC1
	*/

	/* 1 = A, 2 = B, 3 = C */
	int detectOptimatSet(std::string data) {
		// First check if data is numeric
		bool numeric = true;
		for (unsigned int i = 0; i < data.size(); i++) {
			if (!std::isdigit(data[i])) {
				numeric = false;
				break;
			}
		}
		if (numeric) return 3;
		// Check if data is in set A
		bool is_setA = true;
		for (unsigned int i = 0; i < data.size(); i++) {
			if (setA.find(data[i]) == std::string::npos) {
				is_setA = false;
				break;
			}
		}
		if (is_setA) return 1;
		// Check if data is in set B
		bool is_setB = true;
		for (unsigned int i = 0; i < data.size(); i++) {
			if (setB.find(data[i]) == std::string::npos) {
				is_setB = false;
				break;
			}
		}
		if (is_setB) return 2;
		return 0; // Unknown - invalid data
	}
}


namespace glbarcode {

	/* Static Code128 barcode creation method */
	Barcode* BarcodeCode128::create(void) { return new BarcodeCode128(); }

	/* Code128 data validation, implements Barcode1dBase::validate() */
	bool BarcodeCode128::validate(const std::string& rawData) {
		for (unsigned int i = 0; i < rawData.size(); i++) {
			char c = rawData[i];
			if (BC_C128::setA.find(c) == std::string::npos &&
				BC_C128::setB.find(c) == std::string::npos &&
				!std::isdigit(c)) return false;
		}
		return true;
	}

	/* Code128 data encoding, implements Barcode1dBase::encode() */
	std::string BarcodeCode128::encode(const std::string& cookedData) {
		std::string code;
		/* Start character */
		code += BC_C128::start;
		int weighted_mod103 = 0;
		int weighted_mod103_count = 1;
		for (unsigned int i = 0; i < cookedData.size(); i++) {
			char c = cookedData[i];
			int set = BC_C128::detectOptimatSet(cookedData);
			if (set == 1) {
				if (weighted_mod103_count == 1) {
					weighted_mod103 = 103;
					weighted_mod103_count = 2;
				} else {
					weighted_mod103 += (i + 1) * 103;
					weighted_mod103_count++;
				}
			} else if (set == 2) {
				if (weighted_mod103_count == 1) {
					weighted_mod103 = 104;
					weighted_mod103_count = 2;
				} else {
					weighted_mod103 += (i + 1) * 104;
					weighted_mod103_count++;
				}
			} else {
				if (weighted_mod103_count == 1) {
					weighted_mod103 = 105;
					weighted_mod103_count = 2;
				} else {
					weighted_mod103 += (i + 1) * 105;
					weighted_mod103_count++;
				}
			}
			code += c;
		}
		/* Checksum */
		int checksum = weighted_mod103 % 103;
		code += BC_C128::checksum;
		code += BC_C128::symbols[checksum];
		/* Stop character */
		code += BC_C128::stop;
		return code;
	}

	/* Code128 prepare text for display, implements Barcode1dBase::prepareText() */
	std::string BarcodeCode128::prepareText(const std::string& rawData) {
		std::string displayText;
		for (unsigned int i = 0; i < rawData.size(); i++) displayText += rawData[i];
		return displayText;
	}

	/* Code128 vectorization, implements Barcode1dBase::vectorize() */
	void BarcodeCode128::vectorize(const std::string& codedData, const std::string& displayText, const std::string& cookedData, double& w, double& h) {
		/* determine width and establish horizontal scale, based on original cooked data */
		double dataSize = cookedData.size();
		double minL = (dataSize + 2) * (3 * BC_C128::N + 6) * BC_C128::MIN_X + (dataSize + 1) * BC_C128::MIN_I;
		double scale;
		if (w == 0) scale = 1.0;
		else {
			scale = w / (minL + 2 * BC_C128::MIN_QUIET);
			if (scale < 1.0) scale = 1.0;
		}
		double width = minL * scale;
		double hTextArea = BC_C128::MIN_TEXT_AREA_HEIGHT;
		double textSize = BC_C128::MIN_TEXT_SIZE;
		double height = showText() ? h - hTextArea : h;
		height = std::max(height, BC_C128::MIN_HEIGHT);
		double xQuiet = std::max((10 * scale * BC_C128::MIN_X), BC_C128::MIN_QUIET);
		double x1 = xQuiet;
		for (unsigned int i = 0; i < codedData.size(); i++) {
			uint16_t symbol = codedData[i];
			int* widths = BC_C128::getWidths(symbol);
			for (int j = 0; j < 6; j++) {
				double width = widths[j] * BC_C128::MIN_X * scale;
				if (width > 0) {
					if (j % 2 == 0) addLine(x1, 0, width, height);
					x1 += width;
				}
			}
		}
		if (showText()) {
			double x = xQuiet + width / 2;
			double y = h - hTextArea / 2;
			addText(x, y, textSize, displayText);
		}
		w = width + 2 * xQuiet;
		h = showText() ? height + hTextArea : height;
	}





}