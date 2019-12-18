#include "device/camera.h"
#include <string.h>
#include <stdlib.h>

#include <mutex>
#include <future>
#include <fstream>

#include <thread>

#include "base/wzerror.h"

#include <m3api/m3Api.h>
#include <memory.h>

#include <boost/date_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include "base/errorcode.h"

using namespace std::chrono ;
using namespace std;
#define HandleResult(res,place) if (res!=XI_OK) {printf("Error after %s (%d)\n",place,res);return;}

Camera::Camera( )
    :_xiHandle(nullptr),
      _countOpen{0},
      _status{DISCONNECTED},
      _bAutoTakePicture{false}
{
    //    connectDevice();
    // std::thread( &Camera::connectDevice, this).detach() ;
    connectDevice();
}

Camera::~Camera()
{
    disconnectDevice();
}

void Camera::connectDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    // 如果相机未开启则打开
    if ( DISCONNECTED == _status ) {
        // 打开设备
        if ( XI_OK != xiOpenDevice( 0, &_xiHandle ) ) {
            std::cout << "open camera error!" << std::endl ;
            return ;
        }
        memset( &_img, 0, sizeof(_img) ) ;
        _img.size = sizeof(_img) ;
        // 设置缓冲
        xiSetParamInt( _xiHandle, XI_PRM_BUFFER_POLICY, XI_BP_SAFE ) ;
        // 设置曝光
        xiSetParamInt(_xiHandle, XI_PRM_AEAG, 0);

        Config config("etc/sys.info") ;

        xiSetParamInt(_xiHandle, XI_PRM_EXPOSURE, config.get<int>("camera.exposure"));

        xiSetParamFloat(_xiHandle, XI_PRM_GAIN, config.get<float>("camera.gain"));

        xiSetParamInt(_xiHandle, XI_PRM_TRG_SOURCE, XI_TRG_OFF);
        initRoi();

        // 相机开始获取图片
        auto ret = xiStartAcquisition(_xiHandle) ;

//        if(ret != XI_OK)
//            throw;
        _status = CONNECTED ;
        int isCoold = 0 ;
        xiGetParamInt( _xiHandle, XI_PRM_IS_COOLED, &isCoold) ;
        if(_xiHandle == NULL)
            throw ERR_CAMERA_SHAREDPTR_IS_NULLPTR;
        std::cout << "isCoold:" << isCoold << std::endl ;
    }
    ++_countOpen ;
}

void Camera::disconnectDevice()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    if ( DISCONNECTED != _status && 0 == --_countOpen ) {
        auto stat = xiStopAcquisition( _xiHandle ) ;
        //std::cout << stat << std::endl ;
        stat = xiCloseDevice( _xiHandle ) ;
        _status = DISCONNECTED ;
        _xiHandle = nullptr ;
    }
}

cv::Mat Camera::takePicture()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    cv::Mat matRet;
    if ( CONNECTED != _status || nullptr == _xiHandle )
    {
        return matRet;
    }
    XI_RETURN xi_ret ;
    try
    {
        xi_ret = xiGetImage(_xiHandle, 20000, &_img) ;
        ofstream ofs;
        ofs.open("img.data",ios_base::binary);
        ofs.write((const char *)_img.bp,_img.bp_size);
        ofs.close();

        if ( XI_OK != xi_ret || nullptr == _img.bp )
        {
            _status = ERROR ;
            cout<<getErrString(ERR_CAMERA_GET_IMAGE_FAILED)<<endl;
            throw ERR_CAMERA_GET_IMAGE_FAILED;
            BOOST_THROW_EXCEPTION(WZError() << err_camera("相机故障,图片采集失败")) ;
        }

        cv::Mat mat(_img.height, _img.width, CV_8UC1) ;
        memcpy( mat.data, _img.bp, _img.bp_size ) ;
        matRet = mat ;
        {
            std::lock_guard<std::recursive_mutex> lck(_mtxMat) ;
            _mat = mat ;
        }
        _timePicture = steady_clock::now() ;
    }
    catch(ReturnStatus error_code)
    {
        cout<< getErrString(error_code) <<endl;
        // throw;
    }
    catch(...)
    {
        resetCamera();
    }
    return _mat ;
}

void Camera::setGain(float f)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    if ( nullptr == _xiHandle ) {
        return ;
    }
    xiSetParamFloat(_xiHandle, XI_PRM_GAIN, f);
}

void Camera::setExposure(int e)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    if ( nullptr == _xiHandle ) {
        return ;
    }
    xiSetParamInt(_xiHandle, XI_PRM_EXPOSURE, e);
}

