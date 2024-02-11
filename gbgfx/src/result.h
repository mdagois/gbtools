#pragma once

//TODO Turn Result into object
//TODO Add error message 
//TODO Add namespace
//TODO Move constants to constants.h
//TODO Add verbose mode with error callback

enum Result : uint32_t
{
	kSuccess,
	kCallbackEarlyReturn,

	kError_CouldNotReadFile,
	kError_CouldNotFindCompatiblePalette,
	kError_CouldNotFindColorInPalette,

	kError_MetatileDimensionsMustNotBeZero,
	kError_MetatileHeightMustBeMultipleOf16,
	kError_ImageDimensionsMustBeMultiplesOfMetatileDimensions,
	kError_MetatileDimensionsMustBeMultiplesOfTileSize,

	kError_TooManyColorInPalette,

	kError_CouldNotWriteTilesetToFile,

	kResult_Count,
};

