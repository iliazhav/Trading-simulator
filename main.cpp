#include "Simulator.h"
#include "FileUtils.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

const double SCALE = 1e6;                                            // Scale value according to the given timestamp format

int main() {
    std::string filename = "md_sim/trades_dogeusdt.csv";   

    const double interval = 10 * SCALE;                              // Scaling (1 hour interval = 60 * 60 * SCALE)
    const double eps = 0.00001; 

    //std::vector<OHLC> candles = ReadFile(filename, interval);      // Creating OHLCs from trades data
    //SaveCandlesToFile("DOGEUSDT_candles.csv", candles);

    std::vector<OHLC> candles_DOGE = ReadCandlesFromFile("DOGEUSDT_candles.csv");   // Reading OHLCs 
    
    std::map<std::string, std::vector<OHLC>> candles;                // Example - one set of OHLCs, 2 strategies (Perf, Rand)
    candles["DOGE_perfect"] = candles_DOGE;
    candles["DOGE_random"]  = candles_DOGE;

    std::vector<int> perfectActions(candles_DOGE.size());            // Perfect strategy
    for (size_t i = 0; i < candles_DOGE.size() - 1; ++i) {
        if (candles_DOGE[i + 1].close - candles_DOGE[i].close < eps) // Костыль. Epsilon for neutral action
        {
            perfectActions[i] = 0;
            continue;
        }
        if (candles_DOGE[i + 1].close > candles_DOGE[i].close) {     // Simply: Bullish - buy, Bearish - sell 
            perfectActions[i] = 1; 
        }
        else {
            perfectActions[i] = -1; 
        }
    }
    Strategy perfectStrategy("DOGE_perfect", perfectActions);

    std::srand(std::time(nullptr));                                  // Random strategy
    std::vector<int> randomActions(candles_DOGE.size());
    for (auto& action : randomActions) {
        action = (std::rand() % 3) - 1; // <=> {-1, 0, 1}
    }
    Strategy randomStrategy("DOGE_random", randomActions);           // It is better to use external function to build/read strats
                                                                     // (will be added later)
    std::map<std::string, Strategy> strategies;                      // Container of OHLCs, strategies
    strategies["DOGE_perfect"] = perfectStrategy;
    strategies["DOGE_random"]  = randomStrategy;

    Simulator simulator;                                             // Testing
    simulator.runSimulation(candles, strategies, 1);                 // Interface could looks better
                                                                     // (will be added later)
    return 0;
}

