#include "mainwindow.h"
#include "utils.h"
#include "ui_mainwindow.h"
#include "GxIAPI.h"
#include <QMessageBox>
#include <QTimer>
#include <string>
#include <opencv2/highgui.hpp>
#include <thread>
#include <time.h>
#include <QDebug>

static edgeCurveDetector ecdtor(true,5);

using namespace std;
using namespace cv;

string getTime()
{
    time_t timep;
    time(&timep);
    char tmp[32];
    strftime(tmp, sizeof(tmp), "%H:%M:%S",localtime(&timep) );
    return tmp;
}

string getFullTime()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );
    return tmp;
}

string getData()
{
    time_t timep;
    time(&timep);
    char tmp[32];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d",localtime(&timep) );
    return tmp;
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initMemberVariables(); //
    initUIControls();
    /*
    */
    initResourceLib();

    initObject();
    initSignals();

    ::mkdir("logs/",S_IRWXU);
    ::mkdir("kanfeng/",S_IRWXU);
    ::mkdir("pictures/",S_IRWXU);
    ::mkdir("result/",S_IRWXU);
    ::mkdir("vertical/",S_IRWXU);
    ::mkdir("pictures/errorImg/",S_IRWXU);

    string picTarget = string("pictures/")+getData().c_str()+string("/");
    ::mkdir(picTarget.c_str(),S_IRWXU);
    _logMember.log.open("logs/Move"+getData()+".log",ios_base::app);
    _logMember.errorLog.open("logs/Error"+getData()+".log",ios_base::app);
    _logMember.MotorLog.open("logs/Motor"+getData()+".log",ios_base::app);
    _logMember.pointLog.open("logs/Point.log",ios_base::app);

    initLastConfig();

}

MainWindow::~MainWindow()
{
    delete ui;
    _logMember.log.close();
    _logMember.errorLog.close();
    _logMember.MotorLog.close();
    _logMember.pointLog.close();
}

bool MainWindow::GetInitPos()
{
    cv::Mat _disp = _camera_ptr->takePicture().clone();
    cout<<"Size:"<<_disp.size()<<endl;
    cout<<"channels:"<<_disp.channels()<<endl;
    cout<<"depth:"<<_disp.depth()<<endl;
    cv::cvtColor(_disp,_disp,CV_BGR2GRAY);


    Vec4d temp1,temp2; //记录图像处理的uv点
    cv::Mat result;
    try{
        ecdtor.getImg(_disp,1);
        result= ecdtor.detector2(temp1,temp2,_disp,100);
        if(result.data == nullptr)
        {
            showMessage("无法获取标定点");
            saveImg(_disp,"pictures/errorImg/","f.png");
            throw TAG_IMAGE_FAIL_WARNING;
        }
    }catch(...){
        saveImg(_disp,"pictures/errorImg/","","");
    }

    bool ret = matLine2du(temp1,temp2,_weldData.last_cali_u);
    if(!ret){
        //showMessage("ret false");
        _logMember.errorLog<<"First Calibration False!"<<endl;
        _logMember.errorLog<<temp1[0]<<","<<temp1[1]<<"\t"<<temp1[2]<<","<<temp1[3]<<endl;
        _logMember.errorLog<<temp2[0]<<","<<temp2[1]<<"\t"<<temp2[2]<<","<<temp2[3]<<endl;
    }

    _weldData.last_motor_pos = _weldData.last_cali_u;
    _weldData.current_motor_pos=_weldData.last_motor_pos;
    _weldData.current_u = _weldData.last_cali_u;
    _weldData.first_image_U = _weldData.last_cali_u;
    return ret;
}


