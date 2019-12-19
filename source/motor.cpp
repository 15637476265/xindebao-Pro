#include "motor.h"
#include "motor.h"
#include "motor.h"
#include <iostream>
#include <thread>

#include <boost/thread.hpp>
#include <boost/algorithm/hex.hpp>

using namespace std ;
using namespace boost::asio;

// 毫秒
typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double, std::ratio<1, 1000>>> Millsecond ;

unsigned char* convertIntToHex(int num){
    unsigned char* _cmd ;
    int count =2;
    for ( int i = 0; i < count; ++i ) {
        _cmd[count-i] = num % 256 ;
        num >>= 8 ;
    }
    return _cmd;
}

void handler(const boost::system::error_code &error, size_t bytes_transferred)
{
        if (!error) {
            //cout << bytes_transferred << endl;
        }else {
            cerr <<__FUNCTION__<< error.message() << endl;
        }
}


Motor::Motor()
{
    memset( _readBuffer, 0, sizeof(_readBuffer));
//    if(!_data){
//        importConfig();
//        _isFirst = true;
//    }
    preConnectDevice();
}


void Motor::reconnectDevice()
{
    std::string name =  OpenConfig::get<std::string>("conf/serial.json","port");
    uint port =  OpenConfig::get<uint>("conf/serial.json","bau");
    //if(getConnecting()) disconnectDevice();
#ifdef Q_OS_LINUX
    name = "/dev/"+name;
#elif
#endif
    connectDevice(name,port);
}


void Motor::importConfig()
{
    reconnectDevice();
}

void Motor::preConnectDevice()
{
    try {
        if ( !_serialPort_ptr ) {
            _serialPort_ptr.reset( new  serial_port(_service));
        }else{
            qWarning("注意，串口资源上次未释放!");
        }
    }
    catch ( const boost::system::system_error &e ) {
        std::cout <<"Motor Pre Failed!\n" <<"system:"<< e.what() << std::endl ;
    }
    catch ( const boost::exception &e ) {
        std::cout <<"exception:" << std::endl ;
        //DUMPERROR(e) ;
    }
}

//连接设备
void Motor::connectDevice(string name , uint port)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    try {
            //_serialPort_ptr.reset( new  serial_port(_service, name));
            //_serialPort_ptr.reset( new  serial_port(_service));
            _serialPort_ptr->open(name);
            _serialPort_ptr->set_option(serial_port::baud_rate(port));//config.get<int>("Motor.baudRate"))) ;
            _serialPort_ptr->set_option(serial_port::flow_control(serial_port::flow_control::none)) ;
            _serialPort_ptr->set_option(serial_port::parity(serial_port::parity::none)) ;
            _serialPort_ptr->set_option(serial_port::stop_bits(serial_port::stop_bits::one)) ;
            _serialPort_ptr->set_option(serial_port::character_size(serial_port::character_size(8))) ;

            boost::asio::async_read( *_serialPort_ptr, boost::asio::buffer(_readBuffer),
                                     boost::bind( &Motor::recvReply, this, _1, _2 ) ) ;

            _thService = std::thread([this]() {_service.run();}) ;

#if PRINTCORRECT
            std::cout << "connect motor success\n" << std::flush ;
#endif

    }
    catch ( const boost::system::system_error &e ) {
        std::cout<<name<<":" <<"system:"<< e.what() << std::endl ;
    }
    catch ( const boost::exception &e ) {
        std::cout <<"exception:" << std::endl ;
        //DUMPERROR(e) ;
    }
}
//断开设备
void Motor::disconnectDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    if ( _serialPort_ptr ) {
        if ( _serialPort_ptr->is_open() ) {
            //this->stop() ;
            ledTurn(LED1,CLOSE);
            ledTurn(LED2,CLOSE);
            ledTurn(LED3,CLOSE);
            _service.stop() ;
            _serialPort_ptr->close();
            _thService.join() ;
        }
        _serialPort_ptr = nullptr;
    }
}

//停止移动
void Motor::stopMove(MotorType mt)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        unsigned char _cmd[3] ;
        memset( _cmd, 0, sizeof(_cmd));
        _cmd[0] = 0x5A ;
        if(mt == MOTORX)
        {
            _cmd[1] = 0xE5 ;
        }else{
            _cmd[1] = 0xEA ;
        }
        _cmd[2] = 0x00 ;

        for ( size_t i = 0; i < sizeof(_cmd); ++i ) {
            printf ( "0X%X ", _cmd[i]) ;
        }
        printf( "\n" ) ;
        boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
        std::cout << "rotate over\n" << std::flush ;
    }
    catch(const boost::exception &e)
    {
        //DUMPERROR(e) ;
        _condReply.notify_all();
    }
}

