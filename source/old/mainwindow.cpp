#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "GxIAPI.h"
#include <QMessageBox>
#include <QTimer>
#include <string>
#include <opencv2/highgui.hpp>
#include <thread>
#include <time.h>
#include <QDebug>



using namespace std;
using namespace cv;

string getTime()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y_%m_%d_%H_%M_%S",localtime(&timep) );
    return tmp;
}

#define MainSerialPortOneFrameSize 4

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
  ,_camera_ptr(new AdjustCamera)
  ,m_is_open(false)
  ,btakePictures(false)
{
    ui->setupUi(this);
    qTimer = new QTimer();
    qDebug()<<"Start Camera!";
    //_camera_ptr = new AdjustCamera();

    //openport();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*打开串口设置回调函数*/
void MainWindow::openport()
{
    /* 先来判断对象是不是为空 */
    if(MainSerial == NULL)
    {
        /* 新建串口对象 */
        MainSerial = new QSerialPort();
    }
    /* 串口已经关闭，现在来打开串口 */
    /* 设置串口名称 */
    MainSerial->setPortName("ttyS1");
    /* 设置波特率 */
    MainSerial->setBaudRate(9600);
    /* 设置数据位数 */
    MainSerial->setDataBits(QSerialPort::Data8);
    /* 设置奇偶校验 */
    MainSerial->setParity(QSerialPort::NoParity);
    /* 设置停止位 */
    MainSerial->setStopBits(QSerialPort::OneStop);
    /* 设置流控制 */
    MainSerial->setFlowControl(QSerialPort::NoFlowControl);
    /* 打开串口 */
    MainSerial->open(QIODevice::ReadWrite);
    /* 设置串口缓冲区大小，这里必须设置为这么大 */
    MainSerial->setReadBufferSize(MainSerialPortOneFrameSize);
    /* 注册回调函数 */
    QObject::connect(MainSerial, &QSerialPort::readyRead, this, &MainWindow::MainSerialRecvMsgEvent);
}

/* 串口接收数据事件回调函数 */
void MainWindow::MainSerialRecvMsgEvent()
{
    /* 如果本次数据帧接受错误，那么先不接受 */
    if(MainSerial->bytesAvailable() >= MainSerialPortOneFrameSize)
    {
        MainSerialRecvData = MainSerial->readAll();
        if(!MainSerialRecvData.isEmpty())
        {
            /// 处理串口读到的内容
        }
    }
}

void MainWindow::on_btnExp1_clicked()
{
    _camera_ptr->setExposure(ui->ldtExp1->text().toInt());
}


void MainWindow::on_pushButton_2_clicked()
{
    std::thread([this](){
        /// DO SOME THING
    }).detach();
}

void MainWindow::on_btnGain1_clicked()
{
    _camera_ptr->setGain(ui->ldtGain1->text().toInt());
}

void MainWindow::on_btnStopTake1_clicked()
{
    btakePictures = false;
}

void MainWindow::on_btnStartTake1_clicked()
{
}

void MainWindow::showPicture()
{
    _camera_ptr->getImg();
    ui->lblCam1->setPixmap(QPixmap::fromImage(*((QImage*)(_camera_ptr->m_show_image))));
}

void MainWindow::saveImg(const cv::Mat &mat, const std::string &path, const std::string &name)
{
    static int i = 0;
    string strName = "picture/" + to_string(i++) + ".jpg";
    if ( !mat.data ) return ;
    imwrite( strName , mat) ;
}

void MainWindow::on_btnOpenClose1_clicked()
{
    if(!m_is_open)
    {
        QObject::connect(qTimer, SIGNAL(timeout()),this, SLOT(showPicture())) ;
        qTimer->start(100);
        m_is_open = true;
        ui->btnOpenClose1->setText("已打开");
    }
    else
    {
        if(m_is_open)
        {
            qTimer->stop();
            QObject::disconnect(qTimer, SIGNAL(timeout()),this, SLOT(showPicture())) ;
        }
        m_is_open = false;
        ui->btnOpenClose1->setText("关闭中");
    }
}

void MainWindow::on_btnRoi1_clicked()
{
    auto offset_x = ui->ldtRoix1->text().toFloat();
    auto offset_y = ui->ldtRoiy1->text().toFloat();
    auto width = ui->ldtRoiw1->text().toFloat();
    auto height = ui->ldtRoih1->text().toFloat();
    _camera_ptr->setRoi(offset_x,offset_y,width,height);
}

void MainWindow::on_btnTakePicture1_clicked()
{
    auto mat = _camera_ptr->takePicture();
    saveImg(mat);
}
