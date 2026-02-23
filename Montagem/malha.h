#pragma once
#include "ponto.h"

namespace mdt
{

constexpr int64_t MAX_ARESTA = 200000;

class Tim
{
/*
struct alignas(32) face
uint32_t maisProximo(uint32_t x, uint32_t y, std::vector<uint32_t>& cand, std::vector<uint32_t>& exceto)
int sentido(uint32_t xa, uint32_t ya, uint32_t xb, uint32_t yb, uint32_t xc, uint32_t yc)
int sentido(uint32_t a, uint32_t b, uint32_t c)
bool semente()
*/
public:
    struct alignas(32) face
    {
        uint32_t v[3];
        uint32_t f[3];
        face()
        {
            v[0] = v[1] = v[2] = (uint32_t) -1;
            f[0] = f[1] = f[2] = (uint32_t) -1;
        }
    };  //  --struct alignas(32) face--

    pnt::QTree& arvore;
    std::vector<face> malha;
    std::vector<uint32_t> fronteira;
private:
    uint32_t maisProximo(uint32_t x, uint32_t y, std::vector<uint32_t>& cand, std::vector<uint32_t>& exceto)
    {
        uint64_t dmin = (uint64_t) -1, dAtual;
        uint32_t r = (uint32_t) -1;
        bool apto;
        for(uint i = 0; i < cand.size(); i++)
        {
            apto = true;
            for(uint j = 0; j < exceto.size(); j++)
                if(cand[i] == exceto[j]) apto = false;
            if(!apto) continue;
            dAtual = arvore.pontos[cand[i]].local.dstSq(x, y);
            if(dAtual < dmin)
            {
                r = cand[i];
                dmin = dAtual;
            }
        }
        return r;
    }   //  --uint32_t maisProximo(uint32_t x, uint32_t y, std::vector<uint32_t>& cand, std::vector<uint32_t>& exceto)--

    int sentido(uint32_t xa, uint32_t ya, uint32_t xb, uint32_t yb, uint32_t xc, uint32_t yc)
    {
        int64_t xab = static_cast<int64_t>(xb) - xa,
            yab = static_cast<int64_t>(yb) - ya,
            xac = static_cast<int64_t>(xc) - xa,
            yac = static_cast<int64_t>(yc) - ya;
        int64_t d = (static_cast<int64_t>(xab) * yac) - (static_cast<int64_t>(xac) * yab);
        if(d > 0) return 1;
        if(d < 0) return -1;
        return 0;
    }   //  --int sentido(uint32_t xa, uint32_t ya, uint32_t xb, uint32_t yb, uint32_t xc, uint32_t yc)--

    int sentido(uint32_t a, uint32_t b, uint32_t c)
    {
        return sentido(arvore.pontos[a].local.x, arvore.pontos[a].local.y,
                       arvore.pontos[b].local.x, arvore.pontos[b].local.y,
                       arvore.pontos[c].local.x, arvore.pontos[c].local.y);
    }   //  --int sentido(uint32_t a, uint32_t b, uint32_t c)--

