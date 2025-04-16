#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <optional>
#include <memory>
#include <stdexcept>
#include <set>
#include <map>
#include <queue>
#include <deque>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#endif /* __PROGTEST__ */

struct Symbol{
    std::string name;
    std::map<int, std::string> imports;

    std::string fileName;
    size_t offset = 0;
    int size = 0;
};

class CLinker
{
public:
    CLinker &addFile(const std::string &fileName)
    {
        //Open binary file
        std::ifstream file(fileName, std::ios::binary);
        if (!file.is_open() || !file.good()) throw std::runtime_error("File not found");

        //Get header data
        int exportCount = 0, importCount = 0, binCount = 0;
        readFromFile(file, &exportCount, 4, "Export count fail");
        readFromFile(file, &importCount, 4, "Import count fail");
        readFromFile(file, &binCount, 4, "Bin count fail");

        std::map<int, std::string> offsets;

        //Get exports
        for (int i = 0 ; i < exportCount ; ++i) {
            //Get name size
            int nameSize = 0;
            readFromFile(file, &nameSize, 1, "Name size fail (Export)");

            //Get name
            Symbol symbol;
            symbol.fileName = fileName;
            symbol.name = std::string(nameSize, '\0');
            readFromFile(file, symbol.name.data(),nameSize, "Name fail (Export)");

            //Get offset
            int offset = 0;
            readFromFile(file, &offset, 4, "Offset fail (Export)");
            offsets[offset] = symbol.name;

            //Add symbol
            if (symbols.contains(symbol.name)) throw std::runtime_error("Symbol already exists (Export)");
            symbols[symbol.name] = symbol;
        }

        //Get imports
        for (int i = 0 ; i < importCount ; ++i) {
            //Get name size
            int nameSize = 0;
            readFromFile(file, &nameSize, 1, "Name size fail (Import)");

            //Get name
            std::string name(nameSize, '\0');
            readFromFile(file, name.data(), nameSize,"Name fail (Import)");

            //Get use count
            int useCount = 0;
            readFromFile(file, &useCount, 4, "Use count fail (Import)");

            //Map imports to exports
            for (int j = 0 ; j < useCount ; ++j) {
                int offset = 0;
                readFromFile(file, & offset, 4, "Offset fail (Import)");

                auto it = offsets.lower_bound(offset);
                if (it->first != offset && it != offsets.begin()) --it;
                symbols[it->second].imports[offset - it->first] = name;
            }
        }

        for (auto [offset, name] : offsets) {
            int from = offset;
            int to = binCount;
            auto it_to = offsets.upper_bound(offset);
            if (it_to != offsets.end()) to = it_to->first;
            int size = to - from;

            symbols[name].size = size;
            symbols[name].offset = file.tellg();

            std::vector<uint8_t> buffer(size);
            readFromFile(file, buffer.data(), size, "Bin fail");
        }

        file.close();
        if (!file) {
            throw std::runtime_error("Failed to write the expected number of bytes to the file");
        }
        return *this;
    }

