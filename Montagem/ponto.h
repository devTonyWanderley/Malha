#pragma once
#include <string>
#include <cstdint>
#include <cctype>
#include <algorithm>
#include <vector>
#include <bit>

/*
namespace pnt
    struct Amostra
    struct alignas(32) Ponto
    struct PNo
    class QTree
*/

namespace pnt
{

struct Amostra
{
/*
    apenas construtores
*/
    std::string nome, atri;
    uint32_t abci, orde, cota;

    Amostra(std::string id, std::string obs, uint32_t x, uint32_t y, uint32_t z):
        abci(x), orde(y), cota(z)
    {
        id.erase(std::remove_if(id.begin(), id.end(), [](unsigned char c){ return std::isspace(c); }), id.end());
        nome = std::move(id);
        obs.erase(std::remove_if(obs.begin(), obs.end(), [](unsigned char c){ return std::isspace(c); }), obs.end());
        atri = std::move(obs);
    }   //  --construtor padrão--

    Amostra(): abci(0), orde(0), cota(0)
    {}  //  --construtor vazio--

};  //  --struct Amostra--

struct alignas(32) Ponto
{
/*
    construtores
    dstSq(const uint32_t xx, const uint32_t yy) const
    uint64_t dstSq(const Ponto &outro) const
*/
    uint32_t x, y, z;
    uint8_t flags;
    uint32_t iOrigem;

    Ponto(const uint32_t xx, const uint32_t yy, const uint32_t zz, const uint32_t x0, const uint32_t y0, const uint32_t ia):
        x(xx - x0), y(yy - y0), z(zz), iOrigem(ia), flags(0)
    {}   //  --construtor padrão--

    Ponto(): x(0), y(0), z(0), iOrigem(-1), flags(0)
    {}  //  --construtor vazio--

    uint64_t dstSq(const uint32_t xx, const uint32_t yy) const
    {
        int64_t dx = static_cast<int64_t>(x) - xx;
        int64_t dy = static_cast<int64_t>(y) - yy;
        return static_cast<uint64_t>(dx * dx + dy * dy);
    }   //  --distância xy quadrada--

    uint64_t dstSq(const Ponto &outro) const
    {
        return dstSq(outro.x, outro.y);
    }   //  --distância xy quadrada--

};  //  --struct alignas(32) Ponto--

struct PNo
{
    uint32_t sw = -1, se = -1, ne = -1, nw = -1;
    uint32_t centro[2]{0, 0}, sDim[2]{0, 0};
    Ponto local;
};  //  --struct PNo--

class QTree
{
/*
    construtor
    void insereAmostra(Amostra a)
    void iniciaQTree()
    void inserePontos(uint32_t lx, uint32_t ly, uint32_t lz, uint32_t ori)
    void inserePontosDeAmostra()
    std::vector<uint32_t> buscaPorRaio(uint32_t lx, uint32_t ly, uint32_t raio)
*/
public:
    std::vector<Amostra> amostras;
    //  vector<[outra fonte de dados]> [nomeFonte]
    std::vector<PNo> pontos;
    uint32_t x0, y0, extremos[4], dim[2];

    QTree()
    {
        extremos[0] = extremos[1] = -1;
        extremos[2] = extremos[3] = 0;
        x0 = y0 = -1;
        amostras.clear();
        //  lembrar de inicializar outra eventual fonte
        pontos.clear();
    }   //  construtor

    void insereAmostra(Amostra a)
    {
        if(a.abci < extremos[0]) extremos[0] = a.abci;
        if(a.orde < extremos[1]) extremos[1] = a.orde;
        if(a.abci > extremos[2]) extremos[2] = a.abci;
        if(a.orde > extremos[3]) extremos[3] = a.orde;
        amostras.push_back(std::move(a));
    }   //  --void insereAmostra(Amostra a)--

    void iniciaQTree()
    {
        //  Calcular as constantes
        uint32_t base, altura, delta[2], cp[2];
        delta[0] = extremos[2] - extremos[0];
        if(delta[0] & 1) delta[0]++;
        delta[1] = extremos[3] - extremos[1];
        if(delta[1] & 1) delta[1]++;
        base = std::bit_ceil(delta[0]);
        altura = std::bit_ceil(delta[1]);
        dim[0] = base >> 1;
        dim[1] = altura >> 1;
        cp[0] = extremos[0] + (delta[0] >> 1);
        cp[1] = extremos[1] + (delta[1] >> 1);
        x0 = (cp[0] > dim[0]) ? cp[0] - dim[0] : 0;
        y0 = (cp[1] > dim[1]) ? cp[1] - dim[1] : 0;
    }   //  --void iniciaQTree()--

