//main.cpp
#include "CG_Proj.h"
#include <QApplication>
#include "model.h"
#include <QFileDialog>
#include<GL/GLU.h>
#include <iostream>
#include <windows.h>

int main(int argc, char *argv[])
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	QApplication a(argc, argv);
	CG_Proj w;
	w.show();

	return a.exec();
}
