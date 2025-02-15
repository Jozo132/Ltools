/*  BarcodeOnecode.h
 *
 *  Copyright (C) 2013  Jim Evins <evins@snaught.com>
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


#include "Barcode1dBase.h"

#include <stdint.h>


namespace glbarcode
{

	/**
	 * @class BarcodeOnecode BarcodeOnecode.h glbarcode/BarcodeOnecode.h
	 *
	 * Onecode barcode, implements Barcode1dBase
	 *
	 * @image html sample-onecode.svg "Sample USPS Onecode Barcode"
	 *
	 */
	class BarcodeOnecode : public Barcode1dBase
	{
	public:
		/**
		 * Static Onecode barcode creation method
		 *
		 * Used by glbarcode::BarcodeFactory
		 */
		static Barcode* create( void );


	private:
		bool validate( const std::string& rawData );

		std::string encode( const std::string& cookedData );

		void vectorize( const std::string& codedData,
				const std::string& displayText,
				const std::string& cookedData,
				double&            w,
				double&            h );


	private:
		uint32_t USPS_MSB_Math_CRC11GenerateFrameCheckSequence( const uint8_t* ByteArrayPtr );

	};

}


#ifdef WEIRD_CPP_IMPORT
#include "BarcodeOnecode.cpp"
#endif // WEIRD_CPP_IMPORT