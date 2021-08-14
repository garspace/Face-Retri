#include "cosine_lsh.h"

/*
  构造函数  
  创建 LSH 对象
*/
LSH::LSH(){
}

LSH::LSH(std::string s) {
  config_path = s;
  this->parse_config(config_path);
}

/*
  解析配置文件
*/
void LSH::parse_config(std::string s) {
  // 保留两位小数
  this->oFile << std::fixed << std::setprecision(2);
  
  std::ifstream temp;
  std::string str;
  temp.open(s);
  int n_t;

  // 读取哈希表的数
  this->move_to_line(temp, 0);
  temp >> n_t;
  this->n_tables = n_t;

  // 读取哈希函数的数目
  this->move_to_line(temp, 1);
  temp >> n_t;
  this->n_functions = n_t;

  // 读取每张图片返回多少查询结果
  this->move_to_line(temp, 2);
  temp >> n_t;
  this->n_query_number = n_t;

  // base file 路径
  this->move_to_line(temp, 3);
  temp >> str;
  this->bFile.open(str);

  // query file 路径
  this->move_to_line(temp, 4);
  temp >> str;
  this->qFile.open(str);

  // out file 路径
  this->move_to_line(temp, 5);
  temp >> str;
  this->oFile.open(str);

  // base query 文件有多少行
  this->get_n_lines();
  // 数据维度
  this->get_n_dimensions();

  // 存储结果，query 有多少行，就存储多少结果
  // 优先级队列，存储结果，选择相似度最接近的
  this->res.resize(this->n_query_lines);

  this->oFile << "Base file dimension : " << this->n_base_lines 
            << " X " << this->n_dim << std::endl;
  this->oFile << "Query file dimension : " << this->n_query_lines
            << " X " << this->n_dim << std::endl;

  // 从文件读取哈希表
  this->move_to_line(temp, 6);
  temp >> n_t;

  if (n_t == 1) {
    this->read = true;

    temp >> str;
    this->r_p_hash_table.open(str);

    temp >> str;
    this->r_p_hash_function.open(str);

    temp >> str;
    this->r_p_amp_function.open(str);

    // 读取数据
    this->read_data();
  }

  this->move_to_line(temp, 7);
  // 保存文件的路径，默认为空
  temp >> n_t;
  // 保存哈希表等数据
  if (n_t == 1) {
    this->save = true;

    temp >> str;
    this->s_p_hash_table.open(str);

    temp >> str;
    this->s_p_hash_function.open(str);

    temp >> str;
    this->s_p_amp_function.open(str);
  }

  // 即读取也保存，则扩容，但不建议扩容太猛
  if (this->read == true && this->save == true) {
    this->hash_from_file();
  }

  temp.close();
}

bool LSH::get_isRead() const {
  return this->read;
}

void LSH::read_data() {
  // 读取哈希表
  int n_t, n_f;
  std::string str;

  this->r_p_hash_table >> n_t >> n_f;
  this->n_tables = n_t;
  this->n_functions = std::log2(n_f);
  this->move_to_line(this->r_p_hash_table, 1);

  this->init_hash_table();
  int cnt{0}, x;
  double y;
  for (int i = 0; i < this->n_tables; i++) {
    for (int j = 0; j < std::pow(2, this->n_functions); j++) {
      std::getline(this->r_p_hash_table, str);
      std::stringstream s{str};
      while (s >> x) {
        this->hashTables[i][j].push_back(x);
      }
    }
  }
  this->r_p_hash_table.close();
  
  // 读取二级哈希 function dim
  this->r_p_hash_function >> n_t >> n_f;
  this->n_functions = n_t;
  this->n_dim = n_f;
  this->hashFunction.resize(this->n_functions, std::vector<double>(this->n_dim));

  this->move_to_line(this->r_p_hash_function, 1);
  for (int i = 0; i < this->n_functions; i++) {
    for (int j = 0; j < this->n_dim; j++) {
      this->r_p_hash_function >> y;
      this->hashFunction[i][j] = y;
    }
  }
  this->oFile << "Hash function's dimension: " << this->n_functions 
            << " X " << this->n_dim << std::endl;
  this->r_p_hash_function.close();

  // 读取一级哈希函数
  this->r_p_amp_function >> n_t >> n_f;
  this->amplifyFunction.resize(this->n_tables, std::vector<int>(this->n_functions));
  this->oFile << "amplify function's dimension: " << this->n_tables 
            << " X " << this->n_functions << std::endl;
  
  this->move_to_line(this->r_p_amp_function, 1);
  for (int i = 0; i < this->n_tables; i++) {
    for (int j = 0; j < this->n_functions; j++) {
      this->r_p_amp_function >> x;
      this->amplifyFunction[i][j] = x;
    }
  }
  this->r_p_amp_function.close();
}

