#ifndef SUBSCRIBE_H
#define SUBSCRIBE_H

#include <string>
#include <stdio.h>

struct Subscribe
{
    std::string id;
    std::string extranonce1;
    int extranonce2_size;

    Subscribe(const std::string &id, const std::string &extranonce1, const int &extranonce2_size)
    {
        this->id = id;
        this->extranonce1 = extranonce1;
        this->extranonce2_size = extranonce2_size;
    }
};

#endif
