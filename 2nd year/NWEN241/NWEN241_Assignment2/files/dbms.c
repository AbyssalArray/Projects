// database program
// by Evgeny Zhilkin (zhilkievge)
// 27/3/23


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dbms.h"

/*
 * takes db_table and a row to display
 * checks if row is not out of bounds
 * access the data in the row
 * reformat the data into string
 */
int db_show_row(const struct db_table *db, unsigned int row) {
    // check for row existence
    if (row > db -> rows_used || row < 0)
        return 0;

    // saving the specific album
    struct album *alb = &db  -> table[row];

    // printing the the data in a formatted way
    printf("%6.0lu:%20.20s:%20.20s:%4hu\n", alb -> id, alb -> title, alb -> artist, alb -> year);
    return 1;
}
/*
 * Takes db_table and an entry to add
 * check if there is space for a new entry
 * - if there is space then append it to the end and increment the rows used
 * - otherewise allocate 5 more spaces into memory
 */
int db_add_row(struct db_table *db, struct album *a) {
    // check for row use
    if (db -> rows_used >= db -> rows_total) {
        // check for db existance
        if (db -> table == NULL) {
            // allocate a new db if it doesnt
            db  -> table = (struct album *) calloc(5,  sizeof(struct album) * 5);
        } else {
            // otherwise allocate more memory to db
            size_t rows = db -> rows_total;
            struct album *alb = db  -> table;
            db  -> table = (struct album *) realloc(alb,  sizeof(struct album) * (rows + 5));
        }
        // check if allocation has failed
        if (db == NULL)
            return 0;
        // update the rows used
        db -> rows_total += 5;
    }

    // entry insertion
    db  -> table[db -> rows_used] = *a;
    db -> rows_used++;
    return 1;
}

/*
 * Takes a db_table and an id to delete,
 * if id is found then
 * -delete it
 * -decrement the rows_used
 * -check if theres 5 or more unused rows
 * -    - if there are free up the space
 * - check if there are no rows used at all
 * -    - then free up the memory fully and set it to null
 */
int db_remove_row(struct db_table *db, unsigned long id) {
    int is_found = 0;
    // find and delete the entry
    for (int i = 0; i < db -> rows_used; i++) {
        struct album *c_row = &db  -> table[i];
        // when found start shifting elements to the left
        if(is_found) {
            db  -> table[i-1] = *c_row;
        } else if (c_row -> id == id) {
            is_found++;
        }

    }
    // failed to delete
    if(!is_found)
        return 0;
    // memory free - 5 rows unused
    db -> rows_used--;
    if ( db -> rows_total - db -> rows_used >= 5) {
        struct album *alb = db  -> table;
        size_t rows = db -> rows_total;
        db -> rows_total -= 5;
        db  -> table = (struct album *) realloc(alb,  sizeof(struct album) * (rows - 5));
        if (db == NULL)
            return 0;
    }
    // memory free - no rows used
    if ( db -> rows_total <= 5 && db -> rows_used == 0)  {
        free(db -> table);
        db -> table = NULL;
    }
    return 1;
}
