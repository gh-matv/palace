#pragma once

#include <vector>
#include <tuple>

#include "lz4.h"

namespace palace
{
	class Compressor
	{
	public:

		static std::tuple<size_t, std::vector<uint8_t>> compress(std::vector<uint8_t>& data)
		{
			std::vector<uint8_t> outp;
			outp.resize(LZ4_compressBound(data.size()));

			auto sz = LZ4_compress_default((char*)data.data(), (char*)outp.data(), data.size(), outp.size());

			outp.resize(sz);
			outp.shrink_to_fit();

			return std::make_tuple(/*compressed size*/sz, /*bytes*/outp);
		}

		static void decompress(std::vector<uint8_t>& data, std::vector<uint8_t>& dest, size_t original_size)
		{
			std::vector<uint8_t> outp;
			outp.resize(original_size);

			auto res = LZ4_decompress_safe((char*)data.data(), (char*)dest.data(), data.size(), original_size + 1);
		}
	};
}
