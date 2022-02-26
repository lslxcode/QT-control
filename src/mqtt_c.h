#ifndef MQTT_C_H
#define MQTT_C_H

#include <qmqtt/qmqtt_client.h>

using namespace QMQTT;

#include <iostream>

class MyQmqttClient : public Client
{

        Q_OBJECT
    public:
        explicit MyQmqttClient(const QString &host,quint32 port);

        void setTopic(const QString & topic);
        void setQos(const QString & qos);
        void mqtt_init();
        quint16 send(QString topic, QByteArray msg, int qos = 0);

    public slots:
        void subscribeTo();
        void showMqttData(const QMQTT::Message &message);
        void showSubscribed();
        void showConnected();
        void showDisConnected();
        //void loadFinished(bool a);

    private:
        QString subTopic;
        quint8 subQos;
    signals:
        int updateUI(QString text);

};

#endif // MQTT_C_H
