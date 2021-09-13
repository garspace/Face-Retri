#include "../include/logger.h"
#include "../include/LRUCache.h"

int main()
{

    const std::string logPth = "../log/log.txt";
    Logger *log = new Logger();

    log->open(logPth);

    LRUCache lru = LRUCache(2, log);

    lru.put(1, 3);
    lru.get(1);
    lru.put(2, 3);
    lru.put(3, 3);

    // std::cout << lru.get(2) << std::endl;
    log->close();
    return 0;
}
