#ifndef CAMERA_H
#define CAMERA_H



#include "src/Method/OpenConfig.h"
#include "GxIAPI.h"
#include <chrono>
#include <mutex>
#include <QImage>
#include <opencv2/opencv.hpp>
#define UPDATE_TIME_OUT 1000


void ShowErrorString(GX_STATUS error_status);


/// 如果结果为false，则跳出循环，否则不处理
#define UI_CHECK_BOOL_RESULT(result) \
    if(!result)\
{\
    break;\
    }

/// 如果结果为NULL，则跳出循环，否则不处理
#define UI_CHECK_NEW_MEMORY(new_memory)\
    if(NULL == new_memory)\
{\
    break;\
    }

#define  GX_VERIFY(emStatus) \
    if (emStatus != GX_STATUS_SUCCESS)\
{\
    ShowErrorString(emStatus); \
    }  ///< 错误提示函数宏定义

#define VERIFY_STATUS_RET(emStatus) \
    if (emStatus != GX_STATUS_SUCCESS) \
{\
    return emStatus;\
    }\

#define BYTE unsigned char


class AdjustCamera
{
public:
    AdjustCamera();
    ~AdjustCamera();

    static int enumCamDevice();
    void initGXLib();

    GX_STATUS OpenDevice();

    GX_STATUS CloseDevice();

    GX_STATUS InitDevice();

    GX_STATUS SetPixelFormat8bit();

    GX_STATUS GetDeviceInitParam();

    cv::Mat getImg();

    cv::Mat takePicture();

    void preConnectDevice();

    void connectDevice(std::string );

    void disconnectDevice();

    void setRoi( float offsetX, float offsetY,int width,int height);

    void setGain( float gain ) ;

    void setExposure( int exposure ) ;
    /// 为显示图像准备资源
    bool PrepareForShowImg();
    /// 为彩色图像显示准备资源,分配Buffer
    bool PrepareForShowColorImg();
    /// 为黑白图像显示准备资源,分配Buffer
    bool PrepareForShowMonoImg();
    /// 释放为显示图像准备的资源
    void UnPrepareForShowImg();

    //发送采集指令
    void sendStartCapture();

    void sendStopCapture();

    GX_FRAME_DATA getFrameDate();


    bool isDeviceOpend();
private:
    GX_DEV_HANDLE  m_device_handle;
    GX_FRAME_DATA m_frame_data;
    int64_t m_pixel_color;
    std::recursive_mutex _mtx;
    mutable std::recursive_mutex _mtxMat;
    cv::Mat _mat;
    bool m_is_open; //相机是否打开
    bool m_is_snap;
    bool m_color_filter;
    int m_time_out;
    //char *serial;
    std::string _cam_index;

public:
    int64_t m_image_width;
    int64_t m_image_height;
    int64_t m_payload_size;
    QImage* m_show_image;
    void* m_result_image;
};

#endif // CONTROL_H
