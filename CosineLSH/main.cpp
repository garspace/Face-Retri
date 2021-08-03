#include "modules/cosine_lsh.h"

int main (int argc, char *argv[]) {

	Config c = Config();

	// 哈希表的函数数量，哈希表的数量
	LSH lsh = LSH(c.get_n_functoin(), c.get_n_table());

	// query base out
	lsh.open_file(c);

	if (c.get_read_hash()) {
		
	}
	// 不读入就重新构建
	else {
		// initialize
		lsh.init_hash_table();
		lsh.init_hash_function();

		// create hash table
		lsh.hash_from_file();
	}

	// query from hash table
	lsh.query_from_file();

	// close files
	lsh.finish();
	return 0;
}