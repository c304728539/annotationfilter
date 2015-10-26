#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<functional>
#include<map>
#include<stack>
using namespace std;

enum Otype {Char,String,Note,SecNote};

inline pair<Otype, size_t> min4(size_t a,size_t b,size_t c, size_t d){
	return (a<b&&a<c&&a<d)?make_pair(Char,a):((b<c&&b<d)? make_pair(String, b):(c<d)? make_pair(Note, c) : make_pair(SecNote, d));
}

pair<Otype, size_t> findTag(const string& source, size_t offset) {
	size_t offsetchar = source.find('\'', offset);	//for char
	size_t offsetStr = source.find('\"', offset);	//for string}
	size_t offsetNote = source.find("//", offset);	//for note like //
	size_t offsetSecNote = source.find("/*", offset);	//for section note like /**/
	return min4(offsetchar, offsetStr, offsetNote, offsetSecNote);
}

stack<pair<size_t, size_t>> notes;

map<Otype, function<size_t(const string&, size_t)>> handler = {
	{ Char,
	[](const string& source, size_t offset) {
	size_t end;
	do {
		end = min(source.find('\'',offset), source.find('\n', offset));
	} while (end != string::npos && source[end - 1] == '\\');
	return end;
}
	}
	,
	{String,
	[](const string& source, size_t offset) {
		size_t end;
		do {
			 end =min(source.find('\"',offset), source.find('\n', offset));
		} while (end!=string::npos && source[end-1]=='\\');
		return end;
		}
	}
	,
	{Note,
	[](const string& source, size_t offset) {
			size_t end = source.find("\n",offset);
			notes.push(make_pair(offset-1, end));
			return end; 
		}
	}
	,
	{SecNote,
	[](const string& source, size_t offset) {
			size_t end = source.find("*/",offset);
			notes.push(make_pair(offset-1, end+2));
			return end;
		}
	}
};

string readfile(const string& filename) {
	ifstream ifs(filename, ios::in);
	if(ifs){
		string source;
		ifs.seekg(0, ios::end);
		source.resize((size_t)ifs.tellg());
		ifs.seekg(0, ios::beg);
		ifs.read(&source[0], source.size());
		ifs.close();
		source.erase(source.find('\0'),string::npos);
		return move(source);
	}
	throw(errno);
}

int main(int argc,char* argv[]) {
	if (argc == 1) return 0;
	string source = readfile(argv[1]);
	pair<Otype, size_t> r;
	size_t offset = 0;
	while ((r = findTag(source,offset)).second != string::npos)
	{
		offset = handler[r.first](source,r.second+1) + 1;
	}
	while (!notes.empty())
	{
		auto& r =notes.top();
		source.erase(r.first, r.second - r.first);

		notes.pop();
	}
	ofstream ofs("result.txt");
	ofs << source;
	cout << source;

	return 0;	
}