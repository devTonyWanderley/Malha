#include <QApplication>
#include <QTimer>
#include <QDebug>
#include <vector>

// Seus headers de lógica (ajuste os caminhos se necessário)
#include "massadados.h"
#include "View/wcad.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 1. Instancia o Painel
    WView::painelCad painel;
    painel.setWindowTitle("Validação de Malha - Curva de Morton");
    painel.resize(1024, 768);
    painel.show();

    // 2. Importação e Processamento (Sua lógica de alta performance)
    std::string caminho = "C:\\2026\\Soft\\Instâncias\\Pontos.pdw";

    qDebug() << "Lendo arquivo:" << caminho.c_str();
    auto amostras = Massa::importar(caminho);

    if (amostras.empty()) {
        qCritical() << "Erro: Arquivo vazio ou não encontrado!";
        return -1;
    }

    qDebug() << "Processando" << amostras.size() << "pontos...";
    auto pontos = Massa::GerenciadorMassa::processar(amostras);

    // 3. Desenho Animado (Para verificar o comportamento/sentido)
    // Usamos um timer para não travar a interface e ver a ordem Morton
    int indice = 0;
    QTimer* timer = new QTimer(&painel);

    QObject::connect(timer, &QTimer::timeout, [&]() {
        if (indice < pontos.size()) {
            // Desenha em blocos de 50 para ser mais rápido, mas ainda perceptível
            for(int i = 0; i < 50 && indice < pontos.size(); ++i) {
                const auto& p = pontos[indice];

                // Adiciona o ponto na cena (xl, yl são coordenadas locais tratadas)
                painel.scene()->addEllipse(p.xl, p.yl, 0.2, 0.2,
                                           QPen(Qt::Cyan, 0), QBrush(Qt::Cyan));
                indice++;
            }
        } else {
            timer->stop();
            qDebug() << "Desenho concluído. Aplicando Zoom Limites.";
            painel.zoomLimites();
        }
    });

    timer->start(5); // Atualiza a cada 5ms

    return a.exec();
}
