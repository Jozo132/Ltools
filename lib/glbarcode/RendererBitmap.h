/*  RendererBitmap.h
 *
 *  Copyright (C) 2025  J.Vovk <jozo132@gmail.com>
 *
 *  This file is part of zpl-compiler for glbarcode++.
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

#include "Renderer.h"

#include "imagex.h"

namespace glbarcode {
class RendererBitmap : public Renderer {
private:
	struct PrivateData;
	struct PrivateData* d;
public:

	RendererBitmap(Image* image, int x, int y);

	/** Copy Constructor */
	RendererBitmap(const RendererBitmap& from);

	/** Destructor */
	virtual ~RendererBitmap();


	/** Assignment operator.
	 * @param[in] from The value to assign to this object.
	 * @return A reference to this object.
	*/
	RendererBitmap& operator=(const RendererBitmap& from);

private:
	/* Virtual methods implemented by the renderer. */
	void drawBegin(double w, double h);
	void drawEnd(void);
	void drawLine(double x, double y, double w, double h);
	void drawBox(double x, double y, double w, double h);
	void drawText(double x, double y, double size, const std::string& text);
	void drawRing(double x, double y, double r, double w);
	void drawHexagon(double x, double y, double h);
}; // class RendererBitmap

}; // namespace glbarcode

#ifdef WEIRD_CPP_IMPORT
#include "RendererBitmap.cpp"
#endif // WEIRD_CPP_IMPORT

