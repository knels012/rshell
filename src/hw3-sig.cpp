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
#include <signal.h>

using namespace std;
using namespace boost;

	//to catch ^C
void parhandler(int s) {
}

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
void docd(char* argv[], int a, int &status) {
	char* olddir = NULL;
	char* newdir = NULL;
	const char pwd[] = "PWD";
	const char oldpwd[] = "OLDPWD";

	if (a > 2) {
		cout << "error: too many commands" << endl;
		status = -1;
		return;
	}
	//plain cd command
	if (a == 1) {
		const char home[] = "HOME";
		newdir = getenv(home);
		if (newdir == NULL) {
			perror("getenv");
			status = -1;
			return;
		}
		olddir = getenv(pwd);
		if (olddir == NULL) {
			status = -1;
			return;
		}
		status = setenv(oldpwd, olddir, 1);
		if (status == -1) {
			perror("setenv");
			return;
		}
		status = setenv(pwd, newdir, 1);
		if (status == -1) {
			perror("setenv");
			return;
		}
		status = chdir(newdir);
		if (status == -1) {
			perror("chdir");
			return;
		}
	}
	//cd - command
	else if (strcmp(argv[1], "-") == 0) {
		newdir = getenv(oldpwd);
		if (newdir == NULL) {
			status = -1;
			return;
		}
		olddir = getenv(pwd);
		if (olddir == NULL) {
			status = -1;
			return;
		}
		status = setenv(oldpwd, olddir, 1);
		if (status == -1) {
			perror("setenv");
			return;
		}
		status = setenv(pwd, newdir, 1);
		if (status == -1) {
			perror("setenv");
			return;
		}
		status = chdir(newdir);
		if (status == -1) {
			perror("chdir");
			return;
		}
	}
	//cd with flag not -
	else {
		struct stat st;
		status  = lstat(argv[1], &st);
		if (status == -1) {
			perror("lstat");
			return;
		}
		//if pathname is a valid directory, change directory
		if (S_ISDIR(st.st_mode)) {
			newdir = argv[1];
			olddir = getenv(pwd);
			if (olddir == NULL) {
				status = -1;
				return;
			}
			
			status = setenv(oldpwd, olddir, 1);
			if (status == -1) {
				perror("setenv");
				return;
			}
			status = setenv(pwd, newdir, 1);
			if (status == -1) {
				perror("setenv");
				return;
			}
			status = chdir(newdir);
			if (status == -1) {
				perror("chdir");
				return;
			}
		}
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
	//check if cd command
	if (strcmp(argv[0], "cd") == 0) {
		docd(argv, a, status);
		//delete allcoated memory
		for (int i = a; i > -1; i--) {
			delete[] argv[i];
		}
		return;
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
		status = -1;
		return;
	}
	else {
		if(wait(&status) == -1) {
			perror("wait");
			status = -1;
			return;
		}
	}
	//delete allocated memory
	for (int i = a; i > -1; i--) {
		delete[] argv[i];
	}
}

//output the directory part of the prompt
void outputdir(char* curdir) {
	char* homedir = NULL;
	const char home[] = "HOME";
	homedir = getenv(home);
	if (homedir == NULL)
		return;
	string curr = curdir;
	string ho = homedir;
	if (ho.size() > curr.size()) {
		cout << ":" << curdir;
		return;
	}
	string sub = curr.substr(0, ho.size());
	if (sub != ho) {
		cout << ":" << curdir;
		return;
	}
	curr = "~" + curr.substr(ho.size());
	cout << ":" << curr;
}

int main() {
	struct sigaction pact;
	pact.sa_handler = parhandler;
	sigemptyset(&pact.sa_mask);
	pact.sa_flags = SA_RESTART;
	
	//sigaction to register ^C
	if (sigaction(SIGINT , &pact, NULL) < 0) {
		perror ("sigaction");
		return 1;
	}

	bool failed = false;
	string input;
	string cur_com;
	struct passwd *pws = getpwuid(geteuid());
	if (!pws)
		perror("getlogin");
	char mach[50];
	if (-1 == gethostname(mach, 50))
		perror("gethostname");
	int status = 0, comnumb = 0;
	char* currdir = NULL;
	const char pwd[] = "PWD";
	while (1) {
		//output prompt
		currdir = getenv(pwd);	
		cout << pws->pw_name << "@" << mach;
		if (currdir != NULL)
			outputdir(currdir);
		cout << " $ ";
		//get input
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
			docommand(cur_com, status);
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
