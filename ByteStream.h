#pragma once

#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <algorithm>
#include <exception>
#include <iostream>

typedef uint16_t stringlen_t;

namespace palace
{
	class ByteStream;

	template<typename T>
	class ByteStreamPtr {
	public:

		ByteStream* bs;
		size_t offset;

		ByteStreamPtr(ByteStream* bs, size_t offs) : bs(bs), offset(offs) {}

		ByteStream* jumpHere()
		{
			bs->seekg(offset);
			return bs;
		}

		T value()
		{
			return *bs->raw_at<T>(offset);
		}

	};

	class ByteStream
	{
	protected:

		size_t pCurPtr;
		std::vector<uint8_t>* pBuffer;

	public:

		// =================== CONSTRUCTORS =====================

		ByteStream(size_t size = 1)
		{
			pBuffer = new std::vector<uint8_t>(size);
			pCurPtr = 0;
		}

		ByteStream(size_t size, void* buffer_to_copy) : ByteStream(size)
		{
			memcpy(pBuffer->data(), buffer_to_copy, size);
		}


		ByteStream(std::istream& is, bool fromBeg = false)
		{
			if (fromBeg) is.seekg(0, std::ios::beg);
			auto oldStreamPos = is.tellg();

			is.seekg(0, std::ios::end);
			auto end = is.tellg();

			pBuffer = new std::vector<uint8_t>(end - oldStreamPos);
			pCurPtr = 0;

			is.seekg(oldStreamPos);

			is.read((char*)pBuffer->data(), end - oldStreamPos);
		}

		~ByteStream()
		{
			delete pBuffer;
		}

		bool resize(size_t newSize)
		{
			pBuffer->resize(newSize);
			return true;
		}

	protected:

		// =================== INTERNAL HANDLERS =====================

		bool _chk_size(size_t size)
		{
			return pBuffer->size() >= pCurPtr + size;
		}

		template<typename T>
		bool _chk_size_for_type()
		{
			return _chk_size(sizeof(T));
		}

		bool _chk_size_for_string(size_t stringlen)
		{
			return _chk_size(sizeof(stringlen_t) + stringlen);
		}

		void _makespace(size_t additionalSize)
		{
			pBuffer->resize(pCurPtr + additionalSize);
		}

		template<typename T>
		void _makespace_for_simple()
		{
			_makespace(sizeof(T));
		}

		void _makespace_for_string(size_t size)
		{
			_makespace(sizeof(stringlen_t) + size);
		}

		void _advance(size_t size)
		{
			pCurPtr += size;
		}

		template<typename T>
		void _advance()
		{
			_advance(sizeof(T));
		}

		void* _cur_addr()
		{
			return pBuffer->data() + pCurPtr;
		}

		void _unsafe_write_here(const void* data, size_t size)
		{
			memcpy(_cur_addr(), reinterpret_cast<const char*>(data), size);
			_advance(size);
		}

		template<typename T>
		void _unsafe_write_here(const T& data)
		{
			_unsafe_write_here(&data, sizeof(T));
		}

	public:

		// ================== PUBLIC METHODS =========================

		// Avoid using this command
		std::vector<uint8_t> getbuf()
		{
			return *pBuffer;
		}

		// Write arbitrary bytes
		void write(const char* in, size_t size)
		{
			if (!_chk_size(pBuffer->size()))
				_makespace(pBuffer->size());

			_unsafe_write_here(in, size);
		}

		// Write simple types
		template <typename T>
		void write(const T in)
		{
			if (!_chk_size_for_type<T>())
				_makespace_for_simple<T>();

			_unsafe_write_here<T>(in);
		}

		// Write strings
		// For strings, write the length as stringlen_t before the string.
		//  some bytes lost in memory, but no need to loop over every char to find
		//  the incoming \0 at the end.
		template<>
		void write<std::string>(const std::string input)
		{
			stringlen_t strSize = input.size();

			if (!_chk_size_for_string(strSize))
				_makespace_for_string(strSize);

			// Write the size of the string
			_unsafe_write_here<stringlen_t>(strSize);

			// Write the string itself
			_unsafe_write_here(input.data(), input.size());
		}

