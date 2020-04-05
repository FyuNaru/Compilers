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
	lexer->gernerate_DFA(); //�Զ�����DFA
	ui.plainTextEdit->setPlainText("create DFA success!\n");
	//�����ɵ�dfa��������ļ���
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
	//������ʷ����������еĴ�����Ϣ
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
			//һ����
			std_str = p.second.second + "\t<" + p.first + "," + p.second.second + ">";
		}
		else {
			//һ��һ��
			std_str = p.second.second + "\t<" + p.first + ", - >";
		}
		//��stringת��ΪQstring
		QString q_str = QString(QString::fromLocal8Bit(std_str.c_str()));
		ui.plainTextEdit->appendPlainText(q_str);
	}
}

//��ȡFA�ļ��ĳ�����ʱ����Ҫ��
/*
void MainWindow::on_pushButton_3_clicked()
{
	//��ȡFA�ļ�
	QString path = QFileDialog::getOpenFileName(this);
	ui.lineEdit->setText(path);
	QFile file(path);
	file.open(QIODevice::ReadOnly);

	QByteArray data = file.readAll();
	//���·����������ʱ���ֵ�����
	faFilename = string((const char*)path.toLocal8Bit());

	file.close();
}
*/


void MainWindow::on_pushButton_4_clicked()
{
	//��ȡ���������ļ�
	QString path = QFileDialog::getOpenFileName(this);
	QFile file(path);
	file.open(QIODevice::ReadOnly);

	QByteArray data = file.readAll();
	ui.plainTextEdit_2->setPlainText(data);
	file.close();
}
