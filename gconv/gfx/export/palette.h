#pragma once

#include <vector>

#include "color.h"
#include "data/palette.h"

namespace gfx {

////////////////////////////////////////////////////////////////////////////////
// Palette
////////////////////////////////////////////////////////////////////////////////

class PaletteSetData
{
public:
	PaletteSetData();
	virtual ~PaletteSetData();

	bool initialize(const PaletteSet& palette_set);

	uint32_t getPaletteCount() const;
	const uint8_t* getData() const;
	uint32_t getDataSize() const;

private:
	std::vector<ColorBGR555> m_data;
	uint32_t m_palette_count;
};

////////////////////////////////////////////////////////////////////////////////

}

