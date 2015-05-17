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
	if (ifork == -1) {
		perror("fork");
		status = -1;
		exit(status);
	}
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
//change to int and return -1 when error
void doiocommand(vector<string> &com, string &connectors) { //int &status, string connectors, int &comnumb) {
	//int needed_con = 0;
	int cur_string = 0;
	int piping_count = 0;
	cout << "string vect: ";
	for (unsigned i = 0; i < com.size(); i++) {
		cout << com.at(i) << " + ";
	}
	cout << endl;
	cout << "connectors: " << connectors << endl;
	//while (connectors[comnumb + needed_con] != '&' && connectors[comnumb + needed_con] 
	//	!= 'o' && connectors[comnumb + needed_con] != ';') {
	for (unsigned i = 0; i < connectors.size(); i++) {
		if (connectors[i] == '|')
			piping_count++;
		//check if multiple <, >, or >>
		//print error and quit if so
	}
	int waitcount = piping_count + 1;
	cout << "pipe count: " << piping_count << endl;	
	if (connectors[0] == '<') {
		cout << "< registered" << endl;
		cur_string += 2;
	}
	if (piping_count > 0) {
		//for (
			int fd[2];
			pipe(fd);
			int ifork = fork();
			if (ifork < 0) {
				perror("fork");
				exit(-1);
			}
			//child fuction
			else if(ifork == 0) {
				cout << "cur srting: " << com[cur_string] << endl;
				char* argv[101];
				int a = parseline(com[cur_string], argv);
				argv[a] = NULL;
				if (strcmp(argv[0], "exit") == 0) {
					exit(0);
				}
				close(1);
				dup(fd[1]);
				if (execvp(argv[0], argv) == -1) {
					perror("execvp");
					exit(-1);	
				}
			}
			//parent fuction
			else {
				//int fd2[2];
				//pipe(fd2);
				int spoon = fork();
				//cout << "spoon " << spoon << endl;
				if (spoon == -1) {
					perror("fork");
					exit(-1);
				}
				//child
				else if (spoon == 0) {
					cout << "cur srting: " << com[cur_string + 1] << endl;
					char* argv[101];
					int a = parseline(com[cur_string + 1], argv);
					argv[a] = NULL;
					cerr << "agrv " << argv[0] << endl;
					if (strcmp(argv[0], "exit") == 0) {
						exit(0);
					}
					close(0);
					dup(fd[0]);
					if (execvp(argv[0], argv) == -1) {
						perror("execvp");
						exit(-1);        
					}
					cout << "shouldn't reach" << endl;
				}
				else {
					close(fd[1]);
					close(fd[0]);
					for (; waitcount != 0; waitcount--) {
						cout << "waiting: " << waitcount << endl;
						int int_wait;
						//wait(&int_wait);
						if (wait(&int_wait) == -1) { //(int_wait == -1) { //wait(&int_wait) == -1) {
							perror("wait");
							exit(-1);
						}
						cout << "waiting: " << waitcount << endl;
					}
				}
			}
	}
	
	if (connectors[connectors.size() - 1] == '>' || connectors[connectors.size() - 1] == 'a')
		cout << "> or >> registered" << endl;



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
		cout << "$ ";//cout << pws->pw_name << "@" << mach << "$ ";
		getline(cin,input);
		string connectors = "";
		unsigned i = 1;
		//if (input[0] == '&' || input[0] == '|' || input[0] == '<') i++;
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
			else if(input[i] == '<')
				connectors += "<";
			else if(input[i] == '>'&& input [i - 1] == '>')
				connectors += "a";
			else if(input[i - 1] == '>' && input[i] != '>')
				connectors += ">";
		}
		vector<string> iostrings;
		string ioconnectors = "";
		bool noconnectors = false;
		char_separator<char> delim("&|;<>");
		tokenizer< char_separator<char> > mytok(input, delim);
		for (tokenizer< char_separator<char> >::iterator it = mytok.begin();
				it != mytok.end() && !failed; ++it) {
			cur_com = *it;
			if (connectors[comnumb] == '>' || connectors[comnumb] == '<' ||
				connectors[comnumb] == 'a' || connectors[comnumb] == '|' || noconnectors) {
				if (!cur_com.empty())
					iostrings.push_back(cur_com);
				ioconnectors += connectors[comnumb];
				noconnectors = true;
			}
			//call fuct to do command w/ input redirecton, pass in connectors and comnumb
				//doiocommand(cur_com, status, connectors, comnumb)
			else if(!noconnectors){
				docommand(cur_com, status);
				//connectors checked here
				if (status != 0 && connectors[comnumb] == '&') {
					failed = true;
				}
				else if(status == 0  && connectors[comnumb] == 'o') {
					failed = true;
				} 
			}
			comnumb++;
			status = 0;
		}
		if (noconnectors)
			doiocommand(iostrings, ioconnectors);
		comnumb = 0;
		failed = false;
	}	
	return 0;
}
