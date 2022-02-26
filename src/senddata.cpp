#include "senddata.h"

senddata::senddata(Ui::MainWindow *ui,QString ipc,quint32 portc)
//    :_ui(ui)
//    ,_init(1)
//    ,pre_pos(-1)
//    ,pre_horizontal(-1)
//    ,pre_vertical(-1)
//    ,speedStatus(-1)
//    ,myclient(new MyQmqttClient(ipc,portc))
{

    this->_ui = ui;
    this->_init = 1;
    this->pre_pos = -1;
    this->pre_horizontal = -1;
    this->pre_vertical = -1;
    this->speedStatus = -1;
    this->myclient = new MyQmqttClient(ipc,portc);
}
void senddata::sendValue()
{
    static int flag = 1;

    int **result_speed = get_point();
    getBtnStatus(result_BtnStatus);
    if(this->_init == 1)
    {
        QByteArray msg;
        msg.resize(2);
        QString topic("telecar1/control/rawcmd");
        msg[0] = (uchar)0;
        msg[1] = (uchar)0;
        myclient->send(topic, msg);
        this->_init = 0;
    }


    int pos = motionConfig(result_speed[0][0],result_speed[0][1]);
    if(pos != pre_pos)
    {
        if(pos != 0)
        {

            QByteArray msg;
            msg.resize(2);
            QString topic("telecar1/control/motion");


            msg[0] = (uchar)pos;
            msg[1] = (uchar)pos; //flag
            myclient->send(topic, msg);
            pre_pos = pos;
        }
        else
        {
            QByteArray msg;
            msg.resize(2);
            QString topic("telecar1/control/motion");


            msg[0] = (uchar)pos;
            msg[1] = (uchar)pos; //flag
            myclient->send(topic, msg);
            pre_pos = pos;
        }
        flag = 1;

    }
    flag ++;

    if(speedStatus !=carSpeed)
    {

        QByteArray msg;
        msg.resize(2);
        QString topic("telecar1/control/speed");
        msg[0] = (uchar)carSpeed;
        msg[1] = (uchar)carSpeed; //flag
        myclient->send(topic, msg);

        speedStatus = carSpeed;
    }

    if(result_BtnStatus[0] == 0)
    {
        _ui->verticalSlider->setValue(_ui->verticalSlider->value()-5);

    }
    else if(result_BtnStatus[3] == 0)
    {
        _ui->verticalSlider->setValue(_ui->verticalSlider->value()+5);
    }
    else if(result_BtnStatus[2] == 0)
    {
        _ui->horizontalSlider->setValue(_ui->horizontalSlider->value()-5);
    }
    else if(result_BtnStatus[1] == 0)
    {
        _ui->horizontalSlider->setValue(_ui->horizontalSlider->value()+5);
    }
    free(result_speed);
    if(result_BtnStatus[7] == 0)
    {
        _ui->horizontalSlider->setValue(50);
        _ui->verticalSlider->setValue(50);
        //qDebug()<<"carSpeed : "<<carSpeed;

    }
    if(result_BtnStatus[5] == 0)
    {
        if(carSpeed < 100)
        {
            carSpeed =carSpeed+10;
        }
        qDebug()<<"carSpeed : "<<carSpeed;
    }
    if(result_BtnStatus[4] == 0)
    {
        if(carSpeed > 70)
        {
            carSpeed =carSpeed-10;
        }
        qDebug()<<"carSpeed : "<<carSpeed;
    }
    int verticalSliderValue = (99 - _ui->horizontalSlider->value())*1.6;
    if(verticalSliderValue != pre_vertical)
    {

        QByteArray msg;
        msg.resize(2);
        QString topic("telecar1/control/steer");
        //QString topic("steer");

        msg[0] = (uchar)1;
        msg[1] = (uchar)verticalSliderValue; //flag

        myclient->send(topic, msg);
        pre_vertical = verticalSliderValue;
    }


    int horizontalSliderValue = (99 - _ui->verticalSlider->value())*1.6;
    if(horizontalSliderValue != pre_horizontal)
    {

        QByteArray msg;
        msg.resize(2);
        QString topic("telecar1/control/steer");
        //QString topic("steer");
        msg[0] = 2;
        msg[1] = (uchar) horizontalSliderValue;
        myclient->send(topic, msg);
        pre_horizontal = horizontalSliderValue;
    }
}



int senddata::motionConfig(int x,int y)
{
    //qDebug()<<"x : " << x << " y : "<<y;
    if((y <= (MIDPOS - THRESHOLD)) && (x<= (MIDPOS - THRESHOLD)))
    {
        qDebug("forward_left");
        return 5;
        //return 1;
    }
    else if((y <= (MIDPOS - THRESHOLD)) && (x <= (MIDPOS + THRESHOLD)))
    {
        qDebug("forward");
        return 1;
    }
    else if((y <= (MIDPOS - THRESHOLD)))
    {
        qDebug("forward_right");
        return 7;
    }
    else if((y<= (MIDPOS + THRESHOLD)) && (x <= (MIDPOS - THRESHOLD)))
    {
        qDebug("turnleft");
        return 4;
    }
    else if((y <= (MIDPOS + THRESHOLD)) && (x<= (MIDPOS + THRESHOLD)))
    {
        // qDebug("stop");
        return 0;
    }
    else if(y <= (MIDPOS + THRESHOLD))
    {
        qDebug("turnright");
        return 3;
    }
    else if(x <= (MIDPOS - THRESHOLD))
    {
        qDebug("backward_left");
        return 6;
    }
    else if(x <= (MIDPOS + THRESHOLD))
    {
        qDebug("backward");
        return 2;
    }
    else
    {
        qDebug("backward_right");
        return 8;
    }

}



