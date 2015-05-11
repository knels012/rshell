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
	else {
		if(wait(&status) == -1)
			perror("wait");
	}
}

//for all I/O redirection commands, only for >>, >, <, and |
void doiocommand(string com, int &status, string connectors, int &comnumb) {
	int needed_con = 0;
	int piping_count = 0;
	while (connectors[comnumb + needed_con] != '&' && connectors[comnumb + needed_con] 
		!= 'o' && connectors[comnumb + needed_con] != ';') {
		if (connectors[comnumb + needed_con] == '|')
			piping_count++;
		needed_con++;
	}
	
	//if (piping != 0) {
		//if <, inputfile to something else
		//if > or >> at end
	//}
	//else {
		//if <
		//if >> or >>
	//}
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
		if (input[0] == '&' || input[0] == '|' || input[0] == '<') i++;
		//registers all '#', ';', "&&", and "||", adding them to connectors string
		for (; i < input.size();i++) {
			if (input[i] == '#') {
				//comments = input.substr(i, input.size() -1);
				input = input.substr(0, i);
			}
			else if(input[i] == ';')
				connectors += ";";
			else if (input[i] == '&' && input[i - 1] == '&')
				connectors += "&";
			else if(input[i] == '|'&& input [i - 1] == '|')
				connectors += "o";
			else if(input[i - 1] == '|' && input[i] != '|')
				connectors += "|";
			//add checks for <, >, and >>
		}
		char_separator<char> delim("&|;");
		tokenizer< char_separator<char> > mytok(input, delim);
		for (tokenizer< char_separator<char> >::iterator it = mytok.begin();
				it != mytok.end() && !failed; ++it) {
			cur_com = *it;
			//call fuct to do command w/ input redirecton, pass in connectors and comnumb
				//doiocommand(cur_com, status, connectors, comnumb)
			//else, just:
			docommand(cur_com, status);
			//connectors checked here
			if (status != 0 && connectors[comnumb] == '&') {
				failed = true;
			}
			else if(status == 0  && connectors[comnumb] == 'o') {
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
