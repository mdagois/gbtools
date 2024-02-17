#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>

#include "color.h"
#include "constants.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Image tile
////////////////////////////////////////////////////////////////////////////////

class ImageTile
{
public:
	ImageTile();
	virtual ~ImageTile();

	ColorRGBA& operator[](int32_t index);
	const ColorRGBA operator[](int32_t index) const;
	uint32_t size() const;

private:
	ColorRGBA m_pixels[kPixelsPerTile];
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
		std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const;
	bool iterateTiles(
		uint32_t start_tile_row, uint32_t tile_row_count,
		uint32_t metatile_width, uint32_t metatile_height,
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

