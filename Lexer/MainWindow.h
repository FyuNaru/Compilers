#pragma once
#ifdef WIN64
#pragma execution_character_set("utf-8")
#endif
#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "lexer.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);

private slots:
	//计算结果槽函数
	void on_pushButton_2_clicked();
	//读取测试用例函数
	void on_pushButton_4_clicked();
private:
	Ui::MainWindowClass ui;
	Lexer* lexer;
	string faFilename = "";
};
