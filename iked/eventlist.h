#pragma once
#include "common/utility.h"

#include <vector>

    template <typename T>
    struct EventList {
        typedef std::vector<T*>::iterator iterator;

        void insert(const T& e) {
            if (Find(e) != 0) {
                return;
            }

            T* evt=e.Copy();
            events->push_back(evt);
        }

        void remove(const T& e) {
            iterator evt = Find(e);
            if (evt == 0) {
                return;
            }

            T* ptr = *evt;
            delete ptr;

            events->erase(evt);
        }

        iterator find(const T& e) {
            // Brute force search.
            for (iterator i = begin(); i != end(); i++) {
                if ((*i)->Value() == e.Value())
                    return i;
            }
            return end();
        }

        size_t size() const {
            return events->size();
        }

        iterator begin() {
            return events->begin();
        }

        iterator end() {
            return events->end();
        }

        void clear() {
            for (iterator i = events->begin(); i != events->end(); i++) {
                T* e = *i;
                delete e;
            }
            events->clear();
        }

        ~EventList() {
            clear();
        }

    private:
        std::vector<T*> events;
    };

};
