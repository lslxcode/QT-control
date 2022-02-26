#ifndef SENDDATA_H
#define SENDDATA_H

#include <QObject>
#include <QTcpSocket>
#include "ui_mainwindow.h"
#include "mqtt_c.h"
#include "joystick.h"
#define MAXPOS 65536
#define MINPOS 0
#define MIDPOS 32767
#define THRESHOLD 3000

class senddata : public QObject
{
    Q_OBJECT
public:
    explicit senddata(Ui::MainWindow *ui,QString ipc,quint32 portc);
    void sendValue();
    MyQmqttClient *myclient;
signals:

private slots:


private:
    Ui::MainWindow *_ui;
    int motionConfig(int x,int y);

    int _init;
    int speedStatus;

    int pre_pos;
    int pre_vertical;
    int pre_horizontal;

    char result_BtnStatus[11];
    int carSpeed = 70;

};

#endif // SENDDATA_H
