#pragma once

#include "image.h"
#include "GrayBMP.h"
#include <vector>

using namespace std;

namespace TrackProcessing {

	typedef std::pair<int, int> ii;
	typedef std::pair<float, float> ff;
	typedef std::vector<ii> vii;

	// In this program coordinates are stored as (Y,X)
	// We generally consider downward to be positive Y direction

	// The ones that look like r, backwards r (7), l, backwards l (J)
	enum class Corner {
		INVALID,
		R,
		BR, // could also be 7, but we can't just call it that because it's a number and c++
		L,
		BL, // could also be J if not for symmetry
		NONE
	};

	// A compound composed of an edge list and a corner (corner can be none)
	typedef pair<vii, Corner> viic;

	// The pixel index difference (in edgelist) of points to check if corner
	const int CORNER_INTERVAL = 20;

	// Useful data pixel limit (only pixels from this point downward will be used in calculations)
	const int HEIGHT_LIMIT = Image::HEIGHT * 40 / 100;

	// Get 90° counterclockwise rotated vector
	ii getCcw(ii a);
	// Get 90° clockwise rotated vector
	ii getCw(ii a);

	// Linear search, returns last point before black (or starting point if it cant even move)
	ii goUntilObstacle(ii start, ff direction, GrayBMP &bmp);
	ii goUntilObstacle(ii start, ii direction, GrayBMP &bmp);

	// Checks if a coordinate is touching image boundaries
	bool onImageBorder(ii x);

	// Returns equivalent angle in [-180;180]
	float normalizeAngle(float angle);
	// Returns unit vector in same direction
	ff normalizeVector(ff vec);

	// Returns real world coordinates of a pixel, warning: values can be NaN
	ff getCoords(ii yx);

	// remark: input pairs are (y,x) where y is according to standard image axes, NOT math axes (plural of axis TIL, lol)
	// atan2(dy, dx) returns angle between -180 and 180 according to standard math axes
	Corner getCornerType(ii a, ii b, ii c);

	// Check if corner value represents an actual corner
	bool isActuallyCorner(Corner c);

	// follows the edge with an optional move limit LEFT EDGE
	viic goUntilCorner_LE(GrayBMP &bmp, ii current, ii direction, int limit = 250);

	// follows the edge with an optional move limit RIGHT EDGE
	viic goUntilCorner_RE(GrayBMP &bmp, ii current, ii direction, int limit = 250);

	// Checks if there is a zebra line
	bool checkZebraLine();

	// Selects optimal* point from midline
	ii amrutSelector(vii midline);

}
