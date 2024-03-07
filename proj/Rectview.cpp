#include"Rectview.h"

Rectview::Rectview(int l, int r, int b, int t)
{
	left = l;
	right = r;
	bottom = b;
	top = t;
	center.x = (left + right) / 2;
	center.y = (bottom + top) / 2;
	width = r - l;
	height = t - b;
} 

bool Rectview::contains(new_face f)   
{
	float x1 = f.v1.x();
	float y1 = f.v1.y();
	float x2 = f.v2.x();
	float y2 = f.v2.y();
	float x3 = f.v3.x();
	float y3 = f.v3.y();
	float Xmin = (x1 < x2&& x1 < x3) ? x1 : (x2 < x3 ? x2 : x3);
	float Xmax = (x1 > x2 && x1 > x3) ? x1 : (x2 > x3 ? x2 : x3);
	float Ymin = (y1 < y2&& y1 < y3) ? y1 : (y2 < y3 ? y2 : y3);
	float Ymax = (y1 > y2 && y1 > y3) ? y1 : (y2 > y3 ? y2 : y3);
	if (Xmin >= left && Xmax <= right && Ymin >= bottom && Ymax <= top) return true;
	else return false;
}