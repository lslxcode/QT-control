#include "mykeyevent.h"

MyKeyEvent::MyKeyEvent(Ui::MainWindow *ui) : _ui(ui)
{
    this->grabKeyboard();
}
void MyKeyEvent::keyPressEvent(QKeyEvent *ev)
{
    //qDebug()<<" keyPressEvent Key_Up11111111!";
    int ret = 0;
    if(ev->key() == Qt::Key_Up)
    {
        //  qDebug()<<" keyPressEvent Key_Up!";
        ret = _ui->verticalSlider->value();
        _ui->verticalSlider->setValue(ret + 5);
        ret = 0;

        return ;
    }
    else     if(ev->key() == Qt::Key_Down)
    {
        //qDebug()<<" keyPressEvent Key_Down!";
        ret = _ui->verticalSlider->value();
        _ui->verticalSlider->setValue(ret - 5);
        ret = 0;
        return ;
    }
    else if(ev->key() == Qt::Key_Left)
    {
        //qDebug()<<" keyPressEvent Key_Left!";
        ret = _ui->horizontalSlider->value();
        _ui->horizontalSlider->setValue(ret - 5);
        ret = 0;
        return ;
    }
    else if(ev->key() == Qt::Key_Right)
    {
        // qDebug()<<" keyPressEvent Key_Right!";
        ret = _ui->horizontalSlider->value();
        _ui->horizontalSlider->setValue(ret + 5);
        ret = 0;
        return ;
    }
    return QWidget::keyPressEvent(ev);
}
