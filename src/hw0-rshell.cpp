#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
using namespace boost;


int parseline(char com[], char* argv[]) {
	char** next = argv;

	char* token = strtok(com, " ");
	while (token != NULL) {
		*next++ = token;
		token = strtok(NULL, " ");
	}
	*next = NULL;
	return 0;
}

/*
int parseline(char com[], char* const argv[]) {
	/ *char *token;
	*token = strtok(com, " ");
	int i = 0;
	while(token != NULL ) {
		argv[i] = token;
		*token = strtok(NULL, " ");
		i++;
	 }
	
	char_separator<char> linedelim(" 	");
	tokenizer< char_separator<char> > linetok(com, linedelim);
	int i = 0;
	for (tokenizer< char_separator<char> >::iterator it = linetok.begin(); it != linetok.end(); ++it) {
		argv[i] = (*it).c_str();
		i++;
	}
	return i;
}*/

int docommand(char com[]) {
	//will actually process the commands
	//int max_arg = com.size();
	char* argv[100];
	int a = parseline(com, argv);
	for (int i = 0; i < a; i++) {
		cout << argv[i] << endl;
	} /*
	//int ifork = fork();
	//child fuction
	if (ifork == 0) {
		//execvp(argv[0], argv);
		//this will only be reached if error in running command
	}
	//if error occured
	else if (ifork == -1) {
		//perror
	}
	else {
		//wait();
	}*/
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
		cout << "input: " << input << endl;
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
			char com[100] = cur_com.c_str();
			//if (!cur_com.empty())
			//	status = 
				docommand(com);
			//put connector code here
		}
		
		cout << "comments: " << comments << endl;
		cout << "input: " << input << endl;
		cout << "connectors: " << connectors << endl;
		exit = true;
	}	
	return 0;
}
