//
//  main.cpp
//  project1
//
//  Created by Sungmin Im on 2016. 10. 11..
//  Copyright © 2016 Sungmin Im. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "linux_kbhit.h"
#include <termios.h>
#include <locale.h>
#include <wchar.h>

#define clean() printf("\033[H\033[J")

using namespace std;

vector<string> states;
vector<string> final_states;
vector<char> input_symbols, output_symbols;
vector< vector<string> > input_table, output_table;
int index_i, index_j;

//result of delta(state, symbol)
void f(string state, char symbol){
    vector<string>::iterator iter;
    vector<char>::iterator iter2;
    int i = 0, j = 0;
    
    for (iter = states.begin(); iter != states.end(); iter ++){
        if (state.compare(*iter) == 0) break;
        else i ++;
    }
    for (iter2 = input_symbols.begin(); iter2 != input_symbols.end(); iter2 ++){
        if (*iter2 == symbol) break;
        else j ++;
    }
    
    index_i = i; index_j = j;
    //return input_table.at(i).at(j);
}

//put element delta(old_state, symbol) = new_state
void put(string old_state, char symbol, string new_state, int sign){
    vector<string>::iterator iter;
    vector<char>::iterator iter2;
    int i = 0, j = 0;
    
    for (iter = states.begin(); iter != states.end(); iter ++){
        if (old_state.compare(*iter) == 0){
            break;
        }
        else i ++;
    }
    for (iter2 = input_symbols.begin(); iter2 != input_symbols.end(); iter2 ++){
        if (*iter2 == symbol) break;
        else j ++;
    }
    
    if(sign == 0) input_table.at(i).at(j) = new_state;
    else output_table.at(i).at(j) = new_state;
    return;
}

int getch(void)
{
    int ch;
    
    struct termios old;
    struct termios nw;
    
    tcgetattr(0, &old);
    
    nw = old;
    nw.c_lflag &= ~(ICANON|ECHO);
    nw.c_cc[VMIN] = 1;
    nw.c_cc[VTIME] = 0;
    
    tcsetattr(0, TCSAFLUSH, &nw);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &old);
    
    return ch;
}

void readText(string filename){
    ifstream mealy;
    string state, symbol, func, ini;
    int i, j;
    
    mealy.open(filename);
    
    //read states
    mealy>>state;
    mealy>>state;
    
    i = 0;
    j = 0;
    while(j < state.size()){
        if (state.at(j) != ',')
            j ++;
        else{
            string subs = state.substr(i, j - i);
            states.push_back(subs);
            //cout << i << j << subs << endl;
            i = j + 1;
            j = i;
        }
    }
    string subs = state.substr(i, j - i);
    states.push_back(subs);
    
    //input symbols
    mealy>>symbol;
    mealy>>symbol;
    mealy>>symbol;
    i = 0;
    while(i < symbol.size()){
        if (i % 2 == 0)
            input_symbols.push_back(symbol.at(i));
        i ++;
    }
    
    //read state transition function
    input_table.assign(states.size(), vector<string>(input_symbols.size(), "."));
    
    mealy >> func; mealy >> func; mealy >> func;
    while(true){
        mealy>>func;
        
        if (func == "Output") break;
        
        string new_state, old_state;
        char sym;
        
        i = 0; j = 0;
        while(func.at(j) != ','){
            j ++;
        }
        old_state = func.substr(i, j - i);
        j ++;
        sym = func.at(j ++);
        new_state = func.substr(j + 1, func.size() - j);
        
        put(old_state, sym, new_state, 0);
    }
    mealy >> func;

    //output symbol
    mealy>>symbol;
    i = 0;
    while(i < symbol.size()){
        if (i % 2 == 0)
            output_symbols.push_back(symbol.at(i));
        i ++;
    }
    
    //output function
    output_table.assign(states.size(), vector<string>(input_symbols.size(), "."));
    
    mealy >> func; mealy >> func;
    while(true){
        mealy>>func;
        if (func == "Initial") break;
        
        string new_state, old_state;
        char sym;
        
        i = 0; j = 0;
        while(func.at(j) != ','){
            j ++;
        }
        old_state = func.substr(i, j - i);
        j ++;
        sym = func.at(j ++);
        new_state = func.substr(j + 1, func.size() - j);
        
        put(old_state, sym, new_state, 1);
    }
    mealy >> func;
    
    //initial state
    mealy >> ini;
}

