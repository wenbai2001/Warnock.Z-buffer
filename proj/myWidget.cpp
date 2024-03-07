#include "myWidget.h"
#include<cmath>
#include<qmessagebox.h>
#include"classifier_polygon.h"

myWidget::myWidget(QWidget* parent) : QGLWidget(parent)
{

}

void myWidget::initializeGL() {
    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);      //取消OpenGL自带的Z-buffer消隐
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);

    glClearColor(1, 1, 1, 1);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    radius = 0;
    w0 = this->width();
    h0 = this->height();
    trans_matrix.setAspect(w0, h0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
}

void myWidget::resizeGL(int w, int h) {

}

void myWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    /* If a File is Loaded*/
    if (objPtr && !warnock_flag && !zbuffer_flag)
    {
        drawObject();
        glFlush();
        this->makeCurrent();
    }
    if (warnock_flag)
    {
        int viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        Rectview q(viewport[0], viewport[0] + viewport[2], viewport[1], viewport[1] + viewport[3]);  //l r b t
        //std::cout << "视口范围：" << viewport[0] << " " << viewport[1] << " " << viewport[2] << " " << viewport[3] << "\n";
        //std::cout << "视口左右底顶：" << q.left << " " << q.right << " " << q.bottom << " " << q.top << "\n";
        warnock(q, trans_faces);
        warnock_flag = 0;
    }
    if (zbuffer_flag)
    {
        zbuffer();
        zbuffer_flag = 0;
    }
}

void myWidget::drawObject()
{
    if (objPtr) {
        glBegin(GL_TRIANGLES);
        face f;
        std::vector<float> normal;
        Vertex v1, v2, v3;
        bool flag = 0;
        for (unsigned i = 0; i < faces.size(); i++)
        {
            float r = 0.0, g = 0.0, b = 0.0;
         
            f = faces.at(i);
            v1 = f.getVertex(1);
            v2 = f.getVertex(2);
            v3 = f.getVertex(3);
            normal = f.getNormal();

            if (flag)  r = 1.0;
            else b = 1.0;
            faces[i].color = flag;
            flag = !flag;
            glNormal3f(normal.at(0), normal.at(1), normal.at(2));
            glColor3f(r, g, b);
            glVertex3f(v1.X(), v1.Y(), v1.Z());
            glColor3f(r, g, b);
            glVertex3f(v2.X(), v2.Y(), v2.Z());
            glColor3f(r, g, b);
            glVertex3f(v3.X(), v3.Y(), v3.Z());
        }
    }
    glEnd();
}

void myWidget::grabObj(model m) {
    objPtr = &m;
    vertices = objPtr->getVertices();
    faces = objPtr->getFacets();
    center = objPtr->findCenter();
    radius = objPtr->findRadius();
    maxCoords = objPtr->getMaxCoords();
    minCoords = objPtr->getMinCoords();

    //设置扫描线zbuffer算法用的正交投影矩阵
    trans_matrix.model_trans_zbuffer(objPtr, this->width(), this->height());
    //设置warnock算法用的透视投影矩阵
    //trans_matrix.model_trans_warnock(objPtr, this->width(), this->height());

}

//用于扫描线z-buffer算法中的将面片正交投影成二维面片形成新的多边形序列，同时保存深度信息和平面斜率等信息
//用于存储的数据结构：vector<new_face> trans_faces_zbuffer;
void myWidget::face_proj_ortho()
{
    std::vector<face> f = faces;
    QMatrix4x4 Mproj;
    Mproj.ortho(trans_matrix.getleft(), trans_matrix.getright(), trans_matrix.getbottom(), trans_matrix.gettop(), -trans_matrix.getnear(), -trans_matrix.getfar());
    for (int i = 0; i < faces.size(); i++)
    {
        //拓展维度
        QVector4D v1(faces[i].getVertex(1).X(), faces[i].getVertex(1).Y(), faces[i].getVertex(1).Z(), 1.0);
        QVector4D v2(faces[i].getVertex(2).X(), faces[i].getVertex(2).Y(), faces[i].getVertex(2).Z(), 1.0);
        QVector4D v3(faces[i].getVertex(3).X(), faces[i].getVertex(3).Y(), faces[i].getVertex(3).Z(), 1.0);
        //投影变换
        std::vector<QVector4D> trans_v(3);
        trans_v[0] = Mproj * v1;
        trans_v[1] = Mproj * v2;
        trans_v[2] = Mproj * v3;
        //视口变换
        std::vector<QPoint> vv(3);
        for (int j = 0; j < 3; j++)
        {
            if (trans_v[j].w() != 0.0)
            {
                QVector3D ndc = QVector3D(trans_v[j].x(), trans_v[j].y(), trans_v[j].z()) / trans_v[j].w();
                int x = (ndc.x() * 0.5 + 0.5) * this->width();
                int y = (ndc.y() * 0.5 + 0.5) * this->height();
                vv[j] = QPoint(x, y);
            }
        }

        float z1 = f[i].getVertex(1).Z();
        float z2 = f[i].getVertex(2).Z();
        float z3 = f[i].getVertex(3).Z();

        new_face this_face(vv[0], vv[1], vv[2], faces[i].depth, faces[i].color, z1, z2, z3);
        trans_faces_zbuffer.push_back(this_face);
        trans_faces.push_back(this_face);
    }
    return;
}


