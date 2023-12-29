/**
 * abstractdb.cpp
 * C++ source file that should contain implementation for member functions
 * - loadCSV()
 * - saveCSV()
 *
 * You need to modify this file to implement the above-mentioned member functions
 * as specified in the hand-out (Tasks 4 and 5)
 */
#include "abstractdb.hpp"
#include <sstream>
#include <fstream>
#include <cstring>
#include <vector>

namespace nwen {
    // Implementation of the loadCSV function
    bool isInteger(const std::string& str)
    {
        try {
            std::stoi(str);
            return true;
        } catch (...) {
            return false;
        }
    }

    bool AbstractDbTable::loadCSV(string s)
    {
        movie temp_mov;
        std::ifstream inf(s);

        if (!inf.is_open()) {
            return false;
        }

        bool error_flag = false;
        string line;
        while (std::getline(inf, line)) {
            std::istringstream iss(line);
            string token;
            // Parsing each field of the CSV line

            // ID
            // getLine not in if statement because isInt will catch it
            std::getline(iss, token, ',');
            if (isInteger(token) && !error_flag)
                temp_mov.id = std::stoul(token);
            // if any of the checks fail then set the error flag
            else
                error_flag  = true;

            // TITLE
            // getLine checks if next token exists and therefore checks the format
            if (std::getline(iss, token, ',') && !error_flag)
                std::strcpy(temp_mov.title, token.c_str());
            else
                error_flag = true;

            // YEAR
            std::getline(iss, token, ',');
            if (isInteger(token) && !error_flag)
                temp_mov.year = std::stoi(token);
            else
                error_flag  = true;

            // DIRECTOR
            if (std::getline(iss, token, ',') && !error_flag)
                std::strcpy(temp_mov.director, token.c_str());
            else
                error_flag = true;

            // exit if error is found
            if (error_flag)
                break;
            else
                add(temp_mov); // Adding the movie to the table
        }
        inf.close();
        return !error_flag;
    }


    // Implementation of the saveCSV function
    bool AbstractDbTable::saveCSV(string s)
    {
        // Creating output file stream
        std::ofstream ofs(s);

        if (ofs.is_open()) {
            // Writing the variables to a CSV line for every element
            for (int line = 0; line < rows(); line++) {
                movie* cur = get(line);
                ofs << cur->id << ",";
                ofs << cur->title << ",";
                ofs << cur->year << ",";
                ofs << cur->director << std::endl;
            }

            ofs.close();
            return true;
        }

        return false;
    }
}
