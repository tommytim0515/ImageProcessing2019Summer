#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <string>
#include <cstdarg>
#include "track_processing.h"
#include "declaration.h"
#include "imageProc.h"
#include "GrayBMP.h"
#include "pathPlanning.h"

const uint8_t *CAM_BUFFER = nullptr;

using namespace std;

namespace TrackProcessing {

	ii getCcw(ii a) { return mp(-a.second, a.first); }
	ii getCw(ii a) { return mp(a.second, -a.first); }
	ii goUntilObstacle(ii start, ff direction, GrayBMP &bmp) {
		direction = normalizeVector(direction);
		if (std::isnan(direction.first) || std::isnan(direction.second)) std::cout << "NaN direction";
		float curY = start.first;
		float curX = start.second;
	    while ( ! Image::isEdge(CAM_BUFFER, curY + direction.first, curX + direction.second)) /*makeSmallDot(bmp, curX, curY),*/ curY += direction.first, curX += direction.second; 
	    return mp((int)curY, (int)curX);
	}
	ii goUntilObstacle(ii start, ii direction, GrayBMP &bmp) {
		return goUntilObstacle(start, mp((float)direction.first, (float)direction.second), bmp);
	}
	bool onImageBorder(ii x) {
		return x.first == 0 || x.first == Image::HEIGHT - 1 || x.second == 0 || x.second == Image::WIDTH - 1;
	}
	float normalizeAngle(float angle) {
		angle = fmod(angle, 360.0f);
		if (angle < -180.0f) return angle + 360.0f;
		else if (angle > 180.0f) return angle - 360.0f;
		else return angle;
	}
	ff normalizeVector(ff vec) {
	    float mag = sqrt(vec.first * vec.first + vec.second * vec.second);
		return vec / mag;
	}
	ff getCoords(ii yx) {
	    return mp(remapping[yx.second][yx.first][1], remapping[yx.second][yx.first][0]);// / 10.0f + mp(3.0f, 0.0f); // 3 cm y-forward constant offset
	}
	Corner getCornerType(ii a, ii b, ii c) {
	    ff fa = getCoords(a); fa.first *= -1;
	    ff fb = getCoords(b); fb.first *= -1;
	    ff fc = getCoords(c); fc.first *= -1;
		ff ab = fb - fa, bc = fc - fb;//, ac = fc - fa;

// int a2 = ab.first * ab.first + ab.second * ab.second;
//     int b2 = bc.first * bc.first + bc.second * bc.second;
//     int c2 = ac.first * ac.first + ac.second * ac.second;
//     uint64_t p = (uint64_t)(std::abs(a2 + b2 - c2));
// 	if (p>0) return Corner::R;
//     float value = ((((float) (p * p) / 4) / a2) / b2);
//     if( value < 0.2) return Corner::R;
//     else return Corner::NONE;

		float dot = ab.second * bc.second + ab.first * bc.first;
		float det = ab.second * bc.first - ab.first * bc.second;
		float angleDiff = 180 / PI * atan2(det, dot);
		//cerr << angleDiff << endl;
	    if (std::isnan(angleDiff)) return Corner::INVALID; // NaN can happen due to unmappable coords
		if (abs(angleDiff) < 75.0f) return Corner::NONE; // too smooth to be a corner
	
		float angleFirst = 180 / PI * atan2(ab.first, ab.second);
		float angleMidRotation = normalizeAngle(angleFirst + angleDiff / 2.0f);
	
		bool ccw = (ab.first * bc.second - ab.second * bc.first > 0); // whether counterclockwise
		if (angleMidRotation >= 0 && angleMidRotation < 90) return ccw ? Corner::L : Corner::BR;
		else if (angleMidRotation >= 90) return ccw ? Corner::R : Corner::BL;
		else if (angleMidRotation < 0 && angleMidRotation >= -90) return ccw ? Corner::BL : Corner::R;
		else if (angleMidRotation < -90) return ccw ? Corner::BR : Corner::L;
		else std::cout << "NaN direction";
		return Corner::INVALID;
		// i hope i didnt make any error here lol
	}
	bool isActuallyCorner(Corner c) {
		return (int)c >= 1 && (int)c <= 4;
	}
	viic goUntilCorner_LE(GrayBMP &bmp, ii current, ii direction, int limit) {
		vii ret;
		while (current.first > HEIGHT_LIMIT && limit--) {
			if (!coorValid(current))
				break;
			//makeDot(bmp, current.first, current.second);
			ret.push_back(current);
	
			if (ret.size() > 2*CORNER_INTERVAL) {
				Corner c = getCornerType(ret[ret.size()-1-2*CORNER_INTERVAL], ret[ret.size()-1-CORNER_INTERVAL], ret[ret.size()-1]);
				
				if (c != Corner::NONE) {
					std::cout << ret[ret.size()-1-CORNER_INTERVAL].second << " " << ret[ret.size()-1-CORNER_INTERVAL].first << endl;
					if (isActuallyCorner(c)) {
						int CoorX = ret[ret.size()-1-CORNER_INTERVAL].second;
						int CoorY = ret[ret.size()-1-CORNER_INTERVAL].first;
						if (CoorX >= 10 && CoorY >= 10 && CoorX < (IMAGE_WIDTH - 10) && CoorY < (IMAGE_HEIGHT - 10)) {
							//makeBigDot(bmp, CoorX, CoorY);
							return mp(std::move(ret), c);
						}
					}
				}
			}
		
			ii frontCell = current + direction;
			ii leftCell = current + getCcw(direction);
			if ( ! Image::isEdge(CAM_BUFFER, leftCell)) {
				direction = getCcw(direction);
				frontCell = current + direction;
				leftCell = current + getCcw(direction);
			}
			if (Image::isEdge(CAM_BUFFER, frontCell)) {
				direction = getCw(direction);
				frontCell = current + direction;
				leftCell = current + getCcw(direction);
			}
			if (Image::isEdge(CAM_BUFFER, frontCell)) {
				direction = getCw(direction);
				frontCell = current + direction;
				leftCell = current + getCcw(direction);
			}
			current += direction;
		}
		return mp(std::move(ret), Corner::NONE);
	}
	viic goUntilCorner_RE(GrayBMP &bmp, ii current, ii direction, int limit) {
		vii ret;
		while (current.first > HEIGHT_LIMIT && limit--) {
			if (!coorValid(current))
				break;
			//makeDot(bmp, current.first, current.second);
			ret.push_back(current);
		
			if (ret.size() > 2*CORNER_INTERVAL) {
				Corner c = getCornerType(ret[ret.size()-1-2*CORNER_INTERVAL], ret[ret.size()-1-CORNER_INTERVAL], ret[ret.size()-1]);
				if (c != Corner::NONE) {
					std::cout << ret[ret.size()-1-CORNER_INTERVAL].second << " " << ret[ret.size()-1-CORNER_INTERVAL].first << endl;
					if (isActuallyCorner(c)) {
						int CoorX = ret[ret.size()-1-CORNER_INTERVAL].second;
						int CoorY = ret[ret.size()-1-CORNER_INTERVAL].first;
						if (CoorX >= 10 && CoorY >= 10 && CoorX < (IMAGE_WIDTH - 10) && CoorY < (IMAGE_HEIGHT - 10)) {
							//makeBigDot(bmp, CoorX, CoorY);
							return mp(std::move(ret), c);
						}
					}
				}
			}
		
			ii frontCell = current + direction;
			ii rightCell = current + getCw(direction);

			if ( ! Image::isEdge(CAM_BUFFER, rightCell)) {
				direction = getCw(direction);
				frontCell = current + direction;
				rightCell = current + getCw(direction);
			}
			if (Image::isEdge(CAM_BUFFER, frontCell)) {
				direction = getCcw(direction);
				frontCell = current + direction;
				rightCell = current + getCw(direction);
			}
			if (Image::isEdge(CAM_BUFFER, frontCell)) {
				direction = getCcw(direction);
				frontCell = current + direction;
				rightCell = current + getCw(direction);
			}
			//makeDot(bmp, current.second, current.first);
			current += direction;
		}
		return mp(std::move(ret), Corner::NONE);
	}
	bool checkZebraLine() {
		const int Y = Image::HEIGHT * 5 / 8;
		const int startX = Image::WIDTH * 1 / 8;
		const int endX = Image::WIDTH * 7 / 8;
		const int scanjump = 5;
		int cnt = 0;
		for (int i = startX + scanjump; i < endX; i += scanjump)
			cnt += (Image::isEdge(CAM_BUFFER, Y, i-scanjump)) != (Image::isEdge(CAM_BUFFER, Y, i));
		return cnt >= 5;
	}
	const int CAR_WIDTH = 16;
	inline int cross(const ii &a, const ii &b) { return a.first * b.second - a.second * b.first; }
	ff amrutSelector(vff midline) {
		for (auto it = midline.end()-1; it != midline.begin(); it--) {
			auto compute = [&](const ii &a){ return abs(cross(a, *it)); };
			auto compare = [&](const ii &a, const ii &b){ return compute(a) < compute(b); };
			// std hypot impl is slow //float maxValue = compute(*std::max_element(midline.begin(), it, compare)) / std::hypot(it->first, it->second);
			float maxValue = compute(*std::max_element(midline.begin(), it, compare)) / sqrt(it->first * it->first + it->second * it->second);
			if (maxValue < CAR_WIDTH/2.0f) return *it;
		}
		return ii(1, 0);
	}
	
}
