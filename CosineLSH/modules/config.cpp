#pragma once

#include <iostream>

class Config {
private:
  // base 数据库路径
	std::string base_file_path{"data/base"};
	
	// query 数据库路径
	std::string query_file_path{"data/query"};
	
	// 保存查询结果的路径
	std::string out_file_path{"data/out"};
	
	// 是否读取哈希表
	bool read_hash{false};
	// 读取哈希表的路径
	std::string read_hash_file{"data/hash_table"};

	// 是否保存哈希表
	bool save_hash{true};
	// 保存哈希表的路径
	std::string save_hash_file{"data/hash_table"};

	// 哈希表和哈希函数的数量
	int n_table{2}, n_functoin{10};

	// 最多返回多少结果
	int n_query_number{5};

public:

	void set_n_query_number(int n) {
		this->n_query_number = n;
	}
	int get_n_query_number() const {
		return this->n_query_number;
	}

	void set_n_table(int n) {
		this->n_table = n;
	}
	int get_n_table() const {
		return this->n_table;
	}

	void set_n_functoin(int n) {
		this->n_functoin = n;
	}
	int get_n_functoin() const {
		return this->n_functoin;
	}

	void set_base_file_path(std::string s) {
		this->base_file_path = s;
	}
	std::string get_base_file_path () const {
		return this->base_file_path;
	}

	void set_query_file_path(std::string s) {
		this->query_file_path = s;
	}
	std::string get_query_file_path () const {
		return this->query_file_path;
	}

	void set_out_file_path(std::string s) {
		this->out_file_path = s;
	}
	std::string get_out_file_path () const {
		return this->out_file_path;
	}

	void set_read_hash_file(std::string s) {
		this->read_hash_file = s;
	}
	std::string get_read_hash_file () const {
		return this->read_hash_file;
	}

	void set_save_hash_file(std::string s) {
		this->save_hash_file = s;
	}
	std::string get_save_hash_file () const {
		return this->save_hash_file;
	}

	void set_read_hash(bool f) {
		this->read_hash = f;
	}
	bool get_read_hash() const {
		return this->read_hash;
	}

	void set_save_hash(bool f) {
		this->read_hash = f;
	}
	bool get_save_hash() const {
		return this->read_hash;
	}
};