#ifndef QSTATUSLIGHT_H
#define QSTATUSLIGHT_H

#include <QWidget>
#include <QObject>
#include <QMetaType>
#include <QPainter>
#include <QTimer>

class QStatusLight : public QWidget
{
    Q_OBJECT
public:

public:
    QStatusLight( QWidget *parent = nullptr)
        : QWidget(parent),_width(20),_height(20) {
              timer = new QTimer(this);
              this->setMaximumWidth(24);
              this->setMaximumHeight(24);
              allowDance = false;
              turnOff();
              connect(timer, SIGNAL(timeout()), this, SLOT(heartDance()));
              timer->start(200);
    }



    void setSize(int width,int height){
        resize(width, height);
        _width = width;
        _height = height;
        update();
    }

public:
    void turnOff() {
        m_file = ":/controls/gray.png";
        update();
    }
    void turnOn() {
        m_file = ":/controls/green.png";
        update();
    }
    void turnWarning(){
        m_file = ":/controls/red.png";
        update();
    }
    void blink(bool value){
        allowDance = value;
    }
    void setOn(bool state){
        state?turnOn():turnOff();
    }

private slots:
    void heartDance(){
        if(!allowDance) return;
        if(last){
           turnOn();
           last = !last;
        }else {
           turnOff();
           last = !last;
        }
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::transparent);
        QRect target(2, height()/2-10, 20, 20);
        QImage image(m_file);
        painter.drawImage(target,image);
    }

private:
    QString m_file;
    bool m_on;
    int _width;
    int _height;
    bool last;
    bool allowDance;
    QTimer *timer;
};

#endif // QSTATUSLIGHT_H