//用于warnock算法中的将模型进行视图、投影、视口变换后转换为屏幕空间坐标中的二维面片序列
//用于存储的数据结构：vector<new_face> trans_faces
void myWidget::face_proj_perspective()
{
    //视图变换矩阵
    QMatrix4x4 Mview;           
    QVector3D center(-trans_matrix.getcenter(0), -trans_matrix.getcenter(1), -trans_matrix.getcenter(2) - trans_matrix.getfdist());
    Mview.translate(center);
    //投影矩阵
    QMatrix4x4 Mproj;
    Mproj.perspective(trans_matrix.getfov(), trans_matrix.getaspect(), trans_matrix.getnear(), trans_matrix.getfar());
    //对每个面片进行变换s
    for (int i = 0; i < faces.size(); i++)
    {
        std::vector<face> f = faces;
        //std::cout << "当前面片：\n" << faces[i].getVertex(1).X() << " " << faces[i].getVertex(1).Y()<<"\n" << faces[i].getVertex(2).X() << " " << faces[i].getVertex(2).Y() << "\n"<< faces[i].getVertex(3).X() << " " << faces[i].getVertex(3).Y() << "\n";
        //拓展维度
        QVector4D v1(faces[i].getVertex(1).X(), faces[i].getVertex(1).Y(), faces[i].getVertex(1).Z(), 1.0);
        QVector4D v2(faces[i].getVertex(2).X(), faces[i].getVertex(2).Y(), faces[i].getVertex(2).Z(), 1.0);
        QVector4D v3(faces[i].getVertex(3).X(), faces[i].getVertex(3).Y(), faces[i].getVertex(3).Z(), 1.0);
        //视图变换、投影变换
        std::vector<QVector4D> trans_v(3);
        trans_v[0] = Mproj * Mview * v1;
        trans_v[1] = Mproj * Mview * v2;
        trans_v[2] = Mproj * Mview * v3;
        //视口变换
        std::vector<QPoint> vv(3);
        for (int j = 0; j < 3; j++)
        {
            if (trans_v[j].w() != 0.0)
            {
                QVector3D ndc = QVector3D(trans_v[j].x(), trans_v[j].y(), trans_v[j].z()) / trans_v[j].w();
                int x = (ndc.x() * 0.5 + 0.5) * this->width();
                int y = (ndc.y() * 0.5 + 0.5) * this->height();
                vv[j] = QPoint(x, y);
            }
        }

        float z1 = f[i].getVertex(1).Z();
        float z2 = f[i].getVertex(2).Z();
        float z3 = f[i].getVertex(3).Z();
        new_face this_face(vv[0], vv[1], vv[2], faces[i].depth, faces[i].color, z1, z2, z3);
        //std::cout << "变换后面片：\n" << vv[0].x() << " " << vv[0].y() << "\n"<< vv[1].x() << " " << vv[1].y() << "\n"<< vv[2].x() << " " << vv[2].y() << "\n";
        trans_faces.push_back(this_face);
    }
    return;
}

