#include "tmmessagewindow.h"
#include "ui_tmmessagewindow.h"

TMMessageWindow::TMMessageWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TMMessageWindow)
{
    ui->setupUi(this);

    this->connect(this->ui->sendBox,&QPushButton::clicked,this,&TMMessageWindow::sendButtonClicked);
    this->connect(this->ui->connectBox,&QPushButton::clicked,this,&TMMessageWindow::connectButtonClicked);
}

TMMessageWindow::~TMMessageWindow()
{
    delete ui;
}

void TMMessageWindow::addMessage(const QString &message)
{
    this->ui->messageBox->appendPlainText(message);
}

void TMMessageWindow::clearMessage()
{
    this->ui->messageBox->clear();
}

QString TMMessageWindow::getInputMessage()
{
    return this->ui->messageSendBox->text();
}

void TMMessageWindow::clearInputMessage()
{
    this->ui->messageSendBox->clear();
}

void TMMessageWindow::setConnectButtonText(const QString &text)
{
    this->ui->connectBox->setText(text);
}

QString TMMessageWindow::getServiceId()
{
    return this->ui->serviceIdBox->text();
}

void TMMessageWindow::clearServiceId()
{
    this->ui->serviceIdBox->clear();
}

void TMMessageWindow::activateServiceId()
{
    this->ui->serviceIdBox->setReadOnly(false);
    this->ui->serviceIdBox->setEnabled(true);
}

void TMMessageWindow::disableServiceId()
{
    this->ui->serviceIdBox->setReadOnly(true);
    this->ui->serviceIdBox->setEnabled(false);
}

QString TMMessageWindow::myServiceId()
{
    return this->ui->myServiceIdBox->text();
}

void TMMessageWindow::setMyServiceId(const QString &serviceId)
{
    this->ui->myServiceIdBox->setText(serviceId);
}

void TMMessageWindow::enableMessageWindow()
{
    this->ui->serviceIdBox->setReadOnly(false);
    this->ui->connectBox->setEnabled(true);

    this->ui->messageSendBox->setReadOnly(false);
    this->ui->sendBox->setEnabled(true);
}

void TMMessageWindow::disableMessageWindow()
{
    this->ui->serviceIdBox->setReadOnly(true);
    this->ui->connectBox->setEnabled(false);

    this->ui->messageSendBox->setReadOnly(true);
    this->ui->sendBox->setEnabled(false);
}

void TMMessageWindow::enableOnlyConnectWindow()
{
    this->ui->serviceIdBox->setReadOnly(false);
    this->ui->connectBox->setEnabled(true);
}

void TMMessageWindow::setConnectedState()
{
    this->ui->serviceIdBox->setReadOnly(true);
    this->ui->serviceIdBox->setEnabled(false);
    this->ui->connectBox->setEnabled(true);
    this->ui->connectBox->setText("DISCONNECT");

    this->ui->messageSendBox->setReadOnly(false);
    this->ui->sendBox->setEnabled(true);
}

void TMMessageWindow::setDisconnectedState()
{
    this->ui->serviceIdBox->setReadOnly(false);
    this->ui->serviceIdBox->setEnabled(true);
    this->ui->connectBox->setEnabled(true);
    this->ui->connectBox->setText("CONNECT");

    this->ui->messageSendBox->setReadOnly(true);
    this->ui->sendBox->setEnabled(false);
}

void TMMessageWindow::enableConnectButton()
{
    this->ui->connectBox->setEnabled(true);
}

void TMMessageWindow::disableConnectButton()
{
    this->ui->connectBox->setEnabled(false);
}

int TMMessageWindow::messageLength()
{
    return this->ui->messageSendBox->text().length();
}

QString TMMessageWindow::connectButtonText()
{
    return this->ui->connectBox->text();
}

void TMMessageWindow::on_messageSendBox_returnPressed()
{
    if(this->ui->sendBox->isEnabled() && !this->ui->messageSendBox->text().isEmpty())
    {
        emit sendButtonClicked();
    }
}

void TMMessageWindow::on_messageSendBox_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    if(!ui->messageSendBox->text().isEmpty())
    {
        emit typingMessage();
    }
}
