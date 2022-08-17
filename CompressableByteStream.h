
#pragma once

#include "ByteStream.h"
#include "Compressor.h"

namespace palace
{
	class CompressableByteStream : public ByteStream
	{
	public:

		CompressableByteStream()
		{
			this->pBuffer = new std::vector<uint8_t>(1);
			this->pCurPtr = 0;
		}

		CompressableByteStream(size_t size)
		{
			this->pBuffer = new std::vector<uint8_t>(size);
			this->pCurPtr = 0;
		}

		CompressableByteStream(std::istream& is, bool fromBeg = false)
		{
			if (fromBeg) is.seekg(0, std::ios::beg);
			auto oldStreamPos = is.tellg();

			is.seekg(0, std::ios::end);
			auto end = is.tellg();

			this->pBuffer = new std::vector<uint8_t>(end - oldStreamPos);
			this->pCurPtr = 0;

			is.seekg(oldStreamPos);

			// Decompress here.
			size_t sz_orig, sz_cpr;

			is.read((char*)& sz_orig, sizeof(size_t));	// Read original size
			is.read((char*)& sz_cpr, sizeof(size_t));	// Read compressed size

			std::vector<uint8_t> compressed_data;			//
			compressed_data.resize(sz_cpr);					//	Read the compressed data
			is.read((char*)compressed_data.data(), sz_cpr);	//

			this->pBuffer->resize(sz_orig + 1);
			Compressor::decompress(compressed_data, *this->pBuffer, sz_orig);
		}

		void ostream(std::ostream& ostream)
		{
			size_t sz = this->pBuffer->size();
			ostream.write((char*)& sz, sizeof(size_t)); // Write original size

			std::tuple<size_t, std::vector<uint8_t>> cpr = Compressor::compress(*this->pBuffer);
			ostream.write((char*)& std::get<0>(cpr), sizeof(size_t)); // Write compressed size
			ostream.write((char*)std::get<1>(cpr).data(), std::get<1>(cpr).size()); // Write data itself
		}

	};

}
