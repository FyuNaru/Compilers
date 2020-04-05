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
	int node_num; //�ڵ����
	vector<vector<pair<int, int>>> adjacency; //�ڽӱ�

	set<int> alphabet; //��ĸ��
	map<pair<int, int>, int> tran; //ת�Ʊ�
	set<int> accept_states; //����̬
	map<int, pair<string, int>> mode; //������ʽ��Ӧ��ģʽ����
public:
	FA(); //����һ����㣨״̬��
	FA(int symbol); //���ɺ���һ����ʼ̬�ͽ���̬��FA��ת���ַ�Ϊsymbol
	FA(string token_name, string regex, int order); //����������ʽ����FA
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

	friend FA& operator - (FA& nfa_1, FA& nfa_2); //����
	friend FA operator | (FA& nfa_1, FA& nfa_2); //��
	friend FA operator * (FA& nfa); //�հ�
	friend FA merge(vector<FA> nfas); //�ϲ�
};

