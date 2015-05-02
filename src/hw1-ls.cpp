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
#include <grp.h>
#include <time.h>
#include <pwd.h>

using namespace std;

//used to sort ignoring case for vector of strings
struct LessNoCase {
	bool operator()(const std::string &x, const std::string &y) const {
		return (strcasecmp (x.c_str(), y.c_str()) < 0);
	}
};

//for ls without any arguments passed in
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
		}
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
	}
	if (!v.empty())
		cout << endl;
	exit(0);
}

//gets all files in given file name, in a vector
void getfiles(vector<string> &v, string &s, bool a) {
	DIR *dirp;
        if(NULL == (dirp = opendir(s.c_str()))) {
            perror("There was an error with opendir(). ");
            exit(1);
        }

	struct dirent *filespecs;
        errno = 0;
        while(NULL != (filespecs = readdir(dirp))) {
		if (a)
			v.push_back(filespecs->d_name);
		else {
			char dot = filespecs->d_name[0];
			if (dot != '.')
				v.push_back(filespecs->d_name);
		}
        }
        if(errno != 0) {
            perror("There was an error with readdir(). ");
            exit(1);
        }
        //cout << endl;
        if(-1 == closedir(dirp)) {
            perror("There was an error with closedir(). ");
            exit(1);
        }
	sort(v.begin(), v.end(), LessNoCase());
}

//prints out a single file in -l format, uses stat and name of file
void printstat(struct stat &st, string s) {
	//DIR d, CHR c, BLK b, LNK l, regular -
	int temp = st.st_mode & S_IFMT; 
	if (temp == S_IFREG) { cout << "-"; }
	else if (temp == S_IFDIR) { cout << "d"; }
	else if (temp == S_IFCHR) { cout << "c"; }
	else if (temp == S_IFBLK) { cout << "b"; }
	else if (temp == S_IFLNK) { cout << "l"; }
	else cout << "?";
	(st.st_mode & S_IRUSR) ? (cout << "r"):(cout << "-");
	(st.st_mode & S_IWUSR) ? (cout << "w"):(cout << "-");
	(st.st_mode & S_IXUSR) ? (cout << "x"):(cout << "-");
	(st.st_mode & S_IRGRP) ? (cout << "r"):(cout << "-");
	(st.st_mode & S_IWGRP) ? (cout << "w"):(cout << "-");
	(st.st_mode & S_IXGRP) ? (cout << "x"):(cout << "-");
	(st.st_mode & S_IROTH) ? (cout << "r"):(cout << "-");
	(st.st_mode & S_IWOTH) ? (cout << "w"):(cout << "-");
	(st.st_mode & S_IXOTH) ? (cout << "x"):(cout << "-");
	cout << "  " << st.st_nlink << "  ";
	struct passwd *pws = getpwuid(st.st_uid);
	if (!pws) {
		perror("error getting user");
		exit(1);
	}
	struct group *gr = getgrgid(st.st_gid);
	if (!gr) {
		perror("error getting group");
		exit(1);
	}
	if (!pws)
		perror("error getpwuid");
	cout << pws->pw_name << "  " << gr->gr_name;
	if (temp == S_IFREG)
		cout << "  " << st.st_size << "  ";
	else
		cout << "  " << (st.st_blocks * 512) << "  ";
	//convert to date
	struct tm date;
	if (localtime_r(&st.st_mtime, &date) == 0) {
		perror("error getting time");
	}
	char buffer[16];
	if (strftime(buffer, 16,"%b %e %R", &date) == 0) {
		//return value 0 does not necessarily indicate an error
		//thus why perror is not used, though there is error checking
		fprintf(stderr, "strftime returned 0");
		exit(0);
	}
	cout << buffer << " " << s << endl; 
}

//takes in file s, gets all files contained in it, prints them out
void dashl(string s, bool a) {
	int total = 0;
	vector<string> v;
	getfiles(v, s, a);
	vector<struct stat> vstat;
	
	for (vector<string>::iterator it = v.begin(); it != v.end(); it++) {
		struct stat st;
		if (stat((s + "/" + *it).c_str(), &st) == -1) {
			perror("stat");
			exit(1);
		}
		vstat.push_back(st);
		total += st.st_blocks;
	}
	cout << "total " << (total/2)  << endl;
	for (unsigned i = 0; i < v.size() && i < vstat.size(); i++) {
		//DIR d, CHR c, BLK b, LNK l, regular -
		printstat(vstat.at(i), v.at(i));
	}
	exit(0);
}

//only l , and maybe 'a' flag also, passed in, but no R flag
void lls(bool a) {
	dashl(".", a);
}

void recurrdashl(string s, bool a) {
	int total = 0;
	vector<string> v;
	vector<string> dir;
	getfiles(v, s, a);
	vector<struct stat> vstat;
	for (vector<string>::iterator it = v.begin(); it != v.end(); it++) {
		struct stat st;
		if (lstat((s + "/" + *it).c_str(), &st) == -1) {
			perror("stat");
			exit(1);
		}
		if ((st.st_mode & S_IFMT) == S_IFDIR) {
			if ((*it) != "." &&  (*it) != "..")
				dir.push_back(s + "/" + *it);
		}
		vstat.push_back(st);
		total += st.st_blocks;
	}
	cout << s << ":" << endl;
	cout << "total " << (total/2)  << endl;
	for (unsigned i = 0; i < v.size() && i < vstat.size(); i++) {
		printstat(vstat.at(i), v.at(i));
	}
	if (!dir.empty()) {
		cout << endl;
	}
	for (unsigned i = 0; i < dir.size() && i < dir.size(); i++) {
		recurrdashl(dir[i], a);
		if (i + 1 < dir.size())
			cout  <<  endl;
	}
}

