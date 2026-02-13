#pragma once
#include <cstdint>
#include <array>

namespace Geo
{
struct Ponto
{
    //  entram construtores
    //  entra função de acesso a _loc
    static uint64_t x0, y0; //  contantes x0 e y0 de translação
private:
    std::array<uint32_t, 3> _loc;   //  x, y, z locais
};   //  --struct Ponto--

struct Qno: public Ponto
{
    //  entram construtores
    std::array<uint32_t, 4> sd; //  semi-largura e semi-altura do quadro, x e y do centro do quadro (binário perfeito)
    Ponto p;    //  único morador do quadro
    std::array<Qno*, 4> filhos;
};   //  --struct Qno: public Ponto--

class QTree
{
    static std::array<uint32_t, 4> _sd; //  semi-largura e semi-altura do espaço total, x e y do centro do espaço total (binário perfeito)
public:
    Qno *raiz;
    //  entram construtores
};
}   //  --namespace Geo--
