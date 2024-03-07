#ifndef myWidget_H
#define myWidget_H
#include <QWidget>
#include <QGLWidget>
#include <QT>
#include <QTimer>
#include <QVector2D>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>
#include "model.h"
#include "vertex.h"
#include "face.h"
#include "model_trans.h"
#include <QQuaternion>
#include <QObject>
#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLFramebufferObjectFormat>
#include "Rectview.h"

class myWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit myWidget(QWidget* parent = nullptr);
    void initializeGL();
    void paintGL();
    void drawObject();
    void grabObj(model m);
    void resizeGL(int w, int h);


    void myWidget::face_proj_perspective();
    void myWidget::face_proj_ortho();
    void method1();
    void method2();
    void warnock(Rectview& viewport, std::vector<new_face>);
    void zbuffer();
    bool is_intersection(new_face f, Rectview q);
 
    bool warnock_flag = 0;
    bool zbuffer_flag = 0;
    int need_clip = 0;

    struct active_polygon {
        float a, b, c, d;
        int id;
        int dy;                 //ʣ�����ɨ������
        bool color;
        active_polygon* pre;    //˫������
        active_polygon* next;
    };

    struct active_edge {
        float xl, xr;           //���ҽ����x����
        float dxl, dxr;         //������ɨ���ߵ�x�����
        int dyl, dyr;           //ʣ�����ɨ������
        float zl, zr;           //���ҽ�������
        float dZx;              //����ÿ������������� -a/c
        float dZy;              //����ÿ������������� b/c
        int id;
        bool color;
        active_edge* pre;       //˫������
        active_edge* next;
    };

private:
    QTimer timer;

    model* objPtr = 0;
    std::vector<Vertex> vertices;
    std::vector<face> faces;
    std::vector<float> center;
    std::vector<float> maxCoords;
    std::vector<float> minCoords;

    std::vector<new_face> trans_faces;
    std::vector<new_face> trans_faces_zbuffer;
    
    float radius;
    float fdist;
    double viewAngle;
    float w0;
    float h0;
    
    model_trans trans_matrix;
};

#endif // myWidget_H