/*
  n_tables, 2^n_functions, []
*/
void LSH::init_hash_table() {
  this->hashTables.resize(this->n_tables,
    std::vector<std::list<int> > (std::pow(2, this->n_functions), std::list<int>(0)));
  for (int i = 0; i < this->n_tables; i++) {
    for (int j = 0; j < std::pow(2, this->n_functions); j++) {
      this->hashTables[i][j].resize(0);
    }
  }
  this->oFile << "Hash tables' dimension: " << this->n_tables
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
  this->oFile << "Hash function's dimension: " << this->n_functions 
            << " X " << this->n_dim << std::endl;
  
  for (int i = 0; i < this->n_functions; i++) {
    this->hashFunction[i].clear();
    for (int j = 0; j < this->n_dim; j++) {
      this->hashFunction[i][j] = dis(gen);
    }
  }
  
  this->amplifyFunction.resize(this->n_tables, std::vector<int>(this->n_functions));
  this->oFile << "amplify function's dimension: " << this->n_tables 
            << " X " << this->n_functions << std::endl;
  
  std::vector<int> ivec(this->n_functions);
  std::iota(ivec.begin(), ivec.end(), 0);
  std::srand ( unsigned ( 1 ) );
  for (int i = 0; i < this->n_tables; i++) {
    // 乱序并分配
    std::random_shuffle(ivec.begin(), ivec.end());
    this->amplifyFunction[i].assign(ivec.begin(), ivec.end());
  }
}

/*
  单表哈希
*/
void LSH::hash_one_table(std::vector<double>& tmp, int t, int line) {
  int pos{0};
  double sum{0.0};

  // 每一个函数
  for (int f = 0; f < this->n_functions; f++) {
    sum = 0;
    for (int i = 0; i < this->n_dim; i++) {
      sum += tmp[i] * this->hashFunction[ this->amplifyFunction[t][f] ][i];
    }
    if (sum > 0)
      pos += std::pow(2, f);
  }
  // 越界处理
  if (pos >= std::pow(2, this->n_functions)) {
      // 2^9 = 511.9998 -> 511
      int a = std::pow(2, this->n_functions);
      pos %= a;
  }
  // 容器追加，避免处理哈希冲突
  this->hashTables[t][pos].push_back(line);
}

/*
  由文件构建哈希表，放入某个表的 pos 位置的桶中
*/
void LSH::hash_from_file() {
  double x, sum{0};
  int pos{0};
  std::vector<double> tmp;
  auto start = std::chrono::high_resolution_clock::now();

  ThreadPool pool{this->n_tables};
  pool.init();
  // 处理每一条数据
  this->set_pointer_begin(this->bFile);
  for (int line = 0; line < this->n_base_lines; line++) {
    if (line % 500 == 499) {
      auto end = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed1 = end - start;
      this->oFile << "After " << elapsed1.count() << " seconds, "
                << line + 1 <<  " items has been hashed ! " << std::endl;
    }
    // 空间换时间
    tmp.clear();
    for (int i = 0; i < this->n_dim; i++) {
      this->bFile >> x;
      tmp.push_back(x);
    }

    // 处理每一张表
    for (int t = 0; t < this->n_tables; t++) {
      // this->hash_one_table(tmp, t, line);
      auto func = std::bind(&LSH::hash_one_table, this, tmp, t, line);
      pool.submit(func);
    }
  }
  pool.shutdown();

  // 保存数据
  if (this->save == true) {
    this->save_data();
  }
}

/*
  保存结果
*/
void LSH::save_data() {
  
  // 保存哈希表
  this->s_p_hash_table << this->n_tables << " " 
                       << std::pow(2, this->n_functions) << std::endl;
  for (int i = 0; i < this->n_tables; i++) {
		for (int j = 0; j < std::pow(2, this->n_functions); j++) {
			for (auto& k : this->hashTables[i][j]) {
					this->s_p_hash_table << k << " ";
			}
			if (i == this->n_tables-1 && j == std::pow(2, this->n_functions)-1)
				continue;
			this->s_p_hash_table << std::endl;
		}
	}
  this->s_p_hash_table.close();
  
  // 保存 hash functoin
  this->s_p_hash_function << this->n_functions << " " << this->n_dim << std::endl;
  for (int i = 0; i < this->n_functions; i++) {
    for (int j = 0; j < this->n_dim; j++) {
      if (j == 0)
        this->s_p_hash_function << this->hashFunction[i][j];
      else
        this->s_p_hash_function << " " << this->hashFunction[i][j];
    }
    if (i != this->n_functions-1)
      this->s_p_hash_function << std::endl;
  }
  this->s_p_hash_function.close();

  // 保存 amp function
  this->s_p_amp_function << this->n_tables << " " << this->n_functions << std::endl;
  for (int i = 0; i < this->n_tables; i++) {
    for (int j = 0; j < this->n_functions; j++) {
      if (j == 0)
        this->s_p_amp_function << this->amplifyFunction[i][j];
      else
        this->s_p_amp_function << " " << this->amplifyFunction[i][j];
    }
    if (i != this->n_tables-1)
      this->s_p_amp_function << std::endl;
  }
  this->s_p_amp_function.close();

  this->oFile << "Hash Table, Hash Function, AmplifyFunction has been saved. "
              << std::endl
              << "===================================================="
              << std::endl;
}


