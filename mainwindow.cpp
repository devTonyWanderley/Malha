#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Teste();
}

void MainWindow::Teste()
{
    std::vector<std::vector<std::string>> buf = IO::importaFix("C:/Tony/NovaSede/Pontos.pdw", IO::cfgColunas({16, 16, 12, 12, 12}));
    if(!buf.empty())
    {
        for(auto ln : buf)
        {
            std::string linha = "";
            for(auto cmp : ln)
                linha += (' ' + cmp);
            qDebug() << linha;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
