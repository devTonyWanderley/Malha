#include <iostream>
#include <iomanip>
#include "massadados.h"

int main() {
    // Caminho da instância conforme solicitado
    const std::string caminhoArquivo = "C:\\2026\\Soft\\Instâncias\\Pontos.pdw";

    std::cout << "--- Iniciando Processamento de Massa de Dados ---" << std::endl;
    std::cout << "Arquivo: " << caminhoArquivo << "\n" << std::endl;

    // 1. Ingestão e Harmonização (Leitura de Amostras)
    std::vector<Geo::Amostra> amostrasBrutas = Massa::importar(caminhoArquivo);

    if (amostrasBrutas.empty()) {
        std::cerr << "Erro: Arquivo vazio ou nao encontrado!" << std::endl;
        return 1;
    }

    std::cout << "Pontos lidos: " << amostrasBrutas.size() << std::endl;

    // 2. Inteligência Espacial (Geração dos Pontos com DNA Morton)
    // O GerenciadorMassa executa o AnalisadorEspacial internamente
    std::vector<Geo::Ponto> pontosProcessados = Massa::GerenciadorMassa::processar(amostrasBrutas);

    // 3. Exibição dos Resultados
    std::cout << std::left << std::setw(18) << "Nome"
              << std::setw(12) << "X_Rel"
              << std::setw(12) << "Y_Rel"
              << std::setw(10) << "Z"
              << "DNA (Morton 2D Hex)" << std::endl;
    std::cout << std::string(75, '-') << std::endl;

    for (const auto& p : pontosProcessados) {
        // Recuperamos a amostra original para exibir o nome
        const auto& original = amostrasBrutas[p.id_ref];

        std::cout << std::left << std::setw(18) << original.nome()
                  << std::setw(12) << p.xl
                  << std::setw(12) << p.yl
                  << std::setw(10) << p.z
                  << "0x" << std::hex << std::uppercase << std::setw(16) << std::setfill('0')
                  << p.dna << std::dec << std::setfill(' ') << std::endl;
    }

    std::cout << "\n--- Processamento Concluido com Sucesso ---" << std::endl;

    return 0;
}
