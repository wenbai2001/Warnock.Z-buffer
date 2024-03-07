#include "model_trans.h"

model_trans::model_trans()
{
}

void model_trans::setAspect(float w, float h)  //设置视图宽高比
{
    aspect = w / h;
}


void model_trans::model_trans_warnock(model* o, float w, float h)  //计算摄像机的参数，使视图覆盖给定3D模型
{
    radius = o->findRadius();
    center = o->findCenter();
    aspect = w / h;
    fov = 60;
    fdist = radius / tan(fov * 0.5f * (3.14 / 180.0f));
    fdist *= 2;
    _near = fdist - radius;
    _far = fdist + radius;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspect, _near, _far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-center[0], -center[1], -center[2] - fdist);
}

void model_trans::model_trans_zbuffer(model* o, float w, float h)
{
    radius = o->findRadius();
    center = o->findCenter();
    aspect = w / h;
    left = o->getMinCoords()[0];
    right = o->getMaxCoords()[0];
    bottom = o->getMinCoords()[1];
    top = o->getMaxCoords()[1];
    _near = o->getMinCoords()[2];
    _far = o->getMaxCoords()[2];
    
    // 计算对象边界的长宽比
    float objectWidth = right - left;
    float objectHeight = top - bottom;
    float objectAspect = objectWidth / objectHeight;

    // 调整边界以保持长宽比
    if (objectAspect > aspect) {
        // 对象更宽，调整上下边界
        float newHeight = objectWidth / aspect;
        float heightDiff = newHeight - objectHeight;
        bottom -= heightDiff / 2;
        top += heightDiff / 2;
    }
    else {
        // 对象更高，调整左右边界
        float newWidth = objectHeight * aspect;
        float widthDiff = newWidth - objectWidth;
        left -= widthDiff / 2;
        right += widthDiff / 2;
    }   
    
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, bottom, top, -_near, -_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

double model_trans::getleft()
{
    return left;
}

double model_trans::getright()
{
    return right;
}

double model_trans::gettop()
{
    return top;
}

double model_trans::getbottom()
{
    return bottom;
}

double model_trans::getnear()
{
    return _near;
}

double model_trans::getfar()
{
    return _far;
}

float model_trans::getcenter(int op)
{
    return center[op];
}

double model_trans::getfdist()
{
    return fdist;
}

double model_trans::getfov()
{
    return fov;
}

double model_trans::getaspect()
{
    return aspect;
}