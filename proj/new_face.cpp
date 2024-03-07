#include"new_face.h"

new_face::new_face()
{

}

new_face::new_face(QPoint vv1, QPoint vv2, QPoint vv3, float depthh, bool colorr)
{
	v1 = vv1;
	v2 = vv2;
	v3 = vv3;
	depth = depthh;
	color = colorr;
}

new_face::new_face(QPoint vv1, QPoint vv2, QPoint vv3, float depthh, bool colorr, float zz1, float zz2, float zz3)
{
	v1 = vv1;
	v2 = vv2;
	v3 = vv3;
	depth = depthh;
	color = colorr;
	z1 = zz1;
	z2 = zz2;
	z3 = zz3;
}