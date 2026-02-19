#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <bit>
#include "../IO/arquivo.h"

namespace whlg   //  quadtree HighLander
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
    }
};  //  --struct WAmostra--

struct alignas(32) WPonto
{
    uint32_t xl, yl, z;
    size_t aref; // Referência ao índice original em 'amostras', ou em outra estrutura (pavimento, projeto, camada, etc)
    WPonto() : xl(0), yl(0), z(0), aref(0) {}
    WPonto(uint32_t x_real, uint32_t y_real, uint32_t z_real, size_t indice, uint32_t x0, uint32_t y0)
        : xl(x_real - x0), yl(y_real - y0), z(z_real), aref(indice)
    {}
    uint64_t distSq(uint32_t cx, uint32_t cy) const
    {
        int64_t dx = static_cast<int64_t>(xl) - cx;
        int64_t dy = static_cast<int64_t>(yl) - cy;
        return static_cast<uint64_t>(dx * dx + dy * dy);
    }
};  //  --struct alignas(32) WPonto--

struct WNo
{
    size_t sw, se, ne, nw;
    WPonto local;
};  //  --struct WNo--

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

}   //  --namespace whlg--

namespace whlio
{
class WMassa
{
public:
    uint32_t xmin, ymin, xmax, ymax;
    uint32_t x0, y0; // Coordenadas de origem no mundo real
    uint32_t sb, sh; // Meia-largura e meia-altura do quadro (potências de 2)
    std::vector<whlg::WAmostra> amostras;
    std::vector<whlg::WNo> quadro;
    void inserirWPonto(whlg::WPonto p_novo)
    {
        if(quadro.size() <= 1)
        {
            if(quadro.empty()) quadro.emplace_back(); // Índice 0 vazio
            whlg::WNo raiz;
            raiz.sw = raiz.se = raiz.ne = raiz.nw = 0;
            quadro.push_back(raiz); // Índice 1
            return;
        }
        size_t idx_atual = 1;
        uint32_t cx, cy, dx, dy;
        cx = dx = sb;
        cy = dy = sh;
        while(true)
        {
            if(p_novo.xl == quadro[idx_atual].local.xl && p_novo.yl == quadro[idx_atual].local.yl) return;
            uint64_t dist_nova = p_novo.distSq(cx, cy), dist_atual = quadro[idx_atual].local.distSq(cx, cy);
            if(dist_nova < dist_atual) std::swap(quadro[idx_atual].local, p_novo);
            dx >>= 1;
            dy >>= 1;
            size_t* proximo_idx_ptr = nullptr;
            if(p_novo.yl >= cy) //  Norte
            {
                cy += dy;
                if(p_novo.xl >= cx) //  Nordeste
                {
                    cx += dx;
                    proximo_idx_ptr = &quadro[idx_atual].ne;
                }
                else    //  Noroeste
                {
                    cx -= dx;
                    proximo_idx_ptr = &quadro[idx_atual].nw;
                }
            }
            else    //  Sul
            {
                cy -= dy;
                if(p_novo.xl >= cx) //  Sudeste
                {
                    cx += dx;
                    proximo_idx_ptr = &quadro[idx_atual].se;
                }
                else    //  Sudoeste
                {
                    cx -= dx;
                    proximo_idx_ptr = &quadro[idx_atual].sw;
                }
            }
            if(*proximo_idx_ptr == 0)   //  inserir
            {
                whlg::WNo novo_no;
                novo_no.sw = novo_no.se = novo_no.ne = novo_no.nw = 0;
                novo_no.local = p_novo;
                quadro.push_back(novo_no);
                *proximo_idx_ptr = quadro.size() - 1;
                break;
            }
            else
            {
                idx_atual = *proximo_idx_ptr;
                if(dx == 0 && dy == 0)
                {
                    break;
                }
            }
        }
    }   //  --void inserirWPonto(whlg::WPonto p_novo)--

    void importa(const std::string& arquivo)
    {
        auto dados = IO::MIO::importarFixo(arquivo, {16, 16, 12, 12, 12});
        if (dados.empty()) return;
        xmin = ymin = 0xFFFFFFFF;
        xmax = ymax = 0;
        amostras.clear();
        amostras.reserve(dados.size());
        for (auto& linha : dados)
        {
            if (linha.size() < 5) continue;
            uint32_t y = static_cast<uint32_t>(std::stoul(linha[2]));
            uint32_t x = static_cast<uint32_t>(std::stoul(linha[3]));
            uint32_t z = static_cast<uint32_t>(std::stoul(linha[4]));
            if (x < xmin) xmin = x; if (x > xmax) xmax = x;
            if (y < ymin) ymin = y; if (y > ymax) ymax = y;
            amostras.emplace_back(std::move(linha[0]), std::move(linha[1]), x, y, z);
        }
        uint32_t ampX = xmax - xmin;
        uint32_t ampY = ymax - ymin;
        ampX += 500000;
        ampY += 500000;
        sb = std::bit_ceil(ampX) >> 1;
        sh = std::bit_ceil(ampY) >> 1;
        if (ampX % 2 != 0) ampX++;
        if (ampY % 2 != 0) ampY++;
        x0 = xmin - (sb - (ampX / 2));
        y0 = ymin - (sh - (ampY / 2));
        quadro.clear();
        quadro.reserve(amostras.size() + 1);
        size_t indice = 0;
        for(auto& a : amostras)
        {
            whlg::WPonto p(a.abci, a.orde, a.cota, indice, x0, y0);
            inserirWPonto(p);
            indice++;
        }
    }   //  --void importa(const std::string& arquivo)--
};  //  --class WMassa--
}   //  --namespace whlio--
