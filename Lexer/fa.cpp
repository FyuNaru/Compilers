#include "fa.h"

FA::FA() {
	node_num = 1;
	vector < pair<int, int>> new_node;
	adjacency.push_back(new_node);
}

FA::FA(int symbol) {
	if (symbol == '@')
		symbol = -1;
	else
		alphabet = { symbol };
	node_num = 2;
	vector < pair<int, int>> start_node;
	vector < pair<int, int>> accept_node;
	start_node.push_back(make_pair(1, symbol)); //初态结点 与节点1相连。转移符号是symbol(-1表示空串ε)
	adjacency.push_back(start_node);
	adjacency.push_back(accept_node);
	accept_states.insert(1);
}

FA::FA(string token_name, string regex, int order)
{
	stack<char> signs;
	stack<FA> NFAs;
	FA nfa, nfa_1, nfa_2, temp;

	if (regex == "(" || regex == ")" || regex == "*") {
		FA nfa(regex[0]);
		NFAs.push(nfa);
	}


	else if (regex == "||") {
		FA nfa_1('|');
		FA nfa_2('|');
		FA nfa = nfa_1 - nfa_2;
		NFAs.push(nfa);
	}
	else {
		for (unsigned i = 0; i < regex.size(); ++i) {
			char ch = regex[i];
			//处理左右括号

			switch (ch)
			{
			case '|':
				if (signs.empty()) {
					signs.push('|');
				}
				else if (signs.top() == '~') {
					while (!signs.empty() && signs.top() == '~') {
						signs.pop();
						FA nfa_2 = NFAs.top();
						NFAs.pop();
						FA nfa_1 = NFAs.top();
						NFAs.pop();
						temp = nfa_1 - nfa_2;
						NFAs.push(temp);
					}
					signs.push('|');
				}
				else {
					signs.push('|');
				}
				break;
			case '*':
				nfa = NFAs.top();
				NFAs.pop();
				temp = *nfa;
				NFAs.push(temp);
				if (i + 1 < regex.size() && regex[i + 1] != '|' && regex[i + 1] != '*' && regex[i + 1] != ')')
					signs.push('~');
				break;
			case '(':
				signs.push('(');
				break;
			case ')':
				while (signs.top() != '(')
				{
					if (signs.top() == '|') {
						signs.pop();
						nfa_2 = NFAs.top();
						NFAs.pop();
						nfa_1 = NFAs.top();
						NFAs.pop();
						temp = nfa_1 | nfa_2;
						NFAs.push(temp);
					}
					else if (signs.top() == '~') {
						signs.pop();
						nfa_2 = NFAs.top();
						NFAs.pop();
						nfa_1 = NFAs.top();
						NFAs.pop();
						temp = nfa_1 - nfa_2;
						NFAs.push(temp);
					}
				}
				signs.pop();
				if (i + 1 < regex.size() && regex[i + 1] != '|' && regex[i + 1] != '*' && regex[i + 1] != ')')
					signs.push('~');
				break;
			default:
				//if (alphabet.find(ch) == alphabet.end()) alphabet.insert(ch); //不存在则将新符号添加到字母表中
				FA nfa(ch);
				NFAs.push(nfa);
				if (i + 1 < regex.size() && regex[i + 1] != '|' && regex[i + 1] != '*' && regex[i + 1] != ')')
					signs.push('~');
				break;
			}
		}

		while (!signs.empty()) {
			if (signs.top() == '|') {
				signs.pop();
				nfa_2 = NFAs.top();
				NFAs.pop();
				nfa_1 = NFAs.top();
				NFAs.pop();
				temp = nfa_1 | nfa_2;
				NFAs.push(temp);
			}
			else if (signs.top() == '~') {
				signs.pop();
				nfa_2 = NFAs.top();
				NFAs.pop();
				nfa_1 = NFAs.top();
				NFAs.pop();
				temp = nfa_1 - nfa_2;
				NFAs.push(temp);
			}
		}
	}

	adjacency = NFAs.top().adjacency;
	node_num = NFAs.top().node_num;
	accept_states = NFAs.top().accept_states;
	alphabet = NFAs.top().alphabet;
	mode[node_num - 1].first = token_name;
	mode[node_num - 1].second = order;
}

void FA::add_index(int bias)
{
	for (auto& node : adjacency) {
		for (auto& neighbour : node) {
			neighbour.first += bias;
		}
	}
}

set<int> FA::epsilon_closure(set<int> current_state)
{
	stack<int> s;
	for (auto state : current_state)  s.push(state); //将current_state集合中所有状态压栈

	set<int> closure = current_state; //用current_state集合对返回集合初始化

	while (!s.empty()) {
		int top = s.top(); //将栈顶元素弹出栈
		s.pop();
		for (auto neighbour : adjacency[top]) {
			if (neighbour.second == -1 && closure.find(neighbour.first) == closure.end()) {
				closure.insert(neighbour.first);
				s.push(neighbour.first);
			}
		}
	}
	return closure;
}


int FA::move(int current_state, int symbol)
{
	for (auto neighbor : adjacency[current_state]) {
		if (neighbor.second == symbol)
			return neighbor.first;
	}
	return -1;
}

