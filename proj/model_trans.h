#ifndef model_trans_H
#define model_trans_H
#include <QVector3D>
#include <QQuaternion>
#include <QMouseEvent>
#include <math.h>
#include <QTime>
#include "model.h"

class model_trans
{
public:
    model_trans();
    void setAspect(float w, float h);
    void model_trans_warnock(model* o, float w, float h);
    void model_trans_zbuffer(model* o, float w, float h);
    double getleft();
    double getright();
    double gettop();
    double getbottom();
    double getnear();
    double getfar();
    float getcenter(int op);
    double getfdist();
    double getfov();
    double getaspect();

private:
    std::vector<float> center;  // 中心点的坐标
    float radius;               // 模型半径
    float aspect;               // 视口宽高比
    double fov;                 // 视场的角度
    double left;                // 左边界
    double right;               // 右边界
    double top;                 // 上边界
    double bottom;              // 下边界
    double _near;               // 近平面
    double _far;                // 远平面
    double fdist;               // 摄像头距离模型中心距离
};

#endif // model_trans_H