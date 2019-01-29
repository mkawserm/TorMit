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

void TMMainWindow::setStatusBarText(const QString &message, int stime)
{
    this->ui->statusBar->showMessage(message,stime);
}

void TMMainWindow::clearStatusBarText()
{
    this->ui->statusBar->clearMessage();
}
