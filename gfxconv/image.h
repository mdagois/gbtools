#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>

#include "color.h"

enum : uint32_t
{
	kIterateAllRows = 0,
};

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
		uint32_t start_row, uint32_t row_count,
		uint32_t tile_width, uint32_t tile_height,
		std::function<bool(const ColorRGBA*, uint32_t, uint32_t)> tile_callback) const;

private:
	bool validateSize(uint32_t tile_width, uint32_t tile_height) const;

private:
	std::string m_filename;
	ColorRGBA* m_pixels;
	int32_t m_width;
	int32_t m_height;
};