/*
  单表查询
*/
void LSH::query_one_table(int t, int line, const std::vector<double>& tmp) {
  auto s = std::chrono::high_resolution_clock::now();
  double sum{0};
  int pos{0};

  for (int i = 0; i < this->n_functions; i++) {
    sum = 0;
    for (int j = 0; j < this->n_dim; j++) {
      sum += tmp[j] * this->hashFunction[this->amplifyFunction[t][i]][j];
    }
    if (sum > 0)
      pos += std::pow(2, i);
  }
  if (pos >= std::pow(2, this->n_functions)) {
    int a = std::pow(2, this->n_functions);
    pos %= a;
  }

  for (auto& i: this->hashTables[t][pos]) {
    // std::cout << i << " ";
    double dis = this->calcute_cosine_distance(i, tmp);
    // 距离与项，按照第一项进行排序
    this->res.emplace(line, dis, i);
    if (this->res.size(line) > this->n_query_number * this->n_tables) {
      this->res.pop(line);
    }
  }
}


/*
  对 query 文件的每一行进行查询
  记录查询的平均时间
*/
void LSH::query_from_file() {
  double s{0.0};
  this->n_query_lines = 1;

  ThreadPool pool{this->n_tables};
  pool.init();

  auto start = std::chrono::high_resolution_clock::now();
  for (int line = 0; line < this->n_query_lines; line++) {
    
    std::vector<double> tmp;
    this->move_to_line(this->qFile, line);
    double t{0.0};
    for (int i = 0; i < this->n_dim; i++) {
      this->qFile >> t;
      tmp.push_back(t);
    }

    for (int t = 0; t < this->n_tables; t++) {
      auto func = std::bind(&LSH::query_one_table, this, 
            t, line, tmp);
      pool.submit(func);
      // query_one_table(t, line, pos);
    }

  }
  pool.shutdown();
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed1 = end - start;
  s += elapsed1.count();

  for (int line = 0; line < this->n_query_lines; line++) {
    // 当前行的查询与计时
    this->oFile << "Query: " << line << ", NN LSH Items:" 
                << std::endl;

    int tmp{-2}, cnt{0};
    while (!this->res.empty(line) && cnt < this->n_query_number) {
      if (tmp != this->res.top(line).second) {
        tmp = this->res.top(line).second;
        this->oFile << " ==>> " << this->res.top(line).first
                    << "\t" << this->res.top(line).second << std::endl;
        cnt++;
      }
      this->res.pop(line);
    }
    
    // this->oFile << "time: LSH: " << elapsed1.count() << std::endl;
    this->oFile << "Item " << line << " has been queried" << std::endl;
    this->oFile << "====================================================" << std::endl;

  }
  this->oFile << "Average Time: " << s / this->n_query_lines << " seconds. ";
}


/*
  计算余弦相似度，取值范围是 [-1, 1]
    -1 完全相反
    0 毫无关系
    1 正相关
*/
double LSH::calcute_cosine_distance(int line, const std::vector<double>& v2) {
  double dis{0}, x{0.0}, product{0.0}, x_norm{0.0}, y_norm{0.0};

  std::ifstream temp, b;
  temp.open(this->config_path);
  this->move_to_line(temp, 3);
  std::string str;
  temp >> str;
  temp.close();

  b.open(str);
  this->move_to_line(b, line);

  std::vector<double> v1;
  for (int i = 0; i < this->n_dim; i++) {
    b >> x;
    v1.push_back(x);
  }
  b.close();
  
  for (int i = 0; i < this->n_dim; i++) {
    product += v1[i] * v2[i];
    x_norm += v1[i] * v1[i];
    y_norm += v2[i] * v2[i];
  }

  x_norm = std::sqrt(x_norm);
  y_norm = std::sqrt(y_norm);
  if (std::abs(x_norm * y_norm) < 1e-6)
    return -1;

  return (product / (x_norm * y_norm));
}


/*
  获取数据的维度
*/
void LSH::get_n_dimensions() {
  int dim{1};
  std::string str;
  std::getline(this->bFile, str);
  for (int i = 0; i < str.size(); i++) {
    if (str[i] == '\t')
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

/*
  每次建立哈希表都太慢了保存哈希表
*/

void LSH::set_pointer_begin(std::ifstream& f) {
  f.clear();
  f.seekg(0, std::ios::beg);
}


void LSH::move_to_line(std::ifstream& f, int line) {
  std::string s;
  f.clear();
  f.seekg(0, std::ios::beg);
  for (int i = 0; i < line; i++)
    std::getline(f, s);
}


void LSH::finish() {
  this->bFile.close();
  this->qFile.close();
  this->oFile.close();
}