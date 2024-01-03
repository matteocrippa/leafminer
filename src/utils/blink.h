#ifndef BLINK_H
#define BLINK_H

#include <Arduino.h>

#define BLINK_SINGLE 1
#define BLINK_DOUBLE 2
#define BLINK_START 5
#define BLINK_SUCCESS 3
#define BLINK_SUBMIT 2

class Blink
{
public:
    static Blink &getInstance()
    {
        static Blink instance; // This will be created only once
        return instance;
    }

    void setup();
    void blink(int number);

private:
    Blink();                                  // Private constructor to prevent instantiation
    Blink(const Blink &) = delete;            // Delete copy constructor
    Blink &operator=(const Blink &) = delete; // Delete copy assignment operator
    char TAG_BLINK[6] = "Blink";
};

#endif
