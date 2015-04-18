#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>

using namespace std;

int docommand(string com) {
	//will actually process the commands
}

int main() {
	bool exit = false;
	string input;
	string cur_com;
	string comments;
	int status = 0;
	while (!exit) {
		cout << "$ ";
		cin >> input;
		string connectors;
		for (int i = 0; i < input.size();i++) {
			if (input[i] == '#') {
				comments = input.substr(i, input.size() -1);
				input = input.substr(0, i);
			}
			else if(input[i] == ';')
				connectors += ";";
			else if (input[i] == '&')
				connectors += "&";
			else if(input[i] == '|') {
				connectors += "|";
		}
		char_seperator<char> delim(" 	&|;");
		tokenizer< char_seperator<char> > mytok(input, delim);
		for (auto it = mytok.begin(); it != mytok.end(); ++it) {
			cur_com = *it;
			if (!cur_com.empty())
				status = docommand(cur_com);
			//put connector code here
		}
	}	
	return 0;
}
