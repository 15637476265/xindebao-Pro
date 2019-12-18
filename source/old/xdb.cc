#include "xdb.h"
//#include "ransac.h"


//edgeCurveDetector::edgeCurveDetector(bool v,int ksz)
//{
//    getKernel(ksz);
//}

void edgeCurveDetector::getKernel(int sz)
{
    if(sz%2 == 0 || sz < 3)
    {
        cout << "wrong input kernel size!!!\n";
    }
    else
    {
        _kernel= cv::Mat::zeros(sz-2,sz,CV_32FC1);
        int hf= (sz-1)/2;
        float v= 2.0/(sz-2)/(sz-1.0);
        for(int n= 0;n < sz-2;n++)
        {
            for(int m= 0;m < hf;m++) _kernel.at<float>(n,m)= -v;
            for(int m= hf+1;m < sz; m++) _kernel.at<float>(n,m)= v;
        }
        if(!_vertical) _kernel= _kernel.t();
        //cout << 0.16666667*3*5 << "kernel: \n" << _kernel << endl;
    }
}

bool edgeCurveDetector::getImg(const cv::Mat &input,const int resize)
{
    if(resize == 1) _img= _vertical? input.clone() : input.t();
    else if(resize == 2) pyrDown(_vertical? input : input.t(),_img,cv::Size(input.cols/resize,input.rows/resize));
    else if(resize == 4)
    {
        cv::Mat inputc= _vertical? input.clone() : input.t();
        _img= cv::Mat::zeros(inputc.rows/resize,inputc.cols/resize,CV_8UC1);
        uchar *p0,*p1,*p2,*p3;
        uchar *pd;
        int vc;
        //Rect2i roi= Rect2i(0,0,scale,scale);
        for(int i= 0;i < 1;i++)
        {
            for(int n= 0;n < _img.rows;n++)
            {
                p0= inputc.ptr<uchar>(4*n);
                p1= inputc.ptr<uchar>(4*n+1);
                p2= inputc.ptr<uchar>(4*n+2);
                p3= inputc.ptr<uchar>(4*n+3);
                pd= _img.ptr<uchar>(n);
                for(int m= 0;m < _img.cols-1;m++)
                {
                    vc= 0;
                    vc+= *(p0++);vc+= *(p0++);vc+= *(p0++);vc+= *(p0++);
                    vc+= *(p1++);vc+= *(p1++);vc+= *(p1++);vc+= *(p1++);
                    vc+= *(p2++);vc+= *(p2++);vc+= *(p2++);vc+= *(p2++);
                    vc+= *(p3++);vc+= *(p3++);vc+= *(p3++);vc+= *(p3++);
                    *(pd++)= round(vc/16);
                }
                vc= 0;
                vc+= *(p0++);vc+= *(p0++);vc+= *(p0++);vc+= *(p0);
                vc+= *(p1++);vc+= *(p1++);vc+= *(p1++);vc+= *(p1);
                vc+= *(p2++);vc+= *(p2++);vc+= *(p2++);vc+= *(p2);
                vc+= *(p3++);vc+= *(p3++);vc+= *(p3++);vc+= *(p3);
                *(pd)= round(vc/16);
            }
        }
    }
    else
    {
        cout << "wrong resize!!!\n";
        return false;
    }
    _resize= resize;
    //cout << "getImg: " << cv::Point(_img.rows,_img.cols) << "resize: " << _resize << endl;
//    cv::namedWindow( "i", CV_WINDOW_AUTOSIZE );
//    cv::imshow("i",_img);
//    cv::waitKey(0);
    return true;
}

