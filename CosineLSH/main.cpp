#include "modules/cosine_lsh.h"

int main (int argc, char *argv[]) {

    int n_t{1},  n_f{10};
    LSH lsh = LSH(n_f, n_t);

    // query base out
    lsh.open_file(argv);

    // initialize
    lsh.init_hash_table();
    lsh.init_hash_function();
    
    // create hash table
    lsh.hash_from_file();

    // query from hash table
    lsh.query_from_file();

    // close files
    lsh.finish();
    return 0;
}