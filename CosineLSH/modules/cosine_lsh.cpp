#include "cosine_lsh.h"

LSH::LSH(){
  std::cout << "LSH objected was created." << std::endl;
}

LSH::LSH(int n_f, int n_t) {
  this->n_functions = n_f;
  this->n_tables = n_t;
  std::cout << "LSH objected was created." << std::endl;
}

void LSH::open_file(char* argv[]) {
  this->qFile.open(argv[1]);
  this->bFile.open(argv[2]);
  this->oFile.open(argv[3]);

  this->get_n_dimensions();
  this->get_n_lines();

  this->res.resize(this->n_query_lines);
}

void LSH::init_hash_table() {
  this->hashTables.resize(this->n_tables,
    std::vector<std::vector<int> > (std::pow(2, this->n_functions), std::vector<int>(1)));
  for (int i = 0; i < this->n_tables; i++) {
    for (int j = 0; j < std::pow(2, this->n_functions); j++) {
      this->hashTables[i][j].resize(0);
    }
  }
  std::cout << "Hash tables' dimension: " << this->n_tables
            << " X " << std::pow(2, this->n_functions) << " X " << 0 << std::endl;
}

void LSH::init_hash_function() {


  std::default_random_engine gen(3);
  std::normal_distribution<double> dis(0,1);

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
    std::random_shuffle(ivec.begin(), ivec.end());
    this->amplifyFunction[i].assign(ivec.begin(), ivec.end());
  }
}

void LSH::hash_from_file() {
  double x;
  unsigned long long sum{0};
  int pos{0};
  for (int line = 1; line <= this->n_base_lines; line++) {
    std::cout << "Item: " << line << " was created" << std::endl;
    this->move_to_line(this->bFile, line);
    pos = 0;
    for (int k = 0; k < this->n_tables; k++) {
      for (int t = 0; t < this->n_functions; t++) {
        sum = 0;
        for (int i = 0; i < this->n_dim; i++) {
          this->bFile >> x;
          sum += x * this->hashFunction[ this->amplifyFunction[k][t] ][i];
        }
        this->move_to_line(this->bFile, line);
        if (sum > 0)
          pos += std::pow(2, t);
      }
      if (pos >= std::pow(2, this->n_functions))
        pos = std::pow(2, this->n_functions) - 1;
      // 容器追加，避免处理哈希冲突
      // std::cout << k << "===" << pos << std::endl;
      this->hashTables[k][pos].push_back(line);
    }
  }
  std::cout << "Hash Table has been created !" << std::endl;
}

void LSH::query_from_file() {
  double s{0.0};
  for (int line = 1; line <= this->n_query_lines; line++) {
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

  for (int i = 0; i < this->n_tables; i++) {
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

  return 1.0 - (product / x_norm * y_norm);
}

double LSH::nearest_query_cosine(int line) {
  auto start = std::chrono::high_resolution_clock::now();

  for (int t = 0; t < this->n_tables; t++) {
    int pos = hash_query(t, line);
    for (auto& i: this->hashTables[t][pos]) {
      double dis = this->calcute_cosine_distance(i, line);
      this->res[line-1].emplace(dis, i);
    }
  }
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed1 = end - start;

  this->oFile << "Query: " << line << std::endl;
  this->oFile << "NN LSH Five Items:" << std::endl;
  for (int i = 0; i < 5; i++) {
    this->oFile << "\t" << this->res[line-1].front().first 
                << "\t" << this->res[line-1].front().second << std::endl;
    this->res[line-1].pop();
  }

  this->oFile << "time: LSH: " << elapsed1.count() << std::endl;
  this->oFile << "==================================================== " << std::endl;

  return elapsed1.count();
}

void LSH::get_n_dimensions() {
  int dim{0};
  std::string str;
  std::getline(this->bFile, str);
  for (int i = 0; i < str.size(); i++) {
    if (str[i] != ' ')
      dim ++;
  }
  this->set_pointer_begin(this->bFile);
  this->n_dim = dim;
}

void LSH::get_n_lines() {
  unsigned long long lines{0};
  std::string str;
  this->set_pointer_begin(this->bFile);
  while (std::getline(bFile, str))
    lines++;
  this->set_pointer_begin(this->bFile);
  this->n_base_lines = lines;

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