`modules/config` 文件每一行的含义：

1. 哈希表的数量
2. 哈希函数的数量
3. 每张图片的查询结果
4. base 文件的路径
5. query 文件的路径
6. out 文件的路径
7. 1 表示读取哈希表和哈希函数，之后跟读取路径；0 表示不读取而是重新构建哈希表
8. 1 表示存储哈希表和哈希函数，之后跟存储路径；0 表示不存储

# 哈希优化流程

- [x] 算法相关：读取配置文件确定哈希参数，避免改动代码造成的重新编译
- [x] 业务相关：保存哈希函数、哈希表
- [x] 业务相关：读取哈希函数、哈希表，直接查询
- [x] 算法相关：优化性能，从文件哈希时，空间换时间，取消每次重复的读文件，哈希时间从 5000 秒降低至 8 秒
- [x] 算法相关：多线程（[线程池](https://github.com/mtrebi/thread-pool)）实现建立哈希表、多表查询，去除重复结果，性能提升与线程数有关，他这个线程池写的有 bug，我提出了 [pull request](https://github.com/mtrebi/thread-pool/pull/40)，建议用我的。**多线程处理时一定注意全局变量与锁的粒度，前者会导致出错，后者会影响性能！**。
- [ ] 算法相关：不均匀哈希改进，有的桶数据很多，有的桶没数据
