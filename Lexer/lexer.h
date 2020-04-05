#pragma once
#include"fa.h"

class Lexer
{
private:
	FA dfa;
	vector<pair<string, string>> regex_definition;
	vector<pair<string, pair<int, string>>> tokens;

	//状态转移函数
	int move(int state, int symbol);
public:
	Lexer();
	//用文件创建DFA
	Lexer(string filename);
	FA get_DFA() { return dfa; }
	void gernerate_DFA();
	vector<pair<string, pair<int, string>>> scan(string code);
	// 其它可能需要用到的成员函数和变量

	//用于储存一码多值的键
	set<string> modeset;
	//用于储存错误信息及其对应行号
	vector<pair<int, char>> error_message;
};



