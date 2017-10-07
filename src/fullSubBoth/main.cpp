#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <omp.h>
#include <set>

using namespace std;

bool isFullDown(const set<string>::iterator& it, const string& str, const set<string>& word_state);
int getCharactersFromUTF8String(const std::string &s, std::vector<string>& sentence);

int main(int argc, char* argv[]) {
	ifstream inf(argv[1]);
	ofstream full_word(argv[2]);
	ofstream sub_word(argv[3]);
	ofstream both_word(argv[4]);
	string word;
	set<string> word_state;
	while (inf >> word) {
		word_state.insert(word);
	}
	
	set<string> sub;
	set<string> full;
	set<string> both;
omp_set_num_threads(24);
#pragma omp parallel 
	for (set<string>::iterator it = word_state.begin(); it != word_state.end(); it++) {
		const string curword = *it;
		cout << *it << endl;
		if (isFullDown(it, curword, word_state)) {
#pragma omp critical 
			full.insert(curword);
		} else {
#pragma omp critical 
			both.insert(curword);
		}
	}
#pragma omp parallel 
	for (set<string>::iterator it = word_state.begin(); it != word_state.end(); it++) {
		vector<string> chars;
		const string curword = *it;
		getCharactersFromUTF8String(curword, chars);
		int char_num = chars.size();
		string tmp = "";
		for (int idx = 0; idx < char_num - 1; idx++) {
			tmp += chars[idx];
			if (word_state.find(tmp) == word_state.end()) {
#pragma omp critical 
				sub.insert(tmp);
			}
			else {
#pragma omp critical 
				both.insert(tmp);
			}
		}
	}

	for (set<string>::iterator it = full.begin(); it != full.end(); it++) {
		full_word << *it << endl;
	}

	for (set<string>::iterator it = sub.begin(); it != sub.end(); it++) {
		sub_word << *it << endl;
	}

	for (set<string>::iterator it = both.begin(); it != both.end(); it++) {
		both_word << *it << endl;
	}

	inf.close();
	full_word.close();
	sub_word.close();
	both_word.close();
	return 0;
}

bool isFullDown(const set<string>::iterator& it, const string& str, const set<string>& word_state) {
	bool IS_FULL = true;
	set<string>::iterator down = it;
	down++;
	string sub_str;
	if (down != word_state.end()) {
		string curWord = *down;
		int str_len = str.length();
		if (str_len <= curWord.length()) {
			sub_str = curWord.substr(0, str_len);
			if (sub_str == str) {
				IS_FULL = false;
			}
		}
	}
	return IS_FULL;
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