//CG_Proj.cpp
#include "CG_Proj.h"
#include "ui_CG_Proj.h"
#include "QtDebug"

CG_Proj::CG_Proj(QWidget* parent) : QMainWindow(parent)
{
    ui.setupUi(this);
    this->setAutoFillBackground(true);
}

void CG_Proj::on_toolButton_load_clicked()
{
    std::string fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open File"),
        "..//OpenglOBJ//obj",
        "Object File (*.obj)"
    ).toStdString();

    model loader(fileName);
    ui.widget16->grabObj(loader);   
}

void CG_Proj::on_quitButton_clicked()  
{
    close();
}


void CG_Proj::on_toolButton_zbuffer_pressed()
{
    ui.widget16->method1();
}

void CG_Proj::on_toolButton_warnock_pressed()
{
    ui.widget16->method2();
}

