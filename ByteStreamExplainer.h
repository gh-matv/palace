#pragma once

#include <map>
#include <sstream>
#include <iomanip>
#include <conio.h>

#include "ByteStream.h"

namespace palace
{
	class ByteStreamInformation
	{
	public:

		std::string strInformation;
		size_t data_length;

		ByteStreamInformation() : strInformation(""), data_length(NULL) {}
		ByteStreamInformation(std::string information) : strInformation(information), data_length(NULL) {}
		ByteStreamInformation(std::string information, size_t len) : strInformation(information), data_length(len) {}

		std::string string()
		{
			return strInformation;
		}
	};

	template<typename bytestream_stringsize_type = uint16_t>
	class ByteStreamExplainer
	{
		std::string stream_name = "";
		ByteStream<bytestream_stringsize_type>* m_BS;

		std::map<size_t, ByteStreamInformation> m_Expl;
		std::map<size_t, void*> m_Data;

		std::stringstream identify()
		{
			std::stringstream ss;
			ss << "[BSE] ";

			if (!stream_name.empty())
				ss << '(' << stream_name << ") ";

			return ss;
		}

	public:

		ByteStreamExplainer(const char* streamname, ByteStream<bytestream_stringsize_type>& bs)
		{
			m_BS = &bs;
			stream_name = streamname;
		}

		ByteStreamExplainer(ByteStream<bytestream_stringsize_type>& bs)
		{
			m_BS = &bs;
		}

		void addExplanation(std::string explanation, size_t data_size = NULL)
		{
			ByteStreamInformation bsi(explanation, data_size);
			m_Expl[m_BS->tellg()] = bsi;
		}

		std::string explainPrevious()
		{
			auto ss = identify();

			if (m_Expl.size() == 1 && m_Expl.begin()->first >= m_BS->tellg())
			{
				ss << "No explanation found";
				return ss.str();
			}

			auto upper_bound = m_Expl.lower_bound(m_BS->tellg());

			if (upper_bound == m_Expl.end() && std::prev(upper_bound, 1)->first > m_BS->tellg())
			{
				// Touched end() and still bigger. Should rarely happen
				ss << "No explanation found";
				return ss.str();
			}

			upper_bound--;
			if (upper_bound != m_Expl.end())
				ss << "[!SEARCH:" << m_BS->tellg()
				<< "] [FOUND AT:" << upper_bound->first
				<< "] (OFFSET:" << m_BS->tellg() - upper_bound->first
				<< "): " << upper_bound->second.string();

			return ss.str();
		}

		std::string explain()
		{
			auto searchingPos = m_BS->tellg();

			if (m_Expl.find(m_BS->tellg()) != m_Expl.end())
			{
				auto ss = identify();

				ss << "[POS:" << searchingPos << "] "
					<< m_Expl[searchingPos].string();

				return ss.str();
			}

			return explainPrevious();
		}

		ByteStream<bytestream_stringsize_type> export_bs()
		{
			ByteStream<bytestream_stringsize_type> bs;

			// Write size of the explanation
			bs.write<size_t>(m_Expl.size());

			// Write each element
			for (auto me : m_Expl)
			{
				bs.write<size_t>(me.first);
				bs << me.second.string();
			}

			return bs;
		}

		void import_bs(ByteStream< bytestream_stringsize_type>& bs)
		{
			size_t counter = bs.read<size_t>();

			for (auto i = 0; i != counter; ++i)
			{
				auto left = bs.read<size_t>();

				std::string right;
				bs >> right;

				m_Expl[left] = right;
			}
		}

	};

}
