#pragma once
#include "CImg.h"
#include <string>
typedef float Float;


struct Color {
	Float r, g, b;
	Color(Float _r = 0, Float _g = 0, Float _b = 0) :r(_r), g(_g), b(_b) {}

	Color operator*(Float f) {
		return Color(r * f, g * f, b * f);
	}
	Color operator-(const Color& c) {
		return Color(r - c.r, g - c.g, b - c.b);
	}
	Color operator+(const Color& c) {
		return Color(r + c.r, g + c.g, b + c.b);
	}
	Float operator[](int idx) const {
		if (idx == 0) return r;
		else if (idx == 1) return g;
		else  return b;
	}
	Float& operator[](int idx)  {
		if (idx == 0) return r;
		else if (idx == 1) return g;
		else  return b;
	}
};

Float Clamp(Float x, Float a, Float b) {
	Float res = x;
	if (res < a) res = a;
	if (res > b) res = b;
	return res;
}
Color Clamp(const Color& c, Float a, Float b) {
	Color res;
	res.r = Clamp(c.r, a, b);
	res.g = Clamp(c.g, a, b);
	res.b = Clamp(c.b, a, b);
	return res;
}

class ImageRGB {
public:
	ImageRGB() :width(-1), height(-1) {}
	ImageRGB(int x, int y)
		:width(x), height(y) {
		pixels.resize(width * height);
	}
	ImageRGB(int x, int y, const std::vector<Color>& arr) :width(x), height(y) {
		pixels.resize(width * height);
	
		for (size_t i = 0; i < width * height; ++i) {
			pixels[i] = arr[i];
		}
	}

	ImageRGB(const ImageRGB& img) {
		width = img.width;
		height = img.height;
		pixels.resize(img.width * img.height);

		for (size_t i = 0; i < width * height; ++i) {
			pixels[i] = img.pixels[i];
		}
	}

	ImageRGB& operator=(const ImageRGB& img) {
		width = img.width;
		height = img.height;
		pixels.resize(img.width * img.height);

		for (size_t i = 0; i < width * height; ++i) {
			pixels[i] = img.pixels[i];
		}

		return *this;
	}

	bool load(const std::string& name, bool need_normalize = true) {

		cimg_library::CImg<Float> image(name.c_str());

		width = image.width();
		height = image.height();
		pixels.resize(width * height);

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				Float r = image(i, j, 0, 0);
				Float g = image(i, j, 0, 1);
				Float b = image(i, j, 0, 2);
				if (need_normalize) {
					Set(i, j, Color(r, g, b) * (1.0/255));
				}
				else {
					Set(i, j, Color(r, g, b));
				}
			}
		}
		return true;
	}
	void save(const std::string& name, Float inv_gamma = 1.0 / 2.2) {
		cimg_library::CImg<Float> image(width, height, 1, 3);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				Color c = At(i, j);
				c = Clamp(c, 0, 1);
				c = Color(std::pow(c.r, inv_gamma), std::pow(c.g, inv_gamma), std::pow(c.b, inv_gamma));
				c = c * 255;
				for (int k = 0; k < 3; ++k) {
					image(i, j, 0, k) = c[k];
				}
			}
		}
		try {
			image.save(name.c_str());
		}
		catch (cimg_library::CImgIOException) {
			printf("IO Exception from CImg, ignored.");
		}
	}

	Color At(int x, int y) const {
		int idx = y * width + x;
		return pixels[idx];
	}
	Color At(Float x, Float y) const {
		int ix = (int)x;
		int iy = (int)y;

		Float dx = x - ix;
		Float dy = y - iy;

		Color c00 = At(ix, iy);
		Color c10 = At(ix + 1, iy);
		Color c01 = At(ix, iy + 1);
		Color c11 = At(ix + 1, iy + 1);

		Color c0 = c00 + (c10 - c00) * dx;
		Color c1 = c01 + (c11 - c01) * dx;
		return c0 + (c1 - c0) * dy;
	}

	
	bool Set(int x, int y, const Color& v) {
		int idx = y * width + x;
		if (idx >= pixels.size()) return false;

		pixels[y * width + x] = v;
		return true;
	}
	

	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
private:
	int width, height;
	std::vector<Color> pixels;
};
