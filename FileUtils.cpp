#include "FileUtils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>
#include <cstring>

Trade ParseLine(const std::string& line) {                           // This function was created for the certain format of the Trades data
    std::istringstream ss(line);                                     // For huge (relatevily) files, bucket(?) method can be used
    std::string item;                                                // Here we are simply parsing a line...
    Trade trade;

    std::getline(ss, item, ',');
    trade.timestamp = std::stod(item);

    std::getline(ss, trade.side, ',');

    std::getline(ss, item, ',');
    trade.price = std::stod(item);

    std::getline(ss, item, ',');
    trade.amount = std::stod(item);

    return trade;
}

std::vector<OHLC> ReadFile(const std::string& filename, double interval) {
    std::vector<OHLC> candles;
    std::ifstream file(filename);

    std::string line;                                                // Reading from trades file

    Trade trade = ParseLine(line);                                  
    double startTime = trade.timestamp;                              // Variable for proper interval selection
    OHLC candle(trade);

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        trade = ParseLine(line);

        if (trade.timestamp - startTime < interval) {                // Creating candles
            candle.high = std::max(candle.high, trade.price);
            candle.low = std::min(candle.low, trade.price);
            candle.close = trade.price;

            if (trade.side == "buy") {
                candle.totalBuyVolume += trade.amount;
                candle.totalBuyPrice += trade.price * trade.amount;
                candle.buyTrades++;
            }
            else if (trade.side == "sell") {
                candle.totalSellVolume += trade.amount;
                candle.totalSellPrice += trade.price * trade.amount;
                candle.sellTrades++;
            }

        }
        else {
            candles.push_back(candle);                               // Save completed candle
            candle.reset(trade);                                     // Start new candle
            startTime = trade.timestamp;
        }
    }

    file.close();
    return candles;
}

void SaveCandlesToFile(const std::string& filename, const std::vector<OHLC>& candles) {
    std::ofstream outFile(filename);                                 // Obviously it is the most genious and important function in the whole project
                                                                     // Saves time
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    outFile << "Open,High,Low,Close,TotalBuyVolume,TotalSellVolume,TotalBuyPrice,TotalSellPrice,BuyTrades,SellTrades,AvgBuyPrice,AvgSellPrice\n";

    for (const auto& candle : candles) {
        outFile << candle.open << ","
            << candle.high << ","
            << candle.low << ","
            << candle.close << ","
            << candle.totalBuyVolume << ","
            << candle.totalSellVolume << ","
            << candle.totalBuyPrice << ","
            << candle.totalSellPrice << ","
            << candle.buyTrades << ","
            << candle.sellTrades << ","
            << candle.averageBuyPrice() << ","
            << candle.averageSellPrice() << "\n";
    }

    outFile.close();
}


std::vector<OHLC> ReadCandlesFromFile(const std::string& filename) { // I have added more that required stats for candles
    std::vector<OHLC> candles;
    std::ifstream inFile(filename);

    std::string line;

    std::getline(inFile, line);

    while (std::getline(inFile, line)) {
        std::istringstream ss(line);
        std::string item;
        OHLC candle;

        std::getline(ss, item, ',');
        candle.open = std::stod(item);

        std::getline(ss, item, ',');
        candle.high = std::stod(item);

        std::getline(ss, item, ',');
        candle.low = std::stod(item);

        std::getline(ss, item, ',');
        candle.close = std::stod(item);

        std::getline(ss, item, ',');
        candle.totalBuyVolume = std::stod(item);

        std::getline(ss, item, ',');
        candle.totalSellVolume = std::stod(item);

        std::getline(ss, item, ',');
        candle.totalBuyPrice = std::stod(item);

        std::getline(ss, item, ',');
        candle.totalSellPrice = std::stod(item);

        std::getline(ss, item, ',');
        candle.buyTrades = std::stoi(item);

        std::getline(ss, item, ',');
        candle.sellTrades = std::stoi(item);

        double temp; 

        std::getline(ss, item, ',');
        temp = std::stoi(item);

        std::getline(ss, item, ',');
        temp = std::stoi(item);

        candles.push_back(candle);
    }

    inFile.close();
    
    return candles;
}




