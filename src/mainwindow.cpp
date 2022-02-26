#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /************load html******************/
    view = new QWebEngineView(parent);
    view->load(QUrl("file:///home/haha/Desktop/ah8301/code/camera/qtcamera/map.html"));
    /************load html******************/

    /**************4G UDP******************/
    QUdpSocket *videoSocket = new QUdpSocket;
    videoSocket->bind(30002,QUdpSocket::ShareAddress);
    videoSocket->writeDatagram("V", 1, QHostAddress("123.56.126.168"), 30000);

    QUdpSocket *audioSocket = new QUdpSocket;

    audioSocket->bind(30004);
    audioSocket->writeDatagram("A", 1, QHostAddress("123.56.126.168"), 30000);
    /****************4G UDP****************/

    /*******************QT-vlc**************/
    Instance = new VlcInstance(VlcCommon::args(), this);
    VideoPlayer = new VlcMediaPlayer(Instance);
    AudioPlayer = new VlcMediaPlayer(Instance);
    Video = new VlcWidgetVideo(this);
    VideoPlayer->setVideoWidget(Video);


    VlcWidgetVolumeSlider *VolumeSlider = new VlcWidgetVolumeSlider;
    VolumeSlider->setMediaPlayer(AudioPlayer);
    VolumeSlider->setVolume(80);
    ui->gridLayout->addWidget(Video, 0, 0);
    QString file1 = "/home/haha/Desktop/ah8301/code/camera/remote/decode/h264.sdp";
    QString file2 = "/home/haha/Desktop/ah8301/code/camera/remote/decode/aac1.sdp";

    VideoMedia = new VlcMedia(file1, true, Instance);
    AudioMedia = new VlcMedia(file2, true, Instance);

    VideoMedia->setOption(":network-caching=100");
    AudioMedia->setOption(":network-caching=100");
    /*********************************************/

    connect(ui->pushButton,&QAbstractButton::clicked,[=](){
        VideoPlayer->open(VideoMedia);
        AudioPlayer->open(AudioMedia);

    });  //vlc连接事件

    fd_Init();                              //joystick init
    timer_start =new QTimer(this);          //senddata timer init
    myKeyEvent = new MyKeyEvent(ui);        //myKeyEvent init
    updateUI("");

    connect(timer_start,&QTimer::timeout,[=](){
        if(ui->comboBox->currentIndex()==1)
        {
            Mysenddataah8301->sendValue();
        }
        else if(ui->comboBox->currentIndex()==2)
        {
            Mysenddata4g->sendValue();
        }
    }); //定时器事件 mqtt发送数据

    connect(this,&MainWindow::destroyed,[=](){
        timer_start->stop();
        delete timer_start;
        delete Mysenddataah8301;
        delete Mysenddata4g;
    });//窗口退出事件

    connect(ui->actionrefresh,&QAction::triggered,[=](){
        if(ui->comboBox->currentIndex()==1)
        {
            if(mqttconnectstatus == 1)
            {
                delete Mysenddataah8301;
                Mysenddataah8301 = new senddata(ui,"10.10.1.2",1883);
            }
        }
        else if(ui->comboBox->currentIndex()==2)
        {
            if(mqttconnectstatus == 2)
            {
                delete Mysenddata4g;
                Mysenddata4g = new senddata(ui,"123.56.126.168",1883);
            }
        }
    }); //刷新mqtt连接

    connect(ui->comboBox,static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),[=](){
        timer_start->stop();}
    );  //当combobox索引切换时停止计时器
    //ui->label_12->setStyleSheet("color:red;");

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::loadFinished()
{
    QString lat2,lon2;
    lat2=lat1.mid(11,9);
    lon2=lon1.mid(12,10);
    double lat3=lat2.toDouble();
    double lon3=lon2.toDouble();
    QString command = QString("addpoint(%1,%2)").arg(QString::number(lon3, 'f', 6)).arg(QString::number(lat3, 'f', 6));
    view->page()->runJavaScript(command);
}
void MainWindow::on_pushButton_3_clicked()
{
    ui->horizontalSlider->setValue(50);
    ui->verticalSlider->setValue(50);

    if(ui->comboBox->currentIndex()==1)
    {
        if(mqttconnectstatus != 1)
        {
            if(mqttconnectstatus == 2)
            {
                delete Mysenddata4g;
            }
            Mysenddataah8301 = new senddata(ui,"10.10.1.2",1883);
            connect(Mysenddataah8301->myclient,&MyQmqttClient::updateUI,this,&MainWindow::updateUI);
            qDebug()<<"this is ah8301";
            mqttconnectstatus = 1;
            timer_start->start(5);
        }
    }
    else if(ui->comboBox->currentIndex()==2)
    {
        if(mqttconnectstatus != 2)
        {
            if(mqttconnectstatus == 1)
            {
                delete Mysenddataah8301;
            }
            Mysenddata4g = new senddata(ui,"123.56.126.168",1883);
            connect(Mysenddata4g->myclient,&MyQmqttClient::updateUI,this,&MainWindow::updateUI);
            qDebug()<<"this is 4G";
            mqttconnectstatus = 2;
            timer_start->start(5);
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{

    timer_start->stop();

}
void MainWindow::on_pushButton_4_clicked()
{
    view->show();
}


void MainWindow::updateUI(QString text)
{


    QPalette pe;
    if(text.contains("cappercent"))
    {
        ui->label_7->setText(text);}


    if(text.contains("remain"))
    {
        ui->label_8->setText(text);
    }


    if(text.contains("date"))
    {
        ui->label_11->setText(text);
    }

    if(text.contains("time"))
    {
        ui->label_3->setText(text);
    }

    if(text.contains("longitude"))
    {
        ui->label_4->setText(text);
        lon1=text;
        printf("label4_longitude is %s\n",lon1.toStdString().data());
        loadFinished();
        /*if(mqttconnectstatus == 1)
        {
            ui->label_2->setText("4Gsignalstength : NULL");
        }
        else if(mqttconnectstatus == 2)
        {
            ui->label_2->setText("4Gsignalstength : -86");
        }*/
    }

    if(text.contains("latitude"))
    {
        ui->label_5->setText(text);
        lat1=text;
        printf("label4_latitude is %s\n",lat1.toStdString().data());
        loadFinished();
    }

    if(text.contains("altitude"))
    {
        ui->label_6->setText(text);
    }

    if(text.contains("temperature"))
    {
        ui->label_9->setText(text);
    }

    if(text.contains("humidity"))
    {
        ui->label_10->setText(text);
    }

    if(text.contains("point"))
    {
        lat1=text.mid(8,6);
        ui->label_13->setText(text);
        ui->widget->set_direction(lat1.toFloat());
    }
    if(text.contains("fire"))
    {
        int textmid;
        //qDebug()<<"this is fire mqtt : " << text;
        textmid = text.mid(7,15).toInt();
         qDebug()<<"this is fire mqtt : " << textmid;
        if(textmid<100)
        {
            firestatus = 0;
        }
        else if(textmid>100&&textmid<3000)
        {
            firestatus = 1;
        }
        else if(textmid>3000&&textmid<10000)
        {
            firestatus = 2;
        }
        else if(textmid>10000&&textmid<30000)
        {
            firestatus = 3;
        }
        else
            if(textmid>30000)
            {
                firestatus = 4;
            }
    }

    if(text.contains("4Gsignalstength"))
    {

        ui->label_2->setText(text);
    }

    if(firestatus == 0)
    {
        ui->label_12->setText("security!");
        ui->label_12->setStyleSheet("color: rgb(78, 154, 6);");
        ui->label_7->setStyleSheet("");
        ui->label_8->setStyleSheet("");
        ui->label_11->setStyleSheet("");
        ui->label_3->setStyleSheet("");
        ui->label_2->setStyleSheet("");
        ui->label_4->setStyleSheet("");
        ui->label_5->setStyleSheet("");
        ui->label_6->setStyleSheet("");
        ui->label_9->setStyleSheet("");
        ui->label_10->setStyleSheet("");
        ui->label_13->setStyleSheet("");
        ui->pushButton_5->setStyleSheet("background-color: rgb(78, 154, 6);");
        ui->pushButton_6->setStyleSheet("");
        ui->pushButton_7->setStyleSheet("");
        ui->pushButton_8->setStyleSheet("");
        ui->pushButton_9->setStyleSheet("");
    }
    else if(firestatus == 1)
    {
        pe.setColor(QPalette::WindowText,Qt::black);
        ui->label_12->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_7->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_8->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_11->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_3->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_2->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_4->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_5->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_6->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_9->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_10->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_13->setStyleSheet("color: rgb(252, 175, 62);");
        ui->label_12->setText("Fire warning!");
        ui->pushButton_5->setStyleSheet("background-color: rgb(252, 175, 62);");
        ui->pushButton_6->setStyleSheet("background-color: rgb(252, 175, 62);");
        ui->pushButton_7->setStyleSheet("");
        ui->pushButton_8->setStyleSheet("");
        ui->pushButton_9->setStyleSheet("");
    }
    else if(firestatus == 2)
    {

        ui->label_12->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_7->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_8->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_11->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_3->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_2->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_4->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_5->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_6->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_9->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_10->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_13->setStyleSheet("color: rgb(206, 92, 0);");
        ui->label_12->setText("Fire warning!");
        ui->pushButton_5->setStyleSheet("background-color: rgb(206, 92, 0);");
        ui->pushButton_6->setStyleSheet("background-color: rgb(206, 92, 0);");
        ui->pushButton_7->setStyleSheet("background-color: rgb(206, 92, 0);");
        ui->pushButton_8->setStyleSheet("");
        ui->pushButton_9->setStyleSheet("");
    }
    else if(firestatus == 3)
    {

        ui->label_12->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_7->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_8->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_11->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_3->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_2->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_4->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_5->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_6->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_9->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_10->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_13->setStyleSheet("color: rgb(239, 41, 41);");
        ui->label_12->setText("Fire warning!");
        ui->pushButton_5->setStyleSheet("background-color: rgb(239, 41, 41);");
        ui->pushButton_6->setStyleSheet("background-color: rgb(239, 41, 41);");
        ui->pushButton_7->setStyleSheet("background-color: rgb(239, 41, 41);");
        ui->pushButton_8->setStyleSheet("background-color: rgb(239, 41, 41);");
        ui->pushButton_9->setStyleSheet("");
    }
    else if(firestatus == 4)
    {
        pe.setColor(QPalette::WindowText,Qt::red);
        ui->label_12->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_7->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_8->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_11->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_3->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_2->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_4->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_5->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_6->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_9->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_10->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_13->setStyleSheet("color: rgb(164, 0, 0);");
        ui->label_12->setText("Fire warning!");
        ui->pushButton_5->setStyleSheet("background-color: rgb(164, 0, 0);");
        ui->pushButton_6->setStyleSheet("background-color: rgb(164, 0, 0);");
        ui->pushButton_7->setStyleSheet("background-color: rgb(164, 0, 0);");
        ui->pushButton_8->setStyleSheet("background-color: rgb(164, 0, 0);");
        ui->pushButton_9->setStyleSheet("background-color: rgb(164, 0, 0);");
    }
}
