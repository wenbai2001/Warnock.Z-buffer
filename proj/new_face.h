#pragma once
#include "face.h"
#include<qobject.h>
class new_face
{
public:
	new_face();
	new_face(QPoint vv1, QPoint vv2, QPoint vv3, float depthh, bool colorr);
	new_face(QPoint vv1, QPoint vv2, QPoint vv3, float depthh, bool colorr, float zz1, float zz2, float zz3);
	QPoint v1;
	QPoint v2;
	QPoint v3;
	bool color;
	float depth;
	float z1;  
	float z2;
	float z3;
};