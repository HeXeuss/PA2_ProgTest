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

struct ObjectFile
{
    std::map<std::string, int> exports;
    std::map<std::string, std::vector<int>> imports;
    std::vector<uint8_t> binary;
};

struct Symbol{
    std::string name;
    std::vector<uint8_t> data;
    std::multimap<int, std::string> imports;
};

class CLinker
{
public:
    CLinker &addFile(const std::string &fileName)
    {
        std::ifstream file(fileName, std::ios::binary);
        if (!file)
            throw std::runtime_error("Cannot read input file");

        int exportCount = 0;
        file.read(reinterpret_cast<char*>(&exportCount), sizeof(exportCount));
        if (file.gcount() != sizeof(exportCount))
            throw std::runtime_error("Input file does not have the correct size");

        int importCount = 0;
        file.read(reinterpret_cast<char*>(&importCount), sizeof(importCount));
        if (file.gcount() != sizeof(exportCount))
            throw std::runtime_error("Input file does not have the correct size");

        int binCount = 0;
        file.read(reinterpret_cast<char*>(&binCount), sizeof(binCount));
        if (file.gcount() != sizeof(exportCount))
            throw std::runtime_error("Input file does not have the correct size");

        ObjectFile objectFile;
        for (int i = 0; i < exportCount; i++) {
            // Get the size of the name
            int nameSize = 0;
            file.read(reinterpret_cast<char*>(&nameSize), 1);
            if (file.gcount() != 1)
                throw std::runtime_error("Invalid name size! (Export)");
            // Get the name
            std::string name(nameSize, '\0');
            file.read(&name[0], nameSize);
            if (file.gcount() != nameSize)
                throw std::runtime_error("Invalid name! (Export)");

            // Get the offset
            int offset = 0;
            file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
            if (file.gcount() != sizeof(offset))
                throw std::runtime_error("Invalid offset! (Export)" );

            if (objectFile.exports.contains(name))
                throw std::runtime_error("Duplicate symbols!");
            objectFile.exports[name] = offset;
        }
        for (int i = 0; i < importCount; i++) {
            // Get the size of the name
            int nameSize = 0;
            file.read(reinterpret_cast<char*>(&nameSize), 1);
            if (file.gcount() != 1)
                throw std::runtime_error("Invalid name size! (Import)");
            // Get the name
            std::string name(nameSize, '\0');
            file.read(&name[0], nameSize);
            if (file.gcount() != nameSize)
                throw std::runtime_error("Invalid name! (Import)");

            // Get the importCount
            int count = 0;
            file.read(reinterpret_cast<char*>(&count), sizeof(count));
            if (file.gcount() != sizeof(count))
                throw std::runtime_error("Invalid count! (Import)");

            // Get imports
            for (int j = 0; j < count; j++) {
                int address = 0;
                file.read(reinterpret_cast<char*>(&address), sizeof(address));
                if (file.gcount() != sizeof(address))
                    throw std::runtime_error("Invalid address! (Import)");
                objectFile.imports[name].push_back(address);
            }
        }

        // Get binary data
        for (int i = 0; i < binCount; i++) {
            uint8_t byte;
            file.read(reinterpret_cast<char*>(&byte), sizeof(byte));
            if (file.gcount() != sizeof(byte))
                throw std::runtime_error("Invalid bin! (Import)");
            objectFile.binary.push_back(byte);
        }

        file.close();

        // Create symbols map
        addSymbols(objectFile);

        return *this;
    }

    void linkOutput(const std::string &fileName, const std::string &entryPoint)
    {
        //Check if entry point is defined
        if (!symbols.contains(entryPoint))
            throw std::runtime_error("Undefined symbol!");

        //Setup BFS
        std::queue<std::string> toVisit;
        std::set<std::string> visited;
        toVisit.push(entryPoint);
        visited.insert(entryPoint);

        //Order and offsets of symbols in final binary
        std::vector<std::pair<std::string, std::vector<uint8_t>>> result;
        std::map<std::string, int> offsets;
        result.emplace_back(entryPoint, symbols[entryPoint].data);
        offsets[entryPoint] = 0;
        int offset = symbols.at(entryPoint).data.size();

        //BFS loop
        while (!toVisit.empty()) {
            std::string current = toVisit.front();
            toVisit.pop();

            for (const auto & n : symbols[current].imports) {
                if (!visited.contains(n.second)) {
                    //Missing export
                    if (!symbols.contains(n.second))
                        throw std::runtime_error("Undefined symbol!");

                    visited.insert(n.second);
                    toVisit.push(n.second);

                    result.emplace_back(n.second, symbols[n.second].data);
                    offsets[n.second] = offset;
                    offset += symbols[n.second].data.size();
                }
            }
        }

        //Patch addresses
        for (auto & n : result) {
            for (auto &v : symbols[n.first].imports) {
                patchIntIntoVector(n.second,v.first, offsets[v.second]);
            }
        }

        // Write to the file
        std::ofstream file(fileName, std::ios::binary);
        for (auto &n : result) {
            const auto& vec = n.second;
            file.write(reinterpret_cast<const char*>(vec.data()), vec.size() * sizeof(vec[0]));
        }
    }

private:
    std::map<std::string, Symbol> symbols;

