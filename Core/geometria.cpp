#include "geometria.h"
#include "../IO/arquivo.h"

namespace Core
{
std::vector<Ponto> Superficie::importaPontos(const std::string &path)
{
    std::vector<std::vector<std::string>> buffer = IO::importaFix(path, IO::cfgColunas({16, 16, 12, 12, 12}));
    if(buffer.empty()) return {};
    uint32_t xmin = 0xFFFFFFFF, ymin = 0xFFFFFFFF;
    for(auto ln : buffer)
    {
        uint32_t y = static_cast<uint32_t>(std::stoul(ln[2])), x = static_cast<uint32_t>(std::stoul(ln[3]));
        if(y < ymin) ymin = y;
        if(x < xmin) xmin = x;
    }
    Ponto::y0 = ymin;
    Ponto::x0 = xmin;
    pontos.clear();
    for(auto ln : buffer)
    {
        std::string id = ln[0], atr = ln[1];
        uint32_t
            y = static_cast<uint32_t>(std::stoul(ln[2])),
            x = static_cast<uint32_t>(std::stoul(ln[3])),
            z = static_cast<uint32_t>(std::stoul(ln[4]));
        pontos.push_back(Ponto(id, atr, x, y, z));
    }
    if(pontos.empty()) return {};
    std::ranges::sort(pontos, std::ranges::less{}, &Ponto::_dna);
}
}
