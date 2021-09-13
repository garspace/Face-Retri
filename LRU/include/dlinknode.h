#pragma once

class DLinkNode
{
public:
    int key;
    int val;
    DLinkNode *pre;
    DLinkNode *next;
    DLinkNode() : key{0}, val{0}, pre{nullptr}, next{nullptr} {};
    DLinkNode(int k, int v) : key{k}, val{v}, pre{nullptr}, next{nullptr} {};
};