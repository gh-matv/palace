#pragma once
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <format>
#include <iostream>
#include <iomanip>
#include <numeric>

class DataTable {



public:

    template <typename... Args>
    static std::string dyna_print(std::string_view rt_fmt_str, Args&&... args) {
        return std::vformat(rt_fmt_str, std::make_format_args(args...));
    }

    class DataTableProperty {
    public:
        bool display{ true };
        std::string format{ "{}" };
    };

    class DataTableEntry {
    public:
        std::map<std::string, std::string> mValues;
    };

    std::vector<std::string> vTableOrder;
    std::map<std::string, DataTableProperty> mProperties;
    std::vector<DataTableEntry> vEntries;
};

class DataTableRenderer {

protected:
    DataTable* dt;

public:
    DataTableRenderer(DataTable* dt) : dt(dt) {}
    virtual std::string GetStr() { return ""; };
};

class DataTableConsoleRenderer : public DataTableRenderer {

public:
    DataTableConsoleRenderer(DataTable* dt) : DataTableRenderer(dt) {}
    std::string GetStr()
    {
        std::stringstream ss;

        int colcount = 0;

        // Print header
        std::vector<std::string> headers;
        for (const auto& valueOrder : dt->vTableOrder)
            if (!dt->mProperties.count(valueOrder) || dt->mProperties.at(valueOrder).display)
            {
                // ss << valueOrder << "\t";
                headers.push_back(valueOrder);
                ++colcount;
            }
        ss << std::endl;

        std::vector<size_t> maxLenOfColumn(colcount);
        std::vector<std::vector<std::string>> tmpString;

        // Compute the lengths and store the strings into tmpString
        int linenum = 0;
        for (const auto& entry : dt->vEntries)
        {
            tmpString.push_back({});

            int colnum = 0;
            for (const auto& valueOrder : dt->vTableOrder)
            {
                if (entry.mValues.count(valueOrder) && dt->mProperties[valueOrder].display)
                {
                    ++colnum;
                    auto& cell = entry.mValues.at(valueOrder);
                    auto formatted = DataTable::dyna_print(dt->mProperties[valueOrder].format, cell);

                    auto collen = formatted.size();
                    maxLenOfColumn[colnum - 1] = (maxLenOfColumn[colnum - 1] < collen) ? collen : maxLenOfColumn[colnum - 1];

                    tmpString.back().push_back(formatted);
                }
            }
            // ss << "\n";
        }

        // ss << std::endl;
        auto maxwidth = std::accumulate(maxLenOfColumn.begin(), maxLenOfColumn.end(), 0, [](int a, int b) { return a + b; });

        // Draw horizontal line
        ss << " " << std::setw(maxwidth + 1 + 3 * maxLenOfColumn.size()) << std::setfill('-') << "-" << std::setfill(' ') << "\n";

        // Draw header titles
        ss.setf(std::ios::left, std::ios::adjustfield);
        for (auto l = 0; l != headers.size(); ++l)
        {
            if (l == 0) ss << " | ";
            ss << std::setw(maxLenOfColumn[l]) << headers[l] << " | ";
        }
        ss << "\n";

        // Draw horizontal line
        ss << " " << std::setw(maxwidth + 1 + 3 * maxLenOfColumn.size()) << std::setfill('-') << "-" << std::setfill(' ') << "\n";

        // Draw content of the table
        for (auto l = 0; l != headers.size(); ++l)
        {
            for (auto c = 0; c != tmpString[0].size(); ++c)
            {
                auto& line = tmpString[l];
                auto& cell = line[c];

                if (c == 0) ss << " | ";
                ss << std::setw(maxLenOfColumn[c]) << cell << " | ";
            }
            ss << "\n";
        }

        // Draw horizontal line
        ss << " " << std::setw(maxwidth + 1 + 3 * maxLenOfColumn.size()) << std::setfill('-') << "-" << std::setfill(' ') << "\n";

        return ss.str();
    }
};

class DataTableCsvRenderer : public DataTableRenderer {
public:

    DataTableCsvRenderer(DataTable* dt) : DataTableRenderer(dt) {}

    std::string GetStr() {

        std::stringstream ss;

        // Draw header
        for (const auto& valueOrder : dt->vTableOrder)
            if (!dt->mProperties.count(valueOrder) || dt->mProperties.at(valueOrder).display)
                ss << valueOrder << ",";
        ss << "\n";

        for (const auto& entry : dt->vEntries)
        {

            int colnum = 0;
            for (const auto& valueOrder : dt->vTableOrder)
            {
                if (entry.mValues.count(valueOrder) && dt->mProperties[valueOrder].display)
                {
                    ++colnum;
                    auto& cell = entry.mValues.at(valueOrder);
                    auto formatted = DataTable::dyna_print(dt->mProperties[valueOrder].format, cell);

                    ss << formatted << ",";
                }
            }
            ss << "\n";
        }

        return ss.str();
    }
};

int DataTable_Test()
{
    DataTable dt;

    // Properties
    dt.mProperties["id"].display = false;
    dt.mProperties["name"].format = "name={}";

    // Data
    DataTable::DataTableEntry entry1;
    entry1.mValues = { {"id", "monId1"},{"name", "a super longgggggg name"},{"bite", "test"} };
    DataTable::DataTableEntry entry2;
    entry2.mValues = { {"id", "monId2"},{"name", "myName2"},{"bite", "test"} };
    DataTable::DataTableEntry entry3;
    entry3.mValues = { {"id", "monId3"},{"name", "myName3"},{"bite", "test"} };

    dt.vEntries.push_back(entry1);
    dt.vEntries.push_back(entry2);
    dt.vEntries.push_back(entry3);

    // Order
    dt.vTableOrder = { "id", "name", "bite" };

    std::map<std::string, std::shared_ptr<DataTableRenderer>> renderers = {
        {"console", std::make_shared<DataTableConsoleRenderer>(&dt)},
        {"csv", std::make_shared<DataTableCsvRenderer>(&dt)}
    };

    std::cout << renderers["csv"]->GetStr() << std::endl;
}
