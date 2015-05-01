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

int plainls(bool a) {
	vector<string> v;
	DIR *dirp;
        if(NULL == (dirp = opendir("."))) {
            perror("There was an error with opendir(). ");
            exit(1);
        }
        struct dirent* filespecs;
        errno = 0;
        while(NULL != (filespecs = readdir(dirp))) {
		if (a)
			v.push_back(filespecs->d_name);
		else {
			char dot = filespecs->d_name[0];
			if (dot != '.')
				v.push_back(filespecs->d_name);
		}	//cout << filespecs->d_name << " ";
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
		cout << *it;
		if (it + 1 != v.end())
			cout  << "  ";
		//add formating here
	}
	if (!v.empty())
		cout << endl;
	exit(0);
}

//only l , and maybe 'a' flag also, passed in, but no R flag
void lls(bool a) {

}

//Recursive l flags
void lRls(vector<string> &files, vector<string> &dir, bool a) {

}

//files and directories without recursion
void lfilels(vector<string> &files, vector<string> &dir, bool a) {

}

//no L flag, but recursive flag, no specific files
void Rls(bool a) {

}

//no L flag, but recursive flag on specific files
void Rfiles(vector<string> &files, vector<string> &dir, bool a) {

}

//no l or R flags, but specific files and dir
void filels(vector<string> &files, vector<string> &dir, bool a) {
	sort(files.begin(), files.end(), LessNoCase());
	sort(dir.begin(), dir.end(), LessNoCase());
	cout << "testing space" << endl;
	for (vector<string>::iterator it = files.begin(); it != files.end(); it++) {
		struct stat st;
		if (stat((*it).c_str(), &st) == -1) {
			perror("error accessing file");
			exit(1);
		}
		cout << *it << "		";
	}
	if (!dir.empty() && !files.empty())
		cout << endl;
	for (vector<string>::iterator it = dir.begin(); it != dir.end(); it++) {
		vector<string> v;
		DIR *dirp;
		if(NULL == (dirp = opendir((*it).c_str()))) {
			perror("error with opendir()");
			exit(1);
		}
		struct dirent *filespecs;
		errno = 0;
		while(NULL != (filespecs = readdir(dirp))) {
			if (a)
				v.push_back(filespecs->d_name);
			else {
				char dot = filespecs->d_name[0];
				if (dot != '.') {
					v.push_back(filespecs->d_name);
				}
			}
		}
		if(errno != 0) {
			perror("error with readdir(). ");
			exit(1);
		}
		cout << endl;
		if(-1 == closedir(dirp)) {
			perror("error with closedir(). ");
			exit(1);
		}
		cout << *it << ":" << endl;
		for (vector<string>::iterator it2 = v.begin(); it2 != v.end(); it2++) {
			cout << *it2;
			if (it2 + 1 != v.end())
				cout  << "		";
		}
		cout << endl;
		if (it + 1 != dir.end())
			cout << endl;
	}
	exit (0);
}


int main(int argc, char* argv[]) {
	bool l = false;
	bool a = false;
	bool R = false;
	//bool files = false;
	vector<string> files;
	vector<string> dir;
	string curr;
	if (argc == 1) {
		plainls(a);
	}
	bool donewithdash = false;
	for (int i = 1; i < argc; i++) {
		curr = argv[i];
		if (curr[0] == '-' && !donewithdash) {
			for (unsigned j = 1; j < curr.size(); j++) {
				if (curr[j] == 'a')
					a = true;
				else if (curr[j] == 'l')
					l = true;
				else if (curr[j] == 'R')
					R = true;
				else
					cout << j << " is a bad flag." << endl;
			}
		}
		else {
			struct stat st;
			int err = lstat(argv[i], &st);
			if (err == -1) {
				perror("error with lstat.");
				exit(1);
			}
			if (S_ISDIR(st.st_mode)) {
				dir.push_back(argv[i]);
			}
			else
				files.push_back(argv[i]);
		}
	}

	if (l) {
		//if this is true, then only l , and maybe 'a' flag also, passed in
		if (files.empty() && dir.empty() && !R)
			lls(a);
		//recurrsive call on files
		else if (R)
			lRls(files, dir, a);
		//files and directories passed in, but no R flag
		else
			lfilels(files, dir, a);
	}
	else if (R) {
		if (files.empty() && dir.empty())
			Rls(a);
		else
			Rfiles(files, dir, a);
	}
	else if (!files.empty() || !dir.empty())
		filels(files, dir, a);
	else
		plainls(a);
	return 0;
}
