/**
 * vectordb.hpp
 * C++ header file that should contain declaration for
 * - VectorDbTable class
 * 
 * You need to modify this file to declare VectorDbTable class 
 * as specified in the hand-out (Task 2)
 */ 

#include "abstractdb.hpp"
#include <unistd.h>
#include <vector>
namespace nwen {
    class VectorDbTable: public AbstractDbTable {
        public:
            // redeclaring all the methods other than load and save
            int rows();
            movie* get(int a);
            bool add(movie m);
            bool update(unsigned long int a, movie m);
            bool remove(unsigned long int a);
        // set the movs variable private so that only the class can access it
        private:
            std::vector<movie> movs;
    };
};
