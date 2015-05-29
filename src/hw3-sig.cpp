#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <sys/types.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>

using namespace std;
using namespace boost;

	//for parsing a single command
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
	//for executing the cd command
void docd(string com, int &status) {
	char* argv[101];
	int a = parseline(com, argv);
	argv[a] = NULL;

	if (a > 2) {
		cout << "too many commands after cd, all but the first will be ignored" << endl;
	}
	if (a == 1) {
		cout << "cd" << endl;
	}
	else if (strcmp(argv[1], "-") == 0) {
		cout << "cd -" << endl;
	}
	else {
		struct stat st;
		status  = lstat(argv[1], &st);
		if (status == -1) {
			perror("lstat");
			return;
		}
		if (S_ISDIR(st.st_mode)) {
			cout << "cd valid_path" << endl;
		}
		//else
		//	cout << "Not a valid directory" << endl;
	}
}

	//for executing a single command
void docommand(string com, int &status) {
	char* argv[101];
	int a = parseline(com, argv);
	argv[a] = NULL;
	if (strcmp(argv[0], "exit") == 0) {
		exit(0);
	}
	int ifork = fork();
	//child fuction
	if (ifork == 0) {
		status = execvp(argv[0], argv);
		//this will only be reached if error in running command
		if (status == -1) {
			perror("execvp");
			exit(status);
		}
	}
	else if (ifork < 0) {
		perror("fork");
		exit(status);
	}
	else {
		if(wait(&status) == -1)
			perror("wait");
	}
}

int main() {
	bool failed = false;
	string input;
	string cur_com;
	//string comments;
	struct passwd *pws = getpwuid(geteuid());
	if (!pws)
		perror("getlogin");
	char mach[50];
	if (-1 == gethostname(mach, 50))
		perror("gethostname");
	int status = 0, comnumb = 0;
	while (1) {
		cout << pws->pw_name << "@" << mach << "$ ";
		getline(cin,input);
		string connectors = "";
		unsigned i = 0;
		if (input[0] == '&' || input[0] == '|') i++;
		//registers all '#', ';', "&&", and "||", adding them to connectors string
		for (; i < input.size();i++) {
			if (input[i] == '#') {
				//comments = input.substr(i, input.size() -1);
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
			if (cur_com[0] == 'c' && cur_com[1] == 'd') {
				docd(cur_com, status);
			}
			else {
				docommand(cur_com, status);
			}
			//connectors checked here
			if (status != 0 && connectors[comnumb] == '&') {
				failed = true;
			}
			else if(status == 0  && connectors[comnumb] == '|') {
				failed = true;
			}
			comnumb++;
			status = 0;
		}
		comnumb = 0;
		failed = false;
	}	
	return 0;
}