QByteArray Motor::read_from_serial()
{
    unsigned char dd[11];
    QByteArray array;

    _bReply = true ;
    _condReply.notify_one() ;


    for ( size_t i = 0; i < sizeof(dd); ++i ) {
        array.push_back(dd[i]);
    }

    std::this_thread::sleep_for( std::chrono::milliseconds(100) ) ;

    boost::asio::async_read(*_serialPort_ptr,  boost::asio::buffer(dd),boost::bind( &Motor::recvReply, this, _1, _2 ));

    return array;
}



QByteArray Motor::getGasFlowmeter()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    QByteArray array;
    array.clear();
    try{
        std::list<unsigned char> __cmd;

        __cmd.push_back(0xA5);
        __cmd.push_back(0xB1);
        __cmd.push_back(0xFF);
        __cmd.push_back(0x00);
        __cmd.push_back(0x00);
        __cmd.push_back(0x00);
        __cmd.push_back(0xFF);

        string str(__cmd.begin(), __cmd.end());
        boost::asio::write(*_serialPort_ptr, buffer(str.c_str(), __cmd.size()));
        _bReply = false ;
        waitReply() ;

    }
    catch(const boost::exception &e)
    {
        qWarning("Send Buff Error!");
        _condReply.notify_all();
    }
    //if(0x5A == _readBuffer[0])
        for ( size_t i = 0; i < sizeof(_readBuffer); ++i ) {
            array.push_back(_readBuffer[i]);

        }
    return array;
}



//
void Motor::posMove(MotorType mt, Direction direct,int length)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        std::list<unsigned char> __cmd;

        __cmd.push_back(0xA5);
        if(mt == MOTORX) __cmd.push_back(0xA2);
        else             __cmd.push_back(0xA1);
        if(direct == INCREASE)
            __cmd.push_back(0x00);
        else
            __cmd.push_back(0x01);

        __cmd.push_back(0xC1);

        __cmd.push_back(length/0x100);
        __cmd.push_back(length%0x100);

        __cmd.push_back(0x00);
        __cmd.push_back(0x01);
        __cmd.push_back(0x00);
        __cmd.push_back(0x00);
        __cmd.push_back(0x00);
        __cmd.push_back(0x00);
        __cmd.push_back(0xFF);


        foreach (unsigned char ind, __cmd) {
            printf ( "0X%X ", ind) ;
        }

        printf( "\n" ) ;

        string str(__cmd.begin(), __cmd.end());
        boost::asio::write(*_serialPort_ptr, buffer(str.c_str(), __cmd.size()));
        _bReply = false ;
        waitReply() ;
    }
    catch(const boost::exception &e)
    {
        //DUMPERROR(e) ;
        qWarning("Send Buff Error!");
        _condReply.notify_all();
    }
}

bool Motor::isDeviceOpend()
{
    return _serialPort_ptr->is_open();
}

void Motor::ledTurn(LedType lt,Control c)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx);
    if(!_serialPort_ptr)
        return;
    try{
        std::list<unsigned char> __cmd;
        __cmd.push_back(0xA5);

        if(lt == LED1)
            __cmd.push_back(0xC1);
        else if (lt == LED2) {
            __cmd.push_back(0xC2);
        }else
            __cmd.push_back(0xC3);
        __cmd.push_back( c == OPEN ? 0xF1 : 0xF0);

        __cmd.push_back(0x00);
        __cmd.push_back(0x00);
        __cmd.push_back(0x00);
        __cmd.push_back(0xff);

        std::string str(__cmd.begin(), __cmd.end());
        boost::asio::write(*_serialPort_ptr, buffer(str.c_str(), __cmd.size()));

        //boost::asio::write(*_serialPort_ptr, buffer(_cmd, sizeof(_cmd)));
        _bReply = false ;
        waitReply() ;
    }
    catch(const boost::exception &e)
    {
        //DUMPERROR(e) ;
        _condReply.notify_all();
    }
}



void Motor::recvReply( const boost::system::error_code &ec, size_t )
{
    if ( !ec ) {
        std::string out ;
        boost::algorithm::hex(_readBuffer, std::back_inserter(out)) ;

        _bReply = true ;
        _condReply.notify_one() ;

        boost::asio::async_read( *_serialPort_ptr, boost::asio::buffer(_readBuffer),
                                 boost::bind( &Motor::recvReply, this, _1, _2 ) ) ;


    }else {
        qWarning("Receive ec warning!");
    }
}

void Motor::waitReply()
{
    using namespace std::chrono ;
    Millsecond m1 = steady_clock::now() ;
    std::unique_lock<std::mutex> lck(_mtxReply) ;
    // 考虑到异常情况，设置超时时间
    _condReply.wait_for( lck, std::chrono::milliseconds(100), [this]{return _bReply ;}) ;
    Millsecond m2 = steady_clock::now() ;
    //std::cout << (m2 - m1).count() << std::endl ;
    std::this_thread::sleep_for( std::chrono::milliseconds(50) ) ;
}



void Motor::portChanged()
{
    importConfig();
}



void Motor::decodeComplate(QString)
{

}