    void addSymbols(const ObjectFile &objectFile) {
        for (auto &ex : objectFile.exports) {
            Symbol newSymbol;
            newSymbol.name = ex.first;
            int offset = ex.second;

            // Find the smallest offset greater than the current offset
            int size = objectFile.binary.size();
            for (auto &sym : objectFile.exports) {
                if (sym.second > offset && sym.second < size) {
                    size = sym.second;
                }
            }

            // Extract the binary data for the symbol
            for (int i = offset; i < size; i++) {
                newSymbol.data.push_back(objectFile.binary[i]);
            }

            // Process imports within the symbol's range
            for (auto &imp : objectFile.imports) {
                for (auto &ofs : imp.second) {
                    if (ofs >= offset && ofs < size) {
                        newSymbol.imports.emplace(ofs - offset, imp.first);
                    }
                }
            }

            // Add the symbol to the map
            symbols[newSymbol.name] = newSymbol;
        }
    }

    void patchIntIntoVector(std::vector<uint8_t>& vec, size_t offset, int value) {
        //patch address
        for (int i = 0; i < 4; ++i) {
            vec[offset + i] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
        }
    }

};

bool cmpFiles(const std::string& filePath1, const std::string& filePath2) {
    std::ifstream file1(filePath1, std::ios::binary);
    std::ifstream file2(filePath2, std::ios::binary);

    if (!file1 || !file2) return false;

    char byte1, byte2;
    while (true) {
        file1.read(&byte1, 1);
        file2.read(&byte2, 1);

        if (file1.gcount() != file2.gcount()) return false;
        if (file1.gcount()  == 0 && file2.gcount() == 0) return true;
        if (byte1 != byte2) return false;
    }
}

#ifndef __PROGTEST__
int main()
{
    CLinker() .addFile("0in0.o") .linkOutput("0out", "strlen");
    assert(cmpFiles("0out", "0ref"));

    CLinker() .addFile("1in0.o") .linkOutput("1out", "main");
    assert(cmpFiles("1out", "1ref"));

    CLinker() .addFile("2in0.o") .addFile("2in1.o") .linkOutput("2out", "main");
    assert(cmpFiles("2out", "2ref"));

    CLinker() .addFile("3in0.o") .addFile("3in1.o") .linkOutput("3out", "towersOfHanoi");
    assert(cmpFiles("3out", "3ref"));

    CLinker () . addFile ( "0010_0.o" ) . addFile ( "0010_1.o" ) . addFile ( "0010_2.o" ) . addFile ( "0010_3.o" ) . linkOutput ( "0010_out", "pdrolowjjgdwxiadj" );
    assert(cmpFiles("0010_out", "0010_ref"));
    CLinker () . addFile ( "0011_0.o" ) . addFile ( "0011_1.o" ) . linkOutput ( "0011_out", "yntvlhvtp" );
    assert(cmpFiles("0011_out", "0011_ref"));
    CLinker () . addFile ( "0012_0.o" ) . addFile ( "0012_1.o" ) . addFile ( "0012_2.o" ) . linkOutput ( "0012_out", "acnskqfuegem" );
    assert(cmpFiles("0012_out", "0012_ref"));
    CLinker () . addFile ( "0013_0.o" ) . addFile ( "0013_1.o" ) . addFile ( "0013_2.o" ) . linkOutput ( "0013_out", "yvjbkannhcusuktuhl" );
    assert(cmpFiles("0013_out", "0013_ref"));
    CLinker () . addFile ( "0014_0.o" ) . addFile ( "0014_1.o" ) . addFile ( "0014_2.o" ) . linkOutput ( "0014_out", "adqcwiahautvfi" );
    assert(cmpFiles("0014_out", "0014_ref"));

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