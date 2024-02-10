#include <cstdint>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kTilemapRowMaxCount = 32,
	kTilemapColumnMaxCount = 32,
};

////////////////////////////////////////////////////////////////////////////////
// Tilemap
////////////////////////////////////////////////////////////////////////////////

class Tilemap
{
public:
	Tilemap();
	virtual ~Tilemap();

	bool initialize(uint8_t tile_row_count, uint8_t tile_column_count);
	void push(
		uint8_t tile_index,
		uint8_t palette, uint8_t bank,
		bool flip_horizontal, bool flip_vertical,
		bool priority);

private:
	struct Parameter
	{
		uint8_t palette_index;
		uint8_t bank;
		bool flip_horizontal;
		bool flip_vertical;
		bool priority;
	};

private:
	std::vector<uint8_t> m_tile_indices;
	std::vector<Parameter> m_tile_parameters;

	uint32_t m_row_count;
	uint32_t m_column_count;
};

