#ifndef TMLOGWINDOW_H
#define TMLOGWINDOW_H

#include <QWidget>

namespace Ui {
class TMLogWindow;
}

class TMLogWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TMLogWindow(QWidget *parent = nullptr);
    virtual ~TMLogWindow();


public slots:
    void clear();
    void addLog(const QString &logMessage);

    void setMessage(const QString &message);
    void clearMessage();

    void addTorStatus(const QString &status);

private:
    Ui::TMLogWindow *ui;
};

#endif // TMLOGWINDOW_H
