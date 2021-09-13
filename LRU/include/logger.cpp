#include "logger.h"

void Logger::open(const std::string s)
{
    this->writter.open(s);
    this->path = s;
}

void Logger::write(const std::string s)
{
    this->writter << s << std::endl;
}

void Logger::close()
{
    this->writter.close();
}
