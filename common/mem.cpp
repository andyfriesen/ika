
#ifdef _DEBUG

#include <list>
#include "log.h"

struct sMemblock
{
	char fname[255];
	int nLine;
	int nSize;
	void* pMem;
};

static std::list<sMemblock*>* memblocks=NULL;

void AddChunk(const char* sFile,int nLine,int nSize,void* p)
{
	if (!memblocks)
		memblocks=new(std::list<sMemblock*>);

	sMemblock* m=new(sMemblock);

	strcpy(m->fname,sFile);
	m->nLine=nLine;
	m->nSize=nSize;
	m->pMem=p;

	memblocks->push_back(m);
}

void NukeChunk(void* p)
{
	std::list<sMemblock*>::iterator i;

	if (!memblocks)
		return;

	for (i=memblocks->begin(); i!=memblocks->end(); i++)
	{
		if ((*i)->pMem==p)
		{
			memblocks->remove(*i);
			return;
		}
	}
}

void* operator new (unsigned int nSize,const char* sFile,int nLine)
{
	void* p=malloc(nSize);

	AddChunk(sFile,nLine,nSize,p);

	return p;
}

void operator delete(void* p)
{
	if (!p)
		return;

	NukeChunk(p);
	free(p);
}

#endif	// #ifdef _debug

void DumpList()
{
#ifdef _DEBUG

	std::list<sMemblock*>::iterator i;

	if (!memblocks)
		return;

	for (i=memblocks->begin(); i!=memblocks->end(); i++)
	{
		Log::Write("File: %s\tLine: %i\tSize: %i\tLocation: %i",(*i)->fname,(*i)->nLine,(*i)->nSize,(int)((*i)->pMem));
//		memblocks->remove(*i);
	}
	Log::Write("done!");

#endif
}

