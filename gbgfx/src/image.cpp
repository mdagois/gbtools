#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

#include "image.h"

////////////////////////////////////////////////////////////////////////////////

static bool validateSize(const Image& image, uint32_t width, uint32_t height)
{
	return
		width > 0 && height > 0 &&
		(width % kTileSize == 0) && (height % kTileSize == 0) &&
		(image.getWidth() % width == 0) && (image.getHeight() % height == 0);
}

////////////////////////////////////////////////////////////////////////////////

ImageArea::ImageArea(
	const ColorRGBA* pixels,
	uint32_t x, uint32_t y, uint32_t width, uint32_t height,
	uint32_t pitch)
: m_pixels(pixels)
, m_x(x)
, m_y(y)
, m_width(width)
, m_height(height)
, m_pitch(pitch)
{
}

ImageArea::~ImageArea()
{
}

uint32_t ImageArea::getX() const
{
	return m_x;
}

uint32_t ImageArea::getY() const
{
	return m_y;
}

uint32_t ImageArea::getWidth() const
{
	return m_width;
}

uint32_t ImageArea::getHeight() const
{
	return m_height;
}

const ColorRGBA* ImageArea::getPixels() const
{
	return m_pixels;
}

bool ImageArea::iterateTiles(
	std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const
{
	assert(getWidth() % kTileSize == 0);
	assert(getHeight() % kTileSize == 0);

	const uint32_t iterate_row_count = getHeight() / kTileSize;
	const uint32_t iterate_column_count = getWidth() / kTileSize;
	const ColorRGBA* pixels = getPixels();
	for(uint32_t j = 0; j < iterate_row_count; ++j)
	{
		for(uint32_t i = 0; i < iterate_column_count; ++i)
		{
			const ColorRGBA* tile_pixels = pixels + (j * m_pitch * kTileSize) + (i * kTileSize);
			ImageTile tile;
			for(uint32_t p = 0; p < kTileSize; ++p)
			{
				tile[p][0] = pixels[0];
				tile[p][1] = pixels[1];
				tile[p][2] = pixels[2];
				tile[p][3] = pixels[3];
				tile[p][4] = pixels[4];
				tile[p][5] = pixels[5];
				tile[p][6] = pixels[6];
				tile[p][7] = pixels[7];
				tile_pixels += m_pitch;
			}
			const uint32_t tile_x = m_x * iterate_column_count + i;
			const uint32_t tile_y = m_y * iterate_row_count + j;
			if(!tile_callback(tile, tile_x, tile_y))
			{
				return false;
			}
		}
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////

Image::Image()
: m_pixels(nullptr)
, m_width(0)
, m_height(0)
{
}

Image::~Image()
{
	if(m_pixels == nullptr)
	{
		return;
	}
	stbi_image_free(m_pixels);
	m_pixels = nullptr;
}

bool Image::read(const char* filename)
{
	assert(filename != nullptr);
	int32_t num_channels = 0;
	int32_t width = 0;
	int32_t height = 0;
	m_pixels = reinterpret_cast<ColorRGBA*>(stbi_load(filename, &width, &height, &num_channels, 4));
	assert(width > 0);
	assert(height > 0);
	m_width = static_cast<uint32_t>(width);
	m_height = static_cast<uint32_t>(height);
	m_filename = filename;
	return m_pixels != nullptr;
}

uint32_t Image::getWidth() const
{
	return m_width;
}

uint32_t Image::getHeight() const
{
	return m_height;
}

const char* Image::getFilename() const
{
	return m_filename.c_str();
}

const ColorRGBA* Image::getPixels() const
{
	return m_pixels;
}

bool Image::iterateArea(
	uint32_t start_row, uint32_t row_count,
	uint32_t area_width, uint32_t area_height,
	std::function<bool(const ImageArea&)> area_callback) const
{
	if(row_count == kIterateAllRows)
	{
		row_count = 0xFFFFFFFFU;
	}
	assert(row_count > 0);

	if(!validateSize(*this, area_width, area_height))
	{
		return false;
	}

	const uint32_t total_row_count = getHeight() / area_height;
	const uint32_t iterate_row_count = std::min(total_row_count - start_row, row_count);
	const uint32_t iterate_column_count = getWidth() / area_width;
	const ColorRGBA* pixels = getPixels() + (start_row * getWidth() * area_height);
	for(uint32_t j = 0; j < iterate_row_count; ++j)
	{
		for(uint32_t i = 0; i < iterate_column_count; ++i)
		{
			const ColorRGBA* area_pixels = pixels + (j * getWidth() * area_height) + (i * area_width);
			const ImageArea tile_area(area_pixels, i, j, area_width, area_height, this->getWidth());
			if(!area_callback(tile_area))
			{
				return false;
			}
		}
	}

	return true;
}

bool Image::iterateTiles(
	uint32_t start_row, uint32_t row_count,
	std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const
{
	return iterateArea(
		start_row, row_count, kTileSize, kTileSize,
		[&tile_callback](const ImageArea& area)
		{
			return area.iterateTiles(tile_callback);
		});
}

