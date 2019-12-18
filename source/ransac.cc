#include "ransac.h"
#include <random>
static default_random_engine rng(time(0));
/*
bool RanSac::build(int nIterCnt, double dMaxErrorThreshold, double rmsThreshold, cv::Mat &PLANE)
{
    int nCnt= _pts.size();
    uniform_int_distribution<int> uniform(0,nCnt-1);

    int circleTimes= 0,rtimes= 1;

    int nBestRightCnt= int(_innerProportion*_pts.size()/100.0);

    while (nIterCnt)
    {
        //1.随机选择7个点
        set<int> sIndexs;
        sIndexs.clear();_snc= 7;
        while(sIndexs.size() < _snc) sIndexs.insert(uniform(rng));
        //2.得到模型参数

        vector<cv::Point> points;
        for (auto iter = sIndexs.begin(); iter != sIndexs.end(); iter++)
        {
            points.push_back(s[*iter]);
        }

        cv::Vec4d line;
        cv::fitLine(points,line,cv::DIST_HUBER,0,0.01,0.01);

        cv::Mat errors= points*plane(cv::Rect(0,0,1,3))-plane.at<double>(3,0);
        cv::Mat ae= (errors.t()*errors / errors.rows);

        double rms= pow(ae.at<double>(0,0),0.5);
        if(rms > rmsThreshold*rtimes)
        {
            //cout << "RMS = " << rms << endl;
            circleTimes++;
            if(circleTimes > 33)
            {
                if(++rtimes > 5) return false;
                circleTimes= 0;
            }
            continue;
        }

        //3.统计内点个数

        int nRightCnt = 0;
        double distance= 10000;
        vector<int> vecPoints;
        errors= _pts*plane(cv::Rect(0,0,1,3))-plane.at<double>(3,0);
        for(int i = 0; i < _pts.rows; i++)
        {
            distance= abs(errors.at<double>(i,0));
            if (distance < dMaxErrorThreshold)
            {
                vecPoints.push_back(i);
                nRightCnt++;
            }
        }
        cv::Mat innerPts= cv::Mat::zeros(nRightCnt,3,CV_64FC1);
        //for(auto &i:vecPoints)
        for(int i= 0;i< vecPoints.size();i++)
        {
            //points(cv::Rect(0,i++,3,1))= _pts(cv::Rect(0,*iter,3,1));
            ptrc= innerPts.ptr<double>(i);
            ptr= _pts.ptr<double>(vecPoints[i]);
            *(ptrc++)= *(ptr++);
            *(ptrc++)= *(ptr++);
            *(ptrc)= *(ptr);
        }

        errors= innerPts*plane(cv::Rect(0,0,1,3))-plane.at<double>(3,0);
        ae= (errors.t()*errors / errors.rows);
        rms= pow(ae.at<double>(0,0),0.5);
        //cout << "rms= " << rms << " " << nRightCnt << "/" << nBestRightCnt << endl;

        //4.保存内点个数最多的模型

        if (nRightCnt >= nBestRightCnt && rms <= rmsThreshold)
        {
            nBestRightCnt= nRightCnt;
            rmsThreshold= rms;
            fitPlane(innerPts,plane);
            PLANE= plane.clone();
            errors= innerPts*plane(cv::Rect(0,0,1,3))-plane.at<double>(3,0);
            ae= (errors.t()*errors / errors.rows);
            rms= pow(ae.at<double>(0,0),0.5);
            //cout << plane << "RRMS = " << rms << endl;
        }
        rtimes= 1;
        nIterCnt--;
    }
    return true;
}


bool ransac::build(vector<cv::Point> &pts, int nIterCnt, double dMaxErrorThreshold, double rmsThreshold, cv::Size sz)
{
    int nCnt= pts.size();
    int snc= nCnt/50;
    default_random_engine rng(time(0));
    uniform_int_distribution<int> uniform(0,nCnt);

    //PolyModel1D* rightModel= new PolyModel1D();
    int nBestRightCnt = 0;
    while (nIterCnt)
    {
        //1.随机选择n个点
        set<int> sIndexs;
        sIndexs.clear();
        while (1)
        {
            //cout << rng << " ";
            sIndexs.insert(uniform(rng));
            if (sIndexs.size() == snc)
                break;
        }
        //cout << nIterCnt-- << ":\n";
        for(auto &si:sIndexs) //cout << si << " ";
        //cout << endl;
    }
    //cout << sz << endl;

//    while (nIterCnt)
//    {
//        //1.随机选择两个点
//        set<int> sIndexs;
//        sIndexs.clear();
//        while (1)
//        {
//            sIndexs.insert(uniform(rng));
//            if (sIndexs.size() == snc)
//                break;
//        }

//        //2.得到模型参数
//        //set取值要注意
//        vector<Point> vecPoints;
//        vecPoints.clear();
//        for (auto iter = sIndexs.begin(); iter != sIndexs.end(); iter++)
//        {
//            vecPoints.push_back(pts[*iter]);
//        }

//        deque<double> x;
//        deque<double> y;
//        for(auto &p:vecPoints)
//        {
//            x.push_back(p.x);
//            y.push_back(p.y);
//        }
//        PolyModel1D* model= new PolyModel1D();
//        model->setParams({2});
//        model->setData(y,x);
//        model->build();
//        double rms= model->getRMS();

//        if(rms > rmsThreshold*2)
//        {
//            //cout << "RMS = " << rms << endl;
//            delete model;
//            continue;
//        }

//        //3.统计内点个数
//        int nRightCnt = 0;
//        double distance= 10000;
//        vecPoints.clear();
//        for (int i = 0; i < nCnt; i++)
//        {
//            distance= abs(pts[i].y-model->model(pts[i].x));
//            if (distance < dMaxErrorThreshold)
//            {
//                vecPoints.push_back(pts[i]);
//                nRightCnt++;
//            }
//        }
//        x.clear();y.clear();
//        for(auto &p:vecPoints)
//        {
//            x.push_back(p.x);
//            y.push_back(p.y);
//        }
//        model->setData(y,x);
//        model->build();
//        rms= model->getRMS();

//        //4.保存内点个数最多的模型
//        if (nRightCnt > nBestRightCnt && rms < rmsThreshold)
//        {
//            nBestRightCnt= nRightCnt;
//            rmsThreshold= rms;
//            rightModel= model;
//            //cout << "RRMS = " << rms << endl;
//        }
//        nIterCnt--;
//    }

    return true;
}
*/

