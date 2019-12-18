#ifndef MOTOR_H
#define MOTOR_H
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <thread>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QMutex>
#include <QThread>


#include <boost/asio/serial_port.hpp>
#include <boost/asio.hpp>
#include <boost/timer.hpp>
#include "boost/circular_buffer.hpp"
#include "boost/signals2.hpp"

using namespace std;
enum LimitOperate{TRIGGER,RELEASE};

enum Direction{INCREASE,DECREASE};
enum MotorType{MOTORX,MOTORZ};
enum LedType{LED1,LED2,LED3};
enum Control{OPEN,CLOSE};

#include "src/Method/OpenConfig.h"


#define PRINTCORRECT 0

typedef boost::circular_buffer<unsigned char> CCircularBuffer;


class Motor
{
public:
    Motor();

    static Motor* GetInstance(){
        static QMutex mutex;
        static QScopedPointer<Motor> inst;
        if (Q_UNLIKELY(!inst)) {
            mutex.lock();
            if (!inst) {
                inst.reset(new Motor);
            }
            mutex.unlock();
        }
        return inst.data();
    }
public:
    //send
    void ledTurn(LedType lt,Control c);

    void importConfig();

    void reconnectDevice();//re连接串口
    void disconnectDevice();//断开设备

    //Move
    void posMove(MotorType mt,Direction direct,int length);
    void stopMove(MotorType mt);//停止X移动

    //气体流量计
    QByteArray getGasFlowmeter();

    bool isDeviceOpend();//设备是否正常

    /// 存放应答指令
    unsigned char _readBuffer[5] ;


    /// 线程同步锁
    std::recursive_mutex _mtx ;
    /// 条件变量
    std::condition_variable _condReply ;
    /// 条件变量锁
    std::mutex _mtxReply ;
    /// 是否已经应答
    bool _bReply ;
    /// asio 服务管理
    boost::asio::io_service _service ;
    /// 激光控制通信管理
    std::shared_ptr<boost::asio::serial_port> _serialPort_ptr ;

    std::thread _thService ;
private:
    /// 设备应答
    void recvReply( const boost::system::error_code &ec, size_t ) ;
    /// 等待设备应答
    void waitReply() ;


    void preConnectDevice();//连接串口
    void connectDevice(std::string,uint);//连接串口

    QString port;

public:

    // 中文解码
    void decodeComplate(QString);


public:
    void portChanged();
    QByteArray read_from_serial();
};

#endif // MOTOR_H
