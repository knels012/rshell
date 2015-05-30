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

//void chihandler(int s) {
//	cout << "child registered ^C" << endl;;
//}

void parhandler(int s) {
	cout << endl;
	//puts("parent caught");
//	cout << flush;
	//cout << "regestered ^C" << flush;
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
	//char* argv[101];
	//int a = parseline(com, argv);
	//argv[a] = NULL;
	char* olddir = NULL;
	char* newdir = NULL;
	const char pwd[] = "PWD";
	const char oldpwd[] = "OLDPWD";

	//for (int i = 0; argv[i] != NULL; i++) {
	//	cout << "argv[" << i << "]: " << argv[i] << endl;
	//}

	if (a > 2) {
		cout << "too many commands after cd, all but the first will be ignored" << endl;
	}
	if (a == 1) {
		cout << "cd" << endl;
		const char home[] = "HOME";
		newdir = getenv(home);
		if (newdir == NULL) {
			perror("getenv");
			status = -1;
			return;
		}
		//cout << "HOME is " << *home << endl; 
		olddir = getenv(pwd);
		if (olddir == NULL) {
		//	perror("getenv");
			status = -1;
			return;
		}
		//cout << "pwd is " << *olddir << endl;
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
	else if (strcmp(argv[1], "-") == 0) {
		cout << "cd -" << endl;
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
	else {
		struct stat st;
		status  = lstat(argv[1], &st);
		if (status == -1) {
			perror("lstat");
			return;
		}
		if (S_ISDIR(st.st_mode)) {
			cout << "cd valid_path" << endl;
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
	if (strcmp(argv[0], "cd") == 0) {
		docd(argv, a, status);
		return;
	}
	int ifork = fork();
	//child fuction
	if (ifork == 0) {
		/*
 		struct sigaction cact;
		cact.sa_handler = chihandler;

		if (sigaction(SIGINT, &cact, NULL) < 0) {
			perror ("sigaction");
			exit(-1);
		}
		*/

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
}

void outputdir(char* curdir) {
	char* homedir = NULL;
	const char home[] = "HOME";
	homedir = getenv(home);
	if (homedir == NULL)
		return;
	string curr = curdir;
	string ho = homedir;
	//bool match = false;
	if (ho.size() > curr.size()) {
		cout << ":" << curdir;
		return;
	}
	string sub = curr.substr(0, ho.size());
	//cout << ho << endl;
	//cout << sub << endl;
	//cout << curr << endl;
	if (sub != ho) {
		cout << ":" << curdir;
		return;
	}
	curr = "~" + curr.substr(ho.size());
	cout << ":" << curr;
}

int main() {
	struct sigaction pact;//, cact;
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
	//string comments;
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
		currdir = getenv(pwd);	
		cout << pws->pw_name << "@" << mach;
		if (currdir != NULL)
			outputdir(currdir);
		//	cout << ":" << currdir;
		cout << " $ "; // << flush;
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
			//if (cur_com[0] == 'c' && cur_com[1] == 'd') {
			//	docd(cur_com, status);
			//}
			//else {
			docommand(cur_com, status);
			//}
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
