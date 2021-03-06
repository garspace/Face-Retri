#include "modules/cosine_lsh.h"

int main (int argc, char *argv[]) {

	// 哈希表的函数数量，哈希表的数量
	LSH lsh{"../modules/config"};

	// 从文件读入哈希表？
	if (!lsh.get_isRead() ) {
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