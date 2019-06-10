#include "pathPlanning.h"
#include "declaration.h"
#include "track_processing.h"

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
            return result;
        } else {
            ff result(NAN, NAN);
            return result;
        }
    } else {
        ff result(NAN, NAN);
        return result;
    }
}

ff checkEdge(ff corner, ff vector1, ff vector2);