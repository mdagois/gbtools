#include <cassert>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

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
	kTableEntryByteSize = 2,
	kBoxByteSize = 6,

	kCollisionNone = 0x08,
	kMaxSegmentCount = 256U,
	kInvalidColorIndex = 0xFFFFFFFFU,
};

//////////////////////////////////////////////////////////////////////////////

struct Column
{
	uint32_t num = 0;
	uint32_t start_box = 0;
	uint32_t box_count = 0;
};

struct Box
{
	uint32_t column;
	uint32_t height;
	uint32_t color_id;
	uint8_t offsets[kColumnWidth];
	bool is_last_in_column;
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
		if(current_column.num == kInvalidColumnNum)
		{
			return;
		}

		uint32_t column_count = static_cast<uint32_t>(out_data.columns.size());
		while(column_count < current_column.num)
		{
			Column column;
			column.num = column_count;
			out_data.columns.push_back(column);
			++column_count;
		}
		assert(out_data.columns.size() == current_column.num);
		Box& last_box = out_data.boxes.back();
		last_box.is_last_in_column = true;
		current_column.box_count = static_cast<uint32_t>(out_data.boxes.size()) - current_column.start_box;
		out_data.columns.push_back(current_column);
	};

	uint32_t segment_count = 0;
	ColorRGBA segment_colors[kMaxSegmentCount];

	DivisionInfo division_info;
	Rectangle rectangle;
	if(!image.iterateTiles(
		division_info,
		options.input.divisions.data(),
		static_cast<uint32_t>(options.input.divisions.size()),
		rectangle,
		[&](const ImageTile& tile, uint32_t x, uint32_t y)
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

			Box box;
			box.column = column_num;
			box.height = y;
			box.is_last_in_column = false;
			for(uint32_t col = 0; col < kTileWidth; ++col)
			{
				box.offsets[col] = kCollisionNone;
			}

			bool found = false;
			ColorRGBA segment_color = kRGBA_Magenta;
			for(uint32_t col = 0; col < kTileWidth; ++col)
			{
				for(uint32_t row = 0; row < kTileHeight; ++row)
				{
					const uint32_t index = row * kTileWidth + col;
					const ColorRGBA color = tile[index];
					if(color == kRGBA_Magenta)
					{
						continue;
					}

					box.offsets[col] = row;
					segment_color = color;
					found = true;
					break;
				}
			}
			if(found)
			{
				assert(segment_color != kRGBA_Magenta);
				uint32_t box_color_index = kInvalidColorIndex;
				for(uint32_t i = 0; i < segment_count; ++i)
				{
					if(segment_colors[i] == segment_color)
					{
						box_color_index = i;
						break;
					}
				}
				if(box_color_index == kInvalidColorIndex)
				{
					if(segment_count >= kMaxSegmentCount)
					{
						std::cout << "Too many segment colors used" << std::endl;
						return false;
					}
					segment_colors[segment_count] = segment_color;
					box_color_index = segment_count;
					++segment_count;
				}
				assert(box_color_index != kInvalidColorIndex);

				box.color_id = box_color_index;
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
		if(column.box_count == 0)
		{
			std::cout << "No segment found in column " << column.num << std::endl;
			return false;
		}
		const uint32_t relative_start_box_address = kTableSize + column.start_box * kBoxByteSize;
		assert(relative_start_box_address < kBankByteSize);
		const uint16_t absolute_start_box_address = static_cast<uint16_t>(kBaseAddress + relative_start_box_address);
		fwrite(&absolute_start_box_address, sizeof(absolute_start_box_address), 1, out_file);
	}
	return true;
}

static bool writeBoxes(FILE* out_file, const Data& data, const Options options)
{
	for(const Box& box : data.boxes)
	{
		assert(box.height < kMaxHeight);
		assert(box.height % kTileHeight == 0);

		uint8_t bytes[kBoxByteSize];
		bytes[0] = box.height >> 1;
		bytes[1] = box.color_id;
		bytes[2] = (box.offsets[1] << 4) | box.offsets[0];
		bytes[3] = (box.offsets[3] << 4) | box.offsets[2];
		bytes[4] = (box.offsets[5] << 4) | box.offsets[4];
		bytes[5] = (box.offsets[7] << 4) | box.offsets[6];

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

