#if 0

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#pragma warning(disable:4511)
#pragma warning(disable:4512)

class Writer : public ofstream
{
public:
	Writer(const char *name) :
		ofstream(name, ios_base::out | ios_base::binary)
	{
	}

	template <class T>
	friend Writer& operator <<(Writer &w, const T &f)
	{
		w.write((char*)&f, sizeof(f));
		return w;
	}

	template<class T>
	friend Writer& operator <<(Writer &w, const vector<T> &v)
	{
		size_t s = v.size();
		w << s;
		for (size_t i=0; i<s; i++)
			w << v[i];
		return w;
	}

	friend Writer& operator <<(Writer &w, const string &v)
	{
		size_t s = v.length();
		w << s;
		w.write(v.data(), streamsize(s));
		return w;
	}
};

class Reader : public ifstream
{
public:
	Reader(const char *name) :
		ifstream(name, ios_base::in | ios_base::binary)
	{
	}

	template <class T>
	friend Reader& operator >>(Reader &r, T &f)
	{
		r.read((char*)&f, sizeof(f));
		return r;
	}

	template<class T>
	friend Reader& operator >>(Reader &r, vector<T> &v)
	{
		size_t s;
		r >> s;
		v.resize(s);
		for (size_t i=0; i<s; i++)
			r >> v[i];
		return r;
	}

	friend Reader& operator >>(Reader &r, string &v)
	{
		size_t s;
		r >> s;
		v.resize(s);
		r.read(&v[0], streamsize(s));
		return r;
	}
};

#endif
