#include "coder.h"
#include <bitset>

namespace zip_maker {

Coder::Coder
(std::istream& input)
    : inputStream_(input)
{
    processInput();
    fillCoderTable();
}

void
Coder::fillCoderTable()
{
    for (const auto symbol : charFreq_)
    {
        tree_.FindMatch(symbol.first);
    }
}

void
Coder::Encode(std::ostream& os) 
{
    CoderTable coderTable = tree_.GetCoderTable();

    char buffer;
    while (inputStream_ >> std::noskipws >> buffer)
    {
        std::byte b = static_cast<std::byte>(buffer);
        Code& code = coderTable[b];
        os << code;
    }
}

void
Coder::processInput()
{
    std::for_each(std::istreambuf_iterator<char>(inputStream_),
        std::istreambuf_iterator<char>(),
        [&](const char c) {
            std::byte b = static_cast<std::byte>(c);
            charFreq_[b]++;
        }
    );

    inputStream_.clear();
    inputStream_.seekg(0);

    std::vector<TreePtr> freeNodes;
    std::for_each(charFreq_.begin(), charFreq_.end(),
            [&](auto& p) {
                freeNodes.push_back(
                    std::make_shared<TreeNode>(
                            p.second, std::vector(1u, p.first)));
            });

    tree_.Assign(std::move(freeNodes));
}

size_t toUInt(const std::string& binary)
{
    size_t result = 0;
    int i = binary.size() - 1;
    for (char c : binary)
    {
        size_t tempNum = c - '0';
        if (tempNum == 0) continue;

        result += std::pow(2, i);
        i--;
    }
    
    return result;
}

void
print_coder_table
(const CoderTable& table)
{
    std::cout << "======== CoderTable =========\n";
    for (const auto& [symbol, bin] : table)
    {
        std::cout << std::hex << std::showbase << int(symbol)
                  << " - " << bin << '\n';
    }
}

std::ostream& operator<<(std::ostream& os, const Code& codes)
{
    std::for_each(codes.begin(), codes.end(),
                  [&](uint8_t bit){
                      os << std::to_string(bit);
                  });
    return os;
}

std::string textToBinary(const std::string& source)
{
    using ull = long long unsigned int;
    std::string output;

    for (char c : source)
    {
        auto bits = std::bitset<8>{static_cast<ull>(c)};
        output.append(std::move(bits.to_string()));
    }

    return output;
}

}


