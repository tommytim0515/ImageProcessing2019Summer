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

//using namespace std;

int main(){
	ii checkPoint(IMAGE_HEIGHT - 2, IMAGE_WIDTH / 2);
	ii checkPoint1(IMAGE_HEIGHT - 2, IMAGE_WIDTH / 2);
	ii checkPoint2(3*IMAGE_HEIGHT / 5, IMAGE_WIDTH / 2);
	ii checkPoint3(IMAGE_HEIGHT / 2, IMAGE_WIDTH / 2);
	ff direction1(-0.4, 1.5);
	ff direction2(-0.1, 1);
	ff direction3(-0.3, 1.4);
	ii searchDirR(-2, 1);
	ii searchDirL(-2, -1);
	direction1 = TrackProcessing::normalizeVector(direction1);
	direction2 = TrackProcessing::normalizeVector(direction2);
	direction3 = TrackProcessing::normalizeVector(direction3);
	for (int i = 5; i < 220; ++i) {
		std::cout << to_string(i) << std::endl;
		string name("raw/img" + to_string(i) + ".bmp");
		GrayBMP input(name);
		//medianfilter(input, 2);
		Threshold(input);
		//sobel(input);
		//medianfilter(input, 2);
		//Threshold(input);
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
		// std::cout << resultL3.second << " " << resultL3.first << std::endl;
		// std::cout << resultR3.second << " " << resultR3.first << std::endl;
		// std::cout << resultL2.second << " " << resultL2.first << std::endl;
		// std::cout << resultR2.second << " " << resultR2.first << std::endl;
		// std::cout << resultL1.second << " " << resultL1.first << std::endl;
		// std::cout << resultR1.second << " " << resultR1.first << std::endl;
		ff resultR1f = TrackProcessing::getCoords(resultR1);
		ff resultL1f = TrackProcessing::getCoords(resultL1);
		ff resultR2f = TrackProcessing::getCoords(resultR2);
		ff resultL2f = TrackProcessing::getCoords(resultL2);
		ff resultR3f = TrackProcessing::getCoords(resultR3);
		ff resultL3f = TrackProcessing::getCoords(resultL3);
		TrackProcessing::goUntilCorner_LE(input, resultL1,searchDirL, 100);
		TrackProcessing::goUntilCorner_RE(input, resultR1,searchDirR, 100);
		TrackProcessing::goUntilCorner_LE(input, resultL3,searchDirL, 100);
		TrackProcessing::goUntilCorner_RE(input, resultR3,searchDirR, 100);
		GrayBMP pers(PERS_X, PERS_Y);
		bmp_to_arr(input, img_arr);
		perspectiveBMP(pers, img_arr);
		//bmp_to_arr(original, img_arr);
		for (int i = 0; i < IMAGE_HEIGHT; ++i) {
			for (int j = 0; j < IMAGE_WIDTH; ++j) {
				pers.img[i*PERS_X + j] = img_arr[i*IMAGE_WIDTH + j];
			}
		}
		// for (int i = 0; i < IMAGE_WIDTH; ++i) {
		// 	for (int j = 0; j < IMAGE_HEIGHT; ++j) {
		// 		if (!std::isnan(remapping[i][j][0])) {
		// 			pers.img[remapping[i][j][0] * PERS_X +  remapping[i][j][1]] = input.img[i * IMAGE_WIDTH + j];
		// 		}
		// 	}
		// }
		//sobel(input);
		//CornerDetect1(input, 0.6);
		pers.save(string("out/out" + to_string(i) + ".bmp"));
		//std::cout << "Done" << std::endl;
	}

    return 0;
}
