#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>

#include "color.h"

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
// Image area
////////////////////////////////////////////////////////////////////////////////

class ImageArea
{
public:
	ImageArea(
		const ColorRGBA* pixels,
		uint32_t x, uint32_t y, uint32_t width, uint32_t height,
		uint32_t pitch);
	~ImageArea();

	uint32_t getX() const;
	uint32_t getY() const;
	uint32_t getWidth() const;
	uint32_t getHeight() const;
	const ColorRGBA* getPixels() const;

	bool iterateTiles(
		std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const;

private:
	const ColorRGBA* m_pixels;
	uint32_t m_x;
	uint32_t m_y;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_pitch;
};

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

	bool iterateArea(
		uint32_t start_row, uint32_t row_count,
		uint32_t area_width, uint32_t area_height,
		std::function<bool(const ImageArea&)> area_callback) const;
	bool iterateTiles(
		uint32_t start_row, uint32_t row_count,
		std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const;

private:
	std::string m_filename;
	ColorRGBA* m_pixels;
	uint32_t m_width;
	uint32_t m_height;
};

