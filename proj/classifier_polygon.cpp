#include"classifier_polygon.h"

classifier_polygon::classifier_polygon()
{

}

classifier_polygon::classifier_polygon(std::vector<new_face> f)
{
	for (int i = 0; i < f.size(); i++)
	{
		//构建分类多边形表
		float x1 = f[i].v1.x();
		float y1 = f[i].v1.y();
		float z1 = f[i].z1;
		float x2 = f[i].v2.x();
		float y2 = f[i].v2.y();
		float z2 = f[i].z2;
		float x3 = f[i].v3.x();
		float y3 = f[i].v3.y();
		float z3 = f[i].z3;

		//std::cout << "三个点：" << x1 << " " << y1 << " " << z1 << " " << x2 << " " << y2 << " " << z2 << " " << x3 << " " << y3 << " " << z3 << std::endl;;

		float vx1 = x2 - x1; // 向量v1的x分量
		float vy1 = y2 - y1; // 向量v1的y分量

		float vx2 = x3 - x1; // 向量v2的x分量
		float vy2 = y3 - y1; // 向量v2的y分量

		// 计算叉积
		float crossProduct = vx1 * vy2 - vy1 * vx2;


		float epsilon = 1e-6; // 可以根据需要调整这个值
		if (fabs(crossProduct) < epsilon) {
			continue;
		}

		polygons p;

		float ux = x2 - x1, uy = y2 - y1, uz = z2 - z1; 
		float vx = x3 - x1, vy = y3 - y1, vz = z3 - z1; 
		p.a = uy * vz - uz * vy;
		p.b = uz * vx - ux * vz;
		p.c = ux * vy - uy * vx;
		p.d = -(p.a * x1 + p.b * y1 + p.c * z1);

		p.id = i + 1;
		p.color = f[i].color;
		p.Ymax = int(find_max(y1, y2, y3));
		p.num_span = p.Ymax - int(find_min(y1, y2, y3)) + 1;
		polygons_record[p.Ymax].push_back(p);
		find_polygon[p.id] = p;

		//构建分类边表
		lines l;
		l.x = y1 > y2 ? x1 : x2;		
		l.dx = (x2 - x1) / (y1 - y2);	
		l.Ymax = int(std::max(y1, y2));
		l.num_span = l.Ymax - int(std::min(y1, y2)) + 1;
		l.id = p.id;
		l.z = y1 > y2 ? z1 : z2;
		//std::cout << "第一条边" << l.x << " " << l.dx << " " << l.Ymax << " " << l.num_span << std::endl;

		if (y1 != y2)
		{
			line_record[l.Ymax].push_back(l);
			find_edge[l.id].push_back(l);
		}

		l.x = y1 > y3 ? x1 : x3;
		l.dx = (x3 - x1) / (y1 - y3);
		l.Ymax = int(std::max(y1, y3));
		l.num_span = l.Ymax - int(std::min(y1, y3)) + 1;
		l.id = p.id;
		l.z = y1 > y3 ? z1 : z3;
		//std::cout << "第二条边" << l.x << " " << l.dx << " " << l.Ymax << " " << l.num_span << std::endl;

		if (y1 != y3)
		{
			line_record[l.Ymax].push_back(l);
			find_edge[l.id].push_back(l);
		}

		l.x = y3 > y2 ? x3 : x2;
		l.dx = (x2 - x3) / (y3 - y2);
		l.Ymax = int(std::max(y3, y2));
		l.num_span = l.Ymax - int(std::min(y3, y2)) + 1;
		l.id = p.id;
		l.z = y3 > y2 ? z3 : z2;
		//std::cout << "第三条边" << l.x << " " << l.dx << " " << l.Ymax << " " << l.num_span << std::endl;

		if (y3 != y2)
		{
			line_record[l.Ymax].push_back(l);
			find_edge[l.id].push_back(l);
		}
	}
}

float classifier_polygon::find_max(float x1, float x2, float x3)
{
	return (x1 > x2 && x1 > x3) ? x1 : (x2 > x3 ? x2 : x3);
}

float classifier_polygon::find_min(float x1, float x2, float x3)
{
	return (x1 < x2 && x1 < x3) ? x1 : (x2 < x3 ? x2 : x3);
}