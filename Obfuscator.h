
#pragma once

#define OBFUSCATOR_ENDMARK "\0\1\2\1\0"
#define OBFUSCATOR_BEGMARK "\7\7\7"

#define OBFUSCATOR_ENDMARK_SZ sizeof(OBFUSCATOR_ENDMARK)
#define OBFUSCATOR_BEGMARK_SZ sizeof(OBFUSCATOR_BEGMARK)

// Displays a warning in the console when trying to encrypt data bigger
//  than this number of bytes. Set to 0 to disable.
#ifndef OBFUSCATOR_WARN_THRESHOLD
#define OBFUSCATOR_WARN_THRESHOLD 100
#endif

// 0 = executable to protect (file distributed to clients)
// 1 = executable that will transform the exe to protect (NOT released to clients)
//#define OBFUSCATOR_MODE 1

#ifndef OBFUSCATOR_MODE
#define OBFUSCATOR_MODE 2 // Trigger the error if not defined.
#endif

#if OBFUSCATOR_MODE == 0

	#pragma message("OBFUSCATOR_MODE SET TO 'CLIENT'.");
	#define OBFUSCATOR_MODE_CLIENT
	#define OBFUSCATE(x) (palace::Obfuscator::Decypher( OBFUSCATOR_BEGMARK x OBFUSCATOR_ENDMARK ))

#elif OBFUSCATOR_MODE == 1

	#pragma message("OBFUSCATOR_MODE SET TO 'SERVER'. THIS FILE MUST NOT BE SERVED TO CLIENTS.");
	#define OBFUSCATOR_MODE_SERVER
	#include "ByteStream.h"

#else

	#error OBFUSCATOR_MODE should be set to 0(Client) or 1(Server).

#endif

#include <string>
#include <functional>
#include <fstream>
#include <map>

namespace palace
{
	class Obfuscator
	{

#ifdef OBFUSCATOR_MODE_CLIENT
	private:

		static std::string& _default_decypher_method(std::string& inp)
		{
			// Decreases all values by one.
			for (auto& c : inp) --c;
			return inp;
		}

		inline static std::function<std::string & (std::string&)> vDecypher = NULL;

	public:

		static void SetDecypher(std::function<std::string & (std::string&)> func)
		{
			vDecypher = func;
		}

		static std::string Decypher(const char* dec)
		{
			std::string s(dec);
			if (vDecypher == NULL)
				return _default_decypher_method(s);
			return vDecypher(s);
		}
#endif
#ifdef OBFUSCATOR_MODE_SERVER
	private:

		inline static std::function<void(uint8_t * offsetRead, uint8_t * offsetWrite, size_t sz)> vCypher;

		static void _default_cypher_method(uint8_t* ptrRead, uint8_t* ptrWrite, size_t sz)
		{
			for (auto i = 0; i != sz; ++i)
			{
				// Increase all values by one.
				*((char*)ptrWrite + i) = (*((char*)ptrRead + i) + 1);
			}
		}

		static std::vector<uint8_t> from_string(const char* i)
		{
			std::vector<uint8_t> o;
			o.resize(sizeof(i) - 1);

			memcpy(o.data(), i, sizeof(i) - 1);

			return o;
		}

		static bool _obfuscate_element(
			palace::ByteStream<uint16_t>& bs,
			size_t strPosBeg,
			size_t strPosEnd
		)
		{
			auto szToEnc = strPosEnd - strPosBeg;

#if OBFUSCATOR_WARN_THRESHOLD >= 0
			if (szToEnc >= OBFUSCATOR_WARN_THRESHOLD)
				std::cout
				<< "[WARNING] Encrypting "
				<< szToEnc << " bytes." << std::endl;
#endif

			// Print old string
			std::cout << "PosBeg:" << strPosBeg << " PosEnd:" << strPosEnd << " Sz:" << szToEnc << std::endl;
			std::cout << "Before:";
			for (auto i = strPosBeg; i != strPosEnd; ++i)
				std::cout << *bs.at<char>(i);
			std::cout << std::endl;

			// For improved readability :)
			auto StrBegReadPtr = bs.at<uint8_t>(strPosBeg);
			auto StrBegWritePtr = bs.at<uint8_t>(strPosBeg - OBFUSCATOR_BEGMARK_SZ + 1);

			// Replace the content
			if (vCypher == NULL)
				_default_cypher_method(StrBegReadPtr, StrBegWritePtr, szToEnc);
			else
				vCypher(StrBegReadPtr, StrBegWritePtr, szToEnc);

			// Clean up
			for (auto i = strPosEnd - OBFUSCATOR_BEGMARK_SZ + 1; i != strPosEnd + OBFUSCATOR_ENDMARK_SZ; ++i)
			{
				// Todo: fill with random ?
				*bs.at<char>(i) = 0;
			}

			// Print new string
			std::cout << "After :";
			for (auto i = strPosBeg - OBFUSCATOR_BEGMARK_SZ; i != strPosEnd - OBFUSCATOR_BEGMARK_SZ + 1; ++i)
				std::cout << *bs.at<char>(i);
			std::cout << "\nAfter2:" << bs.at<char>(strPosBeg - OBFUSCATOR_BEGMARK_SZ) << std::endl;
			std::cout << "\n" << std::endl;

			return true;
		}

	public:

		static void SetCypher(std::function<void(uint8_t *, uint8_t *, size_t)> alg)
		{
			vCypher = alg;
		}

		static bool Obfuscate(
			std::string _path_in,
			std::string _path_out,
			std::function<void(uint8_t *ptrRead, uint8_t *ptrWrite, size_t sz)> algo = NULL
		)
		{
			std::ifstream ifs(_path_in, std::ios::binary);
			if (!ifs.is_open()) return false;
			palace::ByteStream<uint16_t> bs(ifs, true);

			std::ofstream ofs(_path_out, std::ios::binary);
			if (!ofs.is_open()) return false;

			int counter = 0;

			// Start at the beginning
			bs.seekg(0, std::ios::beg);
			while (true)
			{
				auto strPosEnd = bs.find(from_string(OBFUSCATOR_ENDMARK));
				if (strPosEnd == bs.size() - 1) break;
				bs.seekg(strPosEnd, std::ios::cur);

				auto strPosBeg = bs.find_backwards(from_string(OBFUSCATOR_BEGMARK));
				if (strPosBeg == 0) break;

				if (!_obfuscate_element(bs, strPosBeg, strPosEnd+1))
					return false;

				++counter;
			}

			std::cout << "Encrypted " << counter << " elements !" << std::endl;

			bs.ostream(ofs);
			return true;
		}
#endif
	};
}
