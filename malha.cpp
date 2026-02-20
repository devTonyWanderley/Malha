#include "malha.h"
#include "ui_malha.h"

malha::malha(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::malha)
{
    ui->setupUi(this);
}

malha::~malha()
{
    delete ui;
}
