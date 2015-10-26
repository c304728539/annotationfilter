
#include<string>
#include<iostream>
#include<fstream>
#include<functional>
#include<map>
#include<stack>
using namespace std;

enum Otype {String,Note,SecNote};

inline size_t min(size_t a, size_t b) {
	return a < b ? a : b;
}

inline pair<Otype, size_t> min3(size_t a,size_t b,size_t c){
	return (a < b&&a < c) ? make_pair(String,a) : (b < c ? make_pair(Note,b) : make_pair(SecNote, c));
}

pair<Otype, size_t> findTag(const string& source, size_t offset) {
	size_t offsetStr = source.find('\"', offset);	//for string
	size_t offsetNote = source.find("//", offset);	//for note like //
	size_t offsetSecNote = source.find("/*", offset);	//for section note like /**/
	return min3(offsetStr, offsetNote, offsetSecNote);
}

stack<pair<size_t, size_t>> notes;

map<Otype, function<size_t(const string&, size_t)>> handler = {
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
	ifstream ifs(filename);
	if(ifs){
		string source;
		ifs.seekg(0, ios::end);
		source.resize((size_t)ifs.tellg());
		ifs.seekg(0, ios::beg);
		ifs.read(&source[0], source.size());
		ifs.close();
		source.shrink_to_fit();
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