#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <vector>
#include "Trade.h"
#include "OHLC.h"

// Check the detailed description in FileUtils.cpp

Trade ParseLine(const std::string& line);                            
std::vector<OHLC> ReadFile(const std::string& filename, double interval);
void SaveCandlesToFile(const std::string& filename, const std::vector<OHLC>& candles);
std::vector<OHLC> ReadCandlesFromFile(const std::string& filename);


#endif 

