/**
 * abstractdb.hpp
 * C++ header file that should contain declaration for
 * - struct movie (given)
 * - AbstractDbTable abstract class
 * 
 * You need to modify this file to declare AbstractDbTable abstract class 
 * as specified in the hand-out (Task 1)
 */ 



#ifndef __ABSTRACT_DB_HPP__
#define __ABSTRACT_DB_HPP__

#include <string>
#include <iostream> // input-output library
#include <unistd.h>
#include <vector>
using namespace std;

namespace nwen 
{
    // declearing the basic datatype
    struct movie {
        unsigned long id;
        char title[50];
        unsigned short year;
        char director[50];
    };
    // declearing all the virtual methods
    class AbstractDbTable{
        public:
            virtual int rows() = 0;
            virtual movie* get(int a) = 0;
            virtual bool add(movie m) = 0;
            virtual bool update(unsigned long int a, movie m) = 0;
            virtual bool remove(unsigned long int a) = 0;
            bool loadCSV(string s);
            bool saveCSV(string s);
    };
}
#endif /* __ABSTRACT_DB_HPP__ */
