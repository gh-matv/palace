#pragma once

#include <string>
#include <iostream>

#include "ByteStream.h"
#include "ByteStreamExplainer.h"

#define IS_SUCCESS(cond) { 	bool result = (cond);							\
if (!result) std::cout << __FUNCTION__ " failed !" << std::endl;			\
return result;}

class ByteStream_Test {

	static bool test_ctor()
	{
		palace::ByteStream bs_zero, bs_one(100);
		IS_SUCCESS(bs_zero.size() == 1 && bs_one.size() == 100);
	}

	static bool test_resize()
	{
		palace::ByteStream bs(10);
		bs << 1 << 1 << 1 << 1; // 4 ints should make it >16 in len

		palace::ByteStream bsa(10);
		bsa.resize(100);

		IS_SUCCESS(bs.size() >= 16 && bsa.size() >= 100);
	}

	static bool test_use_struct()
	{
		struct { int a; int b; int c; int d; } x{ 0, 1, 2, 3 }, y;

		palace::ByteStream bs(100);
		bs << "test";
		bs << x;
		bs << 42;

		bs.seekg(0, std::ios::beg);
		bs.read<std::string>(); // skip the test string

		bs >> y;

		IS_SUCCESS(x.a == y.a && x.b == y.b && x.c == y.c && x.d == y.d);
	}

	static bool test_this_place()
	{
		palace::ByteStream bs;
		bs << 1 << 2 << 3;

		bs.seekg(-1*sizeof(1/*should be int in x86*/), std::ios::cur); // go back one int
		auto here = bs.this_place<int>();

		bs.seekg(0, std::ios::ate);
		bs << 4 << 5 << 6;
				
		IS_SUCCESS((here.value() == 3));
	}

	static bool test_bytestream_explainer()
	{
		palace::ByteStream bs;
		palace::ByteStreamExplainer bse(bs);

		bs << 1, 2, 3;

		struct { int a; int b; } x{ 1, 2 };
		bse.addExplanation("a custom struct", sizeof(x));
		bs << x;

		bs.seekg(0);

		bs.read<decltype(1)>();
		bs.read<decltype(1)>();
		bs.read<decltype(1)>();

		//                                 current searched position
		//                                 |            position of the closest found element
		//                                 |            |            offset between searched and found
		//                                 |            |            |          description
		IS_SUCCESS(bse.explain() == "[BSE] [!SEARCH:12] [FOUND AT:4] (OFFSET:8): a custom struct");
	}

public:

	static bool run_all_tests()
	{
		return test_ctor() && test_resize() && test_use_struct() && test_this_place()
			&& test_bytestream_explainer();
	}

};