    void inserePontos(uint32_t lx, uint32_t ly, uint32_t lz, uint32_t ori)
    {
        if (pontos.empty())
        {
            PNo raiz;
            raiz.centro[0] = raiz.sDim[0] = dim[0];
            raiz.centro[1] = raiz.sDim[1] = dim[1];
            raiz.local.x = lx;
            raiz.local.y = ly;
            raiz.local.z = lz;
            raiz.local.iOrigem = ori;
            pontos.emplace_back(raiz);
            return;
        }
        uint32_t idxAtual = 0; // Inicia navegação pela raiz
        while(true)
        {
            if(pontos[idxAtual].local.x == lx && pontos[idxAtual].local.y == ly) return;
            uint32_t cx = pontos[idxAtual].centro[0];
            uint32_t cy = pontos[idxAtual].centro[1];
            uint64_t dExistente = pontos[idxAtual].local.dstSq(cx, cy);
            int64_t ddx = static_cast<int64_t>(lx) - cx;
            int64_t ddy = static_cast<int64_t>(ly) - cy;
            uint64_t dNova = static_cast<uint64_t>(ddx * ddx + ddy * ddy);
            if(dNova < dExistente)
            {
                std::swap(pontos[idxAtual].local.x, lx);
                std::swap(pontos[idxAtual].local.y, ly);
                std::swap(pontos[idxAtual].local.z, lz);
                std::swap(pontos[idxAtual].local.iOrigem, ori);
            }
            uint8_t q = 0;
            if (lx >= cx) q |= 2;   //  Leste
            if (ly >= cy) q |= 1;   //  Norte
            uint32_t* proximoIdx = nullptr;
            if (q == 0) proximoIdx = &pontos[idxAtual].sw;
            else if (q == 1) proximoIdx = &pontos[idxAtual].nw;
            else if (q == 2) proximoIdx = &pontos[idxAtual].se;
            else proximoIdx = &pontos[idxAtual].ne;
            if(*proximoIdx == (uint32_t)-1)
            {
                uint32_t novaSDimX = pontos[idxAtual].sDim[0] >> 1;
                uint32_t novaSDimY = pontos[idxAtual].sDim[1] >> 1;
                if (novaSDimX == 0 || novaSDimY == 0) return;
                PNo novoNo;
                novoNo.sDim[0] = novaSDimX;
                novoNo.sDim[1] = novaSDimY;
                novoNo.centro[0] = (q & 2) ? cx + novaSDimX : cx - novaSDimX;
                novoNo.centro[1] = (q & 1) ? cy + novaSDimY : cy - novaSDimY;
                novoNo.local.x = lx;
                novoNo.local.y = ly;
                novoNo.local.z = lz;
                novoNo.local.iOrigem = ori;
                uint32_t novoIdx = pontos.size();
                pontos.emplace_back(novoNo);
                if(q == 0) pontos[idxAtual].sw = novoIdx;
                else if(q == 1) pontos[idxAtual].nw = novoIdx;
                else if(q == 2) pontos[idxAtual].se = novoIdx;
                else pontos[idxAtual].ne = novoIdx;
                return;
            }
            idxAtual = *proximoIdx;
        }
    }   //  --void inserePontos(uint32_t lx, uint32_t ly, uint32_t lz, uint32_t ori)--

    void inserePontosDeAmostra()
    {
        if(amostras.empty()) return;
        iniciaQTree();
        pontos.reserve(amostras.size());
        for(uint32_t i = 0; i < amostras.size(); i++)
        {
            const auto& a = amostras[i];
            inserePontos((a.abci - x0), (a.orde - y0), a.cota, i);
        }
    }   //  --void inserePontosDeAmostra()--

    //  adcionar métodos de busca específicos a cada atividade

    //  busca por raio de corte
    std::vector<uint32_t> buscaPorRaio(uint32_t lx, uint32_t ly, uint32_t raio)
    {
        std::vector<uint32_t> encontrados;
        if(pontos.empty()) return encontrados;
        const uint64_t raioSq = static_cast<uint64_t>(raio) * raio;
        std::vector<uint32_t> stack;
        stack.reserve(64);
        stack.push_back(0);
        while(!stack.empty())
        {
            uint32_t idx = stack.back();
            stack.pop_back();
            const PNo& no = pontos[idx];
            if(no.local.dstSq(lx, ly) <= raioSq) encontrados.push_back(idx);
            const uint32_t filhos[4] = {no.sw, no.se, no.ne, no.nw};
            for(uint32_t fIdx : filhos)
            {
                if(fIdx == (uint32_t)-1) continue;
                const PNo& f = pontos[fIdx];
                uint32_t dx = (lx > f.centro[0]) ? lx - f.centro[0] : f.centro[0] - lx,
                    dy = (ly > f.centro[1]) ? ly - f.centro[1] : f.centro[1] - ly;
                uint64_t limitX = static_cast<uint64_t>(raio) + f.sDim[0],
                    limitY = static_cast<uint64_t>(raio) + f.sDim[1];
                if(dx > limitX || dy > limitY) continue;
                stack.push_back(fIdx);
            }
        }
        return encontrados;
    }	//	--std::vector<uint32_t> buscaPorRaio(uint32_t lx, uint32_t ly, uint32_t raio)--
};  //  --class QTree--

}   //  --namespace pnt--
