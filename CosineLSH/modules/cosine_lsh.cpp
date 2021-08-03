#include "cosine_lsh.h"

/*
  构造函数  
  创建 LSH 对象
*/
LSH::LSH(){
  std::cout << "LSH objected was created." << std::endl;
}

LSH::LSH(int n_f, int n_t) {
  this->n_functions = n_f;
  this->n_tables = n_t;
  std::cout << "LSH objected was created." << std::endl;
}


/*
  打开文件后获取数据的行数和维度数
*/
void LSH::open_file(char* argv[]) {
  this->qFile.open(argv[1]);
  this->bFile.open(argv[2]);
  this->oFile.open(argv[3]);
  this->n_query_number = std::stoi(argv[4]);

  this->get_n_dimensions();
  this->get_n_lines();

  // 存储结果，query 有多少行，就存储多少结果
  this->res.resize(this->n_query_lines);

  std::cout << "Base file dimension : " << this->n_base_lines 
            << " X " << this->n_dim << std::endl;
  std::cout << "Query file dimension : " << this->n_query_lines
            << " X " << this->n_dim << std::endl;
}


/*
  n_tables, 2^n_functions, []
*/
void LSH::init_hash_table() {
  this->hashTables.resize(this->n_tables,
    std::vector<std::vector<int> > (std::pow(2, this->n_functions), std::vector<int>(1)));
  for (int i = 0; i < this->n_tables; i++) {
    for (int j = 0; j < std::pow(2, this->n_functions); j++) {
      this->hashTables[i][j].resize(0);
    }
  }
  std::cout << "Hash tables' dimension: " << this->n_tables
            << " X " << std::pow(2, this->n_functions) << std::endl;
}


/*
  初始化两个哈希函数
    一级哈希：
        amplifyFunction， 维度是 [n_tables, n_functions-1]，每一行的取值是 [0, n_functions-1]
    二级哈希：
        hashFunction，维度是 [n_functions, dim-1]，获取 amplifyFunction 的值，作为一维索引
                    ，取值范围是 0 为均值，0.2 为方差的正态分布随机数
*/
void LSH::init_hash_function() {

  std::default_random_engine gen(3);
  std::normal_distribution<double> dis(0, 0.2);

  this->hashFunction.resize(this->n_functions, std::vector<double>(this->n_dim));
  std::cout << "Hash function's dimension: " << this->n_functions 
            << " X " << this->n_dim << std::endl;
  
  for (int i = 0; i < this->n_functions; i++) {
    this->hashFunction[i].clear();
    for (int j = 0; j < this->n_dim; j++) {
      this->hashFunction[i][j] = dis(gen);
    }
  }
  
  this->amplifyFunction.resize(this->n_tables, std::vector<int>(this->n_functions));
  std::cout << "amplify function's dimension: " << this->n_tables 
            << " X " << this->n_functions << std::endl;
  
  std::vector<int> ivec(this->n_functions);
  std::iota(ivec.begin(), ivec.end(), 0);
  std::srand ( unsigned ( std::time(0) ) );
  for (int i = 0; i < this->n_tables; i++) {
    // 乱序并分配
    std::random_shuffle(ivec.begin(), ivec.end());
    this->amplifyFunction[i].assign(ivec.begin(), ivec.end());
  }
}


/*
  由文件构建哈希表，放入某个表的 pos 位置的桶中
*/
void LSH::hash_from_file() {
  double x;
  unsigned long long sum{0};
  int pos{0};
  for (int line = 0; line < this->n_base_lines; line++) {
    std::cout << "Item: " << line << " was hashed now! " << std::endl;
    this->move_to_line(this->bFile, line);
    pos = 0;
    for (int t = 0; t < this->n_tables; t++) {
      for (int f = 0; f < this->n_functions; f++) {
        sum = 0;
        for (int i = 0; i < this->n_dim; i++) {
          this->bFile >> x;
          sum += x * this->hashFunction[ this->amplifyFunction[t][f] ][i];
        }
        this->move_to_line(this->bFile, line);
        if (sum > 0)
          pos += std::pow(2, f);
      }
      if (pos >= std::pow(2, this->n_functions))
        pos = std::pow(2, this->n_functions) - 1;
      // 容器追加，避免处理哈希冲突
      this->hashTables[t][pos].push_back(line);
    }
  }
  std::cout << "Hash Table has been created, now to save it !" << std::endl;
}


