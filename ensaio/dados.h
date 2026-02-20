#pragma once
#include "wgeo.h"
#include <fstream>
#include <filesystem>
#include <bit>
#include <cctype>
#include <cmath>

namespace WIO
{
//  layoutCfg = std::vector<int>;
//  class IOArq

using layoutCfg = std::vector<int>;

class IOA
{
//  static std::vector<std::string> fatiarLinha(const std::string& linha, const layoutCfg& layout)
//  static std::vector<std::vector<std::string>> importarFixo(const std::string& idArq, const layoutCfg& layout)
public:
    static std::vector<std::string> fatiarLinha(const std::string& linha, const layoutCfg& layout)
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

    static std::vector<std::vector<std::string>> importarFixo(const std::string& idArq, const layoutCfg& layout)
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
};   //  --class IOA--
}   //  --namespace WIO--

namespace DDS
{
class dados
{
//  void inserirWPonto(GEO::WPonto p_novo)
//  void importa(const std::string& arquivo)
public:
    uint32_t xmin, ymin, xmax, ymax;
    uint32_t x0, y0; // Coordenadas de origem no mundo real
    uint32_t sb, sh; // Meia-largura e meia-altura do quadro (potências de 2)
    std::vector<GEO::WAmostra> amostras;
    std::vector<GEO::Ramo> quadro;

    void inserirWPonto(GEO::WPonto p_novo)
    {
        if(quadro.size() <= 1)
        {
            if(quadro.empty()) quadro.emplace_back(); // Índice 0 vazio
            GEO::Ramo raiz;
            raiz.sw = raiz.se = raiz.ne = raiz.nw = 0;
            raiz.sbh[0] = raiz.centro[0] = sb;
            raiz.sbh[1] = raiz.centro[1] = sh;
            quadro.push_back(raiz); // Índice 1
            return;
        }
        size_t idx_atual = 1;
        uint32_t cx, cy, dx, dy;
        //cx = dx = sb;
        //cy = dy = sh;
        while(true)
        {
            if(p_novo.xl == quadro[idx_atual].local.xl && p_novo.yl == quadro[idx_atual].local.yl) return;
            uint64_t dist_nova = p_novo.distSq(quadro[idx_atual].centro[0], quadro[idx_atual].centro[1]),
                dist_atual = quadro[idx_atual].local.distSq(quadro[idx_atual].centro[0], quadro[idx_atual].centro[1]);
            if(dist_nova < dist_atual) std::swap(quadro[idx_atual].local, p_novo);
            dx >>= 1;
            dy >>= 1;
            size_t* proximo_idx_ptr = nullptr;
            if(p_novo.yl >= quadro[idx_atual].centro[1]) //  Norte
            {
                //cy += dy;
                if(p_novo.xl >= quadro[idx_atual].centro[0]) //  Nordeste
                {
                    //cx += dx;
                    proximo_idx_ptr = &quadro[idx_atual].ne;
                    quadro[*proximo_idx_ptr].sbh[0] = quadro[idx_atual].sbh[0] >> 1;
                    quadro[*proximo_idx_ptr].sbh[1] = quadro[idx_atual].sbh[1] >> 1;
                    quadro[*proximo_idx_ptr].centro[0] = quadro[idx_atual].centro[0] + quadro[*proximo_idx_ptr].sbh[0];
                    quadro[*proximo_idx_ptr].centro[1] = quadro[idx_atual].centro[1] + quadro[*proximo_idx_ptr].sbh[1];
                }
                else    //  Noroeste
                {
                    //cx -= dx;
                    proximo_idx_ptr = &quadro[idx_atual].nw;
                    quadro[*proximo_idx_ptr].sbh[0] = quadro[idx_atual].sbh[0] >> 1;
                    quadro[*proximo_idx_ptr].sbh[1] = quadro[idx_atual].sbh[1] >> 1;
                    quadro[*proximo_idx_ptr].centro[0] = quadro[idx_atual].centro[0] - quadro[*proximo_idx_ptr].sbh[0];
                    quadro[*proximo_idx_ptr].centro[1] = quadro[idx_atual].centro[1] + quadro[*proximo_idx_ptr].sbh[1];
                }
            }
            else    //  Sul
            {
                //cy -= dy;
                if(p_novo.xl >= quadro[idx_atual].centro[0]) //  Sudeste
                {
                    //cx += dx;
                    proximo_idx_ptr = &quadro[idx_atual].se;
                    quadro[*proximo_idx_ptr].sbh[0] = quadro[idx_atual].sbh[0] >> 1;
                    quadro[*proximo_idx_ptr].sbh[1] = quadro[idx_atual].sbh[1] >> 1;
                    quadro[*proximo_idx_ptr].centro[0] = quadro[idx_atual].centro[0] + quadro[*proximo_idx_ptr].sbh[0];
                    quadro[*proximo_idx_ptr].centro[1] = quadro[idx_atual].centro[1] - quadro[*proximo_idx_ptr].sbh[1];
                }
                else    //  Sudoeste
                {
                    //cx -= dx;
                    proximo_idx_ptr = &quadro[idx_atual].sw;
                    quadro[*proximo_idx_ptr].sbh[0] = quadro[idx_atual].sbh[0] >> 1;
                    quadro[*proximo_idx_ptr].sbh[1] = quadro[idx_atual].sbh[1] >> 1;
                    quadro[*proximo_idx_ptr].centro[0] = quadro[idx_atual].centro[0] - quadro[*proximo_idx_ptr].sbh[0];
                    quadro[*proximo_idx_ptr].centro[1] = quadro[idx_atual].centro[1] - quadro[*proximo_idx_ptr].sbh[1];
                }
            }
            if(*proximo_idx_ptr == 0)   //  inserir
            {
                GEO::Ramo novo_no;
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
    }   //  --void inserirWPonto(GEO::WPonto p_novo)--

    void importa(const std::string& arquivo)
    {
        auto dados = WIO::IOA::importarFixo(arquivo, {16, 16, 12, 12, 12});
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
            GEO::WPonto p(a.abci, a.orde, a.cota, indice, x0, y0);
            inserirWPonto(p);
            indice++;
        }
    }   //  --void importa(const std::string& arquivo)--
};
}
