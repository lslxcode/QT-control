#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWebEngineWidgets>
#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
#include <QWidget>
#include <QUdpSocket>
#include <QComboBox>

#include <VLCQtWidgets/WidgetSeek.h>
#include <VLCQtWidgets/WidgetVideo.h>
#include <VLCQtWidgets/WidgetVolumeSlider.h>
#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>
#include "senddata.h"
#include "mqtt_c.h"
#include "joystick.h"
#include "mykeyevent.h"




QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //void keyPressEvent(QKeyEvent *ev);
    MyKeyEvent *myKeyEvent;
    /*******************QT-vlc**************/
    VlcInstance *Instance;
    VlcMedia *VideoMedia;
    VlcMediaPlayer *VideoPlayer;
    VlcMedia *AudioMedia;
    VlcMediaPlayer *AudioPlayer;
    VlcWidgetVideo *Video;
    /***********QT-vlc END******************/


    senddata *Mysenddataah8301;
    senddata *Mysenddata4g;
    QTimer *timer_start;
    /***************************************/
    QWebEngineView *view;

private:
    int mqttconnectstatus = 0;
    int firestatus = 0;
    Ui::MainWindow *ui;
    QString lon1,lat1;
    QString globaldata;

signals:
    void runThread();
private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_4_clicked();
    void updateUI(QString text);
    void loadFinished();


};
#endif // MAINWINDOW_H
