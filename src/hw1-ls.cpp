#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

using namespace std;

int plainls() {
	
	exit(0);
}

void lls(int argc,char* argv[],bool a) {
 
}

void Rls(int argc,char* argv[], bool a,bool l) {
 
}

void als(int argc,char* argv[]) {

}

void filels(int argc,char* argv[]) {
	DIR *dirp;
        if(NULL == (dirp = opendir(argv[1])))
        {
            perror("There was an error with opendir(). ");
            exit(1);
        }
        struct dirent *filespecs;
        errno = 0;
        while(NULL != (filespecs = readdir(dirp)))
        {
            cout << filespecs->d_name << " ";
        }
        if(errno != 0)
        {
            perror("There was an error with readdir(). ");
            exit(1);
        }
        cout << endl;
        if(-1 == closedir(dirp))
        {
            perror("There was an error with closedir(). ");
            exit(1);
        }
}

int main(int argc, char* argv[]) {
	bool l = false;
	bool a = false;
	bool R = false;
	string curr;
	if (argc == 1) {
		plainls();
	}
	for (int i = 0; i < argc; i++) {
		curr = argv[i];
		if (curr[i] == '-') {
			for (unsigned j = 1; j < curr.size(); j++) {
				if (curr[j] == 'a')
					a = true;
				else if (curr[j] == 'l')
					l = true;
				else if (curr[j] == 'R')
					R = true;
			}
		}
		else
			i = argc;
	}
	if (R)
		Rls(argc, argv, a, l);
	else if (l)
		lls(argc, argv, a);
	else if (a)
		als(argc, argv);
	else
		filels(argc, argv);
	return 0;
}
