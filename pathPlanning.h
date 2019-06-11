#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <string>
#include <cstdarg>

const int VALID_TOPMOST = 40;
const int VALID_BOTTOMMOST = 240;
const int VALID_LEFTMOST = 5;
const int VALID_RIGHTMOST = 371;

const double vectoerPercentage = 0.3;
const double vectorAngleLimit = 0.998;
const double sideAngleLimit = 0.5;

typedef enum patterns {
    bothSide,
    oneSide,

} patterns;

typedef std::pair<int, int> ii;
typedef std::pair<float, float> ff;
typedef std::vector<ii> vii;

bool coorValid(ii coor);
ff getDirectionVector(ii coor1, ii coor2);
ff checkEdge(ii coor1, ii coor2, ii coor3, ii point);
bool leftLoopCheck(ff sideVector, ff vector1, ff vector2);
bool rightLoopCheck(ff sideVector, ff vector1, ff vector2);
