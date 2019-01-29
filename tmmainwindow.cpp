#include "tmmainwindow.h"
#include "ui_tmmainwindow.h"

TMMainWindow::TMMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TMMainWindow)
{
    ui->setupUi(this);
}

TMMainWindow::~TMMainWindow()
{
    delete ui;
}
