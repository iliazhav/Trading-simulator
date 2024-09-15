#ifndef OHLC_H
#define OHLC_H

#include "Trade.h"  

class OHLC {                            // The structure of OHLC
public:                                 // Is it possible to avoid the huge number of stats in practice?
    double open;
    double high;
    double low;
    double close;

    double totalBuyVolume;  
    double totalSellVolume; 
    double totalBuyPrice;   
    double totalSellPrice;  
    int buyTrades;         
    int sellTrades;        

    
    OHLC();
    explicit OHLC(const Trade& trade);
    void reset(const Trade& trade);
    double averageBuyPrice() const;
    double averageSellPrice() const;
};

#endif