    void linkOutput(const std::string &fileName, const std::string &entryPoint)
    {
        //Check if entry point is defined
        if (!symbols.contains(entryPoint))
            throw std::runtime_error("Undefined symbol!");

        //Order and offsets of symbols in final binary
        auto result = findNeededImports(entryPoint);

        std::map<std::string, size_t> offsets;
        offsets[entryPoint] = 0;
        size_t offset = symbols[entryPoint].size;
        for (size_t i = 1; i < result.size(); ++i) {
            offsets[result[i]] = offset;
            offset += symbols[result[i]].size;
        }

        //Gather, Patch, Write data
        std::ofstream file(fileName, std::ios::binary);
        for (const auto &exportName : result) {
            const Symbol &symbol = symbols[exportName];
            std::vector<uint8_t> buffer(symbol.size);

            //Open and read bin data from file
            std::ifstream infile(symbol.fileName, std::ios::binary);
            if (!infile.is_open() || !infile.good()) throw std::runtime_error("Failed to open file: " + symbol.fileName);
            infile.seekg(symbol.offset);
            readFromFile(infile, buffer.data(), buffer.size(), "Export fail");
            infile.close();

            // Patch addresses
            for (const auto &[offset, importName] : symbol.imports) {
                patchIntIntoVector(buffer, offset, offsets[importName]);
            }

            // Write the patched buffer to the output file
            file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
            if (!file) {
                throw std::runtime_error("Failed to write the expected number of bytes to the file");
            }
        }

        file.close();
        if (!file) {
            throw std::runtime_error("Failed to write the expected number of bytes to the file");
        }
    }


private:
    std::map<std::string, Symbol> symbols;
    static void patchIntIntoVector(std::vector<uint8_t>& vec, const size_t offset, const size_t value) {
        for (int i = 0; i < 4; ++i) {
            vec[offset + i] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
        }
    }
    static void readFromFile(std::ifstream &file, void *buffer, size_t size, const std::string &errorMessage) {
        file.read(static_cast<char *>(buffer), size);
        if (file.gcount() != static_cast<std::streamsize>(size)) {
            throw std::runtime_error(errorMessage);
        }
    }
    std::vector<std::string> findNeededImports(const std::string &entryPoint) {
        //Setup BFS
        std::queue<std::string> toVisit;
        std::set<std::string> visited;
        toVisit.push(entryPoint);
        visited.insert(entryPoint);

        //Order and offsets of symbols in final binary
        std::vector<std::string> result;
        result.emplace_back(entryPoint);

        //BFS loop
        while (!toVisit.empty()) {
            std::string current = toVisit.front();
            toVisit.pop();

            for (auto & [offset, name] : symbols[current].imports) {
                if (!visited.contains(name)) {
                    //Missing export
                    if (!symbols.contains(name))
                        throw std::runtime_error("Undefined symbol!");

                    visited.insert(name);
                    toVisit.push(name);

                    result.emplace_back(name);
                }
            }
        }
        std::vector<std::string> sortedResult;
        sortedResult.emplace_back(result[0]);
        std::sort(result.begin() + 1, result.end());
        sortedResult.insert(sortedResult.end(), result.begin() + 1, result.end());
        return sortedResult;
    }

};

#ifndef __PROGTEST__

int main()
{
    CLinker() .addFile("0in0.o") .linkOutput("0out", "strlen");


    CLinker() .addFile("1in0.o") .linkOutput("1out", "main");


    CLinker() .addFile("2in0.o") .addFile("2in1.o") .linkOutput("2out", "main");


    CLinker() .addFile("3in0.o") .addFile("3in1.o") .linkOutput("3out", "towersOfHanoi");


    try
    {
        CLinker() .addFile("4in0.o") .addFile("4in1.o") .linkOutput("4out", "unusedFunc");
        assert("missing an exception" == nullptr);
    }
    catch (const std::runtime_error &e)
    {
        // e.what(): Undefined symbol qsort
    }
    catch (...)
    {
        assert("invalid exception" == nullptr);
    }

    try
    {
        CLinker() .addFile("5in0.o") .linkOutput("5out", "main");
        assert("missing an exception" == nullptr);
    }
    catch (const std::runtime_error &e)
    {
        // e.what(): Duplicate symbol: printf
    }
    catch (...)
    {
        assert("invalid exception" == nullptr);
    }

    try
    {
        CLinker() .addFile("6in0.o") .linkOutput("6out", "strlen");
        assert("missing an exception" == nullptr);
    }
    catch (const std::runtime_error &e)
    {
        // e.what(): Cannot read input file
    }
    catch (...)
    {
        assert("invalid exception" == nullptr);
    }

    try
    {
        CLinker() .addFile("7in0.o") .linkOutput("7out", "strlen2");
        assert("missing an exception" == nullptr);
    }
    catch (const std::runtime_error &e)
    {
        // e.what(): Undefined symbol strlen2
    }
    catch (...)
    {
        assert("invalid exception" == nullptr);
    }

    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
