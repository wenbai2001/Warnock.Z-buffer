#ifndef model_H
#define model_H


#include <iostream>
#include<fstream>
#include <sstream>
#include<vector>
#include<string>
#include<iterator>
#include "vertex.h"
#include <QGLWidget>
#include <QTimer>
#include <QString>
#include <QOpenGLFunctions>
#include <GL/glu.h>
#include "face.h"

class model
{
public:
    model();
    model(std::string fName);
    std::string getFileName();  //直接返回文件名
    void print();
    std::vector<Vertex> getVertices();
    std::vector<face> getFacets();
    std::vector<Vertex> parseFace(std::string line);
    Vertex parseVertex(std::string line);
    std::vector<float> static coordinateScanner(std::string line);
    void checkMin(Vertex v);
    void checkMax(Vertex v);
    std::vector<float> getMaxCoords();
    std::vector<float> getMinCoords();
    std::vector<float> findCenter();
    float findRadius();
private:
    std::string fileName;
    std::vector<Vertex> vertices;
    std::vector<face> facets;
    std::vector<Vertex> normals;
    std::vector<float> maxCoords;
    std::vector<float> minCoords;
    std::vector<float> center;
    float radius;
};


#endif // model_H