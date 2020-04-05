#pragma once
#include"fa.h"

class Lexer
{
private:
	FA dfa;
	vector<pair<string, string>> regex_definition;
	vector<pair<string, pair<int, string>>> tokens;

	//״̬ת�ƺ���
	int move(int state, int symbol);
public:
	Lexer();
	//���ļ�����DFA
	Lexer(string filename);
	FA get_DFA() { return dfa; }
	void gernerate_DFA();
	vector<pair<string, pair<int, string>>> scan(string code);
	// ����������Ҫ�õ��ĳ�Ա�����ͱ���

	//���ڴ���һ���ֵ�ļ�
	set<string> modeset;
	//���ڴ��������Ϣ�����Ӧ�к�
	vector<pair<int, char>> error_message;
};



