#pragma once


#include <QtWidgets/QMainWindow>
#include "ui_CG_Proj.h"
#include <QFileDialog>
#include <QColorDialog>
#include <QColor>
#include <QMessageBox>
#include "model.h"

class CG_Proj : public QMainWindow
{
	Q_OBJECT


	public:
	CG_Proj(QWidget *parent = Q_NULLPTR);
	
private slots:
    void on_quitButton_clicked();

    void on_toolButton_load_clicked();

    void on_toolButton_zbuffer_pressed();

    void on_toolButton_warnock_pressed();
	
	private:
	Ui::CG_Proj ui;
};