#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <omp.h>
#include <map>

using namespace std;

bool isFullUp(const map<string, long>::iterator& it, const string& str, const map<string, long>& word_state);
bool isFullDown(const map<string, long>::iterator& it, const string& str, const map<string, long>& word_state);
bool isSubUp(const map<string, long>::iterator& it, const string& str, const map<string, long>& word_state);
bool isSubDown(const map<string, long>::iterator& it, const string& str, const map<string, long>& word_state);
int getCharactersFromUTF8String(const std::string &s, std::vector<string>& sentence);

int main(int argc, char* argv[]){
	ifstream inf(argv[1]);
	ofstream full_word(argv[2]);
	ofstream sub_word(argv[3]);
	string word;
	map<string, long> word_state;
	while(inf>>word){
		word_state[word]++;
	}
	map<string, int> sub;
	map<string, int> full;
omp_set_num_threads(24);
#pragma omp parallel 
	for (map<string, long>::iterator it = word_state.begin(); it != word_state.end(); it++) {
		vector<string> chars;
		string tmp;
		int char_num;
		string curword;
		curword = it->first;
		if (isFullUp(it, curword, word_state) && isFullDown(it, curword, word_state))
#pragma omp critical 
			full[curword] = 1;
		getCharactersFromUTF8String(curword, chars);
		tmp = "";
		char_num = chars.size();
		for(int idx = 0; idx < char_num - 1; idx++){
			tmp += chars[idx];
			if (sub.find(tmp) == sub.end())
				if (isSubUp(it, tmp, word_state) && isSubDown(it, tmp, word_state))
#pragma omp critical 
					sub[tmp] = 1;
		}
	}

	for(map<string,int>::iterator it = full.begin(); it != full.end(); it++) {
		full_word << it->first << endl;
	}

	for(map<string,int>::iterator it = sub.begin(); it != sub.end(); it++) {
		sub_word << it->first << endl;
	}

	inf.close();
	full_word.close();
	sub_word.close();
	cout << "ok" << endl;
	return 0;
}

bool isFullUp(const map<string, long>::iterator& it, const string& str, const map<string, long>& word_state){
		bool IS_FULL = true;
		map<string, long>::iterator up = it;
		if (up != word_state.begin())
			up--;
		string curWord;
		while(up != word_state.begin()) {
			curWord = up->first;
			if ((curWord)[0] != str[0])
				break;
			if ((curWord).length() >= str.length()) {
				string tmp = (curWord).substr(0, str.length());
				if (tmp == str) {
					IS_FULL = false;
					break;
				}
			}
			up--;
		}
		return IS_FULL;
}

bool isFullDown(const map<string, long>::iterator& it, const string& str, const map<string, long>& word_state) {
		bool IS_FULL = true;
		map<string, long>::iterator down = it;
		if (down != word_state.end())
			down++;
		string curWord;
		while(down != word_state.end()) {
			curWord = down->first;
			if ((curWord)[0] != str[0])
				break;
			if ((curWord).length() >= str.length()) {
				string tmp = (curWord).substr(0, str.length());
				if (tmp == str) {
					IS_FULL = false;
					break;
				}
			}
			down++;
		}
		return IS_FULL;
}

bool isSubUp(const map<string, long>::iterator& it, const string& str, const map<string, long>& word_state) {
		bool IS_SUB = true;
		map<string, long>::iterator up = it;
		if (up != word_state.begin())
			up--;
		string curWord;
		while(up != word_state.begin()) {
			curWord = up->first;
			if ((curWord)[0] != str[0])
				break;
			if (curWord == str) {
				IS_SUB = false;
				break;
			}
			up--;
		}
		return IS_SUB;
}

bool isSubDown(const map<string, long>::iterator& it, const string& str, const map<string, long>& word_state) {
		bool IS_SUB = true;
		map<string, long>::iterator down = it;
		if (down != word_state.end())
			down++;
		string curWord;
		while(down != word_state.end()) {
			curWord = down->first;
			if ((curWord)[0] != str[0])
				break;
			if (curWord == str) {
				IS_SUB = false;
				break;
			}
			down++;
		}
		return IS_SUB;
}

int getCharactersFromUTF8String(const std::string &s, std::vector<string>& sentence) {
	sentence.clear();
	unsigned long int idx = 0;
	unsigned long int len = 0;
	while (idx < s.length()) {
		if ((s[idx] & 0x80) == 0) {
			sentence.push_back(s.substr(idx, 1));
			++len;
			++idx;
		}
		else if ((s[idx] & 0xE0) == 0xC0) {
			sentence.push_back(s.substr(idx, 2));
			++len;
			idx += 2;
		}
		else if ((s[idx] & 0xF0) == 0xE0) {
			sentence.push_back(s.substr(idx, 3));
			++len;
			idx += 3;
		}
		else {
			if (s.length() - idx < 4)
			{
				sentence.push_back(s.substr(idx));
				++len;
				idx = s.length();
			}
			else
			{
				sentence.push_back(s.substr(idx, 4));
				++len;
				idx += 4;
			}
		}
	}
	if (idx != s.length()) {
		//std::cerr << "Warning: " << "in utf.h getCharactersFromUTF8String: std::string '" << s << "' not encoded in utf-8" << std::endl;
		return len + 1;
	}

	return len;
}