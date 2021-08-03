#include "modules/cosine_lsh.h"

int main (int argc, char *argv[]) {

    int n_t{4},  n_f{5};
    LSH lsh = LSH(n_f, n_t);
    // query base out
    lsh.open_file(argv);
    lsh.init_hash_table();
    lsh.init_hash_function();
    lsh.hash_from_file();
    lsh.query_from_file();
    lsh.finish();
    return 0;
}