#include "lexer.h"
#include <fstream>
#include <sstream>
//#include <QDebug>
//#include <QString>

Lexer::Lexer()
{
    //��ʶ�������֣����һ��
    string digit = "(0|1|2|3|4|5|6|7|8|9)";
    string digit_p = "(1|2|3|4|5|6|7|8|9)";
    string letter_ = "(_|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)";
    string id = letter_ + "(" + letter_ + "|" + digit + ")*";
    string digits = "(0|(" + digit_p + digit + "*))";
    string optional_fraction = ".(" + digit + digit + "*)|@";  // @��ʾ��
    string optional_exponent = "(E(+|-|@)" + digits + ")|@";
    string number = digits + "(" + optional_fraction + ")(" + optional_exponent + ")";
    string character = "'(" + digit + "|" + letter_ + ")'";
    string octal = "0(1|2|3|4|5|6|7)(0|1|2|3|4|5|6|7)*";
    string hexadecimal = "0x(1|2|3|4|5|6|7|8|9|A|B|C|D|E|a|b|c|d|e)(0|1|2|3|4|5|6|7|8|9|A|B|C|D|E|a|b|c|d|e)*";
    string string_ = "\"(" + digit + "|" + letter_  + ")*\"";

    //������3.5.3�����г���ģʽ����ѡ��regex_definition[i]��index����������ʽ��˳��

    //�����֣�һ��һ��
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
    //�������ʶ�������һ��
    regex_definition.push_back(make_pair("IDN", id));
    modeset.insert("IDN");
    regex_definition.push_back(make_pair("CONST", number));
    modeset.insert("CONST");
    //��������������һ��һ��
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
    //��չ����Ϊ���һ��
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
�˹������ɴ��ļ��ж�ȡ����ָ����ʽ�����������ʽ��������֮����DFA
���룺������ָ����ʽ��������ʽ���ļ���
*/
Lexer::Lexer(string filename) {
    ifstream file;
    //qDebug() << QString(QString::fromLocal8Bit(filename.c_str()));
    file.open(filename);
    if (!file)
    {
        cout << "���ļ�ʧ�ܣ�" << filename << endl;
        exit(1);//ʧ���˻ز���ϵͳ
    }

    //����������ʽ���Լ�����������ʽ
    map<string, string> regexs;
    map<string, string>::iterator iter;
    //���ļ�����0��ʾ�м�regex����1��ʾ����regex
    int typeOfRegex = 0;
    //�м����
    string tempsting;
    string varname;
    string varvalue;
    string mode;

    string strline;
    while (getline(file, strline)) {
        stringstream ss(strline);
        ss >> typeOfRegex;
        if (typeOfRegex == 1) {
            //����ĸ�����һ������������ʽ�����
            ss >> mode;
        }
        ss >> varname;
        //���Ե�һ���Ⱥ�
        ss >> tempsting;
        while (ss >> tempsting) {
            iter = regexs.find(tempsting);
            if (iter != regexs.end()) {
                //���ҵ�˵�����ַ�����ʾ����һ�����е��м�������ʽ
                //�轫���Ӧ������������ʽ���ں���
                varvalue += (*iter).second;
            }
            else {
                //δ�ҵ�˵�����ַ�����ʾ�Ĳ����м�������ʽ����ֱ�Ӽ���varvalue����
                varvalue += tempsting;
            }
        }
        if (typeOfRegex == 1) {
            //����ĸ�����һ������������ʽ��ʱ��ֱ�Ӵ���regex_definition
            regex_definition.push_back(make_pair(mode, varvalue));
        }
        //���ͳһ����regexs
        regexs.insert(map<string, string>::value_type(varname, varvalue));
        varvalue = "";
    }
}

void Lexer::gernerate_DFA()
{
    vector<FA> nfas;
    //����������ʽ�����������ɶ�Ӧ��nfa
    for (unsigned i = 0; i < regex_definition.size(); ++i) {
        FA nfa(regex_definition[i].first, regex_definition[i].second, i);
        nfas.push_back(nfa);
    }
    FA merged_nfa = merge(nfas); //��nfa�ϲ�
    dfa.to_dfa(merged_nfa); //ת��Ϊdfa
}

// ɨ������Ĵ����
vector<pair<string, pair<int, string>>> Lexer::scan(string code)
{
    //�����հ׷�
    //DFAģ��
    //......
    vector<pair<string, pair<int, string>>> tokens;
    int p1 = 0;//��¼��ǰ��ʶ��token��ʼλ�õ�ָ��
    int p2 = 0;//��code����ǰ�ƶ���ָ��
    int current_state = 0;//��ʾ��ǰ״̬����ʼ�����ʾ��ʼ״̬
    int pp = 0;//��ǿ��ܵĽ�����
    int temp_state = 0;
    bool termination = false;
    bool comment = false;
    int linecount = 0;//���м������������������Ϣ
    set<int> accept_states = dfa.get_accept_states();

    while (p2 < (int)code.size()) {
        if (code[p2] == '\n') {
            linecount++;
        }
        if (code[p2] == '/' && code[p2 + 1] == '*') {
            //ע�Ϳ�ʼ
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
            //ע�ͽ���
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
            //������ַ�
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
            //������token����
            tokens.push_back(make_pair(dfa.get_mode()[temp_state].first, make_pair(dfa.get_mode()[temp_state].second, code.substr(p1, p2 - p1))));
            p1 = p2;
            pp = p2;
            termination = false;
            current_state = 0;
        }
        current_state = move(current_state, code[p2]);
        //cout << current_state << endl;
        //�Ե�ǰ��״̬�������ֿ��ܣ������ڣ��ս�̬�ͷ��ս�̬
        if (current_state == -1) {
            if (p2 > pp) {
                p2 = pp;
                termination = true;
            }
            else {
                //�������ֻ�ģʽ��ȫ����ʼ������ʵ��
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
    //��β����
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


