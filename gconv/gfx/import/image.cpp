#include <cassert>

#include "third_party/stb_image.h"

#include "image.h"
#include "utils/log.h"

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// Image area
////////////////////////////////////////////////////////////////////////////////

typedef std::function<bool(DivisionStatusList* out_division_status_list, const ImageArea&, uint32_t level)> AreaCallback;

class ImageArea
{
public:
	ImageArea(
		const ColorRGBA* pixels,
		uint32_t x, uint32_t y, uint32_t width, uint32_t height,
		uint32_t pitch);
	~ImageArea();

	bool iterateArea(DivisionStatusList* out_division_status_list, uint32_t level, AreaCallback area_callback) const;

	const ColorRGBA operator[](int32_t index) const;
	uint32_t getOffsetX() const;
	uint32_t getOffsetY() const;
	uint32_t getWidth() const;
	uint32_t getHeight() const;
	bool isTransparent() const;

private:
	const ColorRGBA* m_pixels;
	uint32_t m_offset_x;
	uint32_t m_offset_y;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_pitch;
};

////////////////////////////////////////////////////////////////////////////////

ImageArea::ImageArea(
	const ColorRGBA* pixels,
	uint32_t offset_x, uint32_t offset_y, uint32_t width, uint32_t height,
	uint32_t pitch)
: m_pixels(pixels)
, m_offset_x(offset_x)
, m_offset_y(offset_y)
, m_width(width)
, m_height(height)
, m_pitch(pitch)
{
}

ImageArea::~ImageArea()
{
}

static void fillDivisionStatusList(
	DivisionStatusList* out_division_status_list, uint32_t level,
	uint32_t image_x, uint32_t image_y,
	uint32_t top_division_width, uint32_t top_division_height,
	uint32_t image_pitch)
{
	while(true)
	{
		const Division division = out_division_status_list->division;
		assert(top_division_width % division.width == 0);
		assert(top_division_height % division.height == 0);
		const uint32_t division_pitch = image_pitch / division.width;
		const uint32_t row = top_division_height / division.height;
		const uint32_t column = top_division_width / division.width;
		const uint32_t top = image_y / division.height;
		const uint32_t left = image_x / division.width;
		for(uint32_t j = 0; j < row; ++j)
		{
			for(uint32_t i = 0; i < column; ++i)
			{
				const uint32_t x = left + i;
				const uint32_t y = top + j;
				assert((*out_division_status_list)[y * division_pitch + x] == kDivisionStatus_Invalid);
				(*out_division_status_list)[y * division_pitch + x] = kDivisionStatus_Skipped;
			}
		}
		if(level == 0)
		{
			break;
		}
		++out_division_status_list;
		--level;
	}
}

bool ImageArea::iterateArea(DivisionStatusList* out_division_status_list, uint32_t level, AreaCallback area_callback) const
{
	const Division division = out_division_status_list->division;
	assert(m_width % division.width == 0);
	assert(m_height % division.height == 0);

	GBGFX_LOG_DEBUG(
		"Processing area (x=" << m_offset_x << ", y=" << m_offset_y
		<< ", w=" << m_width << ", h=" << m_height << ") with division ("
		<< division.width << "," << division.height << ","
		<< (division.skip_transparent ? "s" : "-") << "," << (isTransparent() ? "t" : "-") << ")");

	const uint32_t row_count = m_height / division.height;
	const uint32_t column_count = m_width / division.width;
	const uint32_t division_pitch = m_pitch / division.width;
	for(uint32_t j = 0; j < row_count; ++j)
	{
		for(uint32_t i = 0; i < column_count; ++i)
		{
			const uint32_t relative_offset_x = i * division.width;
			const uint32_t relative_offset_y = j * division.height;
			const uint32_t absolute_offset_x = m_offset_x + relative_offset_x;
			const uint32_t absolute_offset_y = m_offset_y + relative_offset_y;
			const ColorRGBA* area_pixels = m_pixels + (relative_offset_y * m_pitch) + relative_offset_x;
			const ImageArea sub_area(
				area_pixels, absolute_offset_x, absolute_offset_y,
				division.width, division.height, m_pitch);

			const uint32_t division_number = (m_offset_y / division.height + j) * division_pitch + (m_offset_x / division.width + i);
			(*out_division_status_list)[division_number] = kDivisionStatus_Transparent;
			if(division.skip_transparent && sub_area.isTransparent())
			{
				GBGFX_LOG_DEBUG(
					"Skipping transparent area (x=" << sub_area.m_offset_x << ", y=" << sub_area.m_offset_y
					<< ", w=" << sub_area.m_width << ", h=" << sub_area.m_height << ")");
				fillDivisionStatusList(
					out_division_status_list + 1, level - 1,
					absolute_offset_x, absolute_offset_y,
					division.width, division.height,
					m_pitch);
				continue;
			}
			(*out_division_status_list)[division_number] = kDivisionStatus_Valid;
			if(!area_callback(out_division_status_list, sub_area, level))
			{
				GBGFX_LOG_ERROR(
					"Error in callback for area (x=" << sub_area.m_offset_x << ", y=" << sub_area.m_offset_y
					<< ", w=" << sub_area.m_width << ", h=" << sub_area.m_height << ")");
				return false;
			}
		}
	}

	return true;
}

const ColorRGBA ImageArea::operator[](int32_t index) const
{
	assert(index < static_cast<int32_t>(m_width * m_height));
	const uint32_t row = index / m_width;
	const uint32_t column = index % m_width;
	return m_pixels[row * m_pitch + column];
}

uint32_t ImageArea::getOffsetX() const
{
	return m_offset_x;
}

