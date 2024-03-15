#pragma once

#include <cstdint>
#include <cstring>
#include <functional>
#include <string>
#include <vector>

#include "data/color.h"

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

enum DivisionStatus : uint8_t
{
	kDivisionFlag_Valid,
	kDivisionFlag_Transparent,
	kDivisionFlag_Skipped,
};

struct DivisionStatusList : public std::vector<DivisionStatus>
{
	Division division;
};

struct DivisionInfo : std::vector<DivisionStatusList>
{
	uint32_t image_width;
	uint32_t image_height;
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
		DivisionInfo& out_division_info,
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

