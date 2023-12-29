/**
 * vectordb.cpp
 * C++ source file that should contain implementation for member functions
 * - rows()
 * - get()
 * - add()
 * - update()
 * - remove()
 *
 * You need to modify this file to implement the above-mentioned member functions
 * as specified in the hand-out (Task 3)
 */
#include "abstractdb.hpp"
#include "vectordb.hpp"
#include <algorithm>

namespace nwen {
    int VectorDbTable::rows()
    {
        return movs.size();
    }

    movie* VectorDbTable::get(int a)
    {
        // check if element exists by looking at vector size
        if (movs.size() <= a)
            return nullptr;

        return &movs.at(a);
    }

    bool VectorDbTable::add(movie m)
    {
        // check for duplicates
        for (const movie& cmp : movs)
            if (cmp.id == m.id)
                return false;
        // otherwise add it
        movs.push_back(m);
        return true;
    }

    bool VectorDbTable::update(unsigned long int id, movie m)
    {
        //loop through vector and search for id if found set the element to m
        for (movie& existingMovie : movs) {
            if (existingMovie.id == id) {
                existingMovie = m;
                return true;
            }
        }
        return false; // Movie with the given ID not found
    }

    bool VectorDbTable::remove(unsigned long int id)
    {
        // iterate using a for instead of for each to have an iterator datatype
        for (std::vector<movie>::iterator line = movs.begin(); line != movs.end(); ++line) {
            if (line->id == id) {
                //this iterator is easier to work with for the erase() method
                movs.erase(line);
                return true;
            }
        }
        return false; // Movie with the given ID not found
    }
}
