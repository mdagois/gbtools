#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "data/color.h"
#include "division.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Image tile
////////////////////////////////////////////////////////////////////////////////

class ImageArea;

class ImageTile
{
public:
	ImageTile();
	virtual ~ImageTile();
	void setImageArea(const ImageArea* image_area);

	const ColorRGBA operator[](int32_t index) const;
	uint32_t getWidth() const;
	uint32_t getHeight() const;

private:
	const ImageArea* m_image_area;
};

////////////////////////////////////////////////////////////////////////////////
// Image
////////////////////////////////////////////////////////////////////////////////

class Image
{
public:
	Image();
	virtual ~Image();

	bool read(const char* filename);

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	const char* getFilename() const;
	const ColorRGBA* getPixels() const;

	bool iterateTiles(
		DivisionInfo& out_division_info,
		const Division* divisions, uint32_t division_count,
		std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const;

private:
	void unload();

private:
	std::string m_filename;
	ColorRGBA* m_pixels;
	uint32_t m_width;
	uint32_t m_height;
};

////////////////////////////////////////////////////////////////////////////////

}

