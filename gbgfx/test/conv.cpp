#include <cassert>
#include <iostream>
#include "gbgfx.h"

using namespace std;

int main(int argc, const char** argv)
{
	const char* image_filename = "test/demo.png";

	Image image;
	if(!image.read(image_filename))
	{
		cout << "Cannot read file" << endl;
		return 1;
	}
	cout
		<< "Loaded [" << image_filename << "] "
		<< image.getWidth() << "x" << image.getHeight()
		<< endl;

	uint32_t tile_count = 0;
	if(kSuccess != image.iterateTiles(
		0, kIterateAllRows, kTileSize * 2, kTileSize * 2,
		true,
		[&tile_count](const ImageTile& tile, uint32_t x, uint32_t y)
		{
			Palette palette;
			if(kSuccess != extractTilePalette(palette, tile))
			{
				assert(false);
			}
			cout << tile_count << " -> " << palette.getColorCount() << endl;
			++tile_count;
			return true;
		}))
	{
		cout << "Error on tile" << endl;
		return 1;
	}

	cout << "Tile count: " << tile_count << endl;

	return 0;
}

