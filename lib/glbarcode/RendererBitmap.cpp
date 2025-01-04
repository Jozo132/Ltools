/*  RendererBitmap.cpp
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

#include "RendererBitmap.h"

#include "Version.h"
#include "Renderer.h"

#include "imagex.h"
#include "draw_utils.h"

typedef void callback_t(void);

using namespace glbarcode::Version;

namespace glbarcode
{

    struct RendererBitmap::PrivateData { Image* image; int x; int y; };

    RendererBitmap::RendererBitmap(Image* image, int x, int y) {
        d = new RendererBitmap::PrivateData;
        d->image = image;
        d->x = x;
        d->y = y;
    }

    RendererBitmap::RendererBitmap(const RendererBitmap& from) {
        d = new RendererBitmap::PrivateData;
        *d = *from.d;
    }


    RendererBitmap::~RendererBitmap() {
        delete d;
    }


    RendererBitmap& RendererBitmap::operator=(const RendererBitmap& from) {
        *d = *from.d;
        return *this;
    }



    void RendererBitmap::drawBegin(double w, double h) {
        // if ( d->filename.empty() || ( d->filename == "-" ) ) d->fp = stdout;
        // else d->fp = fopen( d->filename.c_str(), "w" );
        // fprintf( d->fp, "<?xml version=\"1.0\" standalone=\"no\"?>\n" );
        // fprintf( d->fp, "<!-- Created with %s version %s (%s) -->\n",
        // 	 PACKAGE_NAME.c_str(), PACKAGE_VERSION.c_str(), PACKAGE_URL.c_str() );
        // fprintf( d->fp, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"%f\" height=\"%f\" >\n",
        // 	 w, h );
        if (!d->image) return;
        if (d->image->width <= 0 || d->image->height <= 0) return;
        if (w != d->image->width || h != d->image->height) {
            Image image = GenImageColor(w, h, WHITE);
            d->image = &image;
        }
    }


    void RendererBitmap::drawEnd(void) {
        // fprintf( d->fp, "</svg>\n" );
        // if ( !d->filename.empty() || ( d->filename != "-" ) )
        // {
        // 	fclose( d->fp );
        // }
    }


    void RendererBitmap::drawLine(double x, double y, double w, double h) {
        // double x1 = x + w / 2; /* Offset line origin by 1/2 line width. */
        // fprintf( d->fp, "  <line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" stroke-width=\"%f\" style=\"stroke:rgb(0,0,0)\" />\n",
        // 	 x1, y, x1, y+h, w );
        float ix = x + d->x;
        float iy = y + d->y;
        ImageDrawRectangle(d->image, ix, iy, w, h, BLACK);
    }


    void RendererBitmap::drawBox(double x, double y, double w, double h) {
        // fprintf( d->fp, "  <rect x=\"%f\" y=\"%f\" width=\"%f\" height=\"%f\" shape-rendering=\"crispEdges\" style=\"fill:rgb(0,0,0)\" />\n",
        // 	x, y, w, h );
        float ix = x + d->x;
        float iy = y + d->y;
        ImageDrawRectangle(d->image, ix, iy, w, h, BLACK);
    }


    void RendererBitmap::drawText(double x, double y, double size, const std::string& text) {
        // fprintf( d->fp, "  <text x=\"%f\" y=\"%f\" font-size=\"%f\" style=\"font-family:monospace;text-anchor:middle;fill:rgb(0,0,0)\" >%s</text>\n",
        // 	 x, y, size, text.c_str() );
        Font* font_ptr = fontx.getFont("Helvetica", size);
        if (font_ptr == nullptr) return; // Font not found
        Font& font = font_ptr[0];
        float ix = x + d->x;
        float iy = y + d->y;
        ImageDrawTextEx(d->image, font, text.c_str(), (Vector2) { ix, iy }, (float) size, 0, BLACK);
    }


    void RendererBitmap::drawRing(double x, double y, double r, double w) {
        // fprintf( d->fp, "  <circle cx=\"%f\" cy=\"%f\" r=\"%f\" stroke-width=\"%f\" style=\"stroke:rgb(0,0,0)\" />\n",
        // 	 x, y, r, w );
        float ix = x + d->x;
        float iy = y + d->y;
        ImageDrawCircle(d->image, ix,  iy, (float) r, BLACK);
    }


    void RendererBitmap::drawHexagon(double x, double y, double h) {
        // fprintf( d->fp, "  <polygon points=\"%f,%f %f,%f %f,%f %f,%f %f,%f %f,%f\" style=\"fill:rgb(0,0,0)\" />\n",
        // 	 x,           y,
        // 	 x + 0.433*h, y + 0.25*h,
        // 	 x + 0.433*h, y + 0.75*h,
        // 	 x,           y +      h,
        // 	 x - 0.433*h, y + 0.75*h,
        // 	 x - 0.433*h, y + 0.25*h );
        float ix = x + d->x;
        float iy = y + d->y;
        ImageDrawNGon(d->image, (Vector2) { ix, iy }, h, 6, BLACK);
    }

}
