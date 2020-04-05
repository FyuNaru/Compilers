#include "MainWindow.h"
#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <sstream>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	lexer = new Lexer();
	lexer->gernerate_DFA(); //自动生成DFA
	ui.plainTextEdit->setPlainText("create DFA success!\n");
	//将生成的dfa表输出到文件中
	QString path = "dfaTable.txt";
	QFile file(path);
	file.open(QIODevice::WriteOnly);

	FA dfa = lexer->get_DFA();
	file.write("DFA\n");
	set<int> alphabet = dfa.get_alphabet();
	file.write("Alphabet: ");
	stringstream alphabets;
	for (auto symbol : alphabet) {
		alphabets << (char)symbol << " ";
	}

	file.write(QString::fromStdString(alphabets.str()).toUtf8());
	file.write("\n\nDtran:");
	map<pair<int, int>, int> tran = dfa.get_tran();
	int previous = 0;
	stringstream lineOfDtran;
	lineOfDtran << "\n0\t";
	for (auto state : tran) {
		if (state.first.first != previous) {
			previous = state.first.first;
			lineOfDtran << "\n" << previous << "\t";
		}
		lineOfDtran << "(" << state.second << ", " << (char)state.first.second << ")\t";
	}
	file.write(QString::fromStdString(lineOfDtran.str()).toUtf8());
	file.write("\n\naccept states and corresponding mode: ");
	stringstream accept_states;
	for (auto state : dfa.get_accept_states()) {
		accept_states << "(" << state << ", " << dfa.get_mode()[state].first << ") ";
	}
	file.write(QString::fromStdString(accept_states.str()).toUtf8());

	file.close();
	ui.plainTextEdit->appendPlainText("dfa table is output to dfaTable.txt");
}

void MainWindow::on_pushButton_2_clicked()
{
	//string test = "while(value!=100)  {num++;}";
	string test_str = ui.plainTextEdit_2->toPlainText().toStdString();
	vector<pair<string, pair<int, string>>> tokens = lexer->scan(test_str);
	ui.plainTextEdit->clear();
	//先输出词法分析过程中的错误信息
	QString path = "error_log.txt";
	QFile file(path);
	file.open(QIODevice::WriteOnly);
	for (auto e : lexer->error_message) {
		string message = "error character : line " + std::to_string(e.first) + " :'" + e.second + "'\n";
		file.write(QString::fromStdString(message).toUtf8());
	}
	string error_number = "error :" + std::to_string(lexer->error_message.size());
	ui.label_3->setText(QString::fromStdString(error_number));
	lexer->error_message.swap(vector<pair<int, char>>());
	file.close();

	for (auto p : tokens) {
		string std_str = "";
		if (lexer->modeset.count(p.first)) {
			//一码多词
			std_str = p.second.second + "\t<" + p.first + "," + p.second.second + ">";
		}
		else {
			//一码一词
			std_str = p.second.second + "\t<" + p.first + ", - >";
		}
		//将string转换为Qstring
		QString q_str = QString(QString::fromLocal8Bit(std_str.c_str()));
		ui.plainTextEdit->appendPlainText(q_str);
	}
}

//读取FA文件的程序，暂时不需要了
/*
void MainWindow::on_pushButton_3_clicked()
{
	//读取FA文件
	QString path = QFileDialog::getOpenFileName(this);
	ui.lineEdit->setText(path);
	QFile file(path);
	file.open(QIODevice::ReadOnly);

	QByteArray data = file.readAll();
	//解决路径名有中文时出现的乱码
	faFilename = string((const char*)path.toLocal8Bit());

	file.close();
}
*/


void MainWindow::on_pushButton_4_clicked()
{
	//读取测试用例文件
	QString path = QFileDialog::getOpenFileName(this);
	QFile file(path);
	file.open(QIODevice::ReadOnly);

	QByteArray data = file.readAll();
	ui.plainTextEdit_2->setPlainText(data);
	file.close();
}
