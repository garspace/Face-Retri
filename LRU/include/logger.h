#pragma once

#include <iostream>
#include <fstream>

class Logger
{
private:
    std::string path{""};
    std::ofstream writter;

public:
    Logger() = default;

    void open(const std::string s);
    void write(const std::string s);
    void close();
};
