/*

Controller
    Knows how to create documents. 
    knows how to destroy documents
    Knows what documents have been created.
    can reliably be asked for a document with some name.
    can clone a document when it is saved under a new name.
    can handle unnamed documents as well as named.


Problem:
    Documents deallocate themselves with delete and not
    the free() method of the deallocator that created them.
*/


/*
 * This code is a bit more complicated than I'd like.  And it searches for 
 * things the most obvious way possible. (optimize that if it becomes an issue)
 */

#pragma once

#include <cassert>
#include <map>

#include "common/utility.h"
#include "common/log.h"
#include "document.h"

namespace iked {

    template <typename T>
    struct Allocator {
        virtual ~Allocator() { }
	virtual T* allocate(const std::string& name) = 0;
	//virtual void free(T* t) = 0;
    };

    template <class T>
    struct DefaultAllocator : Allocator<T> {
	virtual T* allocate(const std::string& name) {
	    return new new T(name);
	}

	/*virtual void free(T* t) {
	    delete t;
	}*/
    };

    /**
     * Document controller for a set of documents which are identified by their filename.
     */
    template <typename T>
    struct Controller {
        Controller()
            : allocator(new DefaultAllocator<T>)
        {
            assert(allocator != 0);
        }

	explicit Controller(Allocator<T>* alloc)
	    : allocator(alloc)
	{
            assert(allocator != 0);
        }

	~Controller() {
            foreach (Document* doc, documents ) {
		Log::Write("Leak detected! %s", doc->getName().c_str());
                delete doc;
	    }
	    delete allocator;
	}

	Document* get(const std::string& name) {
#ifdef WIN32
	    std::string fileName = toUpper(name);
            fileName.replace(
#else
	    std::string fileName = name;
#endif

	    Document* doc = find(fileName);

	    if (doc) {
		Log::Write("Addref       %s", name.c_str());
		doc->ref();
		return doc;

	    } else {
		T* data = 0;
		std::string errorMessage;

		try {
		    data = allocator->allocate(fileName);
		} catch (std::runtime_error& error) {
		    errorMessage = error.what();
		    data = 0;
		}

		if (data == 0) {
		    Log::Write("allocate(\"%s\") failed: %s", name.c_str(), errorMessage.c_str());
		    delete data;
		    return 0;
		}

		documents.insert(data);

		return data;
	    }
	}

	void free(Document* doc) {
	    assert(doc != 0);
	    assert(doc->getRefCount() > 0);

            if (doc->getRefCount() == 1) {
                documents.erase(doc);
            }

	    doc->unref();
	}

        // Test to see if we own this document.
        bool owns(Document* doc) {
            return documents.count(doc) != 0;
        }

        void documentDestroyed(Document* doc) {
            assert(owns(doc));
            documents.remove(doc);
        }

    private:
	Document* find(const std::string& name) {
            foreach (Document* doc, documents) {
		if (Path::equals(doc->getName(), name)) {
		    return doc;
		}
	    }
	    return 0;
	}

	typedef std::set<Document*> DocumentSet;
	DocumentSet documents;

	Allocator<T>* allocator;
    };
}