set<int> FA::move(set<int> current_states, int symbol)
{
	set<int> res;
	for (auto state : current_states) {
		for (auto neighbor : adjacency[state]) {
			if (neighbor.second == symbol) res.insert(neighbor.first);
		}
	}
	return res;
}

void FA::to_dfa(FA nfa)
{
	/*子集构造法*/

	//一开始，ε-closure(s0)是Dstate中的唯一状态，且它未加标记
	vector<vector<pair<int, int>>>().swap(adjacency);
	alphabet = nfa.alphabet;
	map<set<int>, pair<bool, int>> DFA_states;
	set<int> start_state = nfa.epsilon_closure({ 0 });
	set<int> nfa_accept_states = nfa.get_accept_states();
	map<int, pair<string, int>> nfa_mode = nfa.get_mode();
	set<int> temp1;

	// 查找FA中的接收态
	set_intersection(start_state.begin(), start_state.end(), nfa_accept_states.begin(), nfa_accept_states.end(), inserter(temp1, temp1.begin()));
	if (!temp1.empty()) {
		accept_states.insert(0); //标记位FA的接收态
		for (auto nfa_accept_state : temp1) {
			mode[0] = make_pair(nfa_mode[nfa_accept_state].first, nfa_mode[nfa_accept_state].second); //将nfa优先级最高的接收态对应的模式和FA接收态绑定
		}
	}

	DFA_states[start_state] = make_pair(false, 0);
	vector < pair<int, int>> new_node;
	adjacency.push_back(new_node);

	bool exist_unmarked_state = true;
	int index = 1;
	while (exist_unmarked_state)
	{
		exist_unmarked_state = false;
		for (auto& state : DFA_states) {
			if (state.second.first == false) {
				bool exist_unmarked_state = true;
				state.second.first = true; //给T加上标记
				for (auto symbol : nfa.get_alphabet()) {
					set<int> new_state = nfa.epsilon_closure(nfa.move(state.first, symbol));
					if (new_state.empty()) continue;
					else if (DFA_states.find(new_state) == DFA_states.end()) {
						DFA_states[new_state] = make_pair(false, index);
						vector < pair<int, int>> new_node;
						adjacency.push_back(new_node);
						index++;

						// 查找FA中的接收态
						set<int> temp2;
						set_intersection(new_state.begin(), new_state.end(), nfa_accept_states.begin(), nfa_accept_states.end(), inserter(temp2, temp2.begin()));
						if (!temp2.empty()) {
							accept_states.insert(index - 1); //标记位FA的接收态
							for (auto nfa_accept_state : temp2) {
								if (mode.find(index - 1) == mode.end() || nfa_mode[nfa_accept_state].second < mode[index - 1].second) {
									mode[index - 1] = make_pair(nfa_mode[nfa_accept_state].first, nfa_mode[nfa_accept_state].second); //将nfa优先级最高的接收态对应的模式和FA接收态绑定
								}
							}
						}
					}
					adjacency[state.second.second].push_back(make_pair(DFA_states[new_state].second, symbol));
				}
			}
		}
	}

	/*最小化状态数*/
	vector<set<int>> partition;
	set<int> non_accept_state;
	map<int, set<int>> temp_map; //临时map，建立模式与接收态的映射

	//初始化
	for (unsigned i = 0; i < adjacency.size(); ++i) {
		if (accept_states.find(i) == accept_states.end()) { //非接收态
			non_accept_state.insert(i);
		}
		else { //进一步分划接收态
			temp_map[mode[i].second].insert(i);
		}
	}
	partition.push_back(non_accept_state);
	for (auto state : temp_map) {
		partition.push_back(state.second);
	}

	while (true) {
		vector<set<int>> partition_new;

		for (auto states : partition) {
			map<int, vector<int>> move_table; //转移映射, 如move_table(2, (0，2))表示状态2可以通过alphabet中符号转移后的结果是0，2
			for (auto state : states) {
				for (auto symbol : alphabet) {
					int next_state = move(state, symbol); //当前状态经过symbol转移到的状态
					if (next_state == -1) {
						move_table[state].push_back(-1);
					}
					else {
						for (unsigned i = 0; i < partition.size(); ++i) {
							// 找到转移的状态所在的集合
							if (partition[i].find(next_state) != partition[i].end()) {
								move_table[state].push_back(i);
								break;
							}
						}
					}
				}
			}

			map<vector<int>, set<int>> move_table_inverse;
			for (auto state : move_table) {
				move_table_inverse[state.second].insert(state.first);
			}

			for (auto state : move_table_inverse) {
				partition_new.push_back(state.second);
			}
		}

		if (partition_new == partition) break;

		partition = partition_new;
	}


	//为大小≥2的集合选取代表元素
	map<int, set<int>> representatives;
	for (auto subset : partition) {
		if (subset.size() >= 2) {
			set<int>::iterator iter = ++subset.begin();
			for (; iter != subset.end(); ++iter) {
				representatives[*subset.begin()].insert(*iter);
			}
		}
	}

	//把邻接表改为转换表(方便删除和修改)
	for (unsigned i = 0; i < adjacency.size(); ++i) {
		for (unsigned j = 0; j < adjacency[i].size(); ++j) {
			tran[make_pair(i, adjacency[i][j].second)] = adjacency[i][j].first;
		}
	}

	//删除不可分的状态
	for (auto representative : representatives) {
		for (auto state : representative.second) {
			map<pair<int, int>, int>::iterator iter = tran.begin();
			for (; iter != tran.end();) {
				if ((*iter).first.first == state) {
					tran.erase(iter++);
				}
				else if ((*iter).second == state) {
					(*iter).second = representative.first;
					++iter;
				}
				else {
					++iter;
				}
			}

			//if(accept_states.find(state)!=accept_states.end())
			accept_states.erase(state);
		}
	}

}

