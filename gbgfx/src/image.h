#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>

#include "color.h"

//TODO Add error codes

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTileSize = 8,
	kIterateAllRows = 0,
};

////////////////////////////////////////////////////////////////////////////////
// Tile
////////////////////////////////////////////////////////////////////////////////

typedef ColorRGBA ImageTile[kTileSize][kTileSize];

////////////////////////////////////////////////////////////////////////////////
// Image
////////////////////////////////////////////////////////////////////////////////

class Image
{
public:
	Image();
	~Image();

	bool read(const char* filename);

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	const char* getFilename() const;
	const ColorRGBA* getPixels() const;

	bool iterateTiles(
		uint32_t start_tile_row, uint32_t tile_row_count,
		uint32_t metatile_width, uint32_t metatile_height,
		bool use_microtile_8x16,
		std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const;

private:
	std::string m_filename;
	ColorRGBA* m_pixels;
	uint32_t m_width;
	uint32_t m_height;
};

