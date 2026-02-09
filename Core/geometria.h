#pragma once
#include <cmath>
#include <string>
#include <cstdint>
#include <immintrin.h>
#include <algorithm>
#include <variant>
#include <array>
#include <vector>

namespace Core
{
constexpr double EPSILON = 1e-12;
constexpr size_t VAZIO = 999999999;

inline std::string limpa(std::string s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {return !std::isspace(ch);}));
    return s;
}   //  --limpa--

inline uint64_t BMI2(uint32_t x, uint32_t y)
{
    return (_pdep_u64(x, 0xAAAAAAAAAAAAAAAA) | _pdep_u64(y, 0x5555555555555555));
}   //  --BMI2--

inline uint32_t uBMI2(uint64_t m, bool f)
{
    if(f) return _pext_u64(m, 0xAAAAAAAAAAAAAAAA);
    return _pext_u64(m, 0x5555555555555555);
}

struct Campo
{
    Campo(std::string i, std::string a): _nome(limpa(std::move(i))), _atr(limpa(std::move(a))){}

bool operator ==(const Campo &outro) const
{
    return ((_nome == outro._nome) && (_atr == outro._atr));
}   //  --operator==--

Campo operator = (Campo &outro)
{
    _nome = outro._nome;
    _atr = outro._atr;
    return *this;
}   //  --operator=--

std::string nome()
{
    return _nome;
}   //  --acesso a _nome--

std::string atr()
{
    return _atr;
}   //  --acesso a _atr--

private:
    std::string _nome, _atr;
};  //  --Campo--

struct Prj
{
    Prj(std::string a, int32_t d, int32_t o): _atr(limpa(std::move(a))), _da(d), _af(o){}

bool operator ==(const Prj &outro) const
{
    return ((_atr == outro._atr) && (_da == outro._da) && (_af == outro._af));
}   //  --operator==--

Prj operator =(Prj &outro)
{
    _atr = outro._atr;
    _da = outro._da;
    _af = outro._af;
    return *this;
}   //  --operator=--

std::string atr()
{
    return _atr;
}   //  --acesso a _atr--

int32_t da()
{
    return _da;
}   //  --acesso _da int32_t--

int32_t af()
{
    return _af;
}   //  --acesso _af int32_t--

double da_d()
{
    return (_da / 10000.0);
}   //  --acesso _da double--

double af_d()
{
    return (_af / 10000.0);
}   //  --acesso _af double--

private:
    std::string _atr;
    int32_t _da, _af;
};  //  --Prj--

struct Ponto
{
inline static int32_t x0 = 0, y0 = 0;

Ponto(uint32_t xg, uint32_t yg, uint32_t zg): _xl(xg - x0), _yl(yg - y0), _z(zg)
{
    _dna = BMI2(_xl, _yl);
}   //  --construtor de ponto genérico--

Ponto(std::string i, std::string a, uint32_t xg, uint32_t yg, uint32_t zg): _xl(xg - x0), _yl(yg - y0), _z(zg), _dados(Core::Campo(i, a))
{
    _dna = BMI2(_xl, _yl);
}   //  --construtor de ponto levantado--

Ponto(std::string a, int32_t d, int32_t o, uint32_t xg, uint32_t yg, uint32_t zg):
    _xl(xg - x0), _yl(yg - x0), _z(zg), _dados(Core::Prj(a, d, o))
{
    _dna = BMI2(_xl, _yl);
}   //  --construtor de ponto projetado--

uint64_t dna() const
{
    return _dna;
}

uint32_t xl() const
{
    return _xl;
}   //  --acesso _xl uint32_t--

uint32_t yl() const
{
    return _yl;
}   //  --acesso _yl uint32_t--

uint32_t z() const
{
    return _z;
}   //  --acesso _z uint32_t--

double xl_d() const
{
    return (_xl / 10000.0);
}   //  --acesso _xl em metros--

double yl_d() const
{
    return (_yl / 10000.0);
}   //  --acesso _yl em metros--

double x_d() const
{
    return ((_xl + x0) / 10000.0);
}   //  --acesso _x em metros--

double y_d() const
{
    return ((_yl + y0) / 10000.0);
}   //  --acesso _y em metros--

double z_d() const
{
    return (_z / 10000.0);
}   //  --acesso _z em metros--

auto dados() const
{
    return _dados;
}   //  --acesso a metadados--

private:
    uint32_t _xl, _yl, _z;
    std::variant<std::monostate, Campo, Prj> _dados;
    uint64_t _dna;
};  //  --Ponto--

struct Face
{
Face(size_t v0, size_t v1, size_t v2): _v({v0, v1, v2}), _o({VAZIO, VAZIO, VAZIO})
{}

Face(size_t v0, size_t v1, size_t v2, size_t o0, size_t o1, size_t o2): _v({v0, v1, v2}), _o({o0, o1, o2})
{}

size_t v(size_t i)
{
    return _v[i % 3];
}

size_t o(size_t i)
{
    return _o[i % 3];
}
private:
    std::array<size_t, 3> _v;  //  vértice
    std::array<size_t, 3> _o;  //  face oposta
};
class Superficie
{
//private:
    std::vector<Ponto> pontos;
    std::vector<Ponto> contorno;
    std::vector<Face> faces;
public:
    std::vector<Ponto> importaPontos(const std::string &path);
};
}   //  --Core--
