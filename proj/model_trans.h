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
    std::vector<float> center;  // ���ĵ������
    float radius;               // ģ�Ͱ뾶
    float aspect;               // �ӿڿ�߱�
    double fov;                 // �ӳ��ĽǶ�
    double left;                // ��߽�
    double right;               // �ұ߽�
    double top;                 // �ϱ߽�
    double bottom;              // �±߽�
    double _near;               // ��ƽ��
    double _far;                // Զƽ��
    double fdist;               // ����ͷ����ģ�����ľ���
};

#endif // model_trans_H