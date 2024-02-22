#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>

#include "model/color.h"

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
// Division
////////////////////////////////////////////////////////////////////////////////

struct Division
{
	uint32_t width;
	uint32_t height;
	bool skip_transparent;
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

