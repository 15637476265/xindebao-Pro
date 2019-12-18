#include "utils.h"

bool matLine2du(Vec4d line1,Vec4d line2,double & mu,int v0)
{
    double u11 = line1[0];
    double v11 = line1[1];
    double u12 = line1[2];
    double v12 = line1[3];
    double u21 = line2[0];
    double v21 = line2[1];
    double u22 = line2[2];
    double v22 = line2[3];
    if(abs(v11-v12) <0.0001 || abs(v21-v22) < 0.0001)
        return false;
    // cal du
    double u1 = u12 + (u11-u12)*(v0-v12)/(v11-v12);
    double u2 = u22 + (u21-u22)*(v0-v22)/(v21-v22);
    mu = round((u1+u2)/2);
    return true;
}
