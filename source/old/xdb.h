#ifndef XDB_H
#define XDB_H
#include <opencv2/opencv.hpp>

using namespace std;

static double elementArray55[5][5]= {{-1, -1, 0, 1, 1},
                                     {-1, -1, 0, 1, 1},
                                     {-1, -1, 0, 1, 1},
                                     {-1, -1, 0, 1, 1},
                                     {-1, -1, 0, 1, 1},};
static double elementArray77[7][7]= {{-1, -1, -1, 0, 1, 1, 1},
                                     {-1, -1, -1, 0, 1, 1, 1},
                                     {-1, -1, -1, 0, 1, 1, 1},
                                     {-1, -1, -1, 0, 1, 1, 1},
                                     {-1, -1, -1, 0, 1, 1, 1},
                                     {-1, -1, -1, 0, 1, 1, 1},
                                     {-1, -1, -1, 0, 1, 1, 1},};

class edgeCurveDetector
{
public:
    edgeCurveDetector(bool v= true,int ksz= 5): _vertical(v)
    {
        getKernel(ksz);
        num= 0;
    }
    ~edgeCurveDetector() {}
    bool getImg(const cv::Mat &input, const int resize= 4);
    void getKernel(int sz= 5);
    cv::Mat detector(cv::Vec4d &lineLeft, cv::Vec4d &lineRight, const cv::Mat &input= cv::Mat::zeros(0,0,CV_8UC1), int range= 150);
    bool _vertical;
    int num;

protected:
    cv::Mat _img,_kernel;
    int _resize;
    vector<double> _x;
    vector<double> _t;
};
#endif // XDB_H