/*
  对 query 文件的每一行进行查询
  记录查询的平均时间
*/
void LSH::query_from_file() {
  double s{0.0};
  for (int line = 0; line < this->n_query_lines; line++) {
    // 当前行的查询与计时
    double t = this->nearest_query_cosine(line);
    s += t;
    std::cout << "Item " << line << " was queried" << std::endl;
  }
  oFile << "Average Time: " << s / this->n_query_lines << " seconds. ";
}


int LSH::hash_query(int t, int line) {
  this->move_to_line(this->qFile, line);
  
  unsigned long long int sum{0};
  double x{0.0};
  int pos{0};

  for (int i = 0; i < this->n_functions; i++) {
    sum = 0;
    for (int j = 0; j < this->n_dim; j++) {
      this->qFile >> x;
      sum += x * this->hashFunction[this->amplifyFunction[t][i]][j];
    }
    this->move_to_line(this->qFile, line);
    if (sum > 0)
      pos += std::pow(2, i);
  }
  return pos;
}


/*
  计算余弦相似度，取值范围是 [-1, 1]
    -1 完全相反
    0 毫无关系
    1 正相关
*/
double LSH::calcute_cosine_distance(int base_line, int query_line) {
  double dis{0}, x{0.0}, y{0.0}, product{0.0}, x_norm{0.0}, y_norm{0.0};

  this->move_to_line(this->qFile, query_line);
  this->move_to_line(this->bFile, base_line);
  
  for (int i = 0; i < this->n_dim; i++) {
    this->bFile >> x;
    this->qFile >> y;
    product += x*y;
    x_norm += x*x;
    y_norm += y*y;
  }
  x_norm = std::sqrt(x_norm);
  y_norm = std::sqrt(y_norm);
  // std::cout << x_norm << "==" << y_norm << std::endl;

  return (product / (x_norm * y_norm));
}


/*
  查询最为接近的几个结果，用优先级队列存储查询结果
*/
double LSH::nearest_query_cosine(int line) {
  auto start = std::chrono::high_resolution_clock::now();

  for (int t = 0; t < this->n_tables; t++) {
    // 查询到桶
    int pos = hash_query(t, line);
    // 遍历这个桶
    for (auto& i: this->hashTables[t][pos]) {
      double dis = this->calcute_cosine_distance(i, line);
      // 距离与项，按照第一项进行排序
      this->res[line-1].emplace(dis, i);
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed1 = end - start;

  this->oFile << "Query: " << line << std::endl;
  this->oFile << "NN LSH " << this->n_query_number << " Items:" << std::endl;
  int cnt{0};
  while (!this->res[line-1].empty() && cnt < this->n_query_number) {
    this->oFile << " ==>>" << this->res[line-1].top().first 
                << "\t" << this->res[line-1].top().second << std::endl;
    this->res[line-1].pop();
    cnt++;
  }
  
  this->oFile << "time: LSH: " << elapsed1.count() << std::endl;
  this->oFile << "==================================================== " << std::endl;

  return elapsed1.count();
}


/*
  获取数据的维度
*/
void LSH::get_n_dimensions() {
  int dim{1};
  std::string str;
  std::getline(this->bFile, str);
  for (int i = 0; i < str.size(); i++) {
    if (str[i] == ' ')
      dim ++;
  }
  this->set_pointer_begin(this->bFile);
  this->n_dim = dim;
}


/*
  获取 query 和 base 两个文件有多少行
*/
void LSH::get_n_lines() {
  // base 文件
  unsigned long long lines{0};
  std::string str;
  this->set_pointer_begin(this->bFile);
  while (std::getline(bFile, str))
    lines++;
  this->set_pointer_begin(this->bFile);
  this->n_base_lines = lines;

  // query 文件
  lines = 0;
  this->set_pointer_begin(this->qFile);
  while (std::getline(qFile, str))
    lines++;
  this->set_pointer_begin(this->qFile);
  this->n_query_lines = lines;
}


void LSH::set_pointer_begin(std::ifstream& f) {
  f.clear();
  f.seekg(0, std::ios::beg);
}


void LSH::move_to_line(std::ifstream& f, int line) {
  std::string s;
  f.clear();
  f.seekg(0, std::ios::beg);
  for (int i = 0; i < line; i++)
    std::getline(bFile, s);
}


void LSH::finish() {
  this->bFile.close();
  this->qFile.close();
  this->oFile.close();
}