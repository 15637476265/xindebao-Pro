#include "QCvImageOperate.h"

#define LINEWIDTH 3     //画笔线宽
#define WELD_TORCH 60     //焊枪标定半径


QCvImageOperate::QCvImageOperate()
{

}

cv::Mat QCvImageOperate::draw_calibration(const cv::Mat &cvImg, int x, int y, bool caliEnd)
{
    cv::Mat temp = cvImg;
    //这两个值是预测的，后边要修改掉
    if(!caliEnd)
        temp = draw_calibration_region(temp,440,0,80,80);
    //绘制限位
    //temp = draw_limit(temp,240,1080);
    //绘制焊枪点

//    temp = draw_welding_torch(temp,x,y);
//    temp = draw_focus_line(temp,x,y);


//    temp = draw_welding_torch(temp,500,600);
//    temp = draw_focus_line(temp,500,600);
    return temp;
}




cv::Mat QCvImageOperate::draw_full_line(const cv::Mat &cvImg,const cv::Scalar& color, int x, int y)
{
    cv::Mat temp = cvImg;
    cv::Point start,end;
    if(0 == y){
        start.x = x;
        start.y = 0;
        end.x = x;
        end.y = cvImg.cols;

    }else{
        start.x = 0;
        start.y = y;
        end.x = 1280;//cvImg.rows;
        end.y = y;
    }

    cv::line(temp,start,end,color,LINEWIDTH-1);
    return temp;
}

cv::Mat QCvImageOperate::draw_circle(const cv::Mat &cvImg,const cv::Scalar& color, int length, int x, int y)
{
    cv::Mat temp = cvImg;
    cv::Point point(x, y);
    cv::circle(temp,point,length,color,LINEWIDTH-1);
    return temp;
}

cv::Mat QCvImageOperate::draw_rectangle(const cv::Mat &cvImg,const cv::Scalar& color, int x, int y,int width,int height)
{
    cv::Mat temp = cvImg;
    cv::Rect rect(x,y,width,height);
    cv::rectangle(temp, rect, color, LINEWIDTH);
    return temp;
}


//cv::Scalar(0, 255, 0)-----------green

cv::Mat QCvImageOperate::draw_limit(const cv::Mat &cvImg, int x1, int x2)
{
    cv::Mat temp = cvImg;
    //绘制两条红线
    temp = draw_full_line(temp,cv::Scalar(220,20,60),x1);
    temp = draw_full_line(temp,cv::Scalar(220,20,60),x2);
    return temp;
}

cv::Mat QCvImageOperate::draw_calibration_region(const cv::Mat &cvImg, int x, int y, int width, int height)
{
    cv::Mat temp = cvImg;
    return draw_rectangle(temp,cv::Scalar(0, 255, 0),x,y,width,height);
}

cv::Mat QCvImageOperate::draw_focus_line(const cv::Mat &cvImg, int x, int y)
{
    cv::Mat temp = cvImg;
    //绘制两条白线
    temp = draw_full_line(temp,cv::Scalar(248,248,255),x);
    temp = draw_full_line(temp,cv::Scalar(248,248,255),0,y);
    return temp;
}

cv::Mat QCvImageOperate::draw_focus_plus(const cv::Mat &cvImg, const cv::Scalar& color,int x, int y)
{
    cv::Mat temp = cvImg;
    cv::Point start,end;
    start.x = x-40;
    start.y = y;
    end.x = x+40;
    end.y = y;
    //绘制两条白线
    cv::line(temp,start,end,color,LINEWIDTH+2);
    start.x = x;
    start.y = y-40;
    end.x = x;
    end.y = y+40;
    cv::line(temp,start,end,color,LINEWIDTH+2);
    temp = draw_welding_torch(temp,x,y);
    return temp;
}

cv::Mat QCvImageOperate::draw_welding_torch(const cv::Mat &cvImg, int x, int y)
{
    cv::Mat temp = cvImg;
    return draw_circle(temp,cv::Scalar(0, 255, 0),WELD_TORCH,x,y);
}