//判断线段是否相交
bool line_intersect(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    double a, b, c, d;
    a = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    b = (x2 - x1) * (y4 - y1) - (y2 - y1) * (x4 - x1);//AB * AD
    c = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);//CD * CA
    d = (x4 - x3) * (y2 - y3) - (y4 - y3) * (x2 - x3);//CD * CB
    if (std::max(x1, x2) < std::min(x3, x4) || std::max(x3, x4) < std::min(x1, x2)
        || std::max(y1, y2) < std::min(y3, y4) || std::max(y3, y4) < std::min(y1, y2))
        return false;
    else if (a * b <= 0 && c * d <= 0)
        return true;
    else
        return false;
}

//判断面片是否与视口区域相交
bool myWidget::is_intersection(new_face f, Rectview region)
{
    QPoint v1 = f.v1;
    QPoint v2 = f.v2;
    QPoint v3 = f.v3;
    float xx1 = v1.x();
    float yy1 = v1.y();
    float xx2 = v2.x();
    float yy2 = v2.y();
    float xx3 = v3.x();
    float yy3 = v3.y();
    double x1 = region.left;
    double y1 = region.bottom;
    double x2 = region.right;
    double y2 = y1;
    double x3 = x1;
    double y3 = region.top;
    double x4 = x2;
    double y4 = y3;
    //遍历所有情况判断面片是否与视口相交
    if (line_intersect(v1.x(), v1.y(), v2.x(), v2.y(), x1, y1, x2, y2) ||  //v1-v2
        line_intersect(v1.x(), v1.y(), v2.x(), v2.y(), x3, y3, x2, y2) ||
        line_intersect(v1.x(), v1.y(), v2.x(), v2.y(), x1, y1, x4, y4) ||
        line_intersect(v1.x(), v1.y(), v2.x(), v2.y(), x3, y3, x4, y4) ||
        line_intersect(v3.x(), v3.y(), v2.x(), v2.y(), x1, y1, x2, y2) ||  //v2-v3
        line_intersect(v3.x(), v3.y(), v2.x(), v2.y(), x3, y3, x2, y2) ||
        line_intersect(v3.x(), v3.y(), v2.x(), v2.y(), x1, y1, x4, y4) ||
        line_intersect(v3.x(), v3.y(), v2.x(), v2.y(), x3, y3, x4, y4) ||
        line_intersect(v3.x(), v3.y(), v1.x(), v1.y(), x1, y1, x2, y2) ||  //v3-v1
        line_intersect(v3.x(), v3.y(), v1.x(), v1.y(), x3, y3, x2, y2) ||
        line_intersect(v3.x(), v3.y(), v1.x(), v1.y(), x1, y1, x4, y4) ||
        line_intersect(v3.x(), v3.y(), v1.x(), v1.y(), x3, y3, x4, y4))
        return true;
    float Xmin = (xx1 < xx2&& xx1 < xx3) ? xx1 : (xx2 < xx3 ? xx2 : xx3);
    float Xmax = (xx1 > xx2 && xx1 > xx3) ? xx1 : (xx2 > xx3 ? xx2 : xx3);
    float Ymin = (yy1 < yy2&& yy1 < yy3) ? yy1 : (yy2 < yy3 ? yy2 : yy3);
    float Ymax = (yy1 > yy2 && yy1 > yy3) ? yy1 : (yy2 > yy3 ? yy2 : yy3);
    //如果面片包含视口
    if (Xmin <= x1 && Xmax >= x2 && Ymin <= y1 && Ymax >= y3) return true;
    return false;
}

