#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <bit>
#include <array>
#include <cmath>
#include <unordered_map>
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
    }   //  --Construtor WAmostra--

};  //  --struct WAmostra--

struct alignas(32) WPonto
{
    uint32_t xl, yl, z;
    size_t aref; // Referência ao índice original em 'amostras', ou em outra estrutura (pavimento, projeto, camada, etc)
    // FLAGS:
    // bit 0: Função (0 = Nenhuma/Borda, 1 = Interno)
    // bit 1: Estado (0 = Borda/Não-Validado, 1 = Validado)
    // Seguindo sua tabela:
    // 00 (0): Nenhuma
    // 01 (1): Ponto de Borda
    // 10 (2): Interno Não Validado
    // 11 (3): Interno Validado
    uint8_t flags;
    WPonto() : xl(0), yl(0), z(0), aref(0), flags(0) {}
    WPonto(uint32_t x_real, uint32_t y_real, uint32_t z_real, size_t indice, uint32_t x0, uint32_t y0)
        : xl(x_real - x0), yl(y_real - y0), z(z_real), aref(indice), flags(0)
    {}
    uint64_t distSq(uint32_t cx, uint32_t cy) const
    {
        int64_t dx = static_cast<int64_t>(xl) - cx;
        int64_t dy = static_cast<int64_t>(yl) - cy;
        return static_cast<uint64_t>(dx * dx + dy * dy);
    }

    // Helpers para facilitar a leitura sem perder performance
    inline bool eInterno() const { return flags & 0b10; }
    inline bool isValidado() const { return (flags & 0b11) == 0b11; }
    inline bool eBorda() const { return flags == 0b01; }

    void setEstado(uint8_t f1, uint8_t f0)
    {
        flags = (f1 << 1) | f0;
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

struct WEdge
{
    std::array<size_t, 2> v;    //  v[0] = Origem, v[1] = Destino. A orientação é CCW: a face interna está sempre à esquerda do vetor v0->v1.

    // IDs das faces (triângulos) conectadas.
    size_t faceEsq;     //  O triângulo que criou esta aresta (sempre preenchido).
    size_t faceDir;     //  Inicialmente -1. Torna-se o ID do triângulo vizinho quando a aresta vira interna.
    uint64_t distSq;    // Usado na Fila de Prioridade para processar arestas curtas primeiro (estabilidade).
    uint8_t flags;      // Flags de Estado (compactadas para performance):
    // bit 0: Ativa (1 = está na fronteira, 0 = processada)
    // bit 1: Borda Final (1 = limite de 20m atingido, sem vizinho à direita)
    static constexpr size_t NO_FACE = static_cast<size_t>(-1);  // Valor constante para representar "sem face" (equivalente a -1 em size_t)

    WEdge(size_t v0, size_t v1, size_t idFace, uint64_t d2): v{v0, v1}, faceEsq(idFace), faceDir(NO_FACE), distSq(d2), flags(1)
    {}
    inline bool isFronteira() const { return (flags & 1) && (faceDir == NO_FACE); }
    inline bool isMorta() const { return (flags & 2) != 0; }

    void marcarComoBordaFinal()
    {
        flags &= ~1; // Remove flag ativa
        flags |= 2;  // Marca como borda final do terreno
    }

    void tornarInterna(size_t idNovaFace)
    {
        faceDir = idNovaFace;
        flags &= ~1; // Sai da lista de fronteira
    }

    std::pair<size_t, size_t> getChaveUnica() const // Retorna sempre {menor_indice, maior_indice}
    {
        return (v[0] < v[1]) ? std::make_pair(v[0], v[1]) : std::make_pair(v[1], v[0]);
    }
};  //  --struct WEdge--

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

class WMdt
{
public:
    // Referências aos dados originais da WMassa
    const std::vector<whlg::WNo>& nos;

    // Resultados da Malha
    std::vector<whlg::WFace> faces;
    std::vector<whlg::WEdge> todasArestas;

    // Controle de Fronteira
    // Usamos um Comparator para garantir que a menor aresta (distSq) seja processada primeiro
    struct EdgeComparator {
        bool operator()(size_t a_idx, size_t b_idx, const std::vector<whlg::WEdge>& arestas) const {
            return arestas[a_idx].distSq > arestas[b_idx].distSq;
        }
    };

    // Lista de arestas ativas (IDs para 'todasArestas')
    std::vector<size_t> fronteiraAtiva;

    // Mapa de "Zíper": Vértice -> ID da Aresta na Fronteira
    // Essencial para o Cenário B (fechar triângulos sem criar pontos novos)
    std::unordered_map<size_t, size_t> mapaVertices;

    WMdt(const WMassa& massa) : nos(massa.quadro) {}

    void gerarMalha(uint64_t limiteDistSq = 40000000000ULL) { // 200.000^2
        // 1. Encontrar Semente (Ponto mais central da Quadtree + Vizinho mais próximo)
        // 2. Criar primeiro triângulo e adicionar 3 WEdge em 'todasArestas' e 'fronteiraAtiva'

        while (!fronteiraAtiva.empty()) {
            // 3. Ordenar para pegar a menor aresta (ou usar std::priority_queue)
            size_t idAresta = extrairMenorAresta();
            whlg::WEdge& edge = todasArestas[idAresta];

            if (edge.isMorta()) continue;

            // 4. Buscar melhor ponto 'p' à esquerda de edge.v[0]->edge.v[1]
            size_t p_idx = buscarMelhorPontoDelaunay(edge, limiteDistSq);

            if (p_idx == whlg::WEdge::NO_FACE) {
                edge.marcarComoBordaFinal();
                continue;
            }

            // 5. O Ponto 'p' já está na fronteira? (Cenário B - Zíper)
            if (mapaVértices.count(p_idx)) {
                fecharZiper(idAresta, p_idx);
            } else {
                expandirFronteira(idAresta, p_idx);
            }
        }
    }

private:
    void expandirFronteira(size_t idEdgeBase, size_t p_idx) {
        // Cria nova Face
        // Cria duas novas WEdge (v1->p e p->v0)
        // Atualiza flags e mapas
    }

    size_t buscarMelhorPontoDelaunay(const whlg::WEdge& e, uint64_t maxD2) {
        // Implementa a busca na Quadtree usando o critério de maior ângulo
        // e respeitando o limite de 20m (maxD2)
        return whlg::WEdge::NO_FACE;
    }
};  //  --class WMdt--

}   //  --namespace whlio--
