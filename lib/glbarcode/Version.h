/*  Version.h
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

#ifndef glbarcode_Version_h
#define glbarcode_Version_h


namespace glbarcode
{

	namespace Version
	{
		const std::string PACKAGE_NAME    = "glbarcode++";
		const std::string SHORT_NAME      = "gbc";
		const std::string PACKAGE_URL     = "https://github.com/jimevins/glbarcode";

		const std::string PACKAGE_VERSION = "0.0.0";

		const int         API_VERSION     = 0;
		const int         FEATURE_VERSION = 0;
		const int         BUGFIX_VERSION  = 0;
	}

}


#endif // glbarcode_Version_h
