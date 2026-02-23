#pragma once
#include <vector>
#include <fstream>
#include <filesystem>

namespace IO
{
using layoutConfig = std::vector<int>;
class MIO
{
public:
    static std::vector<std::string> fatiarLinha(const std::string& linha, const layoutConfig& layout)
    {
        std::vector<std::string> fatias;
        fatias.reserve(layout.size());
        size_t cursor = 0;
        for(int largura : layout)
        {
            if(cursor < linha.length())
                fatias.push_back(linha.substr(cursor, largura));
            else
                fatias.push_back("");
            cursor += largura;
        }
        return fatias;
    }

    static std::vector<std::vector<std::string>> importarFixo(const std::string& idArq, const layoutConfig& layout)
    {
        std::vector<std::vector<std::string>> leituras;
        std::filesystem::path Path(idArq);
        std::ifstream arquivo(Path);
        std::string linha;
        if(!arquivo.is_open()) return leituras;
        while(std::getline(arquivo, linha))
        {
            if(linha.empty()) continue;
            if(linha.back() == '\r') linha.pop_back();
            if(linha.empty()) continue;
            if(linha[0] == ';') continue;
            leituras.push_back(fatiarLinha(linha, layout));
        }
        return leituras;
    }
};  //  class MIO
}   //  namespace IO
