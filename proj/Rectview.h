#pragma once
#include "face.h"
#include "new_face.h"
class Rectview
{
public:
	Rectview(int l, int r, int b, int t);		//模拟QRect类，设置正确的坐标范围
	int left;
	int right;
	int bottom;
	int top;
	int width;
	int height;
	struct center_point {
		int x;
		int y;
	};
	bool contains(new_face f);			//视口是否包含面
	center_point center;				
};