void MainWindow::initMemberVariables()
{
    _runFlag.allowUseCam = false;
    _runFlag.isCamCapting = false;
    camIndex = 1;
    allowWeld = false;
    _runFlag.isCaliEnd = false;
    _runFlag.isShieldSport = OpenConfig::get<bool>("conf/App.json","isShieldSport");
    _runFlag.isAllowUseVerticalCam = OpenConfig::get<bool>("conf/App.json","allow_use_vertical_cam");
    _recodeMember.allow_timer_capture = OpenConfig::get<bool>("conf/App.json","allow_timer_capture");

    _weldData.pix2steps = OpenConfig::get<double>("conf/running.json","detect.pix2steps");
    _weldData.purseMap = OpenConfig::get<double>("conf/running.json","detect.purseMap");

    _gasData.gas_rate = OpenConfig::get<double>("conf/gas.json","gas_rate");
    _gasData.factory = OpenConfig::get<double>("conf/gas.json","factory");
    _gasData.max_value =  OpenConfig::get<double>("conf/gas.json","max_value");

    _welPara.averageCount = OpenConfig::get<int>("conf/weld.json","averageCount");
    _welPara.detectMaxPix = OpenConfig::get<int>("conf/weld.json","detectMaxPix");
    _welPara.validUlenPercent = OpenConfig::get<double>("conf/weld.json","validUlenPercent");
    _welPara.imageMotorOffset = OpenConfig::get<int>("conf/weld.json","imageMotorOffset");
    _welPara.errorCount = OpenConfig::get<int>("conf/weld.json","errorCount");
    _welPara.MaxMotorCount = OpenConfig::get<int>("conf/weld.json","MaxMotorCount");
    _welPara.verticalOffset = OpenConfig::get<int>("conf/weld.json","verticalOffset");
    _welPara.Attenuation = OpenConfig::get<double>("conf/weld.json","Attenuation");


    //qDebug()<<_welPara.averageCount<<_welPara.detectMaxPix<<_welPara.validUlenPercent<<_welPara.imageMotorOffset<<_welPara.errorCount;
}

void MainWindow::weldErrServ()
{
    // weld stop
    allowWeld = false;
    ui->lbl_error->setText("焊接失败!");
//    QMessageBox::critical(this , "critical message" , "焊接失败!",
//                          QMessageBox::Ok | QMessageBox::Default ,
//                          QMessageBox::Cancel | QMessageBox::Escape , 	0 );
}

void MainWindow::saveImg(const cv::Mat &mat, const std::string &_path, const std::string &_name,const std::string &modify)
{
    string strName ;
    if("" == _path)
        strName = "pictures/" ;
    else
        strName = _path;
    if("" == _name){
        strName += getData()+ "/";
        if("" == modify){
            strName += getTime() + ".png";
        }else {
            strName += getTime() + modify + ".png";
        }
    }else{
        strName += _name;
    }


    if ( !mat.data ) return ;

    imwrite( strName , mat) ;


}

void MainWindow::slot_onExposureChanged(int index,int value)
{
    GetCameraPointer(index)->setExposure(value);
}

void MainWindow::slot_onGainChanged(int index,int gain)
{
    GetCameraPointer(index)->setGain(gain);
}

void MainWindow::slot_ShieldSport(bool data)
{
    _runFlag.isShieldSport = data;
}

void MainWindow::slot_ROIChanged(int index,float x, float y, int width, int height)
{
    GetCameraPointer(index)->setRoi(x,y,width,height);
}

void MainWindow::slot_PicPressed(QPoint last)
{
    uv.rx() = last.x();
    uv.ry()= last.y();
    ui->tbn_recode->setEnabled(true);
}

void MainWindow::initSignals()
{
    connect(camSetting,&CamSetting::sig_ExpChanged,this,&MainWindow::slot_onExposureChanged);
    connect(camSetting,&CamSetting::sig_GainChanged,this,&MainWindow::slot_onGainChanged);
    connect(camSetting,&CamSetting::sig_ROIChanged,this,&MainWindow::slot_ROIChanged);
    connect(motorSetting,&MotorSetting::sig_ShieldRun,this,&MainWindow::slot_ShieldSport);

    connect(cvWidget,&QCvDisplay::sig_picPressed,this,&MainWindow::slot_PicPressed);

    connect(debugForm,&DebugDialog::Sig_btn_XLeft,this,&MainWindow::slot_btn_XLeft_clicked);
    connect(debugForm,&DebugDialog::Sig_btn_XRight,this,&MainWindow::slot_btn_XRight_clicked);
    connect(debugForm,&DebugDialog::Sig_btn_ZTop,this,&MainWindow::slot_btn_ZTop_clicked);
    connect(debugForm,&DebugDialog::Sig_btn_ZDown,this,&MainWindow::slot_btn_ZDown_clicked);
    connect(debugForm,&DebugDialog::camIndexChanged,[=](int index){

            if(2 == index){
                camIndex = 0;
            }else if(1 == index){
                camIndex = 1;
            }else {
                camIndex = 2;
            }
    });




    connect(motorSetting,&MotorSetting::sig_PortChanged,[=]{
        Motor::GetInstance()->portChanged();
    });

    QObject::connect(motorSetting,&MotorSetting::sig_PortChanged,[=]{
        Motor::GetInstance()->portChanged();
    });

#if RECORD_DEBUG
    connect(this,&MainWindow::logMove,this,&MainWindow::slot_CaptureDebugInfo);
#endif

}

