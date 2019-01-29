#ifndef TMCENTRALWIDGET_H
#define TMCENTRALWIDGET_H

#include <QWidget>

#include "tmlogwindow.h"
#include "tmmessagewindow.h"

namespace Ui {
class TMCentralWidget;
}

class TMCentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TMCentralWidget(QWidget *parent = nullptr);
    virtual ~TMCentralWidget();


    TMLogWindow *getLogWindow();
    TMMessageWindow *getMessageWindow();

    void setCurrentIndex(const int &index);

private:
    Ui::TMCentralWidget *ui;
};

#endif // TMCENTRALWIDGET_H