cv::Mat edgeCurveDetector::detector(cv::Vec4d &lineLeft, cv::Vec4d &lineRight, const cv::Mat &input, int range)
{
    cv::Mat dstl,dstr,dst;
    cv::filter2D(_img(cv::Rect2i(0,0,_img.cols/2,_img.rows)),dstl,-1,_kernel);
    cv::filter2D(_img(cv::Rect2i(_img.cols/2,0,_img.cols/2,_img.rows)),dstr,-1,-_kernel);
    //_img= (dstl+dstr)/2;
            //dstl.convertTo(dst, CV_8UC1);
//    cv::namedWindow( "i", CV_WINDOW_AUTOSIZE );
//    cv::threshold(dstl,dst,11,255,CV_THRESH_OTSU);
//    cv::imshow("i",dst);
//    cv::waitKey(0);

    double maxVal= 0,minVal= 0;dst= cv::Mat::zeros(_img.rows,_img.cols,CV_8UC1);
    cv::Point maxLoc,minLoc;
    //cv::minMaxLoc(dstl, NULL, &maxVal, &minLoc, &maxLoc);
    cv::Rect2i roi(0,0,dstl.cols,1);
    vector<cv::Point> pts;
    //cv::Vec4d lineLeft,lineRight;
    //fitLine(pts, lineRight, DIST_HUBER, 0, 0.01,0.01);
    vector<int> leftu;
    for(int i= 0;i < dstl.rows;i++)
    {
        roi.y= i;
        cv::minMaxLoc(dstl(roi),&minVal,&maxVal,&minLoc,&maxLoc);
        leftu.push_back(maxLoc.x);
    }
    nth_element(leftu.begin(),leftu.begin()+leftu.size()/2,leftu.end());
    int medianl= leftu[leftu.size()/2];
    range/= _resize;
    roi.width= (medianl+range < dstl.cols)? medianl+range:dstl.cols;//cout << roi << endl;

    for(int i= 0;i < dstl.rows;i++)
    {
        roi.y= i;
        cv::minMaxLoc(dstl(roi),&minVal,&maxVal,&minLoc,&maxLoc);
        if(maxVal < 25) continue;
        pts.push_back(cv::Point(i,maxLoc.x));
        dst.at<uchar>(i,maxLoc.x)= maxVal;
        //cout << i << ": " << minVal << endl;
    }
//    cv::namedWindow( "d", CV_WINDOW_AUTOSIZE );
//    cv::imshow("d",dst);
//    cv::waitKey(0);
    if(pts.size() < dstl.rows/5.0) return cv::Mat::zeros(0,0,CV_8UC1);
    fitLine(pts, lineLeft, cv::DIST_HUBER, 0, 0.01,0.01);
    double ddr,dr= lineLeft[0]*(lineLeft[3])-lineLeft[1]*(lineLeft[2]); cout << lineLeft << " " << dr << endl;
    int count= 0;
    for(auto &p:pts)
    {
        ddr= abs(lineLeft[0]*(p.y)-lineLeft[1]*(p.x)-dr);
        if(ddr > 12/_resize)  cout << count++ << ": " << p << " ";
    }
    cout << endl;
    //if(count*1.0/dstl.rows > 0.33) return cv::Mat::zeros(0,0,CV_8UC1);
    lineLeft[2]= (dr+lineLeft[1]*0)/lineLeft[0];
    lineLeft[3]= (dr+lineLeft[1]*dstl.rows)/lineLeft[0];
    lineLeft[0]= lineLeft[2]*_resize;
    lineLeft[1]= 0;
    lineLeft[2]= lineLeft[3]*_resize;
    lineLeft[3]= _img.rows*_resize;
//    double dr= lineRight[0]*(lineRight[3]+roisl.y+mroi.y)-lineRight[1]*(lineRight[2]+roisl.x+mroi.x); cout << lineRight << " " << dr << endl;
//    slcut0.y= (dr+lineRight[1]*(slcut0.x))/lineRight[0];
//    slcut1.y= (dr+lineRight[1]*(slcut1.x))/lineRight[0];

//    if(input.data)
//    {
//        cv::Mat inputc;
//        cv::cvtColor(input,inputc,CV_GRAY2BGR);
//        cv::line(inputc,cv::Point2d(lineLeft[0],lineLeft[1]),cv::Point2d(lineLeft[2],lineLeft[3]),cv::Scalar(0,255,0),1,cv::LINE_AA);
//        cv::namedWindow( "i", CV_WINDOW_AUTOSIZE );
//        cv::imshow("i",inputc);
//        cv::waitKey(0);
//    }
//    cout << "-dstl--" << count << endl;

    pts.clear();
    roi.width= dstr.cols;
    vector<int> rightu;
    for(int i= 0;i < dstr.rows;i++)
    {
        roi.y= i;
        cv::minMaxLoc(dstr(roi),&minVal,&maxVal,&minLoc,&maxLoc);
        rightu.push_back(maxLoc.x);
        //cout << maxLoc.x+_img.cols/2 << " ";
    }
    nth_element(rightu.begin(),rightu.begin()+rightu.size()/2,rightu.end());
    int medianr= rightu[rightu.size()/2];
    range/= _resize;

    roi.x= (medianr-range < 0)? 0:medianr-range;
    roi.width= dstr.cols-roi.x; //cout << roi << endl;
    for(int i= 0;i < dstr.rows;i++)
    {
        roi.y= i;
        cv::minMaxLoc(dstr(roi),&minVal,&maxVal,&minLoc,&maxLoc);
        pts.push_back(cv::Point(i,maxLoc.x+roi.x));
        dst.at<uchar>(i,maxLoc.x+roi.x+_img.cols/2)= maxVal;
        //cout << i << ": " << minVal << endl;
    }
//    cv::namedWindow( "d", CV_WINDOW_AUTOSIZE );
//    cv::imshow("d",dst);
//    cv::waitKey(0);
    cv::imwrite("d.png",dst);

    if(pts.size() < dstr.rows/5.0) return cv::Mat::zeros(0,0,CV_8UC1);
    fitLine(pts, lineRight, cv::DIST_HUBER, 0, 0.01,0.01);
    dr= lineRight[0]*(lineRight[3])-lineRight[1]*(lineRight[2]); cout << lineRight << " " << dr << endl;
    count= 0;
    for(auto &p:pts)
    {
        ddr= abs(lineRight[0]*(p.y)-lineRight[1]*(p.x)-dr);
        if(ddr > 12/_resize)  cout << count++ << ": " << p << " ";
    }
    cout << endl;
    //if(count*1.0/dstr.rows > 0.33) return cv::Mat::zeros(0,0,CV_8UC1);
    lineRight[2]= (dr+lineRight[1]*0)/lineRight[0];
    lineRight[3]= (dr+lineRight[1]*dstl.rows)/lineRight[0];
    lineRight[0]= (lineRight[2]+_img.cols/2)*_resize;
    lineRight[1]= 0;
    lineRight[2]= (lineRight[3]+_img.cols/2)*_resize;
    lineRight[3]= _img.rows*_resize;
//    double dr= lineRight[0]*(lineRight[3]+roisl.y+mroi.y)-lineRight[1]*(lineRight[2]+roisl.x+mroi.x); cout << lineRight << " " << dr << endl;
//    slcut0.y= (dr+lineRight[1]*(slcut0.x))/lineRight[0];
//    slcut1.y= (dr+lineRight[1]*(slcut1.x))/lineRight[0];

    cv::Mat inputc;
    if(input.data)
    {
//        cv::Mat inputc;
//        cv::cvtColor(input,inputc,CV_GRAY2BGR);
//        cv::line(inputc,cv::Point2d(lineRight[0],lineRight[1]),cv::Point2d(lineRight[2],lineRight[3]),cv::Scalar(0,255,0),1,cv::LINE_AA);
//        cv::namedWindow( "i", CV_WINDOW_AUTOSIZE );
//        cv::imshow("i",inputc);
//        cv::waitKey(0);

        cv::cvtColor(input,inputc,CV_GRAY2BGR);
        cv::line(inputc,cv::Point2d(lineRight[0],lineRight[1]),cv::Point2d(lineRight[2],lineRight[3]),cv::Scalar(0,255,0),1,cv::LINE_AA);
        cv::line(inputc,cv::Point2d(lineLeft[0],lineLeft[1]),cv::Point2d(lineLeft[2],lineLeft[3]),cv::Scalar(0,255,0),1,cv::LINE_AA);
        cv::circle(inputc,cv::Point2d(medianl*_resize,lineLeft[1]),15,cv::Scalar(0,0,255),3,cv::LINE_AA);
        cv::circle(inputc,cv::Point2d(medianl*_resize,lineLeft[3]),15,cv::Scalar(0,0,255),3,cv::LINE_AA);
        cv::circle(inputc,cv::Point2d((medianr+_img.cols/2)*_resize,lineRight[1]),15,cv::Scalar(0,0,255),3,cv::LINE_AA);
        cv::circle(inputc,cv::Point2d((medianr+_img.cols/2)*_resize,lineRight[3]),15,cv::Scalar(0,0,255),3,cv::LINE_AA);
        //cv::imwrite("twogreenlines.png",inputc);
        //input= inputc;
//        cv::Mat result= ecd.detector(lineLeft,lineRight,input,100);
//        if(result.data) cv::imwrite(to_string(i)+"twogreenlines.png",result);
    }
    cout << "-dstr--" << count;

    //ransac rc;
    //rc.build(pts,50,33/_resize,10/_resize);

    return inputc;
}