//初始化相机lib
void MainWindow::initResourceLib()
{
    int camDeviceCnt=AdjustCamera::enumCamDevice();
    qDebug()<<"枚举设备:当前检测到相机:"<<camDeviceCnt;
    if(0 == camDeviceCnt){
        qWarning("扫描不到设备!");
    }
}


void MainWindow::initObject()
{
    qTimer = new QTimer();
    camSetting = new CamSetting(this);
    debugForm = new DebugDialog(this);
    motorSetting = new MotorSetting(this);
    connect(this,SIGNAL(weldErr()),this,SLOT(weldErrServ()),Qt::QueuedConnection);
    _camera_ptr= new AdjustCamera;
    _calibrat_ptr= new AdjustCamera;
    _vertical_ptr = new AdjustCamera;

}

void MainWindow::on_actionCamSetting_triggered()
{
    camSetting->show();
}


void MainWindow::on_actiontakePicture_triggered()
{
    if(!_runFlag.allowUseCam) {return;}
    auto mat = GetCameraPointer()->takePicture();
    MainWindow::saveImg(mat);
}


void MainWindow::showPicture()
{
    if(!_runFlag.allowUseCam && !_runFlag.isCamCapting) {return;}
    cv::Mat myShow = GetCameraPointer()->takePicture().clone();
    if(1 == camIndex){
        /* Todo.Get pos */
        //获取焊枪坐标点
        //Vec4d _point=********();
        Vec4d _point = (880,400);
        cv::Mat _cali_mat = QCvImageOperate::draw_calibration(myShow,_point.rows,_point.cols);
        showCvImage(_cali_mat);
    }else if (0 == camIndex) {
        //Motor
        myShow=QCvImageOperate::draw_focus_plus(myShow,cv::Scalar(0,255,0),_weldData.current_motor_pos,512);

        //绘制Image mid轴
        myShow=QCvImageOperate::draw_full_line(myShow,cv::Scalar(20,60,220),round(_weldData.current_u));
        showCvImage(myShow);
    }else {
        //垂直相机
        if(_runFlag.isAllowUseVerticalCam){
            showCvImage(myShow);
        }
    }

}

void MainWindow::on_actioncloseApp_triggered()
{
    Motor::GetInstance()->disconnectDevice();

    _camera_ptr->disconnectDevice();
    _calibrat_ptr->disconnectDevice();
    _vertical_ptr->disconnectDevice();

    qApp->exit(0);
}

//设置上次的参数
void MainWindow::on_actionImportLast_triggered()
{
    initDevices();
    initGasFlowmeter();
}


void MainWindow::initDevices()
{
    Motor::GetInstance()->importConfig();
    if(!Motor::GetInstance()->isDeviceOpend()){
        qCritical("马达打开异常!");
        return;
    }
    _runFlag.allowUseMotor = true;

    //Open Light
    Motor::GetInstance()->ledTurn(LED1,OPEN);
    Motor::GetInstance()->ledTurn(LED2,OPEN);
    Motor::GetInstance()->ledTurn(LED3,OPEN);

    led_Motor->turnOn();

    _camera_ptr->connectDevice("Cam1");
    if(!_camera_ptr->isDeviceOpend()){
        qCritical("Cam1相机打开异常!");
        return;
    }
    led_front->turnOn();

    _calibrat_ptr->connectDevice("Cam2");
    if(!_calibrat_ptr->isDeviceOpend()){
        qCritical("Cam2相机打开异常!");
        return;
    }

    _runFlag.allowUseCam = true;
    led_Cali->turnOn();

    if(_runFlag.isAllowUseVerticalCam){
        _vertical_ptr->connectDevice("Cam3");
        if(!_vertical_ptr->isDeviceOpend()){
            qCritical("Cam2相机打开异常!");
            return;
        }
        led_vertical->turnOn();
    }

}

