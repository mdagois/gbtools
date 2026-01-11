#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "gfx.h"
#include "import/import.h"
#include "options.h"

//////////////////////////////////////////////////////////////////////////////

using namespace gfx;

//////////////////////////////////////////////////////////////////////////////

enum : uint32_t
{
	kInvalidColumnNum = 0xFFFFFFFFU,
	kColumnWidth = 8U,
	kTileWidth = 8U,
	kTileHeight = 8U,
	kMaxHeight = 512U,

	kBaseAddress = 0x4000U,
	kBankByteSize = 16U * 1024U,
	kTableEntryByteSize = 4,
	kBoxByteSize = 4,
};

//////////////////////////////////////////////////////////////////////////////

struct Column
{
	uint32_t num = 0;
	uint32_t start_box = 0;
	uint32_t box_count = 0;
};

struct Point
{
	int32_t x = 0;
	int32_t y = 0;
};

struct Box
{
	uint32_t column;
	Point left;
	Point right;
};

struct Data
{
	std::vector<Column> columns;
	std::vector<Box> boxes;
};

//////////////////////////////////////////////////////////////////////////////

static bool read(Data& out_data, Options options)
{
	Image image;
	if(!image.read(options.input.filename))
	{
		return false;
	}

	if(options.input.divisions.empty())
	{
		options.input.divisions.push_back({ kColumnWidth, image.getHeight(), false });
		options.input.divisions.push_back({ kTileWidth, kTileHeight, true });
	}

	Column current_column;
	current_column.num = kInvalidColumnNum;

	auto addColumn = [&out_data, &current_column]()
	{
		if(current_column.num != kInvalidColumnNum)
		{
			uint32_t column_count = static_cast<uint32_t>(out_data.columns.size());
			while(column_count < current_column.num)
			{
				Column column;
				column.num = column_count;
				out_data.columns.push_back(column);
				++column_count;
			}
			assert(out_data.columns.size() == current_column.num);
			current_column.box_count = static_cast<uint32_t>(out_data.boxes.size()) - current_column.start_box;
			out_data.columns.push_back(current_column);
		}
	};

	int32_t last_column_num = 0;
	DivisionInfo division_info;
	Rectangle rectangle;
	if(!image.iterateTiles(
		division_info,
		options.input.divisions.data(),
		static_cast<uint32_t>(options.input.divisions.size()),
		rectangle,
		[&out_data, &current_column, &addColumn](const ImageTile& tile, uint32_t x, uint32_t y)
		{
			assert(tile.getWidth() == kTileWidth && tile.getHeight() == kTileHeight);
			assert(tile.getWidth() % kTileWidth == 0);

			const uint32_t column_num = x / kTileWidth;
			const uint32_t box_count = static_cast<uint32_t>(out_data.boxes.size());

			if(column_num != current_column.num)
			{
				addColumn();
				current_column.num = column_num;
				current_column.start_box = box_count;
			}

			Point leftmost;
			leftmost.x = kTileWidth;
			Point rightmost;
			rightmost.x = -1;
			bool register_box = false;
			for(uint32_t i = 0; i < kTileWidth * kTileHeight; ++i)
			{
				const ColorRGBA color = tile[i];
				if(color == kRGBA_Magenta)
				{
					continue;
				}
				assert(color == kRGBA_Black);
				register_box = true;

				const int32_t px = i % kTileWidth;
				const int32_t py = i / kTileWidth;
				if(px < leftmost.x)
				{
					leftmost.x = px;
					leftmost.y = py;
				}
				if(px > rightmost.x)
				{
					rightmost.x = px;
					rightmost.y = py;
				}
			}
			if(register_box)
			{
				Box box;
				box.column = column_num;
				box.left = leftmost;
				box.right = rightmost;
				out_data.boxes.push_back(box);
			}

			return true;
		}))
	{
		return false;
	}

	addColumn();
	assert(static_cast<uint32_t>(out_data.columns.size()) == image.getWidth() / kTileWidth);

	return true;
}

