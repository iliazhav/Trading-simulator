#include "Simulator.h"
#include <iostream>
#include <cmath>
#include <numeric>
#include <limits>
#include <algorithm> 

Simulator::Stats Simulator::runSimulation(const std::map<std::string, std::vector<OHLC>>& candles, const std::map<std::string, Strategy>& strategies, int mode) {
    Stats stats;

    for (const auto& pair : strategies) {

        double pnl              = 0.0;         // Reset statistics for each strategy/candles set
        double totalVolume      = 0.0;
        double equity           = 0.0;
        double maxEquity        = 0.0;
        double minEquity        = std::numeric_limits<double>::max();
        int positionFlips       = 0;
        double holdingTime      = 0.0;
        int holdingPeriodCount  = 0;

        std::vector<double> equityCurve;
        int position = 0;
        double entryPrice = 0.0;
        bool inPosition = false;

        std::string instrument = pair.first;
        Strategy strategy = pair.second;
        std::vector<OHLC> candle = candles.at(instrument);
        int lastPosition = position; 

        for (size_t i = 0; i < candle.size() && i < strategy.actions.size(); ++i) {
            int action = strategy.actions[i];

            if (action != 0) {                                       // Update position logic
                if (!inPosition) {
                    entryPrice = candle[i].close;                    // Entering new position
                    inPosition = true;
                    holdingPeriodCount = i;
                }
                else if (position * action < 0) {
                    positionFlips++;
                    holdingTime += i - holdingPeriodCount;
                    holdingPeriodCount = i;                          // Reset holding count for the new position
                }
            }

            double tradePnl = calculateTrade(candle[i], action, mode);
            pnl += tradePnl;                                         // Calculate the trade PnL and adjust position
            totalVolume += std::abs(action);

            equity += tradePnl;                                      // Adjust equity based on pnl
            equityCurve.push_back(equity);
            maxEquity = std::max(maxEquity, equity);
            minEquity = std::min(minEquity, equity);

            position += action;
        }

        if (inPosition) {                                            // Finalize holding time
            holdingTime += candle.size() - holdingPeriodCount;
            inPosition = false;                                      // Position closed at the end of the loop
        }

        stats.pnl = pnl;                                             // Stats update
        stats.tradedVolume = totalVolume;
        stats.maxDrawdown = calculateDrawdown(equityCurve);
        stats.averageHoldingTime = holdingTime / (positionFlips + 1); 
        stats.positionFlips = positionFlips;
        stats.sharpeRatio = calculateSharpeRatio(calculateReturns(equityCurve));
        stats.sortinoRatio = calculateSortinoRatio(calculateReturns(equityCurve));

        std::cout << instrument << "\n";
        printStats(stats);
    }

    return stats;
}

double Simulator::calculateTrade(const OHLC& candle, int action, int mode) {
    double tradePrice = 0.0;

    if (mode == 1) {
        tradePrice = candle.close;                                   // Mode 1: Trading at close
    }
    else if (mode == 2) {
        if (action > 0) {
            tradePrice = candle.averageBuyPrice();                   // Mode 2: Buy at average buy price
        }
        else if (action < 0) {
            tradePrice = candle.averageSellPrice();                  // Mode 2: Sell at average sell price
        }
    }

    return action * tradePrice;                                      // PnL = number of contracts * price
}

std::vector<double> Simulator::calculateReturns(const std::vector<double>& equityCurve) const {
    std::vector<double> returns;                                     // Calculate returns from the equity curve
    for (size_t i = 1; i < equityCurve.size(); ++i) {
        if (equityCurve[i - 1] != 0) {
            returns.push_back((equityCurve[i] - equityCurve[i - 1]) / equityCurve[i - 1]);
        }
    }
    return returns;
}

double Simulator::calculateDrawdown(const std::vector<double>& equityCurve) const {
    double maxDrawdown = 0.0;
    double peak = equityCurve[0];                                    // Calculate the maximum drawdown from the equity curve

    for (const auto& value : equityCurve) {
        peak = std::max(peak, value);
        double drawdown = (peak - value) / peak;
        maxDrawdown = std::max(maxDrawdown, drawdown);
    }

    return maxDrawdown;
}

double Simulator::calculateSharpeRatio(const std::vector<double>& returns) const {
    if (returns.size() < 2) return 0.0; 

    double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double squaredSum = std::accumulate(returns.begin(), returns.end(), 0.0,
        [meanReturn](double sum, double r) { return sum + (r - meanReturn) * (r - meanReturn); });
    double stdDev = std::sqrt(squaredSum / (returns.size() - 1));    // Use sample std deviation

    return stdDev > 0 ? (meanReturn / stdDev) : 0.0;  
}

double Simulator::calculateSortinoRatio(const std::vector<double>& returns) const {
    if (returns.size() < 2) return 0.0; 

    double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double downsideDeviation = std::sqrt(std::accumulate(returns.begin(), returns.end(), 0.0,
        [meanReturn](double sum, double r) { return sum + std::max(0.0, meanReturn - r) * std::max(0.0, meanReturn - r); }) / returns.size());

    return downsideDeviation > 0 ? (meanReturn / downsideDeviation) : 0.0;
}

void Simulator::printStats(const Stats& stats) const {
    std::cout << "PnL: " << stats.pnl << "\n";
    std::cout << "Total Traded Volume: " << stats.tradedVolume << "\n";
    std::cout << "Sharpe Ratio: " << stats.sharpeRatio << "\n";
    std::cout << "Sortino Ratio: " << stats.sortinoRatio << "\n";
    std::cout << "Max Drawdown: " << stats.maxDrawdown << "\n";
    std::cout << "Average Holding Time: " << stats.averageHoldingTime << "\n";
    std::cout << "Number of Position Flips: " << stats.positionFlips << "\n\n";
}


