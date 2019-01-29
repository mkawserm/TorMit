#ifndef TMMESSAGEWINDOW_H
#define TMMESSAGEWINDOW_H

#include <QWidget>

namespace Ui {
class TMMessageWindow;
}

class TMMessageWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TMMessageWindow(QWidget *parent = nullptr);
    virtual ~TMMessageWindow();

public slots:
    void addMessage(const QString &message);
    void clearMessage();

    QString getInputMessage();
    void clearInputMessage();

    void setConnectButtonText(const QString &text);
    QString connectButtonText();

    QString getServiceId();
    void clearServiceId();
    void activateServiceId();
    void disableServiceId();

    QString myServiceId();
    void setMyServiceId(const QString &serviceId);

    void enableMessageWindow();
    void disableMessageWindow();
    void enableOnlyConnectWindow();

    void setConnectedState();
    void setDisconnectedState();

signals:
    void sendButtonClicked();
    void connectButtonClicked();

private slots:
    void on_messageSendBox_returnPressed();

private:
    Ui::TMMessageWindow *ui;
};

#endif // TMMESSAGEWINDOW_H