    bool semente()
    {
        if(arvore.pontos.empty()) return false;
        std::vector<uint32_t> vizinhos;
        std::vector<uint32_t> exceto;
        for(uint32_t i = 0; i < arvore.pontos.size(); i++)
        {
            exceto.clear();
            vizinhos = arvore.buscaPorRaio(arvore.pontos[i].local.x, arvore.pontos[i].local.y, (2 * MAX_ARESTA));
            if(vizinhos.size() < 2) continue;
            exceto.push_back(i);
            uint32_t q, r;
            q = maisProximo(arvore.pontos[i].local.x, arvore.pontos[i].local.y, vizinhos, exceto);
            if(q == (uint32_t) -1) continue;
            if(arvore.pontos[i].local.dstSq(arvore.pontos[q].local) > (MAX_ARESTA * MAX_ARESTA)) continue;
            uint32_t xm = (arvore.pontos[i].local.x & arvore.pontos[q].local.x) +
                          ((arvore.pontos[i].local.x ^ arvore.pontos[q].local.x) >> 1),
                     ym = (arvore.pontos[i].local.y & arvore.pontos[q].local.y) +
                          ((arvore.pontos[i].local.y ^ arvore.pontos[q].local.y) >> 1);
            exceto.push_back(q);
            // loop pra testar alinhamento de r
            bool fr = false;
            while(!fr)
            {
                r = maisProximo(xm, ym, vizinhos, exceto);
                if(r == (uint32_t) -1) break;  //  sai com fr = false
                if(sentido(i, q, r) == 0)
                    exceto.push_back(r);
                else
                    fr = true;
            }
            if(!fr) continue;
            if(arvore.pontos[i].local.dstSq(arvore.pontos[r].local) > (MAX_ARESTA * MAX_ARESTA)) continue;
            if(arvore.pontos[q].local.dstSq(arvore.pontos[r].local) > (MAX_ARESTA * MAX_ARESTA)) continue;
            if(sentido(i, q, r) == -1) std::swap(q, r);
            //  marcar pontos como usados em face (flag 0)
            //  mantem o flag 1 desativado .. ponto de fronteira
            arvore.pontos[i].local.flags = arvore.pontos[i].local.flags | 1;
            arvore.pontos[q].local.flags = arvore.pontos[q].local.flags | 1;
            arvore.pontos[r].local.flags = arvore.pontos[r].local.flags | 1;
            fronteira.push_back(i);
            fronteira.push_back(q);
            fronteira.push_back(r);
            face f;
            f.v[0] = i;
            f.v[1] = q;
            f.v[2] = r;
            malha.push_back(f);
            return true;
        }
        return false;
    }   //  --bool semente()--
};  //  --class Tim--

}   //  --namespace mdt--

/**
 * Critério de Delaunay Robusto com Verificação de Orientação
 *
 * Parâmetros: Coordenadas (x, y) de 32 bits (em décimos de mm)
 * Retorna:
 *   1 : Ponto D está DENTRO do círculo (viola Delaunay)
 *  -1 : Ponto D está FORA do círculo (satisfaz Delaunay)
 *   0 : Pontos são cocirculares
 */

/*
int check_delaunay(int32_t xa, int32_t ya,
                          int32_t xb, int32_t yb,
                          int32_t xc, int32_t yc,
                          int32_t xd, int32_t yd) {

    // 1. Translação dos pontos para que D seja a origem (0,0)
    // Usamos int64_t para evitar overflow nas subtrações e multiplicações simples
    const int64_t ax = (int64_t)xa - xd;
    const int64_t ay = (int64_t)ya - yd;
    const int64_t bx = (int64_t)xb - xd;
    const int64_t by = (int64_t)yb - yd;
    const int64_t cx = (int64_t)xc - xd;
    const int64_t cy = (int64_t)yc - yd;

    // 2. Cálculo da Orientação (Determinante 2x2 do triângulo ABC transladado)
    // Se det_ori > 0: Anti-horário (CCW)
    // Se det_ori < 0: Horário (CW)
    // Se det_ori = 0: Pontos colineares (triângulo degenerado)
    const int64_t det_ori = (ax * (by - cy)) + (bx * (cy - ay)) + (cx * (ay - by));

    if (det_ori == 0) return 0; // Caso degenerado: colinear

    // 3. Termos quadráticos (distância ao quadrado de D)
    // Máximo ~3.2e11 (cabe em int64_t)
    const int64_t a2 = ax * ax + ay * ay;
    const int64_t b2 = bx * bx + by * by;
    const int64_t c2 = cx * cx + cy * cy;

    // 4. Cálculo do Determinante de Delaunay usando __int128
    // Expandindo pela regra de Laplace
    const __int128 det_delaunay = (__int128)a2 * (bx * cy - cx * by)
                                  - (__int128)b2 * (ax * cy - cx * ay)
                                  + (__int128)c2 * (ax * by - bx * ay);

    // 5. Ajuste de Sinal pela Orientação
    // Se o triângulo for CW (det_ori < 0), o sinal do teste de círculo inverte.
    // Para manter a lógica: "positivo = dentro", multiplicamos pelo sinal da orientação.
    if (det_delaunay == 0) return 0;

    // Lógica consolidada:
    // Se det_delaunay e det_ori têm o mesmo sinal, D está DENTRO.
    if ((det_delaunay > 0 && det_ori > 0) || (det_delaunay < 0 && det_ori < 0)) {
        return 1;  // Dentro
    } else {
        return -1; // Fora
    }
}

*/
