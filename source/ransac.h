#ifndef RANSAC_H
#define RANSAC_H
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

class RanSac
{
public:
    RanSac() {}
    ~RanSac() {}
    bool build(int nIterCnt, double dMaxErrorThreshold, double rmsThreshold, cv::Mat &PLANE);
    void setPts(const vector<cv::Point> &pts) {_pts= pts;cout << _pts.size();}
    void setParas(int iter= 50,int snc= 7,int innerProportion= 50,double dMaxErrorThreshold= 18,double rmsThreshold= 5)
    {
        _iter= iter;
        _snc= snc;
        _innerProportion= innerProportion;
        _dMaxErrorThreshold= dMaxErrorThreshold;
        _rmsThreshold= rmsThreshold;
    }

protected:
    int _iter;
    int _snc;
    int _innerProportion;
    double _dMaxErrorThreshold;
    double _rmsThreshold;
    vector<double> _coefficients;
    set<uint32_t> innerSet;
    vector<cv::Point> _pts;
};

bool ranSac(vector<cv::Point> &_pts, int nIterCnt, double dMaxErrorThreshold, double rmsThreshold, cv::Vec4d &LINE, const int k);

#endif // RANSAC_H
