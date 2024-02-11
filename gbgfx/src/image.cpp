#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

#include "image.h"
#include "log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Image tile
////////////////////////////////////////////////////////////////////////////////

ImageTile::ImageTile()
{
}

ImageTile::~ImageTile()
{
}

ColorRGBA& ImageTile::operator[](int32_t index)
{
	assert(index < kPixelsPerTile);
	return m_pixels[index];
}

const ColorRGBA ImageTile::operator[](int32_t index) const
{
	assert(index < kPixelsPerTile);
	return m_pixels[index];
}

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

	bool iterateArea(
		uint32_t start_tile_row, uint32_t tile_row_count,
		uint32_t metatile_width, uint32_t metatile_height,
		std::function<bool(const ImageArea&)> area_callback) const;
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

bool ImageArea::iterateArea(
	uint32_t start_tile_row, uint32_t tile_row_count,
	uint32_t metatile_width, uint32_t metatile_height,
	std::function<bool(const ImageArea&)> area_callback) const
{
	if(tile_row_count == kIterateAllRows)
	{
		tile_row_count = 0xFFFFFFFFU;
	}
	assert(tile_row_count > 0);

	const uint32_t total_row_count = getHeight() / metatile_height;
	const uint32_t iterate_row_count = std::min(total_row_count - start_tile_row, tile_row_count);
	const uint32_t iterate_column_count = getWidth() / metatile_width;
	const ColorRGBA* pixels = getPixels() + (start_tile_row * getWidth() * metatile_height);
	for(uint32_t j = 0; j < iterate_row_count; ++j)
	{
		for(uint32_t i = 0; i < iterate_column_count; ++i)
		{
			const ColorRGBA* area_pixels = pixels + (j * getWidth() * metatile_height) + (i * metatile_width);
			const ImageArea tile_area(area_pixels, i, j, metatile_width, metatile_height, m_pitch);
			if(!area_callback(tile_area))
			{
				return false;
			}
		}
	}

	return true;
}

bool ImageArea::iterateTiles(
	std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const
{
	assert(getWidth() % kTileSize == 0);
	assert(getHeight() % kTileSize == 0);

	const uint32_t iterate_row_count = getHeight() / kTileSize;
	const uint32_t iterate_column_count = getWidth() / kTileSize;
	const ColorRGBA* const pixels = getPixels();
	for(uint32_t j = 0; j < iterate_row_count; ++j)
	{
		for(uint32_t i = 0; i < iterate_column_count; ++i)
		{
			const ColorRGBA* tile_pixels = pixels + (j * m_pitch * kTileSize) + (i * kTileSize);
			ImageTile tile;
			for(uint32_t p = 0; p < kTileSize; ++p)
			{
				const uint32_t base_index = p * kTileSize;
				tile[base_index + 0] = tile_pixels[0];
				tile[base_index + 1] = tile_pixels[1];
				tile[base_index + 2] = tile_pixels[2];
				tile[base_index + 3] = tile_pixels[3];
				tile[base_index + 4] = tile_pixels[4];
				tile[base_index + 5] = tile_pixels[5];
				tile[base_index + 6] = tile_pixels[6];
				tile[base_index + 7] = tile_pixels[7];
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
// Image
////////////////////////////////////////////////////////////////////////////////

Image::Image()
: m_pixels(nullptr)
, m_width(0)
, m_height(0)
{
}

Image::~Image()
{
	unload();
}

bool Image::read(const char* filename)
{
	assert(filename != nullptr);
	unload();

	constexpr int32_t kChannelCount = 4;
	int32_t num_channels = 0;
	int32_t width = 0;
	int32_t height = 0;
	m_pixels = reinterpret_cast<ColorRGBA*>(stbi_load(filename, &width, &height, &num_channels, kChannelCount));
	assert(width > 0);
	assert(height > 0);
	m_width = static_cast<uint32_t>(width);
	m_height = static_cast<uint32_t>(height);
	m_filename = filename;
	if(m_pixels == nullptr)
	{
		LOG_ERROR("Could not read the image file [" << filename << "]");
		return false;
	}
	LOG_INFO("Read image [" << filename << "] " << getWidth() << "x" << getHeight());
	return true;
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

bool Image::iterateTiles(
	std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const
{
	return iterateTiles(0, kIterateAllRows, kTileSize, kTileSize, false, tile_callback);
}

bool Image::iterateTiles(
	uint32_t start_tile_row, uint32_t tile_row_count,
	uint32_t metatile_width, uint32_t metatile_height,
	bool use_microtile_8x16,
	std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const
{
	if(metatile_width == 0 && metatile_height == 0)
	{
		LOG_ERROR("Metatile dimensions must not be zero [" << m_filename << "]");
		return false;
	}
	if(use_microtile_8x16 && ((metatile_height % (kTileSize * 2)) != 0))
	{
		LOG_ERROR("Metatiles' height must be a multiple of 16 when using 8x16 microtiles [" << m_filename << "]");
		return false;
	}
	if((getWidth() % metatile_width != 0) || (getHeight() % metatile_height != 0))
	{
		LOG_ERROR("Image dimension must be a multiple of the metatile dimension [" << m_filename << "]");
		return false;
	}
	if((metatile_height % kTileSize) != 0 || (metatile_height % kTileSize) != 0)
	{
		LOG_ERROR("Metatile dimension must be a multiple of the microtile dimension [" << m_filename << "]");
		return false;
	}
	
	ImageArea full_image(
		m_pixels,
		0, 0, m_width, m_height,
		m_width);
	full_image.iterateArea(
		start_tile_row, tile_row_count,
		metatile_width, metatile_height,
		[&tile_callback, use_microtile_8x16](const ImageArea& metatile_area)
		{
			return metatile_area.iterateArea(
				0, kIterateAllRows, kTileSize, kTileSize * (use_microtile_8x16 ? 2 : 1),
				[&tile_callback](const ImageArea& microtile_area)
				{
					return microtile_area.iterateTiles(tile_callback);
				});
		});
	return true;
}

void Image::unload()
{
	if(m_pixels == nullptr)
	{
		return;
	}
	stbi_image_free(m_pixels);
	m_pixels = nullptr;
}

////////////////////////////////////////////////////////////////////////////////

}

