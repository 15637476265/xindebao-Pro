/*
    ** Auther:zxx
    ** Desc:cv辅助线功能
    ** Date:2019.11.20
*/
#ifndef QCVIMAGEOPERATE_H
#define QCVIMAGEOPERATE_H

#include <QObject>
#include <opencv2/opencv.hpp>



class QCvImageOperate
{
public:
    QCvImageOperate();

    /*
     * Arg1:焊枪x
     * Arg2:焊枪y
    */
    static cv::Mat draw_calibration(const cv::Mat &cvImg,int x,int y,bool caliEnd = false);



protected:
    static cv::Mat draw_limit(const cv::Mat &cvImg, int x1, int x2);    //绘制限位区间
    static cv::Mat draw_calibration_region(const cv::Mat &cvImg, int x, int y,int width,int height);    //绘制标定区域
    static cv::Mat draw_focus_line(const cv::Mat &cvImg, int x, int y);    //绘制聚焦线

    static cv::Mat draw_welding_torch(const cv::Mat &cvImg, int x, int y);    //绘制焊枪位置

public:
    static cv::Mat draw_full_line(const cv::Mat &cvImg,const cv::Scalar& color,int x,int y = 0);
    static cv::Mat draw_circle(const cv::Mat &cvImg,const cv::Scalar& color,int length,int x,int y = 0);
    static cv::Mat draw_rectangle(const cv::Mat &cvImg,const cv::Scalar& color, int x, int y,int width,int height);
    static cv::Mat draw_focus_plus(const cv::Mat &cvImg,const cv::Scalar& color, int x, int y);    //绘制聚焦线
};

#endif // QCVIMAGEOPERATE_H
