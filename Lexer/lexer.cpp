#include "lexer.h"
#include <fstream>
#include <sstream>
//#include <QDebug>
//#include <QString>

Lexer::Lexer()
{
    //标识符与数字，多词一码
    string digit = "(0|1|2|3|4|5|6|7|8|9)";
    string digit_p = "(1|2|3|4|5|6|7|8|9)";
    string letter_ = "(_|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)";
    string id = letter_ + "(" + letter_ + "|" + digit + ")*";
    string digits = "(0|(" + digit_p + digit + "*))";
    string optional_fraction = ".(" + digit + digit + "*)|@";  // @表示ε
    string optional_exponent = "(E(+|-|@)" + digits + ")|@";
    string number = digits + "(" + optional_fraction + ")(" + optional_exponent + ")";
    string character = "'(" + digit + "|" + letter_ + ")'";
    string octal = "0(1|2|3|4|5|6|7)(0|1|2|3|4|5|6|7)*";
    string hexadecimal = "0x(1|2|3|4|5|6|7|8|9|A|B|C|D|E|a|b|c|d|e)(0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|a|b|c|d|e)*";
    string string_ = "\"(" + digit + "|" + letter_  + ")*\"";

    //由龙书3.5.3，先列出的模式优先选择，regex_definition[i]的index看作正则表达式的顺序

    //保留字，一词一码
    regex_definition.push_back(make_pair("WHILE", "while"));
    regex_definition.push_back(make_pair("IF", "if"));
    regex_definition.push_back(make_pair("ELSE", "else"));
    regex_definition.push_back(make_pair("INT", "int"));
    regex_definition.push_back(make_pair("FLOAT", "float"));
    regex_definition.push_back(make_pair("DOUBLE", "double"));
    regex_definition.push_back(make_pair("BOOL", "bool"));
    regex_definition.push_back(make_pair("CHAR", "char"));
    regex_definition.push_back(make_pair("BREAK", "break"));
    regex_definition.push_back(make_pair("CONTINUE", "continue"));
    regex_definition.push_back(make_pair("RETURN", "return"));
    regex_definition.push_back(make_pair("STRING", "string"));
    regex_definition.push_back(make_pair("STRUCT", "struct"));
    regex_definition.push_back(make_pair("CALL", "call"));
    //数字与标识符，多词一码
    regex_definition.push_back(make_pair("IDN", id));
    modeset.insert("IDN");
    regex_definition.push_back(make_pair("CONST", number));
    modeset.insert("CONST");
    //定界符与运算符，一词一码
    regex_definition.push_back(make_pair("LP", "{"));
    regex_definition.push_back(make_pair("RP", "}"));
    regex_definition.push_back(make_pair("SLP", "("));
    regex_definition.push_back(make_pair("SRP", ")"));
    regex_definition.push_back(make_pair("SEMI", ";"));
    regex_definition.push_back(make_pair("NE", "!="));
    regex_definition.push_back(make_pair("INC", "++"));
    regex_definition.push_back(make_pair("PLUS", "+"));
    regex_definition.push_back(make_pair("DEC", "--"));
    regex_definition.push_back(make_pair("SUB", "-"));
    regex_definition.push_back(make_pair("MULT", "*"));
    regex_definition.push_back(make_pair("DIVI", "/"));
    regex_definition.push_back(make_pair("EQU", "=="));
    regex_definition.push_back(make_pair("ASSIGN", "="));
    regex_definition.push_back(make_pair("GE", ">="));
    regex_definition.push_back(make_pair("LE", "<="));
    regex_definition.push_back(make_pair("G", ">"));
    regex_definition.push_back(make_pair("L", "<"));
    regex_definition.push_back(make_pair("AND", "&&"));
    regex_definition.push_back(make_pair("OR", "||"));
    regex_definition.push_back(make_pair("NOT", "!"));
    //扩展，均为多词一码
    regex_definition.push_back(make_pair("CONSTC", character));
    modeset.insert("CONSTC");
    regex_definition.push_back(make_pair("OCT", octal));
    modeset.insert("OCT");
    regex_definition.push_back(make_pair("HEX", hexadecimal));
    modeset.insert("HEX");
    regex_definition.push_back(make_pair("STRINGC", string_));
    modeset.insert("STRINGC");

}