uint32_t ImageArea::getOffsetY() const
{
	return m_offset_y;
}

uint32_t ImageArea::getWidth() const
{
	return m_width;
}

uint32_t ImageArea::getHeight() const
{
	return m_height;
}

bool ImageArea::isTransparent() const
{
	for(uint32_t i = 0; i < m_width * m_height; ++i)
	{
		if((*this)[i] != kRGBA_Magenta)
		{
			return false;
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Image tile
////////////////////////////////////////////////////////////////////////////////

ImageTile::ImageTile()
: m_image_area(nullptr)
{
}

ImageTile::~ImageTile()
{
}

void ImageTile::setImageArea(const ImageArea* image_area)
{
	assert(image_area != nullptr);
	m_image_area = image_area;
}

const ColorRGBA ImageTile::operator[](int32_t index) const
{
	assert(m_image_area != nullptr);
	return (*m_image_area)[index];
}

uint32_t ImageTile::getWidth() const
{
	assert(m_image_area != nullptr);
	return (*m_image_area).getWidth();
}

uint32_t ImageTile::getHeight() const
{
	assert(m_image_area != nullptr);
	return (*m_image_area).getHeight();
}

////////////////////////////////////////////////////////////////////////////////
// Division
////////////////////////////////////////////////////////////////////////////////

static bool validateDivisions(
	uint32_t top_width, uint32_t top_height,
	const Division* divisions, uint32_t division_count)
{
	if(division_count == 0)
	{
		GBGFX_LOG_ERROR("The division list cannot be empty");
		return false;
	}
	uint32_t previous_width = top_width;
	uint32_t previous_height = top_height;
	for(uint32_t i = 0; i < division_count; ++i)
	{
		const Division& current = divisions[i];
		if(current.width == 0 || current.height == 0)
		{
			GBGFX_LOG_ERROR(
				"Division " << i << " (" << current.width << "x" << current.height << ") "
				<< "must not have any dimension at zero");
			return false;
		}
		if(current.width > previous_width || current.height > previous_height)
		{
			GBGFX_LOG_ERROR(
				"Division " << i << " (" << current.width << "x" << current.height << ") "
				<< "must not be larger than " << previous_width << "x" << previous_height);
			return false;
		}
		if(current.width == previous_width && current.height == previous_height)
		{
			GBGFX_LOG_ERROR(
				"Division " << i << " (" << current.width << "x" << current.height << ") "
				<< "must be stricly smaller than " << previous_width << "x" << previous_height);
			return false;
		}
		if((previous_width % current.width != 0) || (previous_height % current.height != 0))
		{
			GBGFX_LOG_ERROR(
				"Division " << i << " (" << current.width << "x" << current.height << ") "
				<< "dimension must divide exactly " << previous_width << "x" << previous_height);
			return false;
		}
		previous_width = current.width;
		previous_height = current.height;
	}
	return true;
}

static bool validateDivisionInfo(
	uint32_t top_width, uint32_t top_height,
	const DivisionInfo& division_info,
	const Division* divisions, uint32_t division_count)
{
	for(uint32_t i = 0; i < division_count; ++i)
	{
		assert(top_width % divisions[i].width == 0);
		assert(top_height % divisions[i].height == 0);
		const uint32_t row = top_height / divisions[i].height;
		const uint32_t column = top_width / divisions[i].width;
		if(division_info[i].size() != row * column)
		{
			assert(false);
			return false;
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
	if(m_pixels == nullptr)
	{
		GBGFX_LOG_ERROR("Could not read the image file [" << filename << "]");
		return false;
	}

	m_width = static_cast<uint32_t>(width);
	m_height = static_cast<uint32_t>(height);
	m_filename = filename;
	GBGFX_LOG_INFO("Read image [" << filename << "] " << getWidth() << "x" << getHeight());
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
	DivisionInfo& out_division_info,
	const Division* divisions, uint32_t division_count,
	std::function<bool(const ImageTile&, uint32_t, uint32_t)> tile_callback) const
{
	if(!validateDivisions(m_width, m_height, divisions, division_count))
	{
		return false;
	}

	AreaCallback area_callback = [&area_callback, &tile_callback](DivisionStatusList* out_division_status_list, const ImageArea& area, uint32_t level)
	{
		if(level == 0)
		{
			ImageTile image_tile;
			image_tile.setImageArea(&area);
			GBGFX_LOG_DEBUG(
				"New tile (x=" << area.getOffsetX() << ", y=" << area.getOffsetY()
				<< ", w=" << area.getWidth() << ", h=" << area.getHeight() << ","
				<< (out_division_status_list->division.skip_transparent ? "s" : "-") << "," << (area.isTransparent() ? "t" : "-") << ")");
			return tile_callback(image_tile, area.getOffsetX(), area.getOffsetY());
		}
		else
		{
			return area.iterateArea(out_division_status_list + 1, level - 1, area_callback);
		}
	};

	out_division_info.image_width = m_width;
	out_division_info.image_height = m_height;
	out_division_info.clear();
	out_division_info.resize(division_count);
	for(uint32_t i = 0; i < division_count; ++i)
	{
		auto& list = out_division_info[i];
		list.division = divisions[i];
		list.resize((m_width / divisions[i].width) * (m_height / divisions[i].height));
		std::fill(list.begin(), list.end(), kDivisionStatus_Invalid);
	}

	ImageArea full_image(
		m_pixels,
		0, 0, m_width, m_height,
		m_width);
	return
		full_image.iterateArea(out_division_info.data(), division_count - 1, area_callback) &&
		validateDivisionInfo(m_width, m_height, out_division_info, divisions, division_count) &&
		computeDivisionInfoOffsets(out_division_info);
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

