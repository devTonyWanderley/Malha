#include <QDebug>
#include "geohl.h"

int main()
{
    qDebug()
        << "Hello World!!!"
        << "\nLer e apresentar as WAmostras:";

    //  instanciar WMassa:
    whlio::WMassa massa;
    massa.importa("C:\\2026\\Soft\\Instâncias\\Pontos.pdw");
    qDebug()
        << "\nTamanho de \"amostras\":\t"
        << massa.amostras.size();
    /*
    for(const auto i : massa.amostras)
    {
        qDebug()
            << i.nome << '\t'
            << i.atri << '\t'
            << i.abci << '\t'
            << i.orde << '\t'
            << i.cota << '\t';
    }
    */
    qDebug()
        << "min: [" << massa.xmin << " , " << massa.ymin << "]\nmax: [" << massa.xmax << " , " << massa.ymax << "]\n"
        << "b: " << massa.xmax - massa.xmin << "\tdx: " << massa.sb << '\n'
        << "h: " << massa.ymax - massa.ymin << "\tdy: " << massa.sh;
    return 0;
}

/*
#include <QApplication>
#include <QDebug>
#include "massadados.h"
#include "View/wcad.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 1. Instancia e configura a janela
    WView::painelCad painel;
    painel.setWindowTitle("Visualizador de Malha");
    painel.resize(1280, 720);
    painel.show();

    // 2. Lógica de Dados
    std::string caminho = "C:\\2026\\Soft\\Instâncias\\Pontos.pdw";
    qDebug() << "Lendo arquivo:" << caminho.c_str();

    auto amostras = Massa::importar(caminho);

    if (amostras.empty()) {
        qCritical() << "Erro: Arquivo não carregado.";
        return -1;
    }

    qDebug() << "Processando" << amostras.size() << "pontos...";
    auto pontos = Massa::GerenciadorMassa::processar(amostras);

    // 3. Desenha tudo de uma vez (Sem animação)
    painel.desenharPontos(pontos);

    return a.exec();
}
*/
