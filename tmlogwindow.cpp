#include "tmlogwindow.h"
#include "ui_tmlogwindow.h"

TMLogWindow::TMLogWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TMLogWindow)
{
    ui->setupUi(this);
}

TMLogWindow::~TMLogWindow()
{
    delete ui;
}

void TMLogWindow::clear()
{
    this->ui->logBox->clear();
}

void TMLogWindow::addLog(const QString &logMessage)
{
    this->ui->logBox->appendPlainText(logMessage);
}

void TMLogWindow::setMessage(const QString &message)
{
    this->ui->messageBox->setText(message);
}

void TMLogWindow::clearMessage()
{
    this->ui->messageBox->clear();
}

void TMLogWindow::addTorStatus(const QString &status)
{
    this->ui->torStatus->setText(QString("Tor Status: <b>%1</b>").arg(status));
}
