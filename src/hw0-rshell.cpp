#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sys/types.h>


using namespace std;
using namespace boost;


int parseline(string com, char* argv[]) {
	//will hold size of argv[]
	int s = 0;
	string strtemp;
	char_separator<char> ldelim(" 	");
	tokenizer< char_separator<char> > ltok(com, ldelim);
	for (tokenizer< char_separator<char> >::iterator it = ltok.begin(); it != ltok.end(); ++it) {
		strtemp = *it;
		char* temp = new char[strtemp.size() + 1];
		strcpy(temp, strtemp.c_str());
		argv[s] = temp;
		s++;
	}
	return s;
}

int docommand(string com) {
	//will actually process the commands
	char* argv[100];// = new char[com.size()];
	int a = parseline(com, argv);
	argv[a] = NULL;
	//cout << "===command here===" << endl;
	//for (int i = 0; i < a; i++) {
	//	cout << argv[i] << endl;
	//}
	//if (argv[0] == "exit")
	//	exit(0);
	int ifork = fork();
	//child fuction
	if (ifork == 0) {
		cout << "arg size:" << a << endl;
		cout << "argv 0:" << argv[0] << endl;
		if (execvp(argv[0], argv) == -1)
			perror("execvp");
		//this will only be reached if error in running command
	}
	//if error occured
	//else if (ifork == -1) {
		//perror
	//}
	else {
		wait(0);
	}
	return 0;
}

int main() {
	bool exit = false;
	string input;
	string cur_com;
	string comments;
	//int status = 0;
	while (!exit) {
		cout << "$ ";
		getline(cin,input);
		//cout << "input: " << input << endl;
		string connectors;
		for (unsigned i = 0; i < input.size();i++) {
			if (input[i] == '#') {
				comments = input.substr(i, input.size() -1);
				input = input.substr(0, i);
			}
			else if(input[i] == ';')
				connectors += ";";
			else if (input[i] == '&')
				connectors += "&";
			else if(input[i] == '|')
				connectors += "|";
		}
		char_separator<char> delim("&|;");
		tokenizer< char_separator<char> > mytok(input, delim);
		for (tokenizer< char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); ++it) {
			cur_com = *it;
			//char com[100] = cur_com;
			//if (!cur_com.empty())
			//	status = 
				docommand(cur_com);
			//put connector code here
		}
		cout << endl;	
		cout << "comments: " << comments << endl;
		cout << "input: " << input << endl;
		cout << "connectors: " << connectors << endl;
		exit = true;
	}	
	return 0;
}
