/*
	This is useful for when you've got data that you want semantically linked with strings, instead of ordinal numbers.
*/

#include <stdarg.h>
#include <string.h>
#include "strk.h"
#include <vector>

const int nHashsize=32;

template	<class T>
struct SHashEntry
{
	char	sWord[255];
	T		data;
};

template	<class T>
class CWordList
{
	typedef SHashEntry<T> HashEntry;
protected:
	std::vector<HashEntry> sTable[nHashsize];
	int	nHashvalue[nHashsize];

	inline int GetHashValue(const char* s) const
	{
		if (strlen(s)<4)
			return 0;	// aribitrary. :)

		return (s[0]+s[3])&31;
	}

public:
	void Clear()
	{
		for (int i=0; i<nHashsize; i++)
			sTable[i].free();
	}

	void Add(string_k s,const T& data)
	{
		HashEntry a;
		strcpy(a.sWord,s.c_str());
		a.data=data;
		
		int h=GetHashValue(s.c_str());
		sTable[h].push_back(a);
	}

	bool Find(string_k s,T& data)
	{
		int h=GetHashValue(s.c_str());
		
		for (int i=0; i<sTable[h].size(); i++)
			if (!stricmp(sTable[h][i].sWord,s.c_str()))
			{
				data=sTable[h][i].data;
				return true;
			}
			
		return false;
	}
};

class CIndexedList : public CWordList<int>
{
private:
	int nElements;
public:
	CIndexedList() { nElements=0; }

/*	void Add(string_k sFirst,...)
	{
		string_k s;
		va_list argptr;

		va_start(argptr,sFirst);
		s=sFirst;

		while (s!="XXX")
		{
			Add(s);
		}

		va_end(argptr);
	}*/

	void AddStringList(const char** sList)
	{
		int i=0;
		string_k s;

		while (1)
		{
			s=string_k(sList[i]);
			if (s=="XXX")
				break;
			Add(string_k(sList[i]),nElements);
			++nElements;

			++i;
		}
	}
};