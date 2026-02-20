#ifndef MALHA_H
#define MALHA_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class malha;
}
QT_END_NAMESPACE

class malha : public QMainWindow
{
    Q_OBJECT

public:
    malha(QWidget *parent = nullptr);
    ~malha();

private:
    Ui::malha *ui;
};
#endif // MALHA_H
