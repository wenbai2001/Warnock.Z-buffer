#pragma once
#include"new_face.h"
#include<qobject.h>

class classifier_polygon
{
public:
	classifier_polygon();
	classifier_polygon(std::vector<new_face> f);    //����һ����Ƭ���������ֱ�

	struct polygons {
		int id;						//id
		bool color;					//1����
		float a, b, c, d;			//ƽ�淽�̵��ĸ�����
		int num_span;				//��Խ��ɨ������Ŀ
		int Ymax;
	};

	struct lines {
		float x;					//�϶˵�x����
		float dx;					//ÿ�������ӵ������
		int num_span;				//��Խ��ɨ������Ŀ
		int id;						//�������id
		int Ymax;
		float z;					//�϶˵����ֵ
	};
	std::map<int, std::vector<polygons>> polygons_record;		//����Ymax���Ҷ����
	std::map<int, std::vector<lines>> line_record;				//����Ymax���ұ�
	std::map<int, polygons> find_polygon;						//����id������Ƭ
	std::map<int, std::vector<lines>> find_edge;				//����id���ұ�

	float find_max(float x1, float x2, float x3);
	float find_min(float x1, float x2, float x3);
};