#pragma once
#include <vector>
#include <format>
#include <charconv>
#include <system_error>
#include <string_view>
#include <fstream>
#include <filesystem>

namespace IO
{
using cfgColunas = std::vector<int>;

inline std::string fix12(const int32_t valor)
{
    return std::format("{:012}", valor);
}

inline int32_t uFix12(const std::string tx)
{
    int32_t valor = 0;
    auto [ptr, ec] = std::from_chars(tx.data(), tx.data() + tx.size(), valor);
    if(ec != std::errc()) return 0;
    return valor;
}

inline std::string fix16(std::string_view s)
{
    return std::format("{:>16.16}", s);
}

inline std::vector<std::string> lerLinhaFix(const std::string &ln, const cfgColunas &cfg)
{
    std::vector<std::string> r;
    r.reserve(cfg.size());
    size_t i = 0;
    for(int w : cfg)
    {
        if(i < ln.length())
            r.push_back(ln.substr(i, w));
        else
            r.push_back("");
        i += w;
    }
    return r;
}

//  pode usar template typename
inline std::vector<std::vector<std::string>> importaFix(const std::string &nomeArquivo, const cfgColunas &cfg)
{
    std::ifstream arquivo{std::filesystem::path(nomeArquivo)};
    if(!arquivo.is_open()) return {};
    std::string ln;
    std::vector<std::vector<std::string>> r;
    while(std::getline(arquivo, ln))
    {
        if(ln.empty()) continue;
        if(ln.back() == '\r') ln.pop_back();
        if(ln.empty()) continue;
        if(ln[0] == ';') continue;
        r.push_back(lerLinhaFix(ln, cfg));
    }
    return r;
}

}
