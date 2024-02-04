#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

#include "image.h"

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
	m_pixels = reinterpret_cast<ColorRGBA*>(stbi_load(filename, &m_width, &m_height, &num_channels, 4));
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

bool Image::validateSize(uint32_t tile_width, uint32_t tile_height) const
{
	return
		getWidth() > 0 && getHeight() > 0 &&
		((getWidth() % tile_width) == 0) && ((getHeight() % tile_height) == 0);
}

bool Image::iterateTiles(
	uint32_t start_row, uint32_t row_count,
	uint32_t tile_width, uint32_t tile_height,
	std::function<bool(const ColorRGBA*, uint32_t, uint32_t)> tile_callback) const
{
	if(row_count == kIterateAllRows)
	{
		row_count = 0xFFFFFFFFU;
	}
	assert(row_count > 0);

	if(!validateSize(tile_width, tile_height))
	{
		return false;
	}

	const uint32_t total_row_count = getHeight() / tile_height;
	const uint32_t iterate_row_count = std::max(total_row_count - start_row, row_count);
	const uint32_t iterate_column_count = getWidth() / tile_width;
	const ColorRGBA* pixels = getPixels() + (start_row * getWidth() * tile_height);
	for(uint32_t j = 0; j < iterate_row_count; ++j)
	{
		for(uint32_t i = 0; i < iterate_column_count; ++i)
		{
			const ColorRGBA* tile_pixels = pixels + (j * getWidth() * tile_height) + (i * tile_width);
			if(!tile_callback(tile_pixels, i, j))
			{
				return false;
			}
		}
	}

	return true;
}

