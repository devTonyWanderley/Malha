#pragma once
#include "geometria.h"
#include "../IO/arquivo.h"

namespace Massa
{

inline std::vector<Geo::Amostra> importar(const std::string& arquivo)
{
    std::vector<std::vector<std::string>> dados = IO::MIO::importarFixo(arquivo, IO::layoutConfig{16, 16, 12, 12, 12});
    if(dados.empty()) return {};
    std::vector<Geo::Amostra> r;
    r.reserve(dados.size());
    for(auto& linha : dados)
    {
        if(linha.size() < 5) continue;
        r.emplace_back(linha[0], linha[1],
                       static_cast<uint32_t>(std::stol(linha[2])),
                       static_cast<uint32_t>(std::stol(linha[3])),
                       static_cast<uint32_t>(std::stol(linha[4]))
                       );
    }
    return r;
}

/**
 * @brief Cérebro do sistema: Transforma dados brutos em inteligência espacial.
 */
class GerenciadorMassa
{
public:
    /**
     * @brief Processa e harmoniza Amostras em Pontos ordenados.
     * @param amostras Vetor de entrada (Campo ou Projeto).
     * @return Vetor de Pontos prontos para QuadTree Linear.
     */
    static std::vector<Geo::Ponto> processar(const std::vector<Geo::Amostra>& amostras)
    {
        if (amostras.empty()) return {};

        // 1. Fase de Análise (Cálculo do Universo Binário)
        Geo::ConfigCenario cfg = analisar_espaco(amostras);

        // 2. Fase de Instanciação e Vínculo
        std::vector<Geo::Ponto> processados;
        processados.reserve(amostras.size());

        for (uint32_t i = 0; i < amostras.size(); ++i) {
            const auto& a = amostras[i];
            processados.emplace_back(a.abc(), a.ord(), a.cot(), i, cfg);
        }

        // 3. Fase de Ordenação (Garante Localidade de Cache)
        std::sort(processados.begin(), processados.end());

        return processados;
    }

private:
    static Geo::ConfigCenario analisar_espaco(const std::vector<Geo::Amostra>& dados)
    {
        uint32_t min_x = dados[0].abc(), max_x = dados[0].abc();
        uint32_t min_y = dados[0].ord(), max_y = dados[0].ord();

        for (const auto& p : dados) {
            min_x = std::min(min_x, p.abc()); max_x = std::max(max_x, p.abc());
            min_y = std::min(min_y, p.ord()); max_y = std::max(max_y, p.ord());
        }

        uint32_t dx = (max_x - min_x > 0) ? (max_x - min_x) : 1;
        uint32_t dy = (max_y - min_y > 0) ? (max_y - min_y) : 1;

        Geo::ConfigCenario cfg;
        cfg.dim_x = prox_pot2(dx);
        cfg.dim_y = prox_pot2(dy);

        // Centralização Matemática (Alinhamento dos Centros)
        uint32_t cx = min_x + (dx / 2);
        uint32_t cy = min_y + (dy / 2);

        // Clamping para evitar underflow em uint32
        cfg.x0 = (cx > cfg.dim_x / 2) ? (cx - cfg.dim_x / 2) : 0;
        cfg.y0 = (cy > cfg.dim_y / 2) ? (cy - cfg.dim_y / 2) : 0;

        // Shifts de Escala (Normalização de Retângulo para Quadrado Virtual)
        // Usamos -1 para garantir que clz não receba 0
        cfg.sx = clz(cfg.dim_x - 1);
        cfg.sy = clz(cfg.dim_y - 1);

        return cfg;
    }

    static uint32_t prox_pot2(uint32_t v) {
        if (v == 0) return 1;
        v--;
        v |= v >> 1; v |= v >> 2; v |= v >> 4; v |= v >> 8; v |= v >> 16;
        return ++v;
    }

    static uint32_t clz(uint32_t v) {
        if (v == 0) return 32;
#ifdef _MSC_VER
        unsigned long lz = 0;
        return _BitScanReverse(&lz, v) ? 31 - lz : 32;
#else
        return __builtin_clz(v);
#endif
    }
};
}   //  namespace Massa