//no files, but both -R and -l flags passed in
void lRnofilesls(bool a) {
	recurrdashl(".", a);
}

//Recursive l flags, with files passed in
void lRls(vector<string> &v, vector<string> &dir, bool a) {
	vector<struct stat> vstat;
	for (vector<string>::iterator it = v.begin(); it != v.end(); it++) {
		struct stat st;
		if (stat((*it).c_str(), &st) == -1) {
			perror("stat");
			exit(1);
		}
		vstat.push_back(st);
	}
	for (unsigned i = 0; i <  v.size() && i <  vstat.size(); i++) {
		printstat(vstat.at(i), v.at(i));
	}
	if (!dir.empty())
		cout << endl;
	for (unsigned i = 0; i < dir.size(); i++) {
		recurrdashl(dir[i], a);
		if (i + 1 < dir.size())
			cout << endl;
	}
}

//files and directories without recursion
void lfilels(vector<string> &files, vector<string> &dir, bool a) {
	vector<struct stat> vstat;
	for (vector<string>::iterator it = files.begin(); it != files.end(); it++) {
		struct stat st;
			if (stat((*it).c_str(), &st) == -1) {
				perror("stat");
				exit(1);
			}
		vstat.push_back(st);
	}
	for (unsigned i = 0; i < files.size() && i < vstat.size(); i++) {
		printstat(vstat.at(i), files.at(i));
	}

	for (vector<string>::iterator it = dir.begin(); it != dir.end(); it++) {
		if (!files.empty()) {
			cout << endl;
			cout << *it << ":" << endl;
		}
		dashl(*it, a);
	}
}

//takes in file (string s), fills 2 vectors with the directories and non-directories
void getdiroth(vector<string> &files, vector<string> &dir, string s, bool a) {
	vector<string> v;
	getfiles(v, s, a);
	for (unsigned i = 0; i < v.size(); i++) {
		struct stat st;
		int err = lstat((s + "/" + v[i]).c_str(), &st);
		if (err == -1) {
			perror("error with lstat.");
			exit(1);
		}
		if (S_ISDIR(st.st_mode)) {
			if(v[i] == "." || v[i] == "..")
				files.push_back(v.at(i));
			else
				dir.push_back(v.at(i));
		}
		else
			files.push_back(v.at(i));
	}	
}

//recursive ls without -l flag
void RecurRls(string s, bool a) {
	cout << s << ":" << endl;
	vector<string> v;
	vector<string> dir;
	getdiroth(v, dir, s, a);
	v.insert(v.end(), dir.begin(), dir.end());
	sort(v.begin(), v.end(), LessNoCase()); 
	for (unsigned i = 0; i < v.size(); i++) {
		cout << v.at(i);
		if (i + 1 < v.size())
			cout << "  ";
	}
	if (!v.empty()) 
		cout << endl;
	if (!dir.empty())
		cout << endl;
	for (unsigned i = 0; i < dir.size(); i++) {
		RecurRls(s + "/" + dir.at(i), a);
		if (i + 1 < dir.size())
			cout << endl;
	}
}

//no L flag, but recursive flag, no specific files
void Rls(bool a) {
	cout << ".:" << endl;
	vector<string> v;
	vector<string> dir;
	getdiroth(v, dir, ".", a);
	v.insert(v.end(), dir.begin(), dir.end());
	sort(v.begin(), v.end(), LessNoCase());
	for (unsigned i = 0; i < v.size(); i++) {
		cout << v.at(i);
		if (i + 1 < v.size())
			cout << "  ";
	}
	if (!v.empty())
		cout << endl;
	if (!dir.empty())
		cout << endl;
	for (unsigned i = 0; i < dir.size(); i++) { 
		RecurRls("./" + dir.at(i), a);
		if (i + 1 < dir.size())
			cout << endl;
	}
}

//no L flag, but recursive flag on specific files
void Rfiles(vector<string> &v, vector<string> &dir, bool a) {
	sort(v.begin(), v.end(), LessNoCase());
	sort(dir.begin(), dir.end(), LessNoCase());
	for (unsigned i = 0; i < v.size(); i++) {
		cout << v.at(i);
		if (i + 1 < v.size())
			cout << "  ";
	}
	if (!v.empty()) 
		cout << endl;
	if (!dir.empty())
		cout << endl;
	for (unsigned i = 0; i < dir.size(); i++) {
		RecurRls(dir.at(i), a);
		if (i + 1 < dir.size())
			cout << endl;
	}
}

//no l or R flags, but specific files and dir
void filels(vector<string> &files, vector<string> &dir, bool a) {
	sort(files.begin(), files.end(), LessNoCase());
	sort(dir.begin(), dir.end(), LessNoCase());
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
					cout << curr[j] << " is a invalid flag." << endl;
			}
		}
		else {
			donewithdash = true;
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
		else if (files.empty() && dir.empty() && R)
			lRnofilesls(a);
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
