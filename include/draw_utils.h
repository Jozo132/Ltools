#pragma once

#include "tools.h"
#include "imagex.h"
#include "fontx.h"
#include "colorx.h"




void ImageDrawRectangleRounded(Image* dst, struct Rectangle rec, float roundness, int segments, Color color, bool inverted = false) {
    float x = rec.x;
    float y = rec.y;
    float w = rec.width;
    float h = rec.height;
    if (roundness <= 0) {
        dst->drawRect(x, y, w, h, 0, BLANK, color, inverted);
    } else {
        float shortHalf = rec.width < rec.height ? rec.width / 2 : rec.height / 2;
        float radius = shortHalf * roundness;
        dst->drawRect(x + radius, y, w - radius, h, 0, BLANK, color, inverted);
        dst->drawRect(x, y + radius, w, h - radius, 0, BLANK, color, inverted);
        dst->drawCircle(x + radius, y + radius, radius, 0, BLANK, color, inverted);
        dst->drawCircle(x + w - radius, y + radius, radius, 0, BLANK, color, inverted);
        dst->drawCircle(x + radius, y + h - radius, radius, 0, BLANK, color, inverted);
        dst->drawCircle(x + w - radius, y + h - radius, radius, 0, BLANK, color, inverted);
    }
}

void ImageDrawArcLines(Image* dst, Vector2 center, float radius, int startAngle, int endAngle, int segments, int thick, Color color, bool inverted = false) {
    if (thick < 1) return;
    dst->drawArc(center, radius, startAngle, endAngle, thick, color, BLANK, inverted);
}

void ImageDrawRectangleRoundedLinesEx(Image* dst, struct Rectangle rec, float roundness, int segments, int thick, Color color, bool inverted = false) {
    float x = rec.x;
    float y = rec.y;
    float w = rec.width;
    float h = rec.height;
    if (roundness <= 0) {
        dst->drawRect(x, y, w, h, thick, color, BLANK, inverted);
    } else {
        float shortHalf = rec.width < rec.height ? rec.width / 2 : rec.height / 2;
        float radius = shortHalf * roundness;
        dst->drawRect(x + radius, y, w - radius * 2, thick, 0, BLANK, color, inverted); // Top
        dst->drawRect(x, y + radius, thick, h - radius * 2, 0, BLANK, color, inverted); // Left
        dst->drawRect(x + w - thick, y + radius, thick, h - radius * 2, 0, BLANK, color, inverted); // Right
        dst->drawRect(x + radius, y + h - thick, w - radius * 2, thick, 0, BLANK, color, inverted); // Bottom
        ImageDrawArcLines(dst, Vector2{ x + radius, y + radius }, radius - thick / 2, 180, 270, segments, thick, color, inverted); // Top Left
        ImageDrawArcLines(dst, Vector2{ x + w - radius, y + radius }, radius - thick / 2, 270, 360, segments, thick, color, inverted); // Top Right
        ImageDrawArcLines(dst, Vector2{ x + w - radius, y + h - radius }, radius - thick / 2, 0, 90, segments, thick, color, inverted); // Bottom Right
        ImageDrawArcLines(dst, Vector2{ x + radius, y + h - radius }, radius - thick / 2, 90, 180, segments, thick, color, inverted); // Bottom Left
    }
}



void ImageDrawQuadrilateral(Image* dst, Vector2 v1, Vector2 v2, Vector2 v3, Vector2 v4, Color color, bool inverted = false) {
    dst->drawTriangle(v1, v2, v3, 0, BLANK, color, inverted);
    dst->drawTriangle(v1, v3, v4, 0, BLANK, color, inverted);
}

void ImageDrawNGon(Image* dst, Vector2 center, float radius, int sides, Color color, bool inverted = false) {
    if (sides < 3) return;
    float step = 360.0f / sides;
    Vector2* points = (Vector2*) malloc(sides * sizeof(Vector2));
    for (int i = 0; i < sides; i++) {
        points[i].x = center.x + cosf(i * step * DEG2RAD) * radius;
        points[i].y = center.y + sinf(i * step * DEG2RAD) * radius;
        if (i > 0) {
            dst->drawTriangle(center, points[i - 1], points[i], 0, BLANK, color, inverted);
        }
    }
    free(points);
}