void Camera::setRoi( float a, float b,int c,int d)
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    if ( nullptr == _xiHandle ) {
        return ;
    }
    xiSetParamInt(_xiHandle, XI_PRM_WIDTH, c);
    xiSetParamInt(_xiHandle, XI_PRM_HEIGHT, d);
    xiSetParamInt(_xiHandle, XI_PRM_OFFSET_X, a);
    xiSetParamInt(_xiHandle, XI_PRM_OFFSET_Y, b);
}

/// 查询相机数
void Camera::getXimeaCount()
{
    ///相機數量
    DWORD devices_count=0 ;
    xiGetNumberDevices(&devices_count) ;
    if(devices_count == 0)
        throw;
}

void Camera::resetCamera()
{
    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    xiSetParamInt(_xiHandle,XI_PRM_DEVICE_RESET,1) ;
    std::this_thread::sleep_for(10000_ms) ;
    ///相機數量
    DWORD devices_count=0 ;
    xiGetNumberDevices(&devices_count) ;
    std::cout<<"after devices_count: "<<devices_count<<std::endl;
    disconnectDevice();
    connectDevice();
}


void Camera::initRoi()
{
    //    std::lock_guard<std::recursive_mutex> lck(_mtx) ;
    //    if ( nullptr == _xiHandle ) {
    //        return ;
    //    }
    Config config("etc/sys.info") ;
    xiSetParamInt(_xiHandle, XI_PRM_WIDTH, config.get<int>("camera.roi_width"));
    xiSetParamInt(_xiHandle, XI_PRM_HEIGHT, config.get<int>("camera.roi_height"));
    xiSetParamFloat(_xiHandle, XI_PRM_OFFSET_X, config.get<float>("camera.roi_offset_x"));
    xiSetParamFloat(_xiHandle, XI_PRM_OFFSET_Y, config.get<float>("camera.roi_offset_y"));

}

void Camera::setAutoTakePicture( bool b )
{
    {
        std::lock_guard<std::recursive_mutex> lck(_mtx) ;
        if ( b == _bAutoTakePicture ) return ;
        _bAutoTakePicture = b ;
    }
    if ( _bAutoTakePicture ) {
        std::thread([this]{
            while ( _bAutoTakePicture ) {
                takePicture() ;
            }
        }).detach() ;
        MillDiff millDiff(20) ;
        while ( steady_clock::now() - _timePicture > millDiff) ;
    }
}

const cv::Mat Camera::getImg() const
{
    Millsecond timeNow = steady_clock::now() ;
    if  ((timeNow - _timePicture)  > MillDiff(20)) {
        const_cast<Camera*>(this)->takePicture() ;
        //      std::cout << "重拍" << std::endl ;
    }
    else {
        //      std::cout << "直接使用" << std::endl ;
    }
    std::lock_guard<std::recursive_mutex> lck(_mtxMat) ;
    return _mat ;
}

unsigned Camera::getTemperature() const
{
    unsigned int temperature = 0 ;
    //std::cout << "get:" << mmGetTemperature( _xiHandle, &temperature ) << std::endl ;
    //std::cout << "temperature:" << temperature << std::endl ;
    float housing ;
    float chip ;
    mmGetTemperatureEx( _xiHandle, &housing, &chip ) ;
    static std::fstream ofs("camera_temperature.txt", std::ios::app) ;
    boost::posix_time::ptime pTime = boost::posix_time::second_clock::local_time() ;
    ofs << pTime << "           " << chip << "\n" ;
    //std::cout << "housing:" << housing << " chip:" << chip << std::endl ;
    return temperature ;
}


void Camera::upsampling()
{
    xiSetParamInt(_xiHandle,XI_PRM_DOWNSAMPLING,1);
    xiSetParamInt(_xiHandle,XI_PRM_DOWNSAMPLING_TYPE,1);
}

void Camera::downsampling()
{
    xiSetParamInt(_xiHandle,XI_PRM_DOWNSAMPLING,2);
    xiSetParamInt(_xiHandle,XI_PRM_DOWNSAMPLING_TYPE,0);
}


/// 设置bit depth 8
void Camera::setSensor8bit()
{
    xiSetParamInt(_xiHandle,XI_PRM_IMAGE_DATA_FORMAT, XI_RAW8);
    xiSetParamInt(_xiHandle,XI_PRM_SENSOR_DATA_BIT_DEPTH, XI_BPP_8);
}

/// 设置bit depth 10
void Camera::setSensor10bit()
{
    xiSetParamInt(_xiHandle,XI_PRM_IMAGE_DATA_FORMAT, XI_RAW16);
    xiSetParamInt(_xiHandle,XI_PRM_SENSOR_DATA_BIT_DEPTH, XI_BPP_10);
}


