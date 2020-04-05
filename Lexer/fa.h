#pragma once

#include <iostream>
#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
#include <string>
#include <stack>
#include <map>

using namespace std;

class FA
{
private:
	int node_num; //节点个数
	vector<vector<pair<int, int>>> adjacency; //邻接表

	set<int> alphabet; //字母表
	map<pair<int, int>, int> tran; //转移表
	set<int> accept_states; //接收态
	map<int, pair<string, int>> mode; //正则表达式对应的模式集合
public:
	FA(); //生成一个结点（状态）
	FA(int symbol); //生成含有一个初始态和接收态的FA，转移字符为symbol
	FA(string token_name, string regex, int order); //根据正则表达式生成FA
	//FA(FA & fa);

	set<int> get_alphabet() { return alphabet; }
	vector<vector<pair<int, int>>> get_adjacency() { return adjacency; }
	map<pair<int, int>, int> get_tran() { return tran; }
	set<int> get_accept_states() { return accept_states; }
	map<int, pair<string, int>> get_mode() { return mode; }

	void add_index(int bias);
	set<int> epsilon_closure(set<int>current_state);
	int move(int current_state, int symbol);
	set<int> move(set<int>current_states, int symbol);
	void to_dfa(FA nfa);

	friend FA& operator - (FA& nfa_1, FA& nfa_2); //连接
	friend FA operator | (FA& nfa_1, FA& nfa_2); //或
	friend FA operator * (FA& nfa); //闭包
	friend FA merge(vector<FA> nfas); //合并
};

