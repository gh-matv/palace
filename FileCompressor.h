#pragma once

#include <string>
#include <fstream>

#include "CompressableByteStream.h"

namespace palace
{
	class FileCompressor
	{
	public:

		static bool Compress(std::string src, std::string target)
		{
			std::ifstream ifs(src, std::ios::binary | std::ios::ate);
			if (!ifs.is_open())
				return false;

			CompressableByteStream<uint16_t> cbs;
			cbs.resize(ifs.tellg());
			ifs.seekg(0, std::ios::beg);

			// Manually copy
			ifs.read((char*)cbs.data(), cbs.size());

			ifs.close();

			std::ofstream ofs(target, std::ios::binary);
			if (!ofs.is_open())
				return false;

			cbs.ostream(ofs);
		}

		static bool Decompress(std::string src, std::string target)
		{
			std::ifstream ifs(src, std::ios::binary);
			if (!ifs.is_open())
				return false;

			CompressableByteStream<uint16_t> cbs(ifs);
			ifs.close();

			std::ofstream ofs(target, std::ios::binary);
			if (!ofs.is_open())
				return false;

			ofs.write((char*)cbs.data(), cbs.size());
		}

	};
}