void MainWindow::slot_btn_ZTop_clicked(int length)
{
    if(!_runFlag.allowUseMotor) { return;}
    Motor::GetInstance()->posMove(MOTORZ,INCREASE,length);
    emit logMove("Z上移:",length);
}

void MainWindow::slot_btn_ZDown_clicked(int length)
{
    if(!_runFlag.allowUseMotor) { return;}
    Motor::GetInstance()->posMove(MOTORZ,DECREASE,length);
    emit logMove("Z下移:",length);
}

void MainWindow::slot_btn_XLeft_clicked(int length)
{
    if(!_runFlag.allowUseMotor) { return;}
    Motor::GetInstance()->posMove(MOTORX,DECREASE,length);
    emit logMove("X左移:",length);
}

void MainWindow::slot_btn_XRight_clicked(int length)
{
    if(!_runFlag.allowUseMotor) { return;}
    Motor::GetInstance()->posMove(MOTORX,INCREASE,length);
    emit logMove("X右移:",length);
}

void MainWindow::showMessage(QString info)
{
    QMessageBox::warning(this , "Warning" , info,
                         QMessageBox::Ok ,0 );
}

void MainWindow::processCalibration(bool result)
{
    allowWeld = result;
    ui->tbn_weld->setEnabled(result);
}


void MainWindow::RecodeResults()
{
    auto mat = _camera_ptr->getImg();
    MainWindow::saveImg(mat,"","","-camera");
    auto _cal_mat = _calibrat_ptr->getImg();
    MainWindow::saveImg(_cal_mat,"","","-calibration");

}

void MainWindow::initGasFlowmeter()
{
    std::thread([this](){
            try
            {
                while (1) {
                    std::this_thread::sleep_for(std::chrono:: milliseconds (1000));
                    QByteArray rec = Motor::GetInstance()->getGasFlowmeter();


                    if(!rec.isEmpty()){
                        int value = rec[0]*0x100;
                        value += rec[1];
                        //       float电压值=整形/100
                        _gasData.mV = value;
                        _gasData.gas = (_gasData.mV - _gasData.factory) / _gasData.gas_rate;
                        if(_gasData.gas<0){
                            _gasData.gas = 0;
                        }
                        if(_gasData.gas > _gasData.max_value){

                        }else
                            ui->lbl_air->setText(QString::number(_gasData.gas));
                    }

                }

            }
            catch(...)
            {
                _logMember.errorLog<<"获取流量数据失败"<<getTime()<<endl;
            }
    }
    ).detach();
}

void MainWindow::initLastConfig()
{
    bool allow_load_default_setting = OpenConfig::get<bool>("conf/App.json","allow_load_default_setting");
    if(allow_load_default_setting) initDevices();
    initGasFlowmeter();
}

AdjustCamera *MainWindow::GetCameraPointer(int index) const
{

    if(-1 != index){
        return (index == 1)? _calibrat_ptr : (index == 0)?_camera_ptr:_vertical_ptr;
    }else{
        return (camIndex == 1)? _calibrat_ptr : (camIndex == 0)?_camera_ptr:_vertical_ptr;
    }

}


void MainWindow::RecodeLogResults()
{
    //记录uv
    _logMember.log<<"line1[0]:"<<to_string(line1[0])<<"line1[1]:"<<to_string(line1[1])
      <<"line1[2]:"<<to_string(line1[2])<<"line1[3]:"<<to_string(line1[3])
     <<"line2[0]:"<<to_string(line2[0])<<"line2[1]:"<<to_string(line2[1])
    <<"line2[2]:"<<to_string(line2[2])<<"line2[3]:"<<to_string(line2[3])<<endl;
}



void MainWindow::slot_CaptureDebugInfo(QString info,int lenth)
{

    _logMember.log<<getTime()+"\t"<<info.toStdString()+to_string(lenth)<<endl;
    RecodeLogResults();
    RecodeResults();
}

void MainWindow::on_actionMotorSetting_triggered()
{
    motorSetting->show();
}

void MainWindow::showCvImage(Mat cvImg)
{
    cvWidget->display(cvImg);
}