wchar_t mk_han(vector<int> current_word, int cho_ind, int jung_ind, int jong_ind){
    int sum = current_word.at(cho_ind) * 21 * 28;
    if (jung_ind != 0) sum += current_word.at(jung_ind) * 28;
    else return (wchar_t)(current_word.at(cho_ind) + 0x1100);
    if (jong_ind != 0) sum += current_word.at(jong_ind);
    
    sum += 0xAC00;
    
    return (wchar_t)sum;
}

int string_conversion(char num){
    if (num >= '0' && num <= '9') return num - '0';
    if (num >= 'A' && num <= 'R') return num - 'A' + 10;
    return -1;
}

int jong_to_cho(int jong){
    switch(jong){
        case 1 :
            return 0;
        case 4 :
            return 2;
        case 7 :
            return 3;
        case 8 :
            return 5;
        case 16 :
            return 6;
        case 17 :
            return 7;
        case 19 :
            return 9;
        case 21 :
            return 11;
        case 22 :
            return 12;
        case 23 :
            return 14;
        case 24 :
            return 15;
        case 25 :
            return 16;
        case 26 :
            return 17;
        case 27 :
            return 18;
        default :
            return -1;
    }
}

int jong_to_single(int jong){
    switch(jong){
        case 3 :
            return 9;
        case 5 :
            return 12;
        case 6 :
            return 18;
        case 9 :
            return 0;
        case 10 :
            return 6;
        case 11 :
            return 7;
        case 12 :
            return 9;
        case 13 :
            return 16;
        case 14 :
            return 17;
        case 15 :
            return 18;
        case 18 :
            return 9;
        default :
            return -1;
    }
}

int jong_to_cho2(int jong){
    switch(jong){
        case 3 :
            return 9;
        case 5 :
            return 12;
        case 6 :
            return 18;
        case 9 :
            return 0;
        case 10 :
            return 6;
        case 11 :
            return 7;
        case 12 :
            return 9;
        case 13 :
            return 16;
        case 14 :
            return 17;
        case 15 :
            return 18;
        case 18 :
            return 9;
        default :
            return -1;
    }
}

wchar_t* appendwChar(wchar_t* array, wchar_t a)
{
    size_t len = wcslen(array);
    
    wchar_t* ret = new wchar_t[len+2];
    
    wcscpy(ret, array);
    ret[len] = a;
    ret[len+1] = '\0';
    
    return ret;
}

void removeLast(wchar_t* array)
{
    array[wcslen(array) - 1] = '\0';
}

