#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <queue>
#include <utility>

using PDI = std::pair<double, int>;

class LSH {
private:
  int n_functions{0};
  int n_tables{0};
  // 数据维度
  int n_dim{0};
  // base 有多少行的数据
  unsigned long long n_base_lines{0};
  // query 有多少行的数据
  int n_query_lines{0};
  // 存储结果
  std::vector<std::queue<PDI> > res;
  std::vector<std::vector<std::vector<int> > > hashTables;
  std::vector<std::vector<double> > hashFunction;
  std::vector<std::vector<int> > amplifyFunction;
  std::ifstream bFile;
  std::ifstream qFile;
  std::ofstream oFile;
public:
  // 构造函数 
  LSH();
  LSH(int, int);
  
  // 打开文件
  void open_file(char* argv[]);

  // 初始化哈希函数和哈希表
  void init_hash_table();
  void init_hash_function();

  // 输入文件的行数、维度数
  void get_n_lines();
  void get_n_dimensions();

  // 文件指针指向开始
  void set_pointer_begin(std::ifstream& );
  // 文件指针指向制定行
  void move_to_line(std::ifstream& , int);

  // 构建 base 的哈希表
  void hash_from_file();

  // 查询 query 数据
  void query_from_file();

  int hash_query(int, int);

  // 最紧邻查询
  double nearest_query_cosine(int);

  // 余弦距离
  double calcute_cosine_distance(int, int);

  // 结束时关闭文件
  void finish();
};