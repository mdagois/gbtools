#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

//enum : uint32_t
//{
//	kTileSize = 8,
//	kColorsPerPalette = 4,
//	kPaletteMaxCount = 8,
//	kVramBankCount = 2,
//	kTilesPerVramBank = 256,
//	kTilesMaxCount = kTilesPerVramBank * kVramBankCount,
//	kTilemapIndexMaxCount = 1024,
//
//	kColorIndex_Invalid = 0xFFFFFFFFU,
//	kPaletteIndex_Invalid = 0xFFFFFFFFU,
//};

////////////////////////////////////////////////////////////////////////////////
// Tilemap
////////////////////////////////////////////////////////////////////////////////

//struct Tilemap
//{
//	vector<uint8_t> indices;
//	vector<uint8_t> parameters;
//};
//
//static bool extractTilemaps(Tilemap& out_tilemap, const Tileset& tileset, const PaletteSet& palette_set, const Image& image)
//{
//	out_tilemap.indices.clear();
//	out_tilemap.parameters.clear();
//
//	const bool success = iterateImageTiles(
//		image,
//		[&image, &palette_set, &out_tilemap, &tileset](const ColorRGBA* tile_pixels, uint32_t tile_column, uint32_t tile_row)
//		{
//			Tile tile;
//			if(!extractTile(tile, palette_set, tile_pixels, image.getWidth()))
//			{
//				cout << "Could not extract tile (" << tile_column << "," << tile_row << ")" << endl;
//				return false;
//			}
//
//			const auto index_it = tileset.flipToIndex.find(tile.flips[kTileFlipType_None]);
//			if(index_it == tileset.flipToIndex.end())
//			{
//				cout << "Could not find a corresponding tile in the tileset" << endl;
//				return false;
//			}
//
//			const TileFlipIndex tile_flip_index = index_it->second;
//			const size_t index = tile_flip_index.index;
//			const uint8_t tile_index = index % kTilesPerVramBank;
//			out_tilemap.indices.push_back(tile_index);
//
//			const uint8_t palette_number = tile.palette_index & 0x7;
//			const uint8_t vram_bank = (index / kTilesPerVramBank) & 0x1;
//			const uint8_t flip_type = tile_flip_index.flip_type;
//
//			const uint8_t parameters =
//				palette_number |
//				(vram_bank << 3) |
//				(flip_type << 5);
//			out_tilemap.parameters.push_back(parameters);
//			return true;
//		});
//	if(!success)
//	{
//		return false;
//	}
//	const size_t index_count = out_tilemap.indices.size();
//	assert(index_count == out_tilemap.parameters.size());
//	if(index_count > kTilemapIndexMaxCount)
//	{
//		cout << "Too many indices in the tilemap (" << index_count << " > " << kTilemapIndexMaxCount << ")" << endl;
//		return false;
//	}
//	return true;
//}

