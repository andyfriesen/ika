
#ifndef TOKENIZE_H
#define TOKENIZE_H

#include <stdio.h>
#include "fileio.h"
#include "strk.h"

class CTokenizer
{
protected:
//	File f;
	int		nCurline;
	int		nLines;

	char	GetChar();
	char	PeekChar();

	void	CountLines();

	char*	pData;					// pointer to all the data in the file (we're reading it all at once)
	int		nDatasize;				// size of all the data
	int		nCurpos;

public:
	CTokenizer();
	~CTokenizer();

	string_k GetLine();

	string_k Get();
	string_k Peek();

	virtual void	EatWhiteSpace();

	bool	IsNumber(string_k tok);
	virtual bool	IsSymbol(string_k tok);
	bool	IsOperator(string_k tok);
	bool	IsLetter(string_k tok);
	bool	IsFloat (string_k tok);
	bool	IsString(string_k tok);

	virtual bool	SkipCppComment(const string_k& s);
	virtual bool	SkipCComment(const string_k& s);

	int		GetPos()	{ return nCurpos; }
	void	Seek(int pos,int line)	{ nCurpos=pos;	nCurline=line; }

	bool	Open(string_k filename);
	void	Close();

	int		CurLine()	{	return nCurline;}
	int		NumLines()	{	return nLines;	}
	bool	Eof();
};

#endif