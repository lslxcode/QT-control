#ifndef COMPASS_H
#define COMPASS_H

#include <QWidget>
#include <qevent.h>
#include <qpainter.h>
#include <qrect.h>

#include <qdebug.h>

namespace Ui {
class Compass;
}

class Compass : public QWidget
{
    Q_OBJECT

public:
    explicit Compass(QWidget *parent = nullptr);
    ~Compass();



public slots:
    void set_direction(const float direction);
    void set_campass_scale(const float scale);  // 设置指南针在控件中的比例
    void set_campass_scale(const quint32 widget_scale, const quint32 campass_scale);  // 设置指南针在控件中的比例

protected:
    void paintEvent(QPaintEvent *event);
private:
    Ui::Compass *ui;
    float _direction;
    float _campass_scale;
};

#endif // COMPASS_H
