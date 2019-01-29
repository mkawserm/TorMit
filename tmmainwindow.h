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

private:
    Ui::TMMainWindow *ui;
};

#endif // TMMAINWINDOW_H