void MainWindow::on_actionDebug_triggered()
{
    debugForm->show();
}

void MainWindow::initUIControls()
{
    cvWidget = new QCvDisplay(this);
    cvWidget->setStyleSheet("background-color: rgb(0,0, 0)");
    ui->layout_Cam->addWidget(cvWidget);
    bool allow_debugForm = OpenConfig::get<bool>("conf/App.json","allow_debug");
    ui->actionDebug->setEnabled(allow_debugForm);
    ui->actionLabelDebugModel->setEnabled(allow_debugForm);

    ui->actiontakePicture->setEnabled(true);
    ui->actionLabeltakePic->setEnabled(true);

    bool allow_use_toolbar = OpenConfig::get<bool>("conf/App.json","allow_use_toolbar");
    ui->toolBar->setVisible(allow_use_toolbar);

    bool allow_use_recode = OpenConfig::get<bool>("conf/App.json","allow_use_recode");
    ui->tbn_recode->setVisible(allow_use_recode);

    ui->tbn_cali->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->tbn_weld->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->tbn_stop->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    ui->tbn_recode->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);


    led_Motor = new QStatusLight(this);
    led_Cali = new QStatusLight(this);
    led_front = new QStatusLight(this);
    led_vertical = new QStatusLight(this);

    ui->light_layout->addWidget(led_Motor,0,2,1,1);
    ui->light_layout->addWidget(led_Cali,1,2,1,1);
    ui->light_layout->addWidget(led_front,2,2,1,1);
    ui->light_layout->addWidget(led_vertical,3,2,1,1);

}

//标定按钮被点击
void MainWindow::on_tbn_cali_clicked()
{
    if(!_runFlag.allowUseCam && !_runFlag.allowUseMotor){
        showMessage("标定失败!请检查马达，相机状态!");
        return;
    }

    camIndex = 1;   //标定前使用标定相机
    _runFlag.isCaliEnd = false;

    showStream(true);

    while(!_runFlag.isCaliEnd){
        /* Todo */
        // isCaliSuccess = function();
        _runFlag.isCaliEnd = true;
        _runFlag.isCaliSuccess = true;
    }

    ui->tbn_cali->setText("重新标定");


    camIndex = 0;   //标定后使用看缝相机
    bool ret = GetInitPos();
    processCalibration(ret);
}

