
#ifndef RESOURCE_H
#define RESOURCE_H

// identifier too long.  MS can suck it. ;D
#pragma warning (disable:4786)

#include <list>
#include "strk.h"

template <class T>
class CResourceController
{
	struct Resource : public T
	{
		string_k sName;
		int nRefcount;

		int IncRef() { return ++nRefcount; }
		int DecRef() { return --nRefcount; }

		Resource(const string_k& s) : T(s) {}
	};

	typedef std::list<Resource*> ResourceList;
	typedef ResourceList::iterator ResourceListIterator;

	ResourceList resources;
	
public:

	T* Load(const char* fname)
	{
		for (ResourceListIterator i=resources.begin(); i!=resources.end(); i++)
		{
			if ((*i)->sName==fname)
			{
				(*i)->IncRef();
				return (*i);
			}
		}

		Resource* r=new Resource(fname);
		r->nRefcount=1;
		r->sName=fname;
		resources.push_back(r);

		return r;
	}

	void Free(T* data)
	{
		int result=((Resource*)data)->DecRef();

		if (!result)
		{
			for (ResourceListIterator i=resources.begin(); i!=resources.end(); i++)
			{
				if (data==(*i))
				{
					resources.remove(*i);
					delete data;
					return;
				}
			}
			__asm int 3;	// should never execute
		}
	}

	~CResourceController()
	{
		for (ResourceListIterator i=resources.begin(); i!=resources.end(); i++)
		{
			Resource* r=*i;
			delete r;
		}
		resources.clear();
	}
};

#endif