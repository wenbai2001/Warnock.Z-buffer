#pragma once
#include"new_face.h"
#include<qobject.h>

class classifier_polygon
{
public:
	classifier_polygon();
	classifier_polygon(std::vector<new_face> f);    //传入一堆面片，构建各种表

	struct polygons {
		int id;						//id
		bool color;					//1代表
		float a, b, c, d;			//平面方程的四个参数
		int num_span;				//跨越的扫描线数目
		int Ymax;
	};

	struct lines {
		float x;					//上端点x坐标
		float dx;					//每步长增加的坐标差
		int num_span;				//跨越的扫描线数目
		int id;						//所属面的id
		int Ymax;
		float z;					//上端点深度值
	};
	std::map<int, std::vector<polygons>> polygons_record;		//根据Ymax查找多边形
	std::map<int, std::vector<lines>> line_record;				//根据Ymax查找边
	std::map<int, polygons> find_polygon;						//根据id查找面片
	std::map<int, std::vector<lines>> find_edge;				//根据id查找边

	float find_max(float x1, float x2, float x3);
	float find_min(float x1, float x2, float x3);
};