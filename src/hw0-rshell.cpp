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

void docommand(string com, int stat) {
	//will actually process the commands
	char* argv[100];// = new char[com.size()];
	int a = parseline(com, argv);
	stat = 0;
	argv[a] = NULL;
	//cout << "===command here===" << endl;
	//for (int i = 0; i < a; i++) {
	//	cout << argv[i] << endl;
	//}
	if (strcmp(argv[0], "exit") == 0)
		exit(0);
	int ifork = fork();
	//child fuction
	if (ifork == 0) {
		//cout << "arg size:" << a << endl;
		//cout << "argv 0:" << argv[0] << endl;
		if (execvp(argv[0], argv) == -1) {
			//this will only be reached if error in running command
			stat = -1;
			perror("execvp");
		}
	}
	else {
		if(wait(0) == -1)
			perror("wait");
		//cout << "checking" << endl;
	}
}

int main() {
	bool failed = false;
	string input;
	string cur_com;
	string comments;
	string user = getlogin();
	//if(	
	//	perror("getlogin");
	//}
	char mach[50];
	gethostname(mach, 50);
	//if (mach
	int status = 0, comnumb = 0;
	while (1) {
		//logname();
		cout << user << "@" << mach << "$ ";
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
			//breaks if & is the first inputed character
			else if (input[i] == '&' && input[i - 1] == '&')
				connectors += "&";
			//breaks if | is the first inputed character
			else if(input[i] == '|'&& input [i - 1] == '|')
				connectors += "|";
		}
		char_separator<char> delim("&|;");
		tokenizer< char_separator<char> > mytok(input, delim);
		for (tokenizer< char_separator<char> >::iterator it = mytok.begin();
				it != mytok.end() && !failed; ++it) {
			cur_com = *it;
			docommand(cur_com, status);
			//put connector code here
			if (status == -1 && connectors[comnumb] == '&') {
				//cout << "& registered, previous command failed" << endl;
				failed = true;
			}
			else if(status == 0 && connectors[comnumb] == '|') {
				//cout << "| registed, previous command worked." << endl;
				failed = true;
			}
			comnumb++;
		}
		//cout << "comments: " << comments << endl;
		//cout << "input: " << input << endl;
		//cout << "connectors: " << connectors << endl;
		comnumb = 0;
		failed = false;
	}	
	return 0;
}
