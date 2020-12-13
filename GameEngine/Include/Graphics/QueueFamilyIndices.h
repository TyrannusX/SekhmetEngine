#pragma once
#include <iostream>
#include <optional>

namespace SekhmetEngine
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentationFamily;
	};
}