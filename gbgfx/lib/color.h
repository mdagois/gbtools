#pragma once

#include <cstdint>

namespace gbgfx {

////////////////////////////////////////////////////////////////////////////////
// RGBA
////////////////////////////////////////////////////////////////////////////////

struct ColorRGBA
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

static ColorRGBA kRGBA_Invalid = {0x00U, 0x00U, 0x00U, 0x00U};
static ColorRGBA kRGBA_Magenta = {0xFFU, 0x00U, 0xFFU, 0xFFU};

bool operator==(const ColorRGBA& lhs, const ColorRGBA& rhs);
bool operator!=(const ColorRGBA& lhs, const ColorRGBA& rhs);
bool operator<(const ColorRGBA& lhs, const ColorRGBA& rhs);
double getLuminance(const ColorRGBA color);
void sortColorsRGBA(ColorRGBA* colors, uint32_t count);

////////////////////////////////////////////////////////////////////////////////
// BGR555
////////////////////////////////////////////////////////////////////////////////

typedef uint16_t ColorBGR555;

enum : ColorBGR555
{
	kBGR555_Invalid = 0x8000U,
	kBGR555_Magenta = 0x7C1FU,
};

double getLuminance(const ColorBGR555 color);
void sortColorsBGR555(ColorBGR555* colors, uint32_t count);

////////////////////////////////////////////////////////////////////////////////
// Conversion
////////////////////////////////////////////////////////////////////////////////

ColorBGR555 convertColor(ColorRGBA rgba);

////////////////////////////////////////////////////////////////////////////////

}
