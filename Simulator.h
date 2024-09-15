#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>
#include <string>
#include <map>
#include "OHLC.h"


class Strategy {                                                     // Header for Strategy. Simply this is a pair: {[EURUSD], {1, 0, -3}}
public:
    std::vector<int> actions;                                        // The vector of actions: +X buy, -X sell, 0 do nothing
    std::string instrument;

    Strategy() : instrument(""), actions({}) {}                      // Df constructor

    Strategy(const std::string& instr, const std::vector<int>& acts) // Constructor
        : instrument(instr), actions(acts) {}
};

class Simulator {                                                    // Simulator is the simply a function of execution of container of different strats/candles
public:
    struct Stats {
        double pnl;
        double tradedVolume;
        double sharpeRatio;
        double sortinoRatio;
        double maxDrawdown;
        double averageHoldingTime;
        int positionFlips;
    };

    Stats runSimulation(const std::map<std::string, std::vector<OHLC>>& candles, const std::map<std::string, Strategy>& strategies, int mode);

private: // Stats calculations
    double calculateTrade(const OHLC& candle, int action, int mode);
    std::vector<double> calculateReturns(const std::vector<double>& equityCurve) const;
    double calculateDrawdown(const std::vector<double>& equityCurve) const;
    double calculateSharpeRatio(const std::vector<double>& returns) const;
    double calculateSortinoRatio(const std::vector<double>& returns) const;
    void printStats(const Stats& stats) const;
};

#endif 
