#include "mqtt_c.h"
#include <QDebug>
using namespace std;

MyQmqttClient::MyQmqttClient(const QString &host,quint32 port)
{
        quint8 qos = 0;
        QString topic = "#";

        cout << "using: host[" << host.toStdString() << "] port[" << port << "] qos[" << qos << "]" << endl;
        cout << "subscribe on topic[" << topic.toStdString() << "]" << endl;

        this->setQos("0");
        this->setTopic(topic);

        this->setHost(host);
        this->setPort(port);

        QObject::connect(this, &MyQmqttClient::connected, this, &MyQmqttClient::showConnected);
        QObject::connect(this, &MyQmqttClient::connected, this, &MyQmqttClient::subscribeTo);
        QObject::connect(this, &MyQmqttClient::disconnected, this, &MyQmqttClient::showDisConnected);
        QObject::connect(this, &MyQmqttClient::subscribed, this, &MyQmqttClient::showSubscribed);
        QObject::connect(this, &MyQmqttClient::received, this, &MyQmqttClient::showMqttData);
        //QObject::connect(&a, &QCoreApplication::aboutToQuit, &thisClient::disconnect);

        this->connect();
}
void MyQmqttClient::showSubscribed()
{
        cout << "subscribed" << endl;
}
void MyQmqttClient::showConnected()
{
        cout << "connected!" << endl;
}
void MyQmqttClient::showDisConnected()
{
     cout << "disconnected!" << endl;
}


void MyQmqttClient::setTopic(const QString & topic)
{
        subTopic = topic;
}
void MyQmqttClient::setQos(const QString & qos)
{
         subQos = qos.toUInt();
}
void MyQmqttClient::subscribeTo()
{
        subscribe(subTopic, subQos);
}
quint16 MyQmqttClient::send(QString topic, QByteArray msg, int qos)
{
        Message m;
        m.setTopic(topic);
        m.setPayload(msg);
        m.setQos(qos);
        return this->publish(m);
}
void MyQmqttClient::showMqttData(const QMQTT::Message &message)
{
        QString data(message.payload());
        //cout << "received data: " << data.toStdString() << endl;
        cout << "received :\ntopic: " << message.topic().toStdString() << "\tmsg: " << message.payload().toStdString() << endl;

        int value = message.payload()[0];

        QString qstr = message.payload();

        if(message.topic().contains("cappercent"))
            emit updateUI("cappercent : " + qstr);
        else if(message.topic().contains("remain"))
            emit updateUI("remain : " + qstr);
        else if(message.topic().contains("date"))
            emit updateUI("date : " + qstr);
        else if(message.topic().contains("time"))
            emit updateUI("time : " + qstr);
        else if(message.topic().contains("longitude"))
            emit updateUI("longitude : " + qstr);
        else if(message.topic().contains("latitude"))
            emit updateUI("latitude : " + qstr);
        else if(message.topic().contains("altitude"))
            emit updateUI("altitude : " + qstr);
        else if(message.topic().contains("4Gsignalstength"))
            emit updateUI("4Gsignalstength : " + qstr);
        else if(message.topic().contains("temperature"))
            emit updateUI("temperature : " + qstr);
        else if(message.topic().contains("humidity"))
            emit updateUI("humidity : " + qstr);
        else if(message.topic().contains("point"))
            emit updateUI("point : " + qstr);
        else if(message.topic().contains("fire"))
            emit updateUI("fire : " + qstr);


}
