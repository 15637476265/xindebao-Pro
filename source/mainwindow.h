#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QButtonGroup>
#include <QTextCursor>
#include <QMainWindow>
#include "cameraControl.h"
#include "motor.h"
#include "xdb.h"
#include "Setting/CamSetting.h"
#include "Setting/MotorSetting.h"
#include "src/Error/error.h"
#include "src/Widget/QCvDisplay.h"
#include "Setting/DebugDialog.h"
#include "src/Method/QCvImageOperate.h"
#include "src/Controls/QStatusLight.h"


#define RECORD_DEBUG 1



struct WeldingData{
    double pix2steps; ///像素距离转换关系
    double purseMap;    //脉冲距离比例关系
    double first_image_U;
    int last_motor_pos;     ///last Motor pos
    double current_motor_pos;
    double last_cali_u;     //上次图像标定计算的中心
    double current_u ;      ///当前算法给出的中线x
};


struct WeldingPara{
    int averageCount;   //
    double validUlenPercent;
    int detectMaxPix;
    int errorCount;
    int imageMotorOffset;
    int MaxMotorCount;
};

struct GasData{
    double pa;
    double mV;
    double gas;
    double dltP;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //void openport();
    bool GetInitPos();


private:

    Ui::MainWindow *ui;
    std::recursive_mutex _mtx;
    AdjustCamera* _camera_ptr;  //采集相机

    AdjustCamera* _calibrat_ptr;    //标定相机

    AdjustCamera* _vertical_ptr;    //垂直相机


    bool isCamCapting; //是否正在采集图像
    bool btakePictures;
    bool allowUseCam;   //允许相机使用
    bool allowUseMotor;   //允许Motor使用
    bool allowWeld;       //允许焊接
    bool isCaliEnd;       //是否标定结束
    bool isCaliSuccess;   //标定是否成功
    bool isAllowUseVerticalCam; //是否允许使用垂直相机
    bool isShieldSport; //是否屏蔽运动

    float gas_rate;         //气体流量计比例
    float gas_init_mV;         //气体流量计初始电压值

    Vec4d line1,line2; //记录图像处理的uv点

    QTimer* qTimer; //定时器用于刷新当前图像

    QPoint uv;
    ofstream log;   //记录图像处理的uv点
    ofstream MotorLog;  //记录Motor log
    ofstream errorLog;  //记录error log
    ofstream pointLog;  //记录error log

    static void saveImg(const cv::Mat &mat, const std::string &_path = "", const std::string &_name = "",const std::string &modify = "");


    WeldingData _weldData;
    WeldingPara _welPara;
    GasData _gasData;


private slots:

    void on_actioncloseApp_triggered();
    void on_actionImportLast_triggered();
    void slot_btn_ZTop_clicked(int);
    void slot_btn_ZDown_clicked(int);
    void slot_btn_XLeft_clicked(int);
    void slot_btn_XRight_clicked(int);


    void on_actionCamSetting_triggered();
    void on_actionCamStartCapture_triggered(bool checked);
    void on_actiontakePicture_triggered();

    void weldErrServ();
    void showPicture();

    void slot_CaptureDebugInfo(QString info,int lenth);
    void slot_onExposureChanged(int,int);//exposure changed
    void slot_onGainChanged(int,int);//exposure changed
    void slot_ShieldSport(bool);//
    void slot_ROIChanged(int,float x,float y,int width,int height);

    void slot_PicPressed(QPoint);

    void on_actionMotorSetting_triggered();

    void on_actionDebug_triggered();

    void on_tbn_cali_clicked();

    void on_tbn_weld_clicked();

    void on_tbn_stop_clicked();

    void on_tbn_recode_clicked();

signals:
    void weldErr();
    void logMove(QString ,int);
private:
    CamSetting *camSetting;
    MotorSetting *motorSetting;
    DebugDialog *debugForm;
    QCvDisplay *cvWidget;

    QStatusLight *led_Motor;
    QStatusLight *led_Cali;
    QStatusLight *led_front;
    QStatusLight *led_vertical;






public:
    void showCvImage(cv::Mat cvImg);


private:
    void initUIControls();  //初始化UI控件
    void initSignals();     //初始化信号槽
    void initObject();      //初始化对象指针
    void initDevices();     //初始化并打开设备接口
    void initResourceLib(); //初始化资源库
    void initMemberVariables(); //初始化成员变量
    void RecodeLogResults();    //记录log
    void RecodeResults();   //记录log
    void initGasFlowmeter();    //初始化气体流量计
    void initLastConfig();

    void showMessage(QString);
    void processCalibration(bool);  //处理标定结果
    void showStream(bool);
    AdjustCamera* GetCameraPointer(int index = -1);   //指向当前显示的相机
    QScopedPointer<std::thread> workThread; //工作线程
    int camIndex; //相机索引

};

#endif // MAINWINDOW_H