FA& operator - (FA& nfa_1, FA& nfa_2)
{
	int bias = nfa_1.node_num - 1;

	for (unsigned i = 0; i < nfa_2.adjacency.size(); ++i) {
		for (auto& neighbour : nfa_2.adjacency[i]) {
			neighbour.first += bias;
			if (i == 0) nfa_1.adjacency[bias].push_back(neighbour);
		}
		if (i >= 1) nfa_1.adjacency.push_back(nfa_2.adjacency[i]);
	}

	nfa_1.node_num = nfa_1.node_num + nfa_2.node_num - 1;
	nfa_1.accept_states = { nfa_1.node_num - 1 };
	set_union(nfa_1.alphabet.begin(), nfa_1.alphabet.end(), nfa_2.alphabet.begin(), nfa_2.alphabet.end(), inserter(nfa_1.alphabet, nfa_1.alphabet.begin()));
	return nfa_1;
}

FA operator | (FA& nfa_1, FA& nfa_2)
{
	FA nfa_res;
	int bias = 1;

	//在nfa_1前增加空边
	nfa_1.add_index(bias);
	for (auto& node : nfa_1.adjacency) {
		nfa_res.adjacency.push_back(node);
	}
	nfa_res.adjacency[0].push_back(make_pair(bias, -1));

	bias += nfa_1.node_num;

	//在nfa_2前增加空边
	nfa_2.add_index(bias);
	for (auto& node : nfa_2.adjacency) {
		nfa_res.adjacency.push_back(node);
	}
	nfa_res.adjacency[0].push_back(make_pair(bias, -1));

	//新增接收结点
	vector < pair<int, int>> accept_node;
	nfa_res.adjacency.push_back(accept_node);

	//在nfa_1, nfa_2后增加空边
	nfa_res.node_num = nfa_1.node_num + nfa_2.node_num + 2;
	nfa_res.adjacency[nfa_1.node_num].push_back(make_pair(nfa_res.node_num - 1, -1));
	nfa_res.adjacency[nfa_1.node_num + nfa_2.node_num].push_back(make_pair(nfa_res.node_num - 1, -1));

	nfa_res.accept_states = { nfa_res.node_num - 1 };
	set_union(nfa_1.alphabet.begin(), nfa_1.alphabet.end(), nfa_2.alphabet.begin(), nfa_2.alphabet.end(), inserter(nfa_res.alphabet, nfa_res.alphabet.begin()));

	return nfa_res;
}

FA operator * (FA& nfa)
{
	FA nfa_res;

	nfa.add_index(1);
	for (auto& node : nfa.adjacency) {
		nfa_res.adjacency.push_back(node);
	}

	//新增接收结点
	vector < pair<int, int>> accept_node;
	nfa_res.adjacency.push_back(accept_node);

	//添加空边
	nfa_res.node_num = nfa.node_num + 2;
	nfa_res.adjacency[0].push_back(make_pair(1, -1));
	nfa_res.adjacency[0].push_back(make_pair(nfa_res.node_num - 1, -1));
	nfa_res.adjacency[nfa.node_num].push_back(make_pair(nfa_res.node_num - 1, -1));
	nfa_res.adjacency[nfa.node_num].push_back(make_pair(1, -1));

	nfa_res.accept_states = { nfa_res.node_num - 1 };
	nfa_res.alphabet = nfa.alphabet;

	return nfa_res;
}

FA merge(vector<FA> nfas)
{
	FA nfa_res;
	int bias = 1;

	for (auto nfa : nfas) {
		//在nfa前增加空边
		nfa.add_index(bias);
		nfa_res.mode[nfa.node_num - 1 + bias] = nfa.mode[nfa.node_num - 1];

		for (auto& node : nfa.adjacency) {
			nfa_res.adjacency.push_back(node);
		}
		nfa_res.adjacency[0].push_back(make_pair(bias, -1));

		bias += nfa.node_num;
		nfa_res.accept_states.insert(bias - 1);
		set_union(nfa.alphabet.begin(), nfa.alphabet.end(), nfa_res.alphabet.begin(), nfa_res.alphabet.end(), inserter(nfa_res.alphabet, nfa_res.alphabet.begin()));
	}

	nfa_res.node_num = bias;
	nfa_res.accept_states.insert(nfa_res.node_num - 1);

	return nfa_res;
}


