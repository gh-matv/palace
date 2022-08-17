
#include <iostream>

#include "ByteStream_Test.h"

int main(int argc, char* argv[])
{
	std::cout << "Running test suites" << std::endl;
	std::cout << std::boolalpha;

	std::cout << "ByteStreams: " << ByteStream_Test::run_all_tests() << std::endl;
}
