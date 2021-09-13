#include "LRUCache.h"

void LRUCache::init()
{
    head->next = tail;
    tail->pre = head;
}

LRUCache::LRUCache(int capacity) : capacity{capacity}
{
    this->init();
}

LRUCache::LRUCache(Logger *l)
{
    log = l;
    this->init();
}

LRUCache::LRUCache(int capacity, Logger *l) : capacity{capacity}
{
    log = l;
    this->init();
}

void LRUCache::addHead(DLinkNode *node)
{
    node->pre = head;
    node->next = head->next;
    head->next->pre = node;
    head->next = node;
}

void LRUCache::removeNode(DLinkNode *node)
{
    node->pre->next = node->next;
    node->next->pre = node->pre;
}

DLinkNode *LRUCache::removeTail()
{
    DLinkNode *node = tail->pre;
    removeNode(node);
    return node;
}

void LRUCache::moveHead(DLinkNode *node)
{
    removeNode(node);
    addHead(node);
}

void LRUCache::record(int key, int val, int mode)
{
    log->write("\n================");

    std::string tmp{""};
    if (mode == 0)
    {
        tmp += "Query Key ";
        tmp += std::to_string(key);
        tmp += " From LRU Cache, Get Value ";
        tmp += std::to_string(val);
    }
    else if (mode == 1)
    {
        tmp += "Key Value : ";
        tmp += std::to_string(key);
        tmp += "-";
        tmp += std::to_string(val);
        tmp += " Push to Head.";
    }
    else if (mode == 2)
    {
        tmp += "Key ";
        tmp += std::to_string(key);
        tmp += " changed value to ";
        tmp += std::to_string(val);
        tmp += " Move to Head.";
    }
    else if (mode == 3)
    {
        tmp += "Key Value : ";
        tmp += std::to_string(key);
        tmp += "-";
        tmp += std::to_string(val);
        tmp += " Has been Moved.";
    }

    log->write(tmp);
    log->write("================");
}

int LRUCache::get(int key)
{
    int val;

    if (!m.count(key))
    {
        val = -1;
    }
    else
    {
        DLinkNode *node = m[key];
        moveHead(node);
        val = node->val;
    }
    record(key, val, _get);
    return val;
}

void LRUCache::put(int key, int value)
{
    if (!m.count(key))
    {
        DLinkNode *node = new DLinkNode(key, value);
        m[key] = node;
        // 添加
        addHead(node);
        size++;
        record(key, value, _put);
        if (size > capacity)
        {
            DLinkNode *node = removeTail();
            record(node->key, node->val, _remove);
            // 链表需要存储 key 的原因，需要删除 map 中的 key
            m.erase(node->key);
            delete node;
            size--;
        }
    }
    // 否则已经在里面
    else
    {
        DLinkNode *node = m[key];
        node->val = value;
        // 移动
        moveHead(node);
        record(key, value, _move);
    }
}
