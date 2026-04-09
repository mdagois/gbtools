#pragma once

#include <vector>
#include "color.h"

namespace gfx {

static ColorRGBA kTilemapMetadata_None = {0x00U, 0x00U, 0x00U, 0x00U};
static ColorRGBA kTilemapMetadata_Foreground = {0xFFU, 0x00U, 0x00U, 0xFFU};

struct TilemapMetadata
{
	ColorRGBA color;
	uint32_t x;
	uint32_t y;
};

typedef std::vector<TilemapMetadata> TilemapMetadataContainer;

}

