#ifndef MYKEYEVENT_H
#define MYKEYEVENT_H

#include <QWidget>
#include "ui_mainwindow.h"
#include <QKeyEvent>

class MyKeyEvent : public QWidget
{
    Q_OBJECT
public:
    explicit MyKeyEvent(Ui::MainWindow *ui);

signals:
private:
    Ui::MainWindow *_ui;
    void keyPressEvent(QKeyEvent *ev);

};

#endif // MYKEYEVENT_H