void myWidget::warnock(Rectview& viewport, std::vector<new_face> relevantFaces) {
    if (!relevantFaces.size()) return;
    int l = viewport.left;
    int r = viewport.right;
    int b= viewport.bottom;
    int t = viewport.top;

    //调试
    //std::cout << "视口：\n" << l << " " << r << " " << b << " " << t << "\n";
    
    
    //如果内部只有一个面片
    if (relevantFaces.size() == 1) {
        float x1, y1, x2, y2, x3, y3;
        x1 = relevantFaces[0].v1.x();
        y1 = relevantFaces[0].v1.y();
        x2 = relevantFaces[0].v2.x();
        y2 = relevantFaces[0].v2.y();
        x3 = relevantFaces[0].v3.x();
        y3 = relevantFaces[0].v3.y();
        //std::cout << "视口内部只有一个面片：\n" << x1 << " " << y1 << " " << x2 << " " << y2 << " " << x3 << " " << y3 << "\n";
        if (viewport.contains(relevantFaces[0]))
        {
            //std::cout << "面片完全在视口范围内：\n";
            //直接绘制这个二维面片relevantFaces[0]
            //glViewport(0, 0, this->width(), this->height());
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, this->width(), 0, this->height());

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glBegin(GL_TRIANGLES);
            if (relevantFaces[0].color) glColor3f(1.0, 0.0, 0.0);
            else glColor3f(0.0, 0.0, 1.0);
            glVertex2f(x1, y1);
            glVertex2f(x2, y2);
            glVertex2f(x3, y3);
            glEnd();
        }
    }
    else if (viewport.width <= 1 && viewport.height <= 1)  //视口已经小到一定程度
    {
        //std::cout << "视口太小了\n";
        new_face f = relevantFaces[0];
        float max_depth = f.depth;
        //找最近的面片的颜色画出
        for (int i = 0; i < relevantFaces.size(); i++)
        {
            if (relevantFaces[i].depth > max_depth)
            {
                f = relevantFaces[i];
                max_depth = relevantFaces[i].depth;
            }
        }
        if (f.color) 
        {
            glColor3f(1.0, 0.0, 0.0); 
            //std::cout << "画红色 "; 
        }
        else
        {
            glColor3f(0.0, 0.0, 1.0);
            //std::cout << "画蓝色 ";
        }
        
        // 绘制点
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0, this->width(), 0, this->height());

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glBegin(GL_POINTS);
        glVertex2f(l, t);
        glEnd();
        //std::cout << "画点 " << l << " " << t << "\n";
        
    }
    else {
        // 将视口划分为更小的视口
        int midX = (l + r) / 2;
        int midY = (t + b) / 2;

        Rectview topLeft(l, midX, midY, t);
        Rectview topRight(midX, r, midY, t);
        Rectview bottomLeft(l, midX, b, midY);
        Rectview bottomRight(midX, r, b, midY);

        // 为每个新区域创建新的面列表
        std::vector<new_face> facesTopLeft, facesTopRight, facesBottomLeft, facesBottomRight;

        foreach(new_face f, relevantFaces) {
            if (is_intersection(f, topLeft)||topLeft.contains(f)) {
                facesTopLeft.push_back(f);
            }
            if (is_intersection(f, topRight)||topRight.contains(f)) {
                facesTopRight.push_back(f);
            }
            if (is_intersection(f, bottomLeft)||bottomLeft.contains(f)) {
                facesBottomLeft.push_back(f);
            }
            if (is_intersection(f, bottomRight)||bottomRight.contains(f)) {
                facesBottomRight.push_back(f);
            }
        }

        // 递归对每个视口进行处理
        myWidget::warnock(topLeft, facesTopLeft);
        myWidget::warnock(topRight, facesTopRight);
        myWidget::warnock(bottomLeft, facesBottomLeft);
        myWidget::warnock(bottomRight, facesBottomRight);
    }
}

