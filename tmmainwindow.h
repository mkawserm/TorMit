#ifndef TMMAINWINDOW_H
#define TMMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class TMMainWindow;
}

class TMMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TMMainWindow(QWidget *parent = nullptr);
    virtual ~TMMainWindow();

public slots:
    void setStatusBarText(const QString &message, int stime=0);
    void clearStatusBarText();

private:
    Ui::TMMainWindow *ui;
};

#endif // TMMAINWINDOW_H
