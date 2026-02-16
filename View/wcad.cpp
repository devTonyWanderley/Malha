
#include "wcad.h"

namespace WView {

painelCad::painelCad(QWidget* parent) : QGraphicsView(parent) {
    cena = new QGraphicsScene(this);
    setScene(cena);
    setBackgroundBrush(Qt::black);


    setAlignment(Qt::AlignCenter);


    // 1. Desativa as barras de rolagem DE VEZ (elas roubam o clique do Pan)
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 2. Garante que o modo de arrasto é a Mãozinha
    setDragMode(QGraphicsView::ScrollHandDrag);

    // 3. O SEGREDO DO PAN INVERTIDO:
    // Quando invertemos o Y com scale(1, -1), o Qt pode se perder no sinal do deslocamento.
    // Esta flag garante que a renderização e a interação usem a matriz de forma direta.
    setOptimizationFlag(QGraphicsView::DontSavePainterState);

    // 1. A ÂNCORA: Aqui você dá a ordem ao processador
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // 2. O PAPEL: Definimos um canteiro de 200m (Suficiente para nossa cruz e ponto)
    // Sem isso, a âncora "escorrega" porque não tem referencial fixo.
    //setSceneRect(-100, -100, 200, 200); -> remover o referencial fixo...

    // 3. A GEOMETRIA: Cruz no zero e ponto no (10,10)
    cena->addLine(-10, 0, 10, 0, QPen(Qt::green, 0));
    cena->addLine(0, -10, 0, 10, QPen(Qt::green, 0));
    cena->addEllipse(10 - 0.5, 10 - 0.5, 1, 1, QPen(Qt::red, 0), QBrush(Qt::red));

    // 4. O NORTE: Inversão do eixo Y
    scale(1, -1);
}

// 2. A Lógica de Expansão do Horizonte
void painelCad::recalcularHorizonte() {
    // Pegamos a área que a lente (view) está enxergando agora (em metros/coordenadas de cena)
    QRectF areaVisivel = mapToScene(viewport()->rect()).boundingRect();

    // Pegamos a área ocupada pelos desenhos (a cruz e o ponto)
    QRectF areaObjetos = cena->itemsBoundingRect();

    // O novo tamanho do papel será a UNIÃO entre o que vemos e o que temos,
    // com uma margem generosa para que a "mãozinha" sempre tenha onde puxar.
    double margem = areaVisivel.width() * 2.0;
    setSceneRect(areaVisivel.united(areaObjetos).adjusted(-margem, -margem, margem, margem));
}

void painelCad::wheelEvent(QWheelEvent* event) {
    double escala = (event->angleDelta().y() > 0) ? 1.15 : 0.85;
    scale(escala, escala);

    recalcularHorizonte(); // Expande o horizonte logo após o zoom
}

// Crucial: Precisamos atualizar o horizonte quando soltamos o mouse após um PAN
void painelCad::mouseReleaseEvent(QMouseEvent* event) {
    recalcularHorizonte();
    QGraphicsView::mouseReleaseEvent(event);
}

// Implementação mínima para satisfazer o Linker
void painelCad::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Z || event->key() == Qt::Key_E) {
        zoomLimites();
    }
    QGraphicsView::keyPressEvent(event);
}

void painelCad::zoomLimites() {
    // 1. Pergunta à cena: "Qual é o retângulo que envolve tudo o que foi desenhado?"
    // Isso inclui a Cruz Verde e o Ponto Vermelho (e futuramente toda a malha)
    QRectF limitesDaObra = cena->itemsBoundingRect();

    // Segurança: se a cena estiver vazia, não há o que enquadrar
    if (limitesDaObra.isEmpty()) return;

    // 2. A Mágica do Qt: ajusta a escala e a translação para caber na tela
    // Qt::KeepAspectRatio garante que o círculo não vire uma elipse (mantém a proporção)
    fitInView(limitesDaObra, Qt::KeepAspectRatio);

    // 3. Após o enquadramento, atualizamos o horizonte para permitir o PAN imediato
    recalcularHorizonte();
}

//  adcionado:
void painelCad::limparCena() {
cena->clear();
// Re-adiciona a cruz de referência (opcional)
cena->addLine(-10, 0, 10, 0, QPen(Qt::green, 0));
cena->addLine(0, -10, 0, 10, QPen(Qt::green, 0));
}

void painelCad::desenharPontos(const std::vector<Geo::Ponto>& pontos) {
    // Pen fina (0) garante que o tamanho do ponto não mude com o zoom
    QPen pen(Qt::yellow, 0);
    QBrush brush(Qt::yellow);
    double tamanhoPonto = 0.5;

    for (const auto& p : pontos) {
        // Usamos as coordenadas originais (recalculando o x_abs do seu Ponto)
        // ou você pode passar o vetor de Amostras original.
        // Aqui assumirei que você quer ver a posição relativa:
        cena->addEllipse(p.xl - (tamanhoPonto/2),
                         p.yl - (tamanhoPonto/2),
                         tamanhoPonto, tamanhoPonto,
                         pen, brush);

        // Se quiser ver a ordem de processamento (DNA Morton), podemos dar um pequeno delay
        // Mas para renderização em tempo real, evite loops com sleep aqui.
    }
    zoomLimites(); // Enquadra tudo automaticamente após desenhar
}

}