void myWidget::zbuffer()
{
    classifier_polygon p(trans_faces);  //这里存储了所有多边形信息
    int w = this->width();
    int h = this->height();
    active_polygon* head_polygon = NULL;
    active_polygon* tail_polygon = head_polygon;

    active_edge* head_edge = NULL;
    active_edge* tail_edge = head_edge;

    //从上向下扫描
    for (int scan = h; scan >= 0; scan--)
    {
        //每次有新的扫描线，刷新颜色缓冲和深度缓冲
        std::vector<int> color_buffer(this->width()+1, -1);
        std::vector<float> depth_buffer(this->width()+1, INT_MIN);

        //根据扫描线的值scan，查找以此为Ymax的多边形，将其加入活化多边形表中
        for (int i = 0; i < p.polygons_record[scan].size(); i++)
        {
            //当前多边形加入活化多边形表中
            classifier_polygon::polygons now_p = p.polygons_record[scan][i];
            active_polygon* poly_ptr = (active_polygon*)malloc(sizeof(active_polygon));
            poly_ptr->a = now_p.a;
            poly_ptr->b = now_p.b;
            poly_ptr->c = now_p.c;
            poly_ptr->d = now_p.d;
            poly_ptr->id = now_p.id;
            poly_ptr->dy = now_p.num_span;
            poly_ptr->color = now_p.color;
            poly_ptr->pre = NULL;
            poly_ptr->next = NULL;
            if (head_polygon)
            {
                tail_polygon->next = poly_ptr;
                poly_ptr->pre = tail_polygon;
                tail_polygon = poly_ptr;
            }
            else
            {
                head_polygon = poly_ptr;
                tail_polygon = poly_ptr;
            }
            //将对应的边也加入活化边表中
            classifier_polygon::lines line_l;
            classifier_polygon::lines line_r;
            for (int k = 0; k < p.find_edge[now_p.id].size(); k++)
            {
                if (p.find_edge[now_p.id][k].Ymax == scan) { line_l = p.find_edge[now_p.id][k]; break; }
            }
            for (int k = p.find_edge[now_p.id].size() - 1; k >= 0; k--)
            {
                if (p.find_edge[now_p.id][k].Ymax == scan) { line_r = p.find_edge[now_p.id][k]; break; }
            }
            if (line_l.dx + line_l.x > line_r.dx + line_r.x)          //走一步试试(可能需要走好几步)
            {
                classifier_polygon::lines mid;
                mid = line_l;
                line_l = line_r;
                line_r = mid;
            }
            active_edge* edge_ptr = (active_edge*)malloc(sizeof(active_edge));
            edge_ptr->xl = line_l.x;            //左右交点x坐标
            edge_ptr->xr = line_r.x;
            edge_ptr->dxl = line_l.dx;          //x增量
            edge_ptr->dxr = line_r.dx;
            edge_ptr->dyl = line_l.num_span;    //左右边剩余横跨扫描线数量
            edge_ptr->dyr = line_r.num_span; 
            edge_ptr->zl = line_l.z;            //左右交点深度值
            edge_ptr->zr = line_r.z;
            edge_ptr->dZx = -now_p.a / now_p.c; //向右步长的深度增量
            edge_ptr->dZy = now_p.b / now_p.c;  //向下步长的深度增量
            edge_ptr->id = now_p.id;
            edge_ptr->color = now_p.color;
            edge_ptr->pre = NULL;
            edge_ptr->next = NULL;
            //std::cout << "左交点："<< edge_ptr->xl << " 右交点：" << edge_ptr->xr << " " << edge_ptr->dxl << " " << edge_ptr->dxr << " " << edge_ptr->dyl << " " << edge_ptr->dyr << "\n";
            if (head_edge)
            {
                tail_edge->next = edge_ptr;
                edge_ptr->pre = tail_edge;
                tail_edge = edge_ptr;
            }
            else 
            {
                head_edge = edge_ptr;
                tail_edge = edge_ptr;
            }
        }

        //遍历活化边表，如果出现边对一条已经结束一条还没有，更新新边
        active_edge* edge_ptr = head_edge;
        while (edge_ptr)
        {
            if (edge_ptr->dyl == 0 && edge_ptr->dyr)  //左边已经结束，右边还有
            {
                std::vector<classifier_polygon::lines> three_edges = p.find_edge[edge_ptr->id];
                for (int i = 0; i < 3; i++)
                {
                    if (three_edges[i].Ymax == scan - 1)  //该边的上端点就是上条边的下端点
                    {
                        edge_ptr->xl = three_edges[i].x + three_edges[i].dx;
                        edge_ptr->dxl = three_edges[i].dx;
                        edge_ptr->dyl = three_edges[i].num_span - 1;
                        edge_ptr->zl = three_edges[i].z + edge_ptr->dZy + edge_ptr->dxl * edge_ptr->dZx;
                    }
                }
            }
            else if (edge_ptr->dyl && edge_ptr->dyr == 0)  //右边已经结束，左边还有
            {
                std::vector<classifier_polygon::lines> three_edges = p.find_edge[edge_ptr->id];
                for (int i = 0; i < 3; i++)
                {
                    if (three_edges[i].Ymax == scan - 1)  //该边的上端点就是上条边的下端点
                    {
                        edge_ptr->xr = three_edges[i].x + three_edges[i].dx;
                        edge_ptr->dxr = three_edges[i].dx;
                        edge_ptr->dyr = three_edges[i].num_span - 1;
                        edge_ptr->zr = three_edges[i].z + edge_ptr->dZy + edge_ptr->dxr * edge_ptr->dZx;
                    }
                }
            }
            edge_ptr = edge_ptr->next;
        }

        //开始处理扫描线，对所有边对进行遍历，更新深度缓冲和颜色缓冲
        edge_ptr = head_edge;
        while (edge_ptr)
        {
            float now_depth = edge_ptr->zl;
            //std::cout << "当前边对：" << edge_ptr->xl << " " << edge_ptr->xr << std::endl;
            //取出一个边对，两端点取整
            int left = int(edge_ptr->xl), right = int(edge_ptr->xr);
            if (left < 0)
            {
                now_depth += abs(left) * edge_ptr->dZx;
                left = 0;
            }
            if (right > 822)
            {
                right = 822;
            }
            for (int i = left; i <= right; i++)
            {
                if (now_depth > depth_buffer[i])
                {
                    depth_buffer[i] = now_depth;
                    color_buffer[i] = edge_ptr->color;
                }
                now_depth += edge_ptr->dZx;
            }
            edge_ptr = edge_ptr->next;
        }

        //画出这一行
        for (int i = 0; i < this->width(); i++)
        {
            if (color_buffer[i] == -1) continue;
            if (color_buffer[i]) glColor3f(1.0, 0.0, 0.0);
            else glColor3f(0.0, 0.0, 1.0);
            // 绘制点
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluOrtho2D(0, this->width(), 0, this->height());

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glBegin(GL_POINTS);
            glVertex2f(i, scan);
            glEnd();
        }

        //更新活化边表
        edge_ptr = head_edge;
        while (edge_ptr)
        {
            edge_ptr->dyl--;
            edge_ptr->dyr--;
            //std::cout << edge_ptr->dyl << std::endl;
            if (edge_ptr->dyl <= 0 && edge_ptr->dyr <= 0)
            {
                if(edge_ptr->pre)   //如果有前驱结点
                {
                    if (edge_ptr->next)   //如果有后继，就是中间结点
                    {
                        edge_ptr->pre->next = edge_ptr->next;
                        edge_ptr->next->pre = edge_ptr->pre;
                        edge_ptr = edge_ptr->next;
                    }
                    else    //它是尾结点，那么删除，更新尾结点
                    {
                        tail_edge = edge_ptr->pre;
                        edge_ptr->pre->next = NULL;
                        break;
                    }
                }
                else                //如果它是头结点
                {
                    head_edge = edge_ptr->next;
                    if (head_edge)  
                    {
                        head_edge->pre = NULL;
                        edge_ptr = head_edge;
                    }
                    else            //表里空了
                    {
                        edge_ptr = NULL;
                        tail_edge = NULL;
                        break;
                    }
                }
                continue;
            }
            edge_ptr->xl += edge_ptr->dxl;
            edge_ptr->xr += edge_ptr->dxr;
            edge_ptr->zl += edge_ptr->dZy + edge_ptr->dZx * edge_ptr->dxl;
            edge_ptr->zr += edge_ptr->dZy + edge_ptr->dZx * edge_ptr->dxr;
            edge_ptr = edge_ptr->next;
        }

        //更新活化多边形表
        active_polygon* poly_ptr = head_polygon;
        while (poly_ptr)
        {
            poly_ptr->dy--;
            if (!poly_ptr->dy) //剩下0个直接删掉
            {
                if (poly_ptr->pre)   //如果有前驱结点
                {
                    if (poly_ptr->next)   //如果有后继，就是中间结点
                    {
                        poly_ptr->pre->next = poly_ptr->next;
                        poly_ptr->next->pre = poly_ptr->pre;
                        poly_ptr = poly_ptr->next;
                    }
                    else    //它是尾结点，那么删除，更新尾结点
                    {
                        tail_polygon = poly_ptr->pre;
                        poly_ptr->pre->next = NULL;
                        break;
                    }
                }
                else                //如果它是头结点
                {
                    head_polygon = poly_ptr->next;
                    if (head_polygon)
                    {
                        head_polygon->pre = NULL;
                        poly_ptr = head_polygon;
                    }
                    else            //表里空了
                    {
                        poly_ptr = NULL;
                        tail_polygon = NULL;
                        break;
                    }
                }
                continue;
            }
            poly_ptr = poly_ptr->next;
        }
    }
}

void myWidget::method1()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);
    
    face_proj_ortho();
    zbuffer_flag = 1;

    update();
}

void myWidget::method2()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);

    face_proj_ortho();
    warnock_flag = 1;
    
    update();
}