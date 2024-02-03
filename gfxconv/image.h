#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>

#include "color.h"

enum : uint32_t
{
	kTileSize = 8,
};

class Image
{
public:
	Image();
	~Image();

	bool read(const char* filename);
	bool validateSize() const;

	uint32_t getWidth() const;
	uint32_t getHeight() const;
	const char* getFilename() const;
	const ColorRGBA* getPixels() const;

	bool iterateTiles(std::function<bool(const ColorRGBA*, uint32_t, uint32_t)> tile_callback);

private:
	std::string m_filename;
	ColorRGBA* m_pixels;
	int32_t m_width;
	int32_t m_height;
};

