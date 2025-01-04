/*  RendererCustom.h
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

namespace glbarcode {
	class RendererCustom : public Renderer {
	public:
		typedef void callback_t(void);
		typedef void callback_draw_t(double w, double h);
		typedef void callback_draw_line_t(double x, double y, double w, double h);
		typedef void callback_draw_box_t(double x, double y, double w, double h);
		typedef void callback_draw_text_t(double x, double y, double size, const char* text);
		typedef void callback_draw_ring_t(double x, double y, double r, double w);
		typedef void callback_draw_hexagon_t(double x, double y, double h);
	private:
		struct PrivateData {
			callback_t* destroy_f;
			callback_draw_t* drawBegin_f;
			callback_t* drawEnd_f;
			callback_draw_line_t* drawLine_f;
			callback_draw_box_t* drawBox_f;
			callback_draw_text_t* drawText_f;
			callback_draw_ring_t* drawRing_f;
			callback_draw_hexagon_t* drawHexagon_f;
		} d;
	public:


		RendererCustom();

		/** Destructor */
		virtual ~RendererCustom();

	private:
		/* Virtual methods implemented by the renderer. */
		void drawBegin(double w, double h);
		void drawEnd(void);
		void drawLine(double x, double y, double w, double h);
		void drawBox(double x, double y, double w, double h);
		void drawText(double x, double y, double size, const std::string& text);
		void drawRing(double x, double y, double r, double w);
		void drawHexagon(double x, double y, double h);

	public:
		void setDestroyFunction(callback_t* f) { d.destroy_f = f; }
		void setDrawBeginFunction(callback_draw_t* f) { d.drawBegin_f = f; }
		void setDrawEndFunction(callback_t* f) { d.drawEnd_f = f; }
		void setDrawLineFunction(callback_draw_line_t* f) { d.drawLine_f = f; }
		void setDrawBoxFunction(callback_draw_box_t* f) { d.drawBox_f = f; }
		void setDrawTextFunction(callback_draw_text_t* f) { d.drawText_f = f; }
		void setDrawRingFunction(callback_draw_ring_t* f) { d.drawRing_f = f; }
		void setDrawHexagonFunction(callback_draw_hexagon_t* f) { d.drawHexagon_f = f; }
	}; // class RendererCustom

}; // namespace glbarcode

#ifdef WEIRD_CPP_IMPORT
#include "RendererCustom.cpp"
#endif // WEIRD_CPP_IMPORT
