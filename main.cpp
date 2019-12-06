#include <iostream>
#include <string>
#include <iomanip>
#include <bitset>
#include <fstream>

#include <algorithm>
#include <execution>

#include "ByteStream.h"
#include "ByteStreamExplainer.h"
#include "CompressableByteStream.h"

#include "FileCompressor.h"
#include "hamming_7_4.h"

// #include "auto_ai.h"

#include "cas.h"
#include "cas_operation_impl.h"

int main(int argc, char* argv[])
{
	/*
	CompressableByteStream<uint16_t> bs;
	ByteStreamExplainer<uint16_t> bse(bs);

	std::string bo("bonjour je suis beau");
	bse.addExplanation(std::string("string: ").append(bo).c_str(), sizeof(bo));
	bs << bo;

	bse.addExplanation("the number 42");
	bs << 42;

	bse.addExplanation("the float 123.45");
	bs << 123.45;

	// Export to file
	std::ofstream ofs("str", std::ios::binary);
	bse.export_bs().ostream(ofs);
	ofs.close();

	// Import from file
	ByteStreamExplainer<uint16_t> bse2("BSE2", bs);	// create an explainer, and bind it to the bytestream we want to analyze
	std::ifstream ifs("str", std::ios::binary);		// open the file
	ByteStream<uint16_t> data_fromfile(ifs);		// load it in a bytestream
	bse2.import_bs(data_fromfile);					// import data from the "file" bytestream

	bs.seekg(12);									// Set pos somewhere in the ByteStream

	std::cout << std::hex << (int)* (bs.ptr_here<char>()) << "\t" << std::bitset<8>(*bs.ptr_here<char>()) << "\t" << *bs.ptr_here<char>() << std::endl;

	std::cout << std::endl << bse.explain();
	std::cout << std::endl << bse2.explain();

	std::cout << std::endl;
	*/

	/*
	if (!FileCompressor::Compress("test.txt", "test1.txt")) std::cout << "Error1" << std::endl;
	if (!FileCompressor::Decompress("test1.txt", "test2.txt")) std::cout << "Error2" << std::endl;
	*/

	/*
	{
		int a[] = { 0,1,2,3,4,5,6,7,8,9,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

		std::atomic_char32_t c = 0;
		//uint32_t c = 0;

		std::vector<int> v;
		std::for_each(std::execution::par, std::begin(a), std::end(a), [&](int i) {
			std::cout << c << c << " " << std::endl;
			c++;
		});

		std::cout << c << std::endl;
	}
	*/

	/*
	{
		int x = 1; float y = 1.00000001f;
		std::cout << std::boolalpha << "Can you compare int and float ? " << palace::has_operator::op_superior<int, float>::value << std::endl;
		std::cout << "X > Y ? " << std::boolalpha << (x > y) << std::endl;

		class not_comparable {};
		not_comparable a, b;
		std::cout << std::boolalpha << "Can you compare 'not_comparable' ? " << palace::has_operator::op_superior<not_comparable>::value << std::endl;

	}
	*/



}
