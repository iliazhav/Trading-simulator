#ifndef TRADE_H
#define TRADE_H

#include <string>                          

struct Trade {               // Dummy class for OHLCs
    double timestamp;
    std::string side;
    double price;
    double amount;
};

#endif 
