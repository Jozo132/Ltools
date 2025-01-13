/*  BarcodeCode128.h
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


#include "glbarcode/Barcode1dBase.h"


namespace glbarcode {

	/**
	 * @class BarcodeCode128 BarcodeCode128.h glbarcode/BarcodeCode128.h
	 *
	 * *Code 128* 1D barcode symbology.
	 */
	class BarcodeCode128 : public Barcode1dBase {
	public:
		/**
		 * Static Code128 barcode creation method
		 *
		 * Used by glbarcode::BarcodeFactory
		 */
		static Barcode* create(void);

	private:
		bool validate(const std::string& rawData);
		std::string encode(const std::string& cookedData);
		std::string prepareText(const std::string& rawData);
		void vectorize(const std::string& codedData, const std::string& displayText, const std::string& cookedData, double& w, double& h);
	};

}


#ifdef WEIRD_CPP_IMPORT
#include "BarcodeCode128.cpp"
#endif // WEIRD_CPP_IMPORT
