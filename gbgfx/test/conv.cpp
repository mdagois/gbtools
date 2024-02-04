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
	if(!image.iterateTiles(
		0, kIterateAllRows,
		[&tile_count](const TileRGBA& tile, uint32_t x, uint32_t y)
		{
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

