#pragma once

enum Result : uint32_t
{
	kSuccess,
	kCallbackEarlyReturn,

	kError_MetatileDimensionsMustNotBeZero,
	kError_MetatileHeightMustBeMultipleOf16,
	kError_ImageDimensionsMustBeMultiplesOfMetatileDimensions,
	kError_MetatileDimensionsMustBeMultiplesOfTileSize,

	kError_TooManyColorInPalette,

	kResult_Count,
};

