#include <cassert>
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

	kBaseAddress = 0x4000U,
	kBankSize = 16U * 1024U,
};

//////////////////////////////////////////////////////////////////////////////

struct Column
{
	uint32_t num = 0;
	uint32_t start_entry = 0;
	uint32_t entry_count = 0;
};

struct Point
{
	int32_t x = 0;
	int32_t y = 0;
};

struct Entry
{
	uint32_t column;
	Point left;
	Point right;
};

struct Data
{
	std::vector<Column> columns;
	std::vector<Entry> entries;
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
			current_column.entry_count = static_cast<uint32_t>(out_data.entries.size()) - current_column.start_entry;
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
			const uint32_t entry_count = static_cast<uint32_t>(out_data.entries.size());

			if(column_num != current_column.num)
			{
				addColumn();
				current_column.num = column_num;
				current_column.start_entry = entry_count;
			}

			Point leftmost;
			leftmost.x = kTileWidth;
			Point rightmost;
			rightmost.x = -1;
			bool register_entry = false;
			for(uint32_t i = 0; i < kTileWidth * kTileHeight; ++i)
			{
				const ColorRGBA color = tile[i];
				if(color == kRGBA_Magenta)
				{
					continue;
				}
				assert(color == kRGBA_Black);
				register_entry = true;

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
			if(register_entry)
			{
				Entry entry;
				entry.column = column_num;
				entry.left = leftmost;
				entry.right = rightmost;
				out_data.entries.push_back(entry);
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
	constexpr uint32_t kEntrySize = 1;
	constexpr uint32_t kTableEntrySize = 3;

	const uint32_t kTableSize = kTableEntrySize * static_cast<uint32_t>(data.columns.size());
	for(const Column& column : data.columns)
	{
		assert(column.entry_count < 256);
		const uint8_t entry_count = static_cast<uint8_t>(column.entry_count);
		fwrite(&entry_count, sizeof(entry_count), 1, out_file);
		
		assert(kTableSize + column.start_entry * kEntrySize < kBankSize);
		const uint16_t start_entry_address = static_cast<uint16_t>(kTableSize + column.start_entry * kEntrySize);
		fwrite(&start_entry_address, sizeof(start_entry_address), 1, out_file);
	}
	return true;
}

static bool writeEntries(FILE* out_file, const Data& data, const Options options)
{
	//TODO Write entries
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
	else if(!writeEntries(file, data, options))
	{
		std::cout << "Could not write entries to [" << options.output.filename << "]" << std::endl;
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
	std::cout << "# entries: " << data.entries.size() << std::endl;
	std::cout << "Entries per column: " << (static_cast<double>(data.entries.size()) / data.columns.size()) << std::endl;

	if(!write(data, options))
	{
		std::cout << "Could not write data [" << options.output.filename << "]" << std::endl;
		return 1;
	}

	return 0;
}

