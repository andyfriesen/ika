#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <assert.h>

/*

	Any class used with this MUST have a void Init() method (sets the resource up) and a Free() method (cleanup).

	TODO: see if there's a more efficient way to manage things than this old thing.

*/
template <class T>
class CController
{
protected:
	std::vector<T>	stuff;
	std::vector<bool>	bUsed;
	int nCount;

public:
	CController<T>() : nCount(0)
	{
		stuff.resize(nCount);
		bUsed.resize(nCount);
		for (int i=0; i<nCount; i++)
			bUsed[i]=false;
	}

	inline int	GetNew()			// gets a new one.
	{
		for (int i=0; i<nCount; i++)
			if (!bUsed[i])
			{
				bUsed[i]=true;
				stuff[i].Init();
				return i;
			}

		T dummy;

		stuff.push_back(dummy);
		bUsed.push_back(true);
		return nCount++;
	}

	inline void Free(int i)		// nukes one
	{
		if (bUsed[i])
		{
			bUsed[i]=false;
			stuff[i].Free();
		}
	}

	inline void Free()				// nukes 'em all
	{
		for (int i=0; i<nCount; i++)
			Free(i);
	}

	inline bool IsValid(int i) const
	{
		if (i<0 || i>=nCount)
			return false;

		return bUsed[i];
	}

	inline T& operator [] (int offset)
	{
		assert(offset>=0 && offset<nCount);
		assert(bUsed[offset]);
/*#ifdef _DEBUG
		static T dummy;

		if (offset<0 || offset>nCount)
		{
			log("controller: offset out of bounds DO SOME DEBUGGING");
			return dummy;
		}

		if (!bUsed[offset])
		{
			log("controller: attempt to access unused resource DO SOME DEBUGGING");
			return dummy;
		}
#endif*/
		return stuff[offset];
	}

	inline int Count() const
	{
		return nCount;
	}
};

#endif