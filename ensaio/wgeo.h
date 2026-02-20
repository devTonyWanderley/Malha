#pragma once
#include <string>
#include <stdint.h>
#include <cctype>
#include <algorithm>
#include <vector>

namespace GEO
/*
struct WAmostra
struct alignas(32) WPonto
struct WFace
struct WNo
struct Ramo
*/
{
struct WAmostra
{
    std::string nome, atri;
    uint32_t abci, orde, cota;
    WAmostra(std::string id, std::string atr, uint32_t x, uint32_t y, uint32_t z): abci(x), orde(y), cota(z)
    {
        id.erase(std::remove_if(id.begin(), id.end(), ::isspace), id.end());
        nome = std::move(id);
        atr.erase(std::remove_if(atr.begin(), atr.end(), ::isspace), atr.end());
        atri = std::move(atr);
    }   //  --Construtor WAmostra--
};  //  --struct WAmostra--

struct alignas(32) WPonto
{
    uint32_t xl, yl, z;
    size_t aref;    // Referência ao índice original em 'amostras', ou em outra estrutura (pavimento, projeto, camada, etc)
    uint8_t flags;  // bit 0: Função (0 = Nenhuma/Borda, 1 = Interno) / bit 1: Estado (0 = Borda/Não-Validado, 1 = Validado)
                    // 00 (0): Nenhuma; 01 (1): Ponto de Borda; 10 (2): Interno Não Validado; 11 (3): Interno Validado

    WPonto() : xl(0), yl(0), z(0), aref(0), flags(0) {}     //  Construtor vazio
    WPonto(uint32_t x_real, uint32_t y_real, uint32_t z_real, size_t indice, uint32_t x0, uint32_t y0)
        : xl(x_real - x0), yl(y_real - y0), z(z_real), aref(indice), flags(0)
    {}      //  Construtor padrão

    uint64_t distSq(const uint32_t cx, const uint32_t cy) const
    {
        uint64_t dx, dy;
        if(cx > xl)
            dx = static_cast<uint64_t>(cx - xl);
        else
            dx = static_cast<uint64_t>(xl - cx);
        if(cy > yl)
            dy = static_cast<uint64_t>(cy - yl);
        else
            dy = static_cast<uint64_t>(yl - cy);
        return ((dx * dx) + (dy * dy));
    }   //  Distância quadrada

    uint64_t distSq(const WPonto p) const
    {
        uint64_t dx, dy;
        if(p.xl > xl)
            dx = static_cast<uint64_t>(p.xl - xl);
        else
            dx = static_cast<uint64_t>(xl - p.xl);
        if(p.yl > yl)
            dy = static_cast<uint64_t>(p.yl - yl);
        else
            dy = static_cast<uint64_t>(yl - p.yl);
        return ((dx * dx) + (dy * dy));
    }   //  Distância quadrada

    // Helpers para facilitar a leitura sem perder performance
    inline bool eInterno() const { return flags & 0b10; }
    inline bool isValidado() const { return (flags & 0b11) == 0b11; }
    inline bool eBorda() const { return flags == 0b01; }

    void setEstado(uint8_t f1, uint8_t f0)
    {
        flags = (f1 << 1) | f0;
    }
};  //  --struct alignas(32) WPonto--

inline int64_t orient2d(const WPonto& a, const WPonto& b, const WPonto& c)
{
    return (((static_cast<int64_t>(b.xl)) - (static_cast<int64_t>(a.xl))) * ((static_cast<int64_t>(c.yl)) - (static_cast<int64_t>(a.yl)))) -
           (((static_cast<int64_t>(b.yl)) - (static_cast<int64_t>(a.yl))) * ((static_cast<int64_t>(c.xl)) - (static_cast<int64_t>(a.xl))));
}   //  --inline int64_t orient2d(const WPonto& a, const WPonto& b, const WPonto& c)--

inline int64_t orient2d(const uint32_t xa, const uint32_t ya,
                        const uint32_t xb, const uint32_t yb,
                        const uint32_t xc, const uint32_t yc)
{
    return (((static_cast<int64_t>(xb)) - (static_cast<int64_t>(xa))) * ((static_cast<int64_t>(yc)) - (static_cast<int64_t>(ya)))) -
           (((static_cast<int64_t>(yb)) - (static_cast<int64_t>(ya))) * ((static_cast<int64_t>(xc)) - (static_cast<int64_t>(xa))));
}

struct WFace
{
    size_t p[3];
    size_t f[3];
    WFace(size_t i0, size_t i1, size_t i2, const std::vector<WPonto>& pontos)
    {
        p[0] = i0;
        p[1] = i1;
        p[2] = i2;
        if (orient2d(pontos[i0], pontos[i1], pontos[i2]) < 0) std::swap(p[1], p[2]);
        f[0] = f[1] = f[2] = static_cast<size_t>(-1);
    }
};  //  --struct WFace--

struct WNo
{
    size_t sw, se, ne, nw;
    WPonto local;
};  //  --struct WNo--

struct Ramo
{
    size_t sw, se, ne, nw;
    uint32_t centro[2], sbh[2];
    WPonto local;
};

}   //  --namespace GEO--
