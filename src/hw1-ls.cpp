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
#include <algorithm>
#include <string>
#include <cstring>

using namespace std;

//used to sort ignoring case for vector of strings
struct LessNoCase {
	bool operator()(const std::string &x, const std::string &y) const {
		return (strcasecmp (x.c_str(), y.c_str()) < 0);
	}
};

int plainls() {
	vector<string> v;
	DIR *dirp;
        if(NULL == (dirp = opendir("."))) {
            perror("There was an error with opendir(). ");
            exit(1);
        }
        struct dirent* filespecs;
        errno = 0;
        while(NULL != (filespecs = readdir(dirp))) {
		char dot = filespecs->d_name[0];
		if (dot != '.')
			v.push_back(filespecs->d_name);
			//cout << filespecs->d_name << " ";
        }
        if(errno != 0)
        {
            perror("There was an error with readdir(). ");
            exit(1);
        }
        if(-1 == closedir(dirp))
        {
            perror("There was an error with closedir(). ");
            exit(1);
        }
	sort(v.begin(), v.end(), LessNoCase());
	for (vector<string>::iterator it = v.begin(); it != v.end(); it++) {
		cout << *it << "  ";
		//add formating here
	}
	cout << endl;
	exit(0);
}

void lls(int argc,char* argv[],bool a, int dwd) {
	cout << "in l" << endl; 
}

void Rls(int argc,char* argv[], bool a,bool l, int dwd) {
 	cout << "in R" << endl;
}

//for -a parameter without any other parameters
void als(int argc,char* argv[], int dwd) {
	//bool donewdash = false;
	//for (int i = dwd; i < argc; i++) {
		//if (!donewdash && argv[i][0] == '-')
		//	continue;
		//else if (donewdash == false)
		//	donewdash = true;
	vector<string> v;
	DIR *dirp;
	if(NULL == (dirp = opendir("."))) {
            	perror("There was an error with opendir(). ");
            	exit(1);
        }
        struct dirent *filespecs;
        errno = 0;
        while(NULL != (filespecs = readdir(dirp))) {
		v.push_back(filespecs->d_name);
		//cout << filespecs->d_name << "  ";
	}
	if(errno != 0) {
		perror("There was an error with readdir(). ");
		exit(1);
	}
        cout << endl;
        if(-1 == closedir(dirp)) {
            	perror("There was an error with closedir(). ");
            	exit(1);
	}
	sort(v.begin(), v.end(), LessNoCase());
	for (vector<string>::iterator it = v.begin(); it != v.end(); it++) {
		cout << *it << "  ";
		//add formating here
	}
	cout << endl;
	exit(0);
}

//for files passed in as parameters, with or without -a (though not -l or -R)
void filels(int argc,char* argv[], bool a, int dwd) {
	vector<vector<string> > vdir;
	vector<string> v; 
	for (int i = dwd; i < argc; i++) {
		//if (!donewdash && argv[i][0] == '-')
		//	continue;
		//else if (donewdash == false)
		//	donewdash = true;
		vector<string> dirfiles;
		cout << "testing, i: " << i << endl;
		cout << "argv[i]: " << argv[i] << endl;
		DIR *dirp;
		if(NULL == (dirp = opendir(argv[i]))) {
            		perror("There was an error with opendir(). ");
            		exit(1);
        	}
        	struct dirent *filespecs;
        	errno = 0;
		bool dir = false;
		struct stat st;
		int err = lstat(argv[i], &st);
		if (err == -1) {
			perror("There was an error with lstat.");
			exit(1);
		}
		if (S_ISDIR(st.st_mode)) {
			dir = true;
			dirfiles.push_back(argv[i]);
		}
        	while(NULL != (filespecs = readdir(dirp))) {
			//cout << argv[i] << ":" << endl;
			if (a) {
				if (dir)
					dirfiles.push_back(filespecs->d_name);
				else
					v.push_back(filespecs->d_name);
			}
			else {
				char dot = filespecs->d_name[0];
				if (dot != '.') {
					if (dir)
						dirfiles.push_back(filespecs->d_name);
					else
						v.push_back(filespecs->d_name);	
				}
			}
		}
		if(errno != 0) {
			perror("There was an error with readdir(). ");
			exit(1);
		}
        	cout << endl;
        	if(-1 == closedir(dirp)) {
            		perror("There was an error with closedir(). ");
            		exit(1);
		}
	}
	sort(v.begin(), v.end(), LessNoCase());
	for (vector<string>::iterator it = v.begin(); it != v.end(); it++) {
		cout << *it << "	";
		//add formating here
	}
	cout << endl;
	if (!vdir.empty()) {
		cout << endl;
		for (vector<vector<string> >::iterator it = vdir.begin(); it != vdir.end(); it++) {
			cout << (*it).at(0) << ":" << endl;
			for (vector<string>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++)
				cout << *it2 << "	";
			if (it + 1 != vdir.end())
				cout << endl;
		}
	}
	exit(0);
}

int main(int argc, char* argv[]) {
	bool l = false;
	bool a = false;
	bool R = false;
	//bool files = false;
	string curr;
	if (argc == 1) {
		plainls();
	}
	int dwd = 0;
	for (int i = 1; i < argc; i++) {
		curr = argv[i];
		if (curr[0] == '-') {
			for (unsigned j = 1; j < curr.size(); j++) {
				if (curr[j] == 'a')
					a = true;
				else if (curr[j] == 'l')
					l = true;
				else if (curr[j] == 'R')
					R = true;
			}
		}
		else {
			//if (i + 1 < argc)
			//	files = true;
			dwd = i;
			i = argc;
		}
	}
	if (R)
		Rls(argc, argv, a, l, dwd);
	else if (l)
		lls(argc, argv, a, dwd);
	else if (dwd == 0)
		filels(argc, argv, a, dwd);
	else //if (a)
		als(argc, argv, dwd);
	return 0;
}
