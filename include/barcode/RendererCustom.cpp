/*  RendererCustom.cpp
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

#include "RendererCustom.h"

#include "glbarcode/Version.h"
#include "glbarcode/Renderer.h"


using namespace glbarcode::Version;

namespace glbarcode {

    RendererCustom::RendererCustom() { d = {}; }
    RendererCustom::~RendererCustom() { if (d.destroy_f) d.destroy_f(); }

    void RendererCustom::drawBegin(double w, double h) {
        if (d.drawBegin_f) d.drawBegin_f(w, h);
    }


    void RendererCustom::drawEnd(void) {
        if (d.drawEnd_f) d.drawEnd_f();
    }


    void RendererCustom::drawLine(double x, double y, double w, double h) {
        if (d.drawLine_f) d.drawLine_f(x, y, w, h);
    }


    void RendererCustom::drawBox(double x, double y, double w, double h) {
        if (d.drawBox_f) d.drawBox_f(x, y, w, h);
    }


    void RendererCustom::drawText(double x, double y, double size, const std::string& text) {
        if (d.drawText_f) d.drawText_f(x, y, size, text.c_str());
    }


    void RendererCustom::drawRing(double x, double y, double r, double w) {
        if (d.drawRing_f) d.drawRing_f(x, y, r, w);
    }


    void RendererCustom::drawHexagon(double x, double y, double h) {
        if (d.drawHexagon_f) d.drawHexagon_f(x, y, h);
    }

}
