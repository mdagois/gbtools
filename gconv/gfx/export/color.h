#pragma once

#include "data/color.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// BGR555
////////////////////////////////////////////////////////////////////////////////

typedef uint16_t ColorBGR555;

enum : ColorBGR555
{
	kBGR555_Invalid = 0x8000U,
};

ColorBGR555 convertColor(ColorRGBA rgba);

////////////////////////////////////////////////////////////////////////////////

}

