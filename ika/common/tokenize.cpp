#include <stdlib.h>
#include "tokenize.h"

inline char lcase(char c)
{
	if (c>='A' && c<='Z')
		c|=32;
	return c;
}

CTokenizer::CTokenizer()
{
	pData=NULL;
	nDatasize=0;
}

CTokenizer::~CTokenizer()
{
	Close();
}

char CTokenizer::GetChar()
{
	char c;

	if (nCurpos<nDatasize)
		c=pData[nCurpos++];
	else
		return 1;

	if (c=='\n')
		nCurline++;

	return c;
}

string_k CTokenizer::GetLine()
{
	string_k line("");
	char c;

	do
	{
		c=PeekChar();
		if (c!='\n')
		{
			line+=c;
			GetChar();
		}
	} while (c!='\n' && !Eof());

	line=line.left(line.length()-1);	// clip the \n off
	return line;
}

void CTokenizer::CountLines()
{
	string_k t;

	nLines=0;

	while (1)
	{
		if (Eof())
			break;
		nLines++;
		t=GetLine();
		GetChar();	// get the \n
	}
	
	nCurpos=0;
	nCurline=0;
}

bool CTokenizer::Eof()
{ 
	if (nCurpos<nDatasize)
		return false;
	return true;
}

char CTokenizer::PeekChar()
{
	char c;
	if (nCurpos<nDatasize)
		c=pData[nCurpos];
	else
		return 1;

	if (c>='A' && c<='Z')	c|=32;					// force lowercase

	return c;
}

void CTokenizer::EatWhiteSpace()
{
	char c;
	
	while (1)
	{
		c=PeekChar();
		if (c==' ' || c=='\t' || c=='\r')
		{
			c=GetChar();
			continue;
		}
		if (c=='\n')			// we ignore these, cuz they don't tell us much. ^_^
		{
			c=GetChar();
			continue;
		}
		break;
	}
}

bool CTokenizer::SkipCComment(const string_k& s)
{
	char peek;
	if (s=="/*")
	{
		GetChar();	// toss the *
		do
		{
			peek=GetChar();
			if (peek=='*' && Peek()=='/')
				break;
		} while (1);
		GetChar();	// toss the /
		return true;
	}
	return false;
}

bool CTokenizer::SkipCppComment(const string_k& s)
{
	char peek;
	if (s=="//")
	{
		GetChar();	// toss the /
		do
		{
			peek=PeekChar();
			
			if (peek==10)
				break;
			
			GetChar();
		}
		while (peek!=10);
		return true;
	}
	return false;
}
	
string_k CTokenizer::Get()
{
	string_k s;
	char c;

	EatWhiteSpace();
	c=GetChar();
	
	s=c;

	if (IsLetter(c))								// Identifier.  Get numbers/letters.
	{
		s=lcase(c);
		while (1)
		{
			c=lcase(PeekChar());
			if (!IsLetter(c) && !IsNumber(c) && c!='.')
				break;
			c=lcase(GetChar());
			s+=c;
		}
		return s;
	}

	if (IsNumber(c))
	{
		bool IsFloat=false;
		while (1)
		{
			c=PeekChar();
			if (c=='.' && !IsFloat)
				IsFloat=true;
			else if (!IsNumber(c))
				break;
			c=GetChar();
			s+=c;
		}
		return s; 
	}

	if (c=='\"')									// Quoted string, get everything until we find another quote
	{
		s=c;
		do
		{
			c=GetChar();
			s+=c;
		} while (c!='\"' && c!='\n');
		return s;
	}

	if (IsSymbol(c))								// symbol return it
	{
		char peek;

		peek=PeekChar();
		if ((c=='+' || c=='-' || c=='*' || c=='/') && peek=='=')
			s+=GetChar();							// +=, -=, etc...

		if ((c=='>' || c=='<') && peek=='=')
			s+=GetChar();

		if (c=='=' && peek=='=')	GetChar();			// toss the extra = if it's ==

		if (c=='+' && peek=='+')	s+=GetChar();
		if (c=='-' && peek=='-')	s+=GetChar();
		if (c=='!' && peek=='=')	s+=GetChar();
		if (c=='&' && peek=='&')	s+=GetChar();
		if (c=='|' && peek=='|')	s+=GetChar();
		if (c=='^' && peek=='^')	s+=GetChar();
		if (c=='/' && peek=='/')	s+=GetChar();
		if (c=='/' && peek=='*')	s+=GetChar();
		if (c=='*' && peek=='/')	s+=GetChar();

		// comment parsing
		if (SkipCComment(s))
			return Get();

		if (SkipCppComment(s))
			return Get();

		return s;
	}

	return "";
}

string_k CTokenizer::Peek()
{
	int i;
	int nSavecurline;

	i=nCurpos;
	nSavecurline=nCurline;
	
	string_k s=Get();

	nCurpos=i;
	nCurline=nSavecurline;

	return s;
}

bool CTokenizer::IsNumber(string_k tok)
{
	float f=(float)atof(tok.c_str());

	if (tok=='0' || f!=0.0f)
		return true;
	
	return false;
}

bool CTokenizer::IsSymbol(string_k tok)
{
	if (
		tok=='('	||	tok==')'	||
		tok=='['	||	tok==']'	||
		tok=='{'	||	tok=='}'	||
		tok==';'	||
		tok=='='	||
		tok=='+'	||	tok=='-'	||
		tok=='*'	||	tok=='/'	||
		tok==','	||
		tok=='!'	||	tok=='&'	||
		tok=='|'	||	tok=='^'	||
		tok=='%'	||
		tok=='<'	||	tok=='>'	)
		return true;

	return false;
}

bool CTokenizer::IsOperator(string_k tok)
{
	if (
		tok=='+'	||	tok=='-'	||
		tok=='/'	||	tok=='*'	||
		tok=='<'	||	tok=='>'	||
		tok=="<="	||	tok==">="	||
		tok=='='	||	tok=="!="	||
		tok=="=="	||
		tok=="&&"	||	tok=="||"	||
		tok=="^^"	||
		tok=='%'	||
		tok==')'	||	tok=='('	)
		return true;
	return false;
}

bool CTokenizer::IsLetter(string_k tok)
{
	char c=tok[0];
	if ((c>='a' && c<='z') || (c>='A' && c<='Z') || c=='_')
		return true;
	return false;
}

bool CTokenizer::IsFloat(string_k tok)
{
	return false;
}

bool CTokenizer::IsString(string_k tok)
{
	if (tok[0]=='\"')
		return true;
	return false;
}

bool CTokenizer::Open(string_k filename)
{
	Close();

	File f;

	bool bResult=f.OpenRead(filename.c_str(),true);
	if (!bResult) return false;

	nDatasize=f.Size();

	pData=new char[nDatasize];
	f.Read(pData,nDatasize);	

	nCurpos=0;
	nCurline=0;

	f.Close();

	CountLines();
	return true;
}

void CTokenizer::Close()
{
	if (pData)
		delete[] pData;
	pData=NULL;
	nDatasize=0;
}