int main(void){
    setlocale(LC_ALL, "ko_KR.UTF-8");
    
    string current_state = "S", next_state;
    wstring word;
    wchar_t* w = L"";
    vector<int> current_word;
    int cho_ind = 0, jung_ind = 0, jong_ind = 0;
    wchar_t temp;
    
    readText("mealy.txt");
    
    printf("Type 1 for 받침우선, Type 2 for 초성우선 : ");
    char type = linux_kbhit();
    clean();
    
    //받침우선
    if (type == '1'){
        while(1)
        {
            char value = linux_kbhit();
            
            clean();
            // key stroke detected
            if (value != -1)
            {
                if (value == '<'){
                    //eraser
                    if (current_word.size() == 0 && wcslen(w) != 0) removeLast(w);
                    else{
                        if (jong_ind == jung_ind + 2){
                            if (current_word.at(jong_ind - 1) == 1)
                                current_state = "K";
                            else if (current_word.at(jong_ind - 1) == 4)
                                current_state = "N";
                            else if (current_word.at(jong_ind - 1) == 8)
                                current_state = "R";
                            else if (current_word.at(jong_ind - 1) == 17)
                                current_state = "B";
                            
                            jong_ind --;
                        }
                        else if (jong_ind == jung_ind + 1){
                            if (jung_ind == cho_ind + 2)
                                current_state = "I";
                            else if (jung_ind == cho_ind + 1){
                                if (current_word.at(jung_ind) == 8)
                                    current_state = "O";
                                else if (current_word.at(jung_ind) == 13)
                                    current_state = "U";
                                else if (current_word.at(jung_ind) == 18)
                                    current_state = "A";
                                else
                                    current_state = "I";
                            }
                            jong_ind = 0;
                        }
                        else if (jung_ind == cho_ind + 2){
                            if (current_word.at(jung_ind - 1) == 8)
                                current_state = "O";
                            else if (current_word.at(jung_ind - 1) == 13)
                                current_state = "U";
                            else if (current_word.at(jung_ind - 1) == 18)
                                current_state = "A";
                            jung_ind --;
                        }
                        else if (jung_ind == cho_ind + 1){
                            current_state = "V";
                            jung_ind = 0;
                        }
                        else{
                            current_state = "S";
                        }
                        current_word.pop_back();
                    }
                    if (current_word.size() != 0){
                        temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                        w = appendwChar(w, temp);
                        wprintf(L"%ls\n", w);
                        removeLast(w);
                    }
                    else
                        wprintf(L"%ls\n", w);
                    continue;
                }
                //printf("%c\n", value);
                f(current_state, value);
                next_state = input_table.at(index_i).at(index_j);
                string sym = output_table.at(index_i).at(index_j);
                
                if(!next_state.compare("V")){
                    if(current_state.compare("S")) //word += mk_han(current_word, cho_ind, jung_ind, jong_ind);
                        w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, jong_ind));
                    current_word.clear();
                    cho_ind = 0;
                    jung_ind = 0;
                    jong_ind = 0;
                    
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    //cout<<sym.at(0)<<endl;
                    //cout<<string_conversion(sym.at(0))<<endl;
                    
                    //cout<<mk_han(current_word, cho_ind, jung_ind, jong_ind)<<endl;
                    //wprintf(L"%lc\n", mk_han(current_word, cho_ind, jung_ind, jong_ind));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                    /*word += temp;
                    wprintf(L"%ls\n", &word);
                    word.pop_back();*/
                    w = appendwChar(w, temp);
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                }
                else if(!next_state.compare("O") || !next_state.compare("U") || !next_state.compare("A")){
                    if(!current_state.compare("K") || !current_state.compare("B") || !current_state.compare("N") || !current_state.compare("R")){
                        //word += mk_han(current_word, cho_ind, jung_ind, 0);
                        w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, 0));
                        int t = current_word.at(jong_ind);
                        current_word.clear();
                        current_word.push_back(jong_to_cho(t));
                        
                        cho_ind = 0; jung_ind = 0; jong_ind = 0;
                    }
                    else if (!current_state.compare("L")){
                        if (jong_ind == jung_ind + 2){
                            //word += mk_han(current_word, cho_ind, jung_ind, jung_ind + 1);
                            w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, jung_ind + 1));
                            int t = current_word.at(jong_ind);
                            current_word.clear();
                            current_word.push_back(jong_to_single(t));
                            
                            cho_ind = 0; jung_ind = 0; jong_ind = 0;
                        }
                        else{
                            //word += mk_han(current_word, cho_ind, jung_ind, 0);
                            w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, 0));
                            int t = current_word.at(jong_ind);
                            current_word.clear();
                            current_word.push_back(jong_to_cho(t));
                            
                            cho_ind = 0; jung_ind = 0; jong_ind = 0;
                        }
                    }
                    
                    jung_ind = cho_ind + 1;
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                    
                    /*word += temp;
                    wprintf(L"%ls\n", &word);
                    word.pop_back();*/
                    w = appendwChar(w, temp);
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                }
                else if(!next_state.compare("I")){
                    if (!current_state.compare("O") || !current_state.compare("U") || !current_state.compare("A"))
                        jung_ind ++;
                    else if (!current_state.compare("V"))
                        jung_ind = cho_ind + 1;
                    else if(!current_state.compare("K") || !current_state.compare("B") || !current_state.compare("N") || !current_state.compare("R")){
                        //word += mk_han(current_word, cho_ind, jung_ind, 0);
                        w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, 0));
                        int t = current_word.at(jong_ind);
                        current_word.clear();
                        current_word.push_back(jong_to_cho(t));
                        
                        cho_ind = 0; jung_ind = 1; jong_ind = 0;
                    }
                    else if (!current_state.compare("L")){
                        if (jong_ind == jung_ind + 2){
                            //word += mk_han(current_word, cho_ind, jung_ind, jung_ind + 1);
                            w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, jung_ind + 1));
                            int t = current_word.at(jong_ind);
                            current_word.clear();
                            current_word.push_back(jong_to_single(t));
                            
                            cho_ind = 0; jung_ind = 1; jong_ind = 0;
                        }
                        else{
                            //word += mk_han(current_word, cho_ind, jung_ind, 0);
                            w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, 0));
                            int t = current_word.at(jong_ind);
                            current_word.clear();
                            current_word.push_back(jong_to_cho(t));
                            
                            cho_ind = 0; jung_ind = 1; jong_ind = 0;
                        }
                    }
                    
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                    //wprintf(L"%lc\n", temp);
                    
                    /*word += temp;
                    wprintf(L"%ls\n", &word);
                    word.pop_back();*/
                    w = appendwChar(w, temp);
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                }
                else if(!next_state.compare("K") || !next_state.compare("B") || !next_state.compare("N") || !next_state.compare("R")){
                    jong_ind = jung_ind + 1;
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                    /*word += temp;
                    wprintf(L"%ls\n", &word);
                    word.pop_back();*/
                    w = appendwChar(w, temp);
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                }
                else if(!next_state.compare("L")){
                    if(!current_state.compare("K") || !current_state.compare("B") || !current_state.compare("N") || !current_state.compare("R"))
                        jong_ind ++;
                    else
                        jong_ind = jung_ind + 1;
                    
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                    /*word += temp;
                    wprintf(L"%ls\n", &word);
                    word.pop_back();*/
                    w = appendwChar(w, temp);
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                }
                current_state = next_state;
            }
            
            /*char value = getch();
            printf("%c\n", value);*/
            
            if(value == '.') break;
            //wprintf(L"%lc\n", word[word.size() - 1]);
            //cout << word.size() << endl;
            //cout << "cho " << cho_ind << " " << current_word.at(cho_ind) << endl;
            //cout << "jung " << jung_ind << " " << current_word.at(jung_ind) << endl;
            //cout << "jong " << jong_ind << " " << current_word.at(jong_ind) << endl;
        }
    }
    
    //초성우선
    else if (type == '2'){
        while(1)
        {
            char value = linux_kbhit();
            
            clean();
            // key stroke detected
            if (value != -1)
            {
                if (value == '<'){
                    //eraser
                    if (current_word.size() == 0 && wcslen(w) != 0) removeLast(w);
                    else{
                        if (jong_ind == jung_ind + 2){
                            if (current_word.at(jong_ind - 1) == 1)
                                current_state = "K";
                            else if (current_word.at(jong_ind - 1) == 4)
                                current_state = "N";
                            else if (current_word.at(jong_ind - 1) == 8)
                                current_state = "R";
                            else if (current_word.at(jong_ind - 1) == 17)
                                current_state = "B";
                            
                            jong_ind --;
                        }
                        else if (jong_ind == jung_ind + 1){
                            if (jung_ind == cho_ind + 2)
                                current_state = "I";
                            else if (jung_ind == cho_ind + 1){
                                if (current_word.at(jung_ind) == 8)
                                    current_state = "O";
                                else if (current_word.at(jung_ind) == 13)
                                    current_state = "U";
                                else if (current_word.at(jung_ind) == 18)
                                    current_state = "A";
                                else
                                    current_state = "I";
                            }
                            jong_ind = 0;
                        }
                        else if (jung_ind == cho_ind + 2){
                            if (current_word.at(jung_ind - 1) == 8)
                                current_state = "O";
                            else if (current_word.at(jung_ind - 1) == 13)
                                current_state = "U";
                            else if (current_word.at(jung_ind - 1) == 18)
                                current_state = "A";
                            jung_ind --;
                        }
                        else if (jung_ind == cho_ind + 1){
                            current_state = "V";
                            jung_ind = 0;
                        }
                        else{
                            current_state = "S";
                        }
                        current_word.pop_back();
                    }
                    if (current_word.size() != 0){
                        temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                        w = appendwChar(w, temp);
                        wprintf(L"%ls\n", w);
                        removeLast(w);
                    }
                    else
                        wprintf(L"%ls\n", w);
                    continue;
                }
                
                f(current_state, value);
                next_state = input_table.at(index_i).at(index_j);
                string sym = output_table.at(index_i).at(index_j);
                
                if(!next_state.compare("V")){
                    if(current_state.compare("S")) //word += mk_han(current_word, cho_ind, jung_ind, jong_ind);
                        w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, jong_ind));
                    current_word.clear();
                    cho_ind = 0;
                    jung_ind = 0;
                    jong_ind = 0;
                    
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    //cout<<sym.at(0)<<endl;
                    //cout<<string_conversion(sym.at(0))<<endl;
                    
                    //cout<<mk_han(current_word, cho_ind, jung_ind, jong_ind)<<endl;
                    //wprintf(L"%lc\n", mk_han(current_word, cho_ind, jung_ind, jong_ind));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                    /*word += temp;
                     wprintf(L"%ls\n", &word);
                     word.pop_back();*/
                    w = appendwChar(w, temp);
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                }
                else if(!next_state.compare("O") || !next_state.compare("U") || !next_state.compare("A")){
                    if(!current_state.compare("K") || !current_state.compare("B") || !current_state.compare("N") || !current_state.compare("R")){
                        //word += mk_han(current_word, cho_ind, jung_ind, 0);
                        w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, 0));
                        int t = current_word.at(jong_ind);
                        current_word.clear();
                        current_word.push_back(jong_to_cho(t));
                        
                        cho_ind = 0; jung_ind = 0; jong_ind = 0;
                    }
                    else if (!current_state.compare("L")){
                        if (jong_ind == jung_ind + 2){
                            //word += mk_han(current_word, cho_ind, jung_ind, jung_ind + 1);
                            w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, jung_ind + 1));
                            int t = current_word.at(jong_ind);
                            current_word.clear();
                            current_word.push_back(jong_to_single(t));
                            
                            cho_ind = 0; jung_ind = 0; jong_ind = 0;
                        }
                        else{
                            //word += mk_han(current_word, cho_ind, jung_ind, 0);
                            w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, 0));
                            int t = current_word.at(jong_ind);
                            current_word.clear();
                            current_word.push_back(jong_to_cho(t));
                            
                            cho_ind = 0; jung_ind = 0; jong_ind = 0;
                        }
                    }
                    
                    jung_ind = cho_ind + 1;
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                    
                    /*word += temp;
                     wprintf(L"%ls\n", &word);
                     word.pop_back();*/
                    w = appendwChar(w, temp);
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                }
                else if(!next_state.compare("I")){
                    if (!current_state.compare("O") || !current_state.compare("U") || !current_state.compare("A"))
                        jung_ind ++;
                    else if (!current_state.compare("V"))
                        jung_ind = cho_ind + 1;
                    else if(!current_state.compare("K") || !current_state.compare("B") || !current_state.compare("N") || !current_state.compare("R")){
                        //word += mk_han(current_word, cho_ind, jung_ind, 0);
                        w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, 0));
                        int t = current_word.at(jong_ind);
                        current_word.clear();
                        current_word.push_back(jong_to_cho(t));
                        
                        cho_ind = 0; jung_ind = 1; jong_ind = 0;
                    }
                    else if (!current_state.compare("L")){
                        if (jong_ind == jung_ind + 2){
                            //word += mk_han(current_word, cho_ind, jung_ind, jung_ind + 1);
                            w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, jung_ind + 1));
                            int t = current_word.at(jong_ind);
                            current_word.clear();
                            current_word.push_back(jong_to_single(t));
                            
                            cho_ind = 0; jung_ind = 1; jong_ind = 0;
                        }
                        else{
                            //word += mk_han(current_word, cho_ind, jung_ind, 0);
                            w = appendwChar(w, mk_han(current_word, cho_ind, jung_ind, 0));
                            int t = current_word.at(jong_ind);
                            current_word.clear();
                            current_word.push_back(jong_to_cho(t));
                            
                            cho_ind = 0; jung_ind = 1; jong_ind = 0;
                        }
                    }
                    
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, jong_ind);
                    //wprintf(L"%lc\n", temp);
                    
                    /*word += temp;
                     wprintf(L"%ls\n", &word);
                     word.pop_back();*/
                    w = appendwChar(w, temp);
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                }
                else if(!next_state.compare("K") || !next_state.compare("B") || !next_state.compare("N") || !next_state.compare("R")){
                    jong_ind = jung_ind + 1;
                    current_word.push_back(string_conversion(sym.at(0)));
                    
                    temp = mk_han(current_word, cho_ind, jung_ind, 0);
                    /*word += temp;
                     wprintf(L"%ls\n", &word);
                     word.pop_back();*/
                    w = appendwChar(w, temp);
                    w = appendwChar(w, 0x1100 + jong_to_cho(current_word.at(jong_ind)));
                    wprintf(L"%ls\n", w);
                    removeLast(w);
                    removeLast(w);
                }
                else if(!next_state.compare("L")){
                    if(!current_state.compare("K") || !current_state.compare("B") || !current_state.compare("N") || !current_state.compare("R")){
                        jong_ind ++;
                        current_word.push_back(string_conversion(sym.at(0)));
                        temp = mk_han(current_word, cho_ind, jung_ind, jung_ind + 1);
                        
                        w = appendwChar(w, temp);
                        w = appendwChar(w, 0x1100 + jong_to_cho2(current_word.at(jong_ind)));
                        wprintf(L"%ls\n", w);
                        removeLast(w);
                        removeLast(w);
                    }
                    else{
                        jong_ind = jung_ind + 1;
                        current_word.push_back(string_conversion(sym.at(0)));
                        
                        temp = mk_han(current_word, cho_ind, jung_ind, 0);
                        /*word += temp;
                         wprintf(L"%ls\n", &word);
                         word.pop_back();*/
                        w = appendwChar(w, temp);
                        w = appendwChar(w, 0x1100 + jong_to_cho(current_word.at(jong_ind)));
                        wprintf(L"%ls\n", w);
                        removeLast(w);
                        removeLast(w);
                    }
                }
                current_state = next_state;
            }
            
            /*char value = getch();
             printf("%c\n", value);*/
            
            if(value == '.') break;
            //wprintf(L"%lc\n", word[word.size() - 1]);
            //cout << word.size() << endl;
            //cout << "cho " << cho_ind << " " << current_word.at(cho_ind) << endl;
            //cout << "jung " << jung_ind << " " << current_word.at(jung_ind) << endl;
            //cout << "jong " << jong_ind << " " << current_word.at(jong_ind) << endl;
        }
    }
    
    printf("\n");
    printf("<Esc> key pressed. Bye bye\n\n");
    
    return 0;
}
