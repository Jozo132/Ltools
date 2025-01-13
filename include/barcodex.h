
#pragma once

#define WEIRD_CPP_IMPORT

#include "imagex.h"
#include "draw_utils.h"
#include "../lib/glbarcode/Factory.h"
#include "../lib/glbarcode/RendererCustom.h"

using namespace glbarcode;

struct RenderStateTemp {
    Image* image = nullptr;
    float x = 0;
    float y = 0;
    float scale_x = 0;
    float scale_y = 0;
    bool inverted = false;
} rst;

// #define DEBUG_DRAWING

void barcode_drawBegin(double w, double h) {
#ifdef DEBUG_DRAWING
    printf("Drawing barcode starting with size %f, %f\n", w, h);
#endif
}

void barcode_drawEnd() {
#ifdef DEBUG_DRAWING
    printf("Drawing barcode end\n");
#endif
    // Nothing to do
}

void barcode_drawLine(double x, double y, double w, double h) {
    float ix = x * rst.scale_x + rst.x;
    float iy = y * rst.scale_y + rst.y;
    float iw = w * rst.scale_x;
    float ih = h * rst.scale_y;
#ifdef DEBUG_DRAWING
    printf("Drawing line at %f, %f with size %f, %f\n", ix, iy, iw, ih);
#endif
    rst.image->drawRect(ix, iy, iw, ih, 0, BLANK, BLACK, rst.inverted);
}

void barcode_drawBox(double x, double y, double w, double h) {
    float ix = x * rst.scale_x + rst.x;
    float iy = y * rst.scale_y + rst.y;
    float iw = w * rst.scale_x;
    float ih = h * rst.scale_y;
#ifdef DEBUG_DRAWING
    printf("Drawing box at %f, %f with size %f, %f\n", ix, iy, iw, ih);
#endif
    rst.image->drawRect(ix, iy, iw, ih, 0, BLANK, BLACK, rst.inverted);
}

void barcode_drawText(double x, double y, double size, const char* text) {
    float ix = x * rst.scale_x + rst.x;
    float iy = y * rst.scale_y + rst.y;
    int is = size * rst.scale_x;
// #ifdef DEBUG_DRAWING
//     printf("Drawing text at %f, %f with size %f and text %s\n", ix, iy, is, text);
// #endif
    rst.image->drawText(ix, iy, is, text, "OCR-B", BLACK, rst.inverted);
    // rst.image->drawText(ix, iy, is, text, "Helvetica", BLACK, rst.inverted);
}

void barcode_drawRing(double x, double y, double r, double w) {
    float ix = x * rst.scale_x + rst.x;
    float iy = y * rst.scale_y + rst.y;
    float ir = r * rst.scale_x;
#ifdef DEBUG_DRAWING
    printf("Drawing ring at %f, %f with radius %f and width %f\n", ix, iy, ir, w);
#endif
    // ImageDrawCircle(rst.image, ix, iy, ir, BLACK);
    rst.image->drawCircle(ix, iy, ir, 0, BLANK, BLACK, rst.inverted);
}

void barcode_drawHexagon(double x, double y, double h) {
    float ix = x * rst.scale_x + rst.x;
    float iy = y * rst.scale_y + rst.y;
    float ih = h * rst.scale_x;
#ifdef DEBUG_DRAWING
    printf("Drawing hexagon at %f, %f with height %f\n", ix, iy, ih);
#endif
    ImageDrawNGon(rst.image, (Vector2) { ix, iy }, ih, 6, BLACK, rst.inverted);
}

void barcode_render_setup(RendererCustom* renderer, Image* image, float x, float y, float scale_x, float scale_y, bool inverted) {
    if (!renderer) return;
    if (!image) return;
    rst.image = image;
    rst.x = x;
    rst.y = y;
    rst.scale_x = scale_x;
    rst.scale_y = scale_y;
    rst.inverted = inverted;
    renderer->setDrawBeginFunction(&barcode_drawBegin);
    renderer->setDrawEndFunction(&barcode_drawEnd);
    renderer->setDrawLineFunction(&barcode_drawLine);
    renderer->setDrawBoxFunction(&barcode_drawBox);
    renderer->setDrawTextFunction(&barcode_drawText);
    renderer->setDrawRingFunction(&barcode_drawRing);
    renderer->setDrawHexagonFunction(&barcode_drawHexagon);
}


void ImageDrawBarcode_Code39(Image* image, const char* text, int x, int y, int height, int scale, bool show_text, bool checksum, bool inverted) {
#ifdef DEBUG_DRAWING
    printf("Drawing barcode Code39 at %d, %d with message %s\n", x, y, text);
    printf("  Height: %d, Scale: %d, Show Text: %d, Checksum: %d\n", height, scale, show_text, checksum);
#endif
    if (!image) {
        notifyf("Error: Image is null\n");
        return;
    }
    if (!text) {
        notifyf("Error: Text is null\n");
        return;
    }
    // Create barcode object
    Barcode* bc = Factory::createBarcode("code39");
    if (!bc) {
        notifyf("Error: Barcode is undefined\n");
        return;
    }
    // Set barcode options
    bc->setChecksum(checksum).setShowText(show_text).build(text, 0, height);

    float scale_x = ((float) scale) * 1.4f;
    float scale_y = 1; //((float) height) * 0.05f;

    // Create renderer
    RendererCustom renderer;
    barcode_render_setup(&renderer, image, x, y, scale_x, scale_y, inverted);
    // Render barcode
    bc->render(renderer);
    // Cleanup
    delete bc;
}

void ImageDrawBarcode_Code128(Image* image, const char* text, int x, int y, int height, int scale, bool show_text, bool inverted) {
#ifdef DEBUG_DRAWING
    printf("Drawing barcode Code128 at %d, %d with message %s\n", x, y, text);
    printf("  Height: %d, Scale: %d, Show Text: %d\n", height, scale, show_text);
#endif
    if (!image) {
        notifyf("Error: Image is null\n");
        return;
    }
    if (!text) {
        notifyf("Error: Text is null\n");
        return;
    }
    // Create barcode object
    Barcode* bc = Factory::createBarcode("code128");
    if (!bc) {
        notifyf("Error: Barcode is undefined\n");
        return;
    }
    // Set barcode options
    bc->setShowText(show_text).build(text, 0, height);

    float scale_x = ((float) scale) * 1.0f;
    float scale_y = 1; //((float) height) * 0.05f;

    // Create renderer
    RendererCustom renderer;
    barcode_render_setup(&renderer, image, x, y, scale_x, scale_y, inverted);
    // Render barcode
    bc->render(renderer);
    // Cleanup
    delete bc;
}