/* WiiLÖVE graphics module
 *
 * This file is part of WiiLÖVE.
 *
 * Copyright (C) 2022  HTV04
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program.  If not, see
 * <https://www.gnu.org/licenses/>.
 */

// Libraries
#include <grrlib-mod.h>
#include <FreeTypeGX.hpp>
#include <ogc/gx.h>
#if !defined(HW_DOL)
#include <ogc/conf.h>
#endif // !HW_DOL
#include <utility>
#include <vector>
#include <tuple>
#include <string>
#include <stdexcept>

// Classes
#include "../classes/graphics/font.hpp"
#include "../classes/graphics/quad.hpp"
#include "../classes/graphics/texture.hpp"

// Header
#include "graphics.hpp"

namespace love {
namespace graphics {

// Local variables
namespace {
	constexpr int width = 640;
	constexpr int height = 480;

	bool widescreen;

	// Transforms are stored for push/pop operations
	std::vector<GRRLIB_matrix> transforms;

	std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> backgroundColor;

	Font *curFont; // Initial font
}

void init() {
	// Init GRRLIB
	GRRLIB_Init();

#if !defined(HW_DOL)
	widescreen = CONF_GetAspectRatio() == CONF_ASPECT_16_9;
#endif // !HW_DOL

	curFont = new Font();

	module::reset(); // Set defaults
}

namespace module {

// Misc. querying functions
std::pair<int, int> getDimensions() { return std::make_pair(width, height); }
int getHeight() { return height; }
int getWidth() { return width; }
#ifdef HW_DOL
bool isWidescreen() { return false; }
#else // HW_DOL
bool isWidescreen() { return widescreen; }
#endif // !HW_DOL

// Transformation functions
void origin() {
	GRRLIB_Origin();
}
void pop() {
	if (transforms.empty()) { throw std::runtime_error("Stack is empty"); }

	GRRLIB_SetMatrix(&transforms.back()); // Use previously stored transform before removing it

	transforms.pop_back();
}
void push() {
	transforms.push_back(GRRLIB_GetMatrix()); // Store the current transform for later
}
void rotate(float angle) {
	GRRLIB_Rotate(angle);
}
void scale(float x, float y) {
	GRRLIB_Scale(x, y);
}
void translate(float dx, float dy) {
	GRRLIB_Translate(dx, dy);
}

// Set and get drawing colors
void clear(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	GRRLIB_FillScreen(GRRLIB_RGBA(r, g, b, a));
}
std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> getBackgroundColor() {
	return backgroundColor;
}
std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> getColor() {
	unsigned long int color = GRRLIB_Settings.color;

	return std::make_tuple(GRRLIB_R(color), GRRLIB_G(color), GRRLIB_B(color), GRRLIB_A(color));
}
void setBackgroundColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	backgroundColor = std::make_tuple(GRRLIB_R(r), GRRLIB_G(g), GRRLIB_B(b), GRRLIB_A(a));
}
void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	GRRLIB_Settings.color = GRRLIB_RGBA(r, g, b, a);
}

// Basic drawing functions
void circle(bool fill, float x, float y, float radius) {
	GRRLIB_Circle(x, y, radius, fill);
}
void line(float x1, float y1, float x2, float y2) {
	GRRLIB_Line(x1, y1, x2, y2);
}
void rectangle(bool fill, float x, float y, float width, float height) {
	GRRLIB_Rectangle(x, y, width, height, fill);
}

// Font functions
Font *getFont() { return curFont; }
void print(const std::wstring &text, float x, float y, float r, float sx, float sy, float ox, float oy) {
	curFont->fontSystem->drawText(x, y, text, sx, sy, ox, oy, r);
}
void setFont(Font *font) { curFont = font; }

// Texture functions
void draw(const Texture &texture, const Quad &textureQuad, float x, float y, float r, float sx, float sy, float ox, float oy) {
	GRRLIB_DrawTexturePart(x, y, texture.texture, textureQuad.texturePart, r, sx, sy, ox, oy);
}
void draw1(const Texture &texture, float x, float y, float r, float sx, float sy, float ox, float oy) {
	GRRLIB_DrawTexturePart(x, y, texture.texture, &texture.texture->part, r, sx, sy, ox, oy);
}

// Graphics state functions
bool getAntiAliasing() {
	return GRRLIB_Settings.antialias;
}
unsigned char getDeflicker() {
	return GRRLIB_Settings.deflicker;
}
unsigned char getLineWidth() {
	return GRRLIB_Settings.lineWidth;
}
unsigned char getPointSize() {
	return GRRLIB_Settings.pointSize;
}
void reset() {
	GRRLIB_Settings.color = 0xFFFFFFFF;
	backgroundColor = std::make_tuple(0, 0, 0, 255);

	origin();

	GRRLIB_SetAntiAliasing(true);
	GRRLIB_SetDeflicker(true);
	GRRLIB_SetPointSize(6);
	GRRLIB_SetLineWidth(6);
}
void setAntiAliasing(bool enable) {
	GRRLIB_Settings.antialias = enable;
}
void setDeflicker(bool enable) {
	GRRLIB_SetDeflicker(enable);
}
void setLineWidth(unsigned char width) {
	GRRLIB_SetLineWidth(width);
}
void setPointSize(unsigned char size) {
	GRRLIB_SetPointSize(size);
}

// Rendering functions
void present() {
	GRRLIB_Render();
}

} // module
} // graphics
} // love