void MainWindow::on_tbn_weld_clicked()
{
    _runFlag.isCaliSuccess = true;
    if(!_runFlag.isCaliSuccess && !_runFlag.allowUseCam && !_runFlag.allowUseMotor){
        showMessage("焊接失败!请检查马达，相机，焊针状态!");
        return;
    }
    allowWeld = true;

    workThread.reset(new std::thread([this](){
                        try
                        {
                            int errCount = 0;
                            static long counts = 0;
                            static int _weldErr = 0;    //平均处理时的误差计数
                            static int averCount = 0;   //平均处理时的计数
                            static double movePix = 0;
                            static int maxErrorCount = 0;
                            while(allowWeld)
                            {
                                std::this_thread::sleep_for( std::chrono::milliseconds(100) ) ;
                                auto mat = _camera_ptr->takePicture().clone();
                                cv::cvtColor(mat,mat,CV_BGR2GRAY);
                                if(_recodeMember.allow_timer_capture)
                                    cv::imwrite("kanfeng/"+to_string(counts)+".png",mat);

                                counts++;
                                ecdtor.getImg(mat,1);


                                cv::Mat result= ecdtor.detector2(this->line1,this->line2,mat,100);
                                if(result.data == nullptr)
                                {
                                    if(errCount > _welPara.errorCount)
                                    {
                                        _logMember.errorLog<<"Not allow Detected!"<<endl;
                                        emit weldErr();
                                        break;
                                    }
                                    errCount++;
                                    continue;
                                }
                                errCount= 0;
                                //不设定名称会产生时间间隔的图像
                                MainWindow::saveImg(result,"result/","twogreenlines.png");

                                if(matLine2du(line1,line2,_weldData.current_u)&&_runFlag.isShieldSport)
                                {

                                    averCount++;
                                    double ulen = _weldData.current_u-_weldData.last_motor_pos;

                                    _logMember.pointLog<<counts-1<<","<<ulen<<","<<_weldData.last_motor_pos<<","<<_weldData.current_u<<endl;
                                    if(averCount >= _welPara.averageCount){
                                            averCount = 0;
                                            if((_weldErr / _welPara.averageCount) > (1.0 - _welPara.validUlenPercent)){
                                                _logMember.errorLog<<"valid Ulen Percent Not > 80%!"<<endl;
                                                throw TAG_WELD_LINE_OUTRANGE_WARNING;
                                            }else{
                                                averCount = 0;
                                                int valid_count = _welPara.averageCount - _weldErr;
                                                if(movePix!=0){
                                                    int _moveStep = (_weldData.purseMap * (movePix/valid_count) * _weldData.pix2steps)*_welPara.Attenuation;
                                                    Direction dirct = (0 > _moveStep)? DECREASE:INCREASE;
                                                    Motor::GetInstance()->posMove(MOTORX,dirct,abs(_moveStep));
                                                    _logMember.MotorLog<<_moveStep<<endl;
                                                }

                                                _weldData.current_motor_pos = _weldData.last_motor_pos+(movePix/valid_count);
                                                _weldData.last_motor_pos = _weldData.current_motor_pos;
                                                movePix = 0;
                                            }

                                    }else{
                                        if(abs(ulen)>_welPara.detectMaxPix){
                                            _weldErr++;
                                        }else {
                                            movePix+=ulen;
                                        }
                                        continue;
                                    }



                                    if(abs(_weldData.first_image_U-_weldData.current_u)>_welPara.imageMotorOffset+200){
                                        _logMember.errorLog<<"焊缝偏移过大，请重新焊接!"<<endl;
                                        qWarning("焊缝偏移过大，请重新焊接2");
                                    }
                                    if(maxErrorCount>_welPara.MaxMotorCount){
                                        emit weldErr();
                                    }

                                }

                            }
                        }catch(...)
                        {
                            emit weldErr();
                            std::string lineResult;
                            for(int i=0; i<4; i++){
                                lineResult += to_string(line1[i]) + "\t";
                            }
                            _logMember.errorLog<<"Weld line1:"<<getTime()<<"\t"<<endl;
                            lineResult.clear();
                            for(int i=0; i<4; i++){
                                lineResult += to_string(line2[i]) + "\t";
                            }
                            _logMember.errorLog<<"Weld line2:"<<getTime()<<"\t"<<lineResult<<endl;
                        }
                    })
    );
    workThread.data()->detach();
    verticalThread.reset(new std::thread([this](){
        static long counts = 0;
        while (allowWeld) {
            std::this_thread::sleep_for( std::chrono::milliseconds(30000) ) ;
            auto ver_mat = _vertical_ptr->takePicture().clone();
            cv::cvtColor(ver_mat,ver_mat,CV_BGR2GRAY);
            cv::imwrite("vertical/"+to_string(counts++)+".png",ver_mat);

            /* Todo */
            /* run */
            /* double ulen = xxxxx(mat); */
//            if(ulen > _welPara.verticalOffset){
//                ui->lbl_vertical->setText(QString::number(ulen));
//            }
        }
    }));
    verticalThread.data()->detach();
    ui->lbl_workStatus->setText("开始跟踪");
}


void MainWindow::on_tbn_stop_clicked()
{
    workThread.reset(nullptr);
    verticalThread.reset(nullptr);
    allowWeld = false;
    ui->tbn_weld->setEnabled(true);
    ui->lbl_workStatus->setText("未开始");
    ui->lbl_error->clear();
}

void MainWindow::on_actionCamStartCapture_triggered(bool checked)
{
    showStream(checked);
}


void MainWindow::showStream(bool state)
{
    if(state)
    {
        QObject::connect(qTimer, SIGNAL(timeout()),this, SLOT(showPicture())) ;
        qTimer->start(100);
        _runFlag.isCamCapting = true;
    }
    else
    {
        if(_runFlag.isCamCapting)
        {
            qTimer->stop();
            QObject::disconnect(qTimer, SIGNAL(timeout()),this, SLOT(showPicture())) ;
        }
        _runFlag.isCamCapting = false;
    }
    ui->actionCamStartCapture->setChecked(_runFlag.isCamCapting);
}

void MainWindow::on_tbn_recode_clicked()
{
    //GetInitPos();
    RecodeResults();
}
