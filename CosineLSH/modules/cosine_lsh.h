#pragma once

#include <iostream>   // 输入输出
#include <vector>     // 哈希函数和哈希表
#include <cmath>      // pow 运算
#include <fstream>    // 文件
#include <random>     // 随机数
#include <chrono>     // 计时
#include <algorithm>  // 打乱序列
#include <numeric>    // 递增序列
#include <functional> // 小顶堆
#include <queue>      // 优先级队列
#include <utility>    // 使用 pair
#include <sstream>    // 使用字符串流
#include <list>       // 存储哈希结果的桶，vertor 会扩容
#include <iomanip>    // 保留两位小数

using PDI = std::pair<double, int>;

class LSH {
private:
  // 哈希函数的数量 
  int n_functions{0};

  // 哈希表的数量
  int n_tables{0};

  // 数据维度
  int n_dim{0};

  // base 有多少行的数据
  unsigned long long n_base_lines{0};

  // query 有多少行的数据
  int n_query_lines{0};

  // 每个查询数据，最多允许的查询结果数
  int n_query_number{100};

  // 是否保存结果
  bool save{false};

  // 是否读取文件构造哈希表
  bool read{false};

  // 优先级队列，存储结果，选择相似度最接近的
  std::vector< std::priority_queue<PDI, std::vector<PDI>, std::greater<PDI>> > res;

  // 哈希表
  std::vector<std::vector<std::list<int> > > hashTables;

  // 二级哈希函数
  std::vector<std::vector<double> > hashFunction;

  // 一级哈希函数
  std::vector<std::vector<int> > amplifyFunction;

  // base file 读取
  std::ifstream bFile;

  // query file 读取
  std::ifstream qFile;

  // 保存结果的文件
  std::ofstream oFile;

  // 保存哈希表的路径 save path
  std::ofstream s_p_hash_table;
  std::ofstream s_p_hash_function;
  std::ofstream s_p_amp_function;

  // 读取哈希表的路径 read path
  std::ifstream r_p_hash_table;
  std::ifstream r_p_hash_function;
  std::ifstream r_p_amp_function;

public:
  // 构造函数 
  LSH();
  LSH(std::string);

  // 解析配置文件
  void parse_config(std::string s);

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

  // 由 base 构建哈希表
  void hash_from_file();

  // 查询 query 数据
  void query_from_file();

  // 查找 query 落入哪个桶
  int hash_query(int, int);

  // 最紧邻查询
  double nearest_query_cosine(int);

  // 余弦距离相似度
  double calcute_cosine_distance(int, int);

  // 结束时关闭文件
  void finish();

  // 保存哈希表，每次构建都太慢了
  void save_data();

  // 读取文件，既然构建太慢，考虑直接读取哈希表和哈希函数实现查询
  void read_data();

  bool get_isRead() const;
};
