//  geometria highlander .. paralelo a geometria .. refazer com diferença no countainer
//  independente, e para substituir geometria
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
};

struct alignas(32) WPonto
{
    uint32_t xl, yl, z;
    size_t aref; // Referência ao índice original em 'amostras'

    // Construtor padrão (necessário para inicializar o vector 'quadro')
    WPonto() : xl(0), yl(0), z(0), aref(0) {}

    // Construtor de conversão: Transforma Real -> Local
    WPonto(uint32_t x_real, uint32_t y_real, uint32_t z_real, size_t indice, uint32_t x0, uint32_t y0)
        : xl(x_real - x0), yl(y_real - y0), z(z_real), aref(indice)
    {
        // Aqui xl e yl já nascem centralizados no Quadro Zero
    }

    // Função utilitária para distância quadrada ao centro do quadrante
    // Essencial para a "Ação de Despejo"
    uint64_t distSq(uint32_t cx, uint32_t cy) const
    {
        int64_t dx = static_cast<int64_t>(xl) - cx;
        int64_t dy = static_cast<int64_t>(yl) - cy;
        return static_cast<uint64_t>(dx * dx + dy * dy);
    }
};

struct WNo
{
    size_t sw, se, ne, nw;
    WPonto local;
};

}   //  --namespace whlg--

namespace whlio
{
class WMassa
{
public:
    // Removido o 'static' para permitir múltiplas instâncias e segurança de thread
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
    }

    void importa(const std::string& arquivo)
    {
        // 1. Importação bruta (Mantendo seu layoutConfig)
        auto dados = IO::MIO::importarFixo(arquivo, {16, 16, 12, 12, 12});
        if (dados.empty()) return;

        // 2. Reset de busca de extremos
        xmin = ymin = 0xFFFFFFFF;
        xmax = ymax = 0;
        amostras.clear();
        amostras.reserve(dados.size());

        // 3. Primeiro Passo: Coleta e Extremos
        for (auto& linha : dados)
        {
            if (linha.size() < 5) continue;

            // Use stoul para uint32_t (mais rápido que stol)
            uint32_t y = static_cast<uint32_t>(std::stoul(linha[2]));
            uint32_t x = static_cast<uint32_t>(std::stoul(linha[3]));
            uint32_t z = static_cast<uint32_t>(std::stoul(linha[4]));

            if (x < xmin) xmin = x; if (x > xmax) xmax = x;
            if (y < ymin) ymin = y; if (y > ymax) ymax = y;

            // Movemos as strings para evitar cópias (Performance!)
            amostras.emplace_back(std::move(linha[0]), std::move(linha[1]), x, y, z);
        }

        // 4. Segundo Passo: Definição do Geometria do Quadro Zero
        uint32_t ampX = xmax - xmin;
        uint32_t ampY = ymax - ymin;

        // sb e sh como potências de 2 (metade do lado total)
        // bit_ceil garante que o quadro caiba a amplitude
        sb = std::bit_ceil(ampX) >> 1;
        sh = std::bit_ceil(ampY) >> 1;

        // Garantir paridade para evitar truncamento no offset
        if (ampX % 2 != 0) ampX++;
        if (ampY % 2 != 0) ampY++;

        // x0 e y0 são os cantos "sudoeste" do quadro no mundo real
        // Eles garantem que o centro {sb, sh} coincida com o centro da nuvem
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
        //  lógica de inserção
    }
};
}

