#ifndef ERROR_H
#define ERROR_H

typedef enum TAG_RunError{
    TAG_NO_ERROR        = 0x00000000,   //
    /* SYSTEM */
    TAG_ENV_ERROR       = 0x00000001,   //缺少环境变量
    TAG_DRIVER_ERROR    = 0x00000002,   //缺失驱动
    TAG_CAM_ERROR       = 0x00000003,   //找不到相机,或者相机被占用
    TAG_MOTOR_ERROR     = 0x00000004,   //找不到马达,或者马达被占用
    TAG_FILE_ERROR      = 0x00000005,   //文件问题，检查文件权限或是否存在

    /* APPLICATION */
    TAG_IMAGE_ERROR            = 0x00000025,   //图像采集错误
    TAR_CALIBRATION_ERROR      = 0x00000025,   //标定失败
    TAG_WELD_ERROR             = 0x00000026,   //焊接错误
    TAG_CALC_ERROR             = 0x00000027,   //图像计算失败
    TAG_RWFILE_ERROR           = 0x00000027,   //写入文件或者读取配置失败，检查索引是否正确

    /* Cam */
    TAG_CAM_OPERATE_ERROR            = 0x00000035,   //相机配置失败

    TAR_ALL_ERROR
}RunError;


typedef enum TAG_RunWarning{
    TAG_NO_WARNING       = 0x00000000,   //
    /* SYSTEM */
    TAG_VERTICAL_WARNING              = 0x10000002,   //垂直方向相机偏离警告
    TAG_VALUE_INPUT_WARNING           = 0x10000003,   //数值设置警告
    TAG_IMAGE_FAIL_WARNING            = 0x10000004,   //图像不正确
    TAG_WELD_LINE_OUTRANGE_WARNING    = 0x10000005,   //焊缝偏移过大
    TAR_ALL_WARNING
}RunWarning;









#endif // ERROR_H
