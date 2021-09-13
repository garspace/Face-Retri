#pragma once

#include "dlinknode.h"
#include "logger.h"
#include <unordered_map>

class LRUCache
{
private:
    std::unordered_map<int, DLinkNode *> m;
    int capacity;
    int size{0};
    DLinkNode *head = new DLinkNode();
    DLinkNode *tail = new DLinkNode();
    Logger *log;
    enum Mode
    {
        _get,
        _put,
        _move,
        _remove
    };

public:
    explicit LRUCache() = default;
    explicit LRUCache(int capacity);
    explicit LRUCache(Logger *log);
    explicit LRUCache(int capacity, Logger *log);

    void init();

    // 插入节点
    void addHead(DLinkNode *node);

    // 删除某个节点,这里不能 free 掉
    // 因为 moveHead 还要用, 只有在 removeTail 时才能删除
    void removeNode(DLinkNode *node);

    // 某个缓存中的页面放到前面
    void moveHead(DLinkNode *node);

    // 超出容量，删除尾部节点
    DLinkNode *removeTail();

    // 获取节点
    int get(int key);

    // 插入节点
    void put(int key, int value);

    void record(int, int, int);
};