/*
此构造器可从文件中读取按照指定格式储存的正则表达式串，并用之生成DFA
输入：储存有指定格式的正则表达式的文件名
*/
Lexer::Lexer(string filename) {
    ifstream file;
    //qDebug() << QString(QString::fromLocal8Bit(filename.c_str()));
    file.open(filename);
    if (!file)
    {
        cout << "打开文件失败！" << filename << endl;
        exit(1);//失败退回操作系统
    }

    //储存正则表达式，以及搜索正则表达式
    map<string, string> regexs;
    map<string, string>::iterator iter;
    //在文件中用0表示中间regex，用1表示最终regex
    int typeOfRegex = 0;
    //中间变量
    string tempsting;
    string varname;
    string varvalue;
    string mode;

    string strline;
    while (getline(file, strline)) {
        stringstream ss(strline);
        ss >> typeOfRegex;
        if (typeOfRegex == 1) {
            //读入的该行是一个最终正则表达式的情况
            ss >> mode;
        }
        ss >> varname;
        //忽略第一个等号
        ss >> tempsting;
        while (ss >> tempsting) {
            iter = regexs.find(tempsting);
            if (iter != regexs.end()) {
                //若找到说明该字符串表示的是一个已有的中间正则表达式
                //需将其对应的真正的正则式加在后面
                varvalue += (*iter).second;
            }
            else {
                //未找到说明该字符串表示的不是中间正则表达式，可直接加在varvalue后面
                varvalue += tempsting;
            }
        }
        if (typeOfRegex == 1) {
            //读入的该行是一个最终正则表达式的时，直接存入regex_definition
            regex_definition.push_back(make_pair(mode, varvalue));
        }
        //最后统一存入regexs
        regexs.insert(map<string, string>::value_type(varname, varvalue));
        varvalue = "";
    }
}

void Lexer::gernerate_DFA()
{
    vector<FA> nfas;
    //根据正则表达式定义依次生成对应的nfa
    for (unsigned i = 0; i < regex_definition.size(); ++i) {
        FA nfa(regex_definition[i].first, regex_definition[i].second, i);
        nfas.push_back(nfa);
    }
    FA merged_nfa = merge(nfas); //将nfa合并
    dfa.to_dfa(merged_nfa); //转换为dfa
}

// 扫描输入的代码段
vector<pair<string, pair<int, string>>> Lexer::scan(string code)
{
    //跳过空白符
    //DFA模拟
    //......
    vector<pair<string, pair<int, string>>> tokens;
    int p1 = 0;//记录当前被识别token开始位置的指针
    int p2 = 0;//在code上向前移动的指针
    int current_state = 0;//表示当前状态，初始化零表示开始状态
    int pp = 0;//标记可能的结束点
    int temp_state = 0;
    bool termination = false;
    bool comment = false;
    int linecount = 0;//对行计数，用于输出错误信息
    set<int> accept_states = dfa.get_accept_states();

    while (p2 < (int)code.size()) {
        if (code[p2] == '\n') {
            linecount++;
        }
        if (code[p2] == '/' && code[p2 + 1] == '*') {
            //注释开始
            if (p1 != p2) {
                tokens.push_back(make_pair(dfa.get_mode()[current_state].first, make_pair(dfa.get_mode()[current_state].second, code.substr(p1, p2 - p1))));
                p1 = p2;
                current_state = 0;
            }
            p1 = p1 + 2;
            p2 = p2 + 2;
            comment = true;
            continue;
        }
        else if (code[p2] == '*' && code[p2 + 1] == '/') {
            //注释结束
            p1 = p1 + 2;
            p2 = p2 + 2;
            comment = false;
            continue;
        }
        if (comment == true) {
            p1++;
            p2++;
            continue;
        }

        if (code[p2] == ' ' || code[p2] == '\t' || code[p2] == '\n') {
            //处理空字符
            if (p1 != p2) {
                tokens.push_back(make_pair(dfa.get_mode()[current_state].first, make_pair(dfa.get_mode()[current_state].second, code.substr(p1, p2 - p1))));
                p1 = p2;
                current_state = 0;
            }
            p1++;
            p2++;
            continue;
        }
        else if (termination == true) {
            //处理多个token连续
            tokens.push_back(make_pair(dfa.get_mode()[temp_state].first, make_pair(dfa.get_mode()[temp_state].second, code.substr(p1, p2 - p1))));
            p1 = p2;
            pp = p2;
            termination = false;
            current_state = 0;
        }
        current_state = move(current_state, code[p2]);
        //cout << current_state << endl;
        //对当前的状态，有三种可能，不存在，终结态和非终结态
        if (current_state == -1) {
            if (p2 > pp) {
                p2 = pp;
                termination = true;
            }
            else {
                //错误处理，恐慌模式，全部初始化即可实现
                error_message.push_back(make_pair((linecount + 1), code[p2]));
                p2++;
                p1 = p2;
                pp = p2;
                termination = false;
                current_state = 0;
                continue;
            }
        }
        else if (accept_states.count(current_state)) {
            pp = p2;
            temp_state = current_state;
        }

        p2++;
    }
    //收尾工作
    if (p1 != p2) {
        tokens.push_back(make_pair(dfa.get_mode()[current_state].first, make_pair(dfa.get_mode()[current_state].second, code.substr(p1, p2 - p1))));
    }
    return tokens;
}

int Lexer::move(int current_state, int symbol) {
    for (auto state : dfa.get_tran()) {
        if (state.first.first == current_state && state.first.second == symbol) {
            return state.second;
        }
    }
    return -1;
}


