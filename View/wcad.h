#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QKeyEvent>
#include "../Core/massadados.h"

namespace WView {
class painelCad : public QGraphicsView {
    Q_OBJECT
public:
    explicit painelCad(QWidget* parent = nullptr);
    void carregarTeste(); // Apenas para validar a cruz e o ponto

    // Desenha os pontos processados pela Geo::Massa
    void desenharPontos(const std::vector<Geo::Ponto>& pontos);

    // Limpa a cena (útil para recarregar dados)
    void limparCena();

    void chamaZoomLimites(){zoomLimites();}
protected:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void recalcularHorizonte();
    void mouseReleaseEvent(QMouseEvent* event) override;
    void zoomLimites();
private:
    QGraphicsScene* cena;
};
}
