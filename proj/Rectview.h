#pragma once
#include "face.h"
#include "new_face.h"
class Rectview
{
public:
	Rectview(int l, int r, int b, int t);		//ģ��QRect�࣬������ȷ�����귶Χ
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
	bool contains(new_face f);			//�ӿ��Ƿ������
	center_point center;				
};