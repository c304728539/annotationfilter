#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<functional>
#include<map>
#include<stack>
using namespace std;

class AnnotationFilter {
private:
	string source;
	stack<pair<size_t, size_t>> notes;
public:
	explicit AnnotationFilter(string& filename){
		try {
			source = readfile(filename);
		}
		catch (int errorr_num) {
			char Buf[128];
			strerror_s(Buf, sizeof(Buf), errorr_num);
			cerr << "somecall fail errno:" << errorr_num << '(' << Buf << ')';
		}
	}

	AnnotationFilter(const AnnotationFilter&) = delete;
	~AnnotationFilter() = default;

	void Filter() {
		size_t offset = 0;
		while (offset != string::npos)
		{
			findTag(offset);
		}
		while (!notes.empty())
		{
			auto& r = notes.top();
			source.erase(r.first, r.second - r.first);
			notes.pop();
		}
	}

	void saveasfile(const string& filename) const {
		ofstream ofs(filename);
		ofs << source;
		cout << source;
	}

	const string& Source() const {
		return source;
	}
private:
	string readfile(const string& filename) {
		ifstream ifs(filename, ios::in);
		if (ifs) {
			string source;
			ifs.seekg(0, ios::end);
			source.resize((size_t)ifs.tellg());
			ifs.seekg(0, ios::beg);
			ifs.read(&source[0], source.size());
			ifs.close();
			source.erase(source.find('\0'), string::npos);
			return move(source);
		}
		throw(errno);
	}

	void findTag(size_t& offset) {
		const map<size_t, function<size_t(const string&, stack<pair<size_t, size_t>>&, size_t)>> handler = {
			{ source.find('\'', offset), handlers[0] },
			{ source.find('\"', offset), handlers[1] },
			{ source.find("//", offset), handlers[2] },
			{ source.find("/*", offset), handlers[3] }
		};

		if ((offset = handler.begin()->first) != string::npos)
			offset = handler.begin()->second(source, notes, offset + 1) + 1;
	}

	static const function<size_t(const string&, stack<pair<size_t, size_t>>&, size_t)> handlers[4];
};

const function<size_t(const string&, stack<pair<size_t, size_t>>&, size_t)> AnnotationFilter::handlers[4] = {
[](const string& source, stack<pair<size_t, size_t>>& notes, size_t offset) {
			size_t end;
			do {
				end = min(source.find('\'',offset), source.find('\n', offset));
			} while (end != string::npos && source[end - 1] == '\\');
			return end;
},
[](const string& source, stack<pair<size_t, size_t>>& notes, size_t offset) {
			size_t end;
			do {
				end = min(source.find('\"',offset), source.find('\n', offset));
			} while (end != string::npos && source[end - 1] == '\\');
			return end;
},
[](const string& source, stack<pair<size_t, size_t>>& notes,  size_t offset) {
			size_t end = source.find("\n",offset);
			notes.push(make_pair(offset - 1, end));
			return end;
},
[](const string& source, stack<pair<size_t, size_t>>& notes, size_t offset) {
			size_t end = source.find("*/",offset);
			notes.push(make_pair(offset - 1, end + 2));
			return end;
}
};

int main(int argc,char* argv[]) {
	if (argc == 1) return 0;
	AnnotationFilter Fil(static_cast<string>(argv[1]));
	Fil.Filter();
	Fil.saveasfile("result.cpp");
	return 0;	
}