//////////////////////////////////////////////////////////////////////////////

static bool writeTable(FILE* out_file, const Data& data, const Options options)
{
	const uint32_t kTableSize = kTableEntryByteSize * static_cast<uint32_t>(data.columns.size());
	for(const Column& column : data.columns)
	{
		assert(column.box_count < 256);
		const uint8_t box_count = static_cast<uint8_t>(column.box_count);
		fwrite(&box_count, sizeof(box_count), 1, out_file);
		
		const uint32_t relative_start_address = kTableSize + column.start_box * kBoxByteSize;
		assert(relative_start_address < kBankByteSize);
		const uint16_t start_box_address = static_cast<uint16_t>(kBaseAddress + relative_start_address);
		fwrite(&start_box_address, sizeof(start_box_address), 1, out_file);

		const uint8_t dummy = 0U;
		fwrite(&dummy, sizeof(dummy), 1, out_file);
	}
	return true;
}

static bool writeBoxes(FILE* out_file, const Data& data, const Options options)
{
	for(const Box& box : data.boxes)
	{
		assert(box.left.y < kMaxHeight);

		const uint8_t height_low = static_cast<uint8_t>(box.left.y & 0xFF);
		const uint8_t height_high = static_cast<uint8_t>((box.left.y >> 8) && 0x1);
		assert(box.left.x < kTileWidth);
		const uint8_t left = static_cast<uint8_t>(box.left.x & 0x7);
		assert(box.right.x < kTileWidth);
		const int32_t left_right_diff = box.right.x - box.left.x;
		assert(left_right_diff >= 0);
		const uint8_t diff = static_cast<uint8_t>((left_right_diff) & 0x7);
		const double slope = left_right_diff == 0 ? 0 : (box.right.y - box.left.y) / left_right_diff;
		assert(slope > -8.0 && slope < 8.0);
		const int16_t slope_integral = static_cast<int16_t>(round(slope * 256.0));

		uint8_t bytes[4];
		bytes[0] = height_low;
		bytes[1] = height_high | (left << 1) | (diff << 4);
		bytes[2] = static_cast<uint8_t>(slope_integral & 0xFF);
		bytes[3] = static_cast<uint8_t>((slope_integral >> 8) & 0xFF);

		fwrite(bytes, sizeof(bytes), 1, out_file);
	}
	return true;
}

static bool write(const Data& data, const Options options)
{
	FILE* file = fopen(options.output.filename, "wb");
	bool result = true;
	if(!writeTable(file, data, options))
	{
		std::cout << "Could not write table to [" << options.output.filename << "]" << std::endl;
		result = false;
	}
	else if(!writeBoxes(file, data, options))
	{
		std::cout << "Could not write boxes to [" << options.output.filename << "]" << std::endl;
		result = false;
	}
	fclose(file);
	return result;
}

//////////////////////////////////////////////////////////////////////////////

int main(int argc, const char** argv)
{
	Options options;
	bool is_help = 0;
	if(!parseCliOptions(options, is_help, argc, argv))
	{
		return is_help ? 0 : 1;
	}

	gfx::setLogLevel(options.verbose ? gfx::kLogLevel_Info : gfx::kLogLevel_Error);

	Data data;
	if(!read(data, options))
	{
		std::cout << "Could not read data [" << options.input.filename << "]" << std::endl;
		return 1;
	}

	std::cout << "# columns: " << data.columns.size() << std::endl;
	std::cout << "# boxes: " << data.boxes.size() << std::endl;
	std::cout << "Boxes per column: " << (static_cast<double>(data.boxes.size()) / data.columns.size()) << std::endl;

	if(!write(data, options))
	{
		std::cout << "Could not write data [" << options.output.filename << "]" << std::endl;
		return 1;
	}

	return 0;
}

