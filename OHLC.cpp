#include "OHLC.h"
#include <algorithm> 

OHLC::OHLC()                                                         // Def constructor
    : open(0.0), high(0.0), low(0.0), close(0.0),
    totalBuyVolume(0.0), totalSellVolume(0.0), totalBuyPrice(0.0), totalSellPrice(0.0),
    buyTrades(0), sellTrades(0) {
}

OHLC::OHLC(const Trade& trade)                                       // Updating
    : open(trade.price), high(trade.price), low(trade.price), close(trade.price),
    totalBuyVolume(0.0), totalSellVolume(0.0), totalBuyPrice(0.0), totalSellPrice(0.0),
    buyTrades(0), sellTrades(0) {

    if (trade.side == "buy") {
        totalBuyVolume += trade.amount;
        totalBuyPrice += trade.price * trade.amount;
        buyTrades++;
    }
    else if (trade.side == "sell") {
        totalSellVolume += trade.amount;
        totalSellPrice += trade.price * trade.amount;
        sellTrades++;
    }
}

void OHLC::reset(const Trade& trade) {                               // Reset function for OHLCs constructing
    open = trade.price;
    high = trade.price;
    low = trade.price;
    close = trade.price;
    totalBuyVolume = 0.0;
    totalSellVolume = 0.0;
    totalBuyPrice = 0.0;
    totalSellPrice = 0.0;
    buyTrades = 0;
    sellTrades = 0;

    if (trade.side == "buy") {
        totalBuyVolume += trade.amount;
        totalBuyPrice += trade.price * trade.amount;
        buyTrades++;
    }
    else if (trade.side == "sell") {
        totalSellVolume += trade.amount;
        totalSellPrice += trade.price * trade.amount;
        sellTrades++;
    }
}

double OHLC::averageBuyPrice() const {
    return (totalBuyVolume > 0) ? (totalBuyPrice / totalBuyVolume) : 0.0;
}

double OHLC::averageSellPrice() const {
    return (totalSellVolume > 0) ? (totalSellPrice / totalSellVolume) : 0.0;
}

