#include "wcad.h"

namespace WView {

painelCad::painelCad(QWidget* parent) : QGraphicsView(parent) {
    cena = new QGraphicsScene(this);
    setScene(cena);
    setBackgroundBrush(Qt::black);
    setAlignment(Qt::AlignCenter);

    // Configurações de Navegação
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setDragMode(QGraphicsView::ScrollHandDrag);
    setOptimizationFlag(QGraphicsView::DontSavePainterState);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Inversão do eixo Y (Matemática -> Engenharia)
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
    // Removidas as linhas da cruz verde que estavam aqui
}

void painelCad::desenharPontos(const std::vector<Geo::Ponto>& pontos) {
    if (pontos.empty()) return;

    limparCena(); // Garante tela limpa antes de novos dados

    QPen penPonto(Qt::yellow, 0);
    QBrush brushPonto(Qt::yellow);
    QPen penLinha(Qt::cyan, 0); // Cor das conexões

    double tamanhoPonto = 50.0;
    const Geo::Ponto* anterior = nullptr;

    for (const auto& p : pontos) {
        // 1. Desenha o ponto
        cena->addEllipse(p.xl - (tamanhoPonto/2),
                         p.yl - (tamanhoPonto/2),
                         tamanhoPonto, tamanhoPonto,
                         penPonto, brushPonto);

        // 2. Conecta ao ponto anterior
        if (anterior) {
            cena->addLine(anterior->xl, anterior->yl, p.xl, p.yl, penLinha);
        }
        anterior = &p;
    }

    zoomLimites(); // Enquadra automaticamente
}
}