		// Operator write
		template <typename T>
		ByteStream& operator<< (const T& input)
		{
			write<T>(input);
			return *this;
		}

		ByteStream& operator<< (const char* input)
		{
			write(std::string(input));
			return *this;
		}

		void read(void* outbuf, size_t size)
		{
			if (!_chk_size(size))
				throw std::out_of_range("The bytestream is too small to contain that element at this position.");

			memcpy(outbuf, _cur_addr(), size);
			_advance(size);
		}

		// Read simple type
		template <typename T>
		T read()
		{
			T out;
			read(&out, sizeof(T));
			return out;
		}

		template<>
		std::string read<std::string>()
		{
			std::string outp;

			// Get the size of the string, and resize outp
			stringlen_t strSize = read<stringlen_t>();
			outp.resize(strSize);

			if (!_chk_size(strSize))
				throw std::out_of_range("The bytestream is too small to contain a string that long.");

			memcpy((void*)outp.data(), _cur_addr(), strSize);
			_advance(strSize);

			return outp;
		}

		// operator for reading simple types
		template <typename T>
		void operator>> (T& outp)
		{
			outp = read<T>();
		}

		// Returns a pointer to memory pointed by an offset given by tellg()
		//  Do not store the pointer address, since the object base offset can move
		//  if the container is resized
		template <typename T>
		T* raw_at(size_t offset)
		{
			return (T*)(pBuffer->data() + offset);
		}

		void seekg(int32_t newPos, std::ios_base::seekdir way = std::ios::beg)
		{

			switch (way)
			{

			case std::ios::beg:
				pCurPtr = newPos;
				return;

			case std::ios::cur:
				_advance(newPos);
				return;

			case std::ios::ate:
			case std::ios::end:
				pCurPtr = pBuffer->size() + newPos;
				return;

			default:
				return;
			}
		}

		size_t tellg()
		{
			return pCurPtr;
		}

		size_t find(std::vector<uint8_t> pattern)
		{
			auto first = std::search(
				pBuffer->begin() + pCurPtr,
				pBuffer->end(),
				pattern.begin(),
				pattern.end()
			);

			return std::distance(pBuffer->begin(), first) - 1 /* just before the beginning */;
		}

		size_t find_backwards(std::vector<uint8_t> pattern)
		{
			auto first = std::search(
				pBuffer->rbegin() + (size() - pCurPtr),
				pBuffer->rend(),
				pattern.rbegin(),
				pattern.rend()
			);

			return size() - std::distance(pBuffer->rbegin(), first) /* just before the beginning */;
		}

		bool foreach_bytes(size_t pos_beg, size_t pos_end, std::function<void(uint8_t&)> callback)
		{
			auto len = pos_end - pos_beg;
			if (len < 0) return false;

			std::for_each(
				pBuffer->begin() + pos_beg,
				pBuffer->begin() + pos_end,
				callback
			);

			return true;
		}

		bool skip(size_t size)
		{
			if (!_chk_size(size))
				return false;

			_advance(size);

			return true;
		}

		uint8_t* data()
		{
			return pBuffer->data();
		}

		size_t size()
		{
			return pBuffer->size();
		}

		uint8_t operator[](size_t offset)
		{
			return pBuffer->data()[offset];
		}

		// Avoid, for same reason as ptr_to, but not deprecated.
		template<typename T>
		T* raw_ptr_here()
		{
			return (T*)_cur_addr();
		}

		template<typename T>
		ByteStreamPtr<T> this_place()
		{
			return ByteStreamPtr<T>{ this, tellg() };
		}

		void ostream(std::ostream& ostream)
		{
			ostream.write((char*)data(), pBuffer->size());
		}

		size_t size_remaining()
		{
			return pBuffer->size() - ((size_t)pCurPtr - (size_t)pBuffer->data());
		}

	};
}