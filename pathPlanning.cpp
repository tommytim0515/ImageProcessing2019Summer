#include "pathPlanning.h"
#include "declaration.h"
#include "track_processing.h"
#include <cmath>

using namespace std;

bool coorValid(ii coor) {
    if (coor.first > VALID_TOPMOST && coor.first < VALID_BOTTOMMOST && coor.second > VALID_LEFTMOST && coor.second < VALID_RIGHTMOST)
        return true;
    return false;
}

ff getDirectionVector(ii coor1, ii coor2) { // we suppose coor1 > coor2
    if (coorValid(coor1)) {
        ff result1 = TrackProcessing::getCoords(coor1);
        if (coorValid(coor2)) {
            ff result2 = TrackProcessing::getCoords(coor2);
            ff result(result1.first - result2.first, result1.second - result2.second);
            return TrackProcessing::normalizeVector(result);
        } else {
            ff result(NAN, NAN);
            return result;
        }
    } else {
        ff result(NAN, NAN);
        return result;
    }
}

ff checkEdge(ii coor1, ii coor2, ii coor3, ii point) {
    ff vector1(NAN, NAN);
    ff vector2(NAN, NAN);
    if (!isnan(coor1.first) && !isnan(coor2.first)) {
        ff vector11 = TrackProcessing::normalizeVector(getDirectionVector(coor1, coor2));
        vector1.first = vector11.first;
        vector1.second = vector11.second;
    }
    if (!isnan(coor2.first) && !isnan(coor3.first)) {
        ff vector22 = TrackProcessing::normalizeVector(getDirectionVector(coor2, coor3));
        vector2.first = vector22.first;
        vector2.second = vector22.second;
    }
    if (!point.first) {
        if(isnan(vector1.first) && isnan(vector2.first)) {
            ff result(NAN, NAN);
            return result;
        } else if (isnan(vector1.first) && !isnan(vector2.first)) {
            return TrackProcessing::normalizeVector(vector2);
        } else if (!isnan(vector1.first) && isnan(vector2.first)) {
            // vector1.first = vectoerPercentage * vector1.first;
            // vector1.second = vectoerPercentage * vector1.second;
            // return vector1;
            ff result(NAN, NAN);
            return result;
        } else if (!isnan(vector1.first) &&  !isnan(vector2.first)) {
            ff result(vector1.first*vectoerPercentage + vector2.first*vectoerPercentage, vector1.second*vectoerPercentage + vector2.second*vectoerPercentage);
            TrackProcessing::normalizeVector(result);
            return result;
        }
    } else {
        ff cornerPoint = TrackProcessing::getCoords(point);
        ff result(TrackProcessing::normalizeVector(getDirectionVector(cornerPoint, coor3)));
        return result;
    } 
}

bool leftLoopCheck(ff sideVector, ff vector1, ff vector2) {

}

bool rightLoopCheck(ff sideVector, ff vector1, ff vector2) {
    std::cout << "start" << endl;
    if (!isnan(sideVector.first) && !isnan(vector1.first) && !isnan(vector2.first)) {
        double vectorAngle = (vector1.first*vector2.first + vector1.second*vector2.second) / (sqrt(vector1.first*vector1.first + vector1.second*vector1.second)*sqrt(vector2.first*vector2.first + vector2.second*vector2.second));
        std::cout << vectorAngle << std::endl;
        double sideAngle = (sideVector.first*vector2.first + sideVector.second*vector2.second) / (sqrt(sideVector.first*sideVector.first + sideVector.second*sideVector.second)*sqrt(vector2.first*vector2.first + vector2.second*vector2.second));
        std::cout << sideAngle << std::endl;
        if (/*vector1.second > 0 &&*/ vector2.second < 0 && sideAngle > 0 && vectorAngle < vectorAngleLimit && sideAngle < sideAngleLimit) {
            std::cout << "Right Loop" << std::endl;
            return true;
        }
    }
    return false;
}