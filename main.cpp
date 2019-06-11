#include <iostream>
#include <algorithm>
#include <functional>
#include <cstdint>
#include <cmath>
#include <fstream>
#include <string>
#include "GrayBMP.h"
#include "functions.h"
#include "declaration.h"
#include "imageProc.h"
#include "track_processing.h"
#include "pathPlanning.h"

using namespace std;

bool leftLoopMode = false;
bool rightLoopMode = false;
bool leftCornerExist = false;
bool rightCornerExist = false;
double turnDirection = 0;


int main(){
	ii checkPoint(IMAGE_HEIGHT - 2, IMAGE_WIDTH / 2);
	ii checkPoint1(IMAGE_HEIGHT - 2, IMAGE_WIDTH / 2);
	ii checkPoint2(3*IMAGE_HEIGHT / 5, IMAGE_WIDTH / 2);
	ii checkPoint3(IMAGE_HEIGHT / 2, IMAGE_WIDTH / 2);
	ff direction1(-0.5, 1.4);
	ff direction2(-0.1, 1);
	ff direction3(-0.3, 1.4);
	ii searchDirR(-2, 1);
	ii searchDirL(-2, -1);
	direction1 = TrackProcessing::normalizeVector(direction1);
	direction2 = TrackProcessing::normalizeVector(direction2);
	direction3 = TrackProcessing::normalizeVector(direction3);
	for (int i = 6; i < 220; ++i) {
		if (i == 58)
		leftLoopMode = rightLoopMode = false;
		if (i >= 112 && i <= 125)
			continue;
		std::cout << "No. " << to_string(i) << std::endl;
		string name("raw/img" + to_string(i) + ".bmp");
		GrayBMP input(name);
		Threshold(input);
		uint8_t img_arr[IMAGE_WIDTH * IMAGE_HEIGHT];
		bmp_to_arr(input, img_arr);
		CAM_BUFFER = img_arr;
		ii resultR1 = TrackProcessing::goUntilObstacle(checkPoint, direction1, input);
		ff subDirection1(direction1.first, -direction1.second);
		ii resultL1 = TrackProcessing::goUntilObstacle(checkPoint, subDirection1, input);
		ii resultR2 = TrackProcessing::goUntilObstacle(checkPoint2, direction2, input);
		ff subDirection2(direction2.first, -direction2.second);
		ii resultL2 = TrackProcessing::goUntilObstacle(checkPoint2, subDirection2, input);
		ii resultR3 = TrackProcessing::goUntilObstacle(checkPoint3, direction3, input);
		ff subDirection3(direction3.first, -direction3.second);
		ii resultL3 = TrackProcessing::goUntilObstacle(checkPoint3, subDirection3, input);
		ff resultR1f = TrackProcessing::getCoords(resultR1);
		ff resultL1f = TrackProcessing::getCoords(resultL1);
		ff resultR2f = TrackProcessing::getCoords(resultR2);
		ff resultL2f = TrackProcessing::getCoords(resultL2);
		ff resultR3f = TrackProcessing::getCoords(resultR3);
		ff resultL3f = TrackProcessing::getCoords(resultL3);
		makeSmallDot(input, resultL1.second, resultL1.first);
		makeSmallDot(input, resultL2.second, resultL2.first);
		makeSmallDot(input, resultL3.second, resultL3.first);
		makeSmallDot(input, resultR1.second, resultR1.first);
		makeSmallDot(input, resultR2.second, resultR2.first);
		makeSmallDot(input, resultR3.second, resultR3.first);
		TrackProcessing::viic leftCornerDetect = TrackProcessing::goUntilCorner_LE(input, resultL1,searchDirL, 100);
		TrackProcessing::viic rightCornerDetect = TrackProcessing::goUntilCorner_RE(input, resultR1,searchDirR, 100);
		ii leftCorner(0, 0);
		ii rightCorner(0, 0);
		if (TrackProcessing::isActuallyCorner(leftCornerDetect.second)) {
			leftCorner.first = leftCornerDetect.first[leftCornerDetect.first.size()-1-TrackProcessing::CORNER_INTERVAL].first;
			leftCorner.second = leftCornerDetect.first[leftCornerDetect.first.size()-1-TrackProcessing::CORNER_INTERVAL].second;
			leftCornerExist = true;
		}
		if (TrackProcessing::isActuallyCorner(rightCornerDetect.second)) {
			rightCorner.first = rightCornerDetect.first[rightCornerDetect.first.size()-1-TrackProcessing::CORNER_INTERVAL].first;
			rightCorner.second = rightCornerDetect.first[rightCornerDetect.first.size()-1-TrackProcessing::CORNER_INTERVAL].second;
			rightCornerExist = true;
		}
		ff leftVector1 = getDirectionVector(resultL3, resultL2);
		ff leftVector2 = getDirectionVector(resultL2, resultL1);
		ff rightVector1 = getDirectionVector(resultR3, resultR2);
		ff rightVector2 = getDirectionVector(resultR2, resultR1);
		// cout << resultL3 << " " << resultR3 << endl
		// 	<< resultL2 << " " << resultR2 << endl
		// 	<< resultL1 << " " << resultR1 << endl << endl;
		// cout << resultL3f << " " << resultR3f << endl
		// 	<< resultL2f << " " << resultR2f << endl
		// 	<< resultL1f << " " << resultR1f << endl << endl;
		// cout << leftVector1 << " " << leftVector2 << endl
		// 	<< rightVector1 << " " << rightVector2 << endl;
		ff turn(0,0);
		ff leftEdgeVector = checkEdge(resultL3, resultL2, resultL1, leftCorner);
		ff rightEdgeVector = checkEdge(resultR3, resultR2, resultR1, rightCorner);
		if(leftLoopCheck(rightEdgeVector, leftVector1, leftVector2)) turnDirection  = -50, leftLoopMode = true;
		if(rightLoopCheck(leftEdgeVector, rightVector1, rightVector2)) turnDirection = -50, leftLoopMode = true;
		if (!leftLoopMode && !rightLoopMode) {
			if (!isnan(leftEdgeVector.first) && !isnan(rightEdgeVector.first)) {
				turn.first = TrackProcessing::normalizeVector(leftEdgeVector + rightEdgeVector).first;
				turn.second = TrackProcessing::normalizeVector(leftEdgeVector + rightEdgeVector).second;
				turn.first = -turn.first;
				turn.second = -turn.second;
				turnDirection = 100*turn.second / sqrt(turn.first*turn.first + turn.second*turn.second);
			} else if (!isnan(leftEdgeVector.first) && isnan(rightEdgeVector.first)) {
				turn.first = TrackProcessing::normalizeVector(leftEdgeVector).first;
				turn.second = TrackProcessing::normalizeVector(leftEdgeVector).second;
				turn.first = -turn.first;
				turn.second = -turn.second;
				turnDirection = 100*turn.second / sqrt(turn.first*turn.first + turn.second*turn.second);
			} else if (isnan(leftEdgeVector.first) && !isnan(rightEdgeVector.first)) {
				turn.first = TrackProcessing::normalizeVector(rightEdgeVector).first;
				turn.second = TrackProcessing::normalizeVector(rightEdgeVector).second;
				turn.first = -turn.first;
				turn.second = -turn.second;
				turnDirection = 100*turn.second / sqrt(turn.first*turn.first + turn.second*turn.second);
			} else 
				turnDirection = 0;
		} else if (!leftLoopMode && rightLoopMode) {
			if (isnan(leftEdgeVector.first) && isnan(rightEdgeVector.first)) {
				turnDirection = 60;
			} else if (!isnan(rightEdgeVector.first)) {
				turn.first = TrackProcessing::normalizeVector(rightEdgeVector).first;
				turn.second = TrackProcessing::normalizeVector(rightEdgeVector).second;
				turn.first = -turn.first;
				turn.second = -turn.second;
				turnDirection = 100*turn.second / sqrt(turn.first*turn.first + turn.second*turn.second);
			} else if (!isnan(leftEdgeVector.first) && isnan(rightEdgeVector.first)) {
				turn.first = TrackProcessing::normalizeVector(rightEdgeVector).first;
				turn.second = TrackProcessing::normalizeVector(rightEdgeVector).second;
				turn.first = -turn.first;
				turn.second = -turn.second;
				turnDirection = 100*turn.second / sqrt(turn.first*turn.first + turn.second*turn.second);
				if (turnDirection > -50)
				turnDirection = -50;
			}
		}
		
		//cout << leftEdgeVector << "|" << rightEdgeVector << endl;
		// if(leftLoopCheck(rightEdgeVector, leftVector1, leftVector2)) turnDirection  = -50;
		// if(rightLoopCheck(leftEdgeVector, rightVector1, rightVector2)) turnDirection = 50;
		cout << turnDirection << endl;
		GrayBMP pers(PERS_X, PERS_Y);
		bmp_to_arr(input, img_arr);
		perspectiveBMP(pers, img_arr);
		//bmp_to_arr(original, img_arr);
		for (int i = 0; i < IMAGE_HEIGHT; ++i) {
			for (int j = 0; j < IMAGE_WIDTH; ++j) {
				pers.img[i*PERS_X + j] = img_arr[i*IMAGE_WIDTH + j];
			}
		}
		pers.save(string("out/out" + to_string(i) + ".bmp"));
		leftCornerExist = rightCornerExist = false;
		//std::cout << "Done" << std::endl;
	}

    return 0;
}
