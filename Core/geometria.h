#pragma once
#include <cstdint>
#include <immintrin.h>
#include <vector>
#include <algorithm>
#include <string>
#include <string_view>

namespace Geo
{

/**
 * @brief Ticket de Configuração Espacial (Imutável por massa de dados).
 */
struct ConfigCenario {
    uint32_t x0, y0;
    uint32_t sx, sy;
    uint32_t dim_x, dim_y;
};

/**
 * @brief Estrutura de Ponto de Alta Performance (Alinhada para Cache/SIMD).
 *
 * Otimizado para buscas espaciais rápidas. O DNA Morton permite que o
 * processamento 2D seja feito em 1D sobre um vetor ordenado.
 */
struct alignas(32) Ponto
{
    uint32_t xl, yl;    // Coordenadas locais (transladadas)
    uint32_t z;         // Cota absoluta (sem modificação)
    uint32_t id_ref;    // Vínculo (índice) com a struct Amostra original
    uint64_t dna;       // Código Morton 2D (Z-Order Curve)

    /**
     * @brief Construtor que projeta o ponto no container binário perfeito.
     * @param x_abs, y_abs Coordenadas brutas (Campo ou Projeto).
     * @param id Índice do ponto no vetor original de Amostras.
     * @param cfg Configuração calculada pelo Analisador.
     */
    Ponto(uint32_t x_abs, uint32_t y_abs, uint32_t z_val, uint32_t id, const ConfigCenario& cfg)
        : xl(x_abs - cfg.x0), yl(y_abs - cfg.y0), z(z_val), id_ref(id)
    {
        // Normalização de Aspecto: "Quadraturização" virtual do retângulo
        uint64_t xv = static_cast<uint64_t>(xl) << cfg.sx;
        uint64_t yv = static_cast<uint64_t>(yl) << cfg.sy;

        // Intercalação de bits via BMI2 (Hardware)
        const uint64_t m_x = 0x5555555555555555;
        const uint64_t m_y = 0xAAAAAAAAAAAAAAAA;
        dna = _pdep_u64(xv, m_x) | _pdep_u64(yv, m_y);
    }

    // Comparador para busca binária e ordenação O(N log N)
    inline bool operator<(const Ponto& outro) const { return dna < outro.dna; }
};

/**
 * @brief Camada de Transporte para Dados Brutos (Identidade do Ponto).
 *
 * Utiliza std::move para evitar duplicar alocações de string no heap
 * durante a ingestão da massa de campo.
 */
struct Amostra
{
    Amostra(std::string id, std::string atr, uint32_t x, uint32_t y, uint32_t z)
        : _nome(std::move(id)), _atributo(std::move(atr)), _abc(x), _ord(y), _cot(z)
    {}

    // Getters por referência constante: Performance crítica para evitar cópias de string
    const std::string& nome() const { return _nome; }
    const std::string& atributo() const { return _atributo; }
    uint32_t abc() const { return _abc; }
    uint32_t ord() const { return _ord; }
    uint32_t cot() const { return _cot; }

private:
    std::string _nome, _atributo;
    uint32_t _abc, _ord, _cot;
};

} // namespace Geo
