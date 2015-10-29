#include<string>
#include<algorithm>
#include<iostream>
#include<fstream>
#include<functional>
#include<map>
#include<stack>

using namespace std;
using namespace placeholders;

class AnnotationFilter {

private:
	string source;
	stack<pair<size_t, size_t>> notes;
public:
	explicit AnnotationFilter(const string& filename){
		try {
			source = readfile(filename);
		}
		catch (int state) {
			if ((state & ifstream::failbit) != 0)
				std::cerr << "Error opening '" << filename << "'\n";
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
			return source;
		}
		throw(ifs.rdstate());
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

	static const function<size_t(const string&, stack<pair<size_t, size_t>>&, string, size_t)> notehdl;
	static const function<size_t(const string&, stack<pair<size_t, size_t>>&, char, size_t)> charhdl;

	static const function<size_t(const string&, stack<pair<size_t, size_t>>&, size_t)> handlers[4];
};

const function<size_t(const string&, stack<pair<size_t, size_t>>&, string, size_t)> AnnotationFilter::notehdl =
[](const string& source, stack<pair<size_t, size_t>>& notes, string str, size_t offset) {
			size_t end = source.find(str,offset + 1);
			notes.push(make_pair(offset - 1, end + ((str=="*/")?2:0)));
			return end;
};

const function<size_t(const string&, stack<pair<size_t, size_t>>&, char, size_t)> AnnotationFilter::charhdl =
[](const string& source, stack<pair<size_t, size_t>>& notes, char c,size_t offset)->size_t {
	size_t end;
	do {
		end = min(source.find(c, offset), source.find('\n', offset));
	} while (end != string::npos && source[end - 1] == '\\');
	return end;
};

const function<size_t(const string&, stack<pair<size_t, size_t>>&, size_t)> AnnotationFilter::handlers[] = {
	bind(charhdl, _1, _2, '\'', _3),
	bind(charhdl, _1, _2, '\"', _3),
	bind(notehdl, _1, _2, "\n", _3),
	bind(notehdl, _1, _2, "*/", _3)
};

int main(int argc,char* argv[]) {
	if (argc == 1) { 
		cerr << "please pass a parameter as filename\n";
		return 0; 
	}
	AnnotationFilter Fil(argv[1]);
	Fil.Filter();
	cout << Fil.Source();
	Fil.saveasfile("result.cpp");
	return 0;	
}