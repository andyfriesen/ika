
#ifndef EVENTS_H
#define EVENTS_H

// Lots of little event arg structs.  Nothing special.

struct Map;

struct MapEvent
{
    Map* map;
    int  arg; // context dependant.
    MapEvent(Map* m, int a = 0)
        : map(m), arg(a)
    {}
};

#endif