bool ranSac(vector<cv::Point> &_pts,int nIterCnt, double dMaxErrorThreshold, double rmsThreshold, cv::Vec4d &LINE,const int k)
{
    bool result= false;

    int nCnt= _pts.size();
    uniform_int_distribution<int> uniform(0,nCnt-1);

    int circleTimes= 0,rtimes= 1;
    int _innerProportion= 30;
    int nBestRightCnt= int(_innerProportion*_pts.size()/100.0);

    while (nIterCnt)
    {
        //1.随机选择7个点
        set<int> sIndexs;
        sIndexs.clear();
        int _snc= 6;
        while(sIndexs.size() < _snc) sIndexs.insert(uniform(rng));
        //2.得到模型参数

        vector<cv::Point> points;
        for (auto iter = sIndexs.begin(); iter != sIndexs.end(); iter++)
        {
            //cout << *iter << " ";
            points.push_back(_pts[*iter]);
        }
        //cout << endl;

        cv::Vec4d line;
        cv::fitLine(points,line,cv::DIST_HUBER,0,0.01,0.01);

        //cout << "dr: " << dr << endl;

        int kk= line[1]/line[0]*k;
        if(kk < 0 || kk > 1) continue;
        double dr= line[0]*line[3]-line[1]*line[2],adr2= 0;
        for(auto &p:points)
        {
            //cout << line[0] << " " << line[1] << " " << pow(line[0],2)+pow(line[1],2) << endl;
            adr2+= pow(dr- (line[0]*p.y-line[1]*p.x),2);
        }

        double rms= pow(adr2/points.size(),0.5);
        if(rms > rmsThreshold*rtimes)
        {
            //cout << "RMS = " << rms << endl;
            circleTimes++;
            if(circleTimes > 33)
            {
                if(++rtimes > 9) return result;
                circleTimes= 0;
            }
            continue;
        }

        //3.统计内点个数

        int nRightCnt = 0;
        double distance= 10000;
        vector<cv::Point> vecPoints;
        for(auto &p:_pts)
        {
            distance= abs(dr- (line[0]*p.y-line[1]*p.x));
            if (distance < dMaxErrorThreshold)
            {
                vecPoints.push_back(p);
                nRightCnt++;
            }
        }

        if (nRightCnt >= nBestRightCnt)
        {
            cv::fitLine(vecPoints,line,cv::DIST_HUBER,0,0.01,0.01);
            kk= line[1]/line[0]*k;
            if(kk < 0 || kk > 1) continue;
            dr= line[0]*line[3]-line[1]*line[2];
            adr2= 0;
            for(auto &p:vecPoints)
            {
                adr2+= pow(dr-(line[0]*p.y-line[1]*p.x),2);
            }
            rms= pow(adr2/vecPoints.size(),0.5);
        }
        else continue;
        //4.保存内点个数最多的模型

        if (rms <= rmsThreshold)
        {
            nBestRightCnt= nRightCnt;
            rmsThreshold= rms;
            //cout << "RRMS = " << rms << endl;
            result= true;
            LINE= line;
        }
        rtimes= 1;
        nIterCnt--;
    }
    return result;
}
