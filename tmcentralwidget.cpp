#include "tmcentralwidget.h"
#include "ui_tmcentralwidget.h"

TMCentralWidget::TMCentralWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TMCentralWidget)
{
    ui->setupUi(this);
}

TMCentralWidget::~TMCentralWidget()
{
    delete ui;
}

TMLogWindow *TMCentralWidget::getLogWindow()
{
    return this->ui->logWindow;
}

TMMessageWindow *TMCentralWidget::getMessageWindow()
{
    return this->ui->messageWindow;
}

void TMCentralWidget::setCurrentIndex(const int &index)
{
    this->ui->tabWidget->setCurrentIndex(index);
}
