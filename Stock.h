#pragma once
#include "Rng.h"
#include <string>
#include <vector>
#include <deque>
#include <random>
#include <iomanip>
#include <iostream>
#include <algorithm>

// ============================================================
//  Stock.h  --  Represents a single stock in the market
// ============================================================

class Stock {
private:
    std::string  ticker;
    std::string  companyName;
    double       price;
    double       openPrice;
    double       basePrice;    // fair-value anchor for mean reversion
    double       volatility;   // daily move size in dollars

    std::deque<double> history;
    static constexpr size_t MAX_HISTORY = 20;

public:
    // -----------------------------------------------------------
    //  Constructors
    // -----------------------------------------------------------
    Stock() : ticker("???"), companyName("Unknown"),
              price(100.0), openPrice(100.0), basePrice(100.0),
              volatility(3.0) {
        history.push_back(price);
    }

    Stock(const std::string& t,
          const std::string& name,
          double startPrice,
          double vol)
        : ticker(t), companyName(name),
          price(startPrice), openPrice(startPrice), basePrice(startPrice),
          volatility(vol)
    {
        history.push_back(price);
    }

    // -----------------------------------------------------------
    //  updatePrice()  --  called once per simulated day
    //
    //  GBM-style model:
    //    normal_shock   = N(0, volatility/3)   -- bell-curve random move
    //    drift          = +0.05% per day        -- long-run upward bias
    //    mean_reversion = 4% pull toward basePrice -- prevents runaway trends
    // -----------------------------------------------------------
    void updatePrice() {
        openPrice = price;

        std::normal_distribution<double> shock(0.0, volatility / 3.0);
        double change    = shock(globalRng());
        double drift     = price * 0.0005;
        double reversion = (basePrice - price) * 0.04;

        price = std::max(1.0, price + change + drift + reversion);

        history.push_back(price);
        if (history.size() > MAX_HISTORY)
            history.pop_front();
    }

    // -----------------------------------------------------------
    //  liveTick()  --  tiny normal-distributed fluctuation
    // -----------------------------------------------------------
    void liveTick() {
        std::normal_distribution<double> micro(0.0, volatility / 22.0);
        price = std::max(1.0, price + micro(globalRng()));
    }

    // -----------------------------------------------------------
    //  applyEvent()  --  market news shock (percent change)
    // -----------------------------------------------------------
    void applyEvent(double percentChange) {
        price = std::max(1.0, price * (1.0 + percentChange / 100.0));
        basePrice += (price - basePrice) * 0.3;  // shift equilibrium slightly
        history.push_back(price);
        if (history.size() > MAX_HISTORY)
            history.pop_front();
    }

    // -----------------------------------------------------------
    //  movingAverage()
    // -----------------------------------------------------------
    double movingAverage(size_t days = 5) const {
        if (history.empty()) return price;
        size_t count = std::min(days, history.size());
        double sum = 0.0;
        for (size_t i = history.size() - count; i < history.size(); ++i)
            sum += history[i];
        return sum / static_cast<double>(count);
    }

    // -----------------------------------------------------------
    //  dailyChangePercent()
    // -----------------------------------------------------------
    double dailyChangePercent() const {
        if (openPrice == 0.0) return 0.0;
        return ((price - openPrice) / openPrice) * 100.0;
    }

    // -----------------------------------------------------------
    //  sparkline()  --  ASCII chart of last 10 prices
    // -----------------------------------------------------------
    std::string sparkline() const {
        if (history.size() < 2) return "--";
        static const char bars[] = {'_', '.', ',', '-', '~', '^', '*', '#'};
        double lo = *std::min_element(history.begin(), history.end());
        double hi = *std::max_element(history.begin(), history.end());
        std::string line;
        size_t start = history.size() > 10 ? history.size() - 10 : 0;
        for (size_t i = start; i < history.size(); ++i) {
            double ratio = (hi == lo) ? 0.5 : (history[i] - lo) / (hi - lo);
            size_t idx = static_cast<size_t>(ratio * 7.0);
            if (idx > 7) idx = 7;
            line += bars[idx];
        }
        return line;
    }

    // -----------------------------------------------------------
    //  Getters
    // -----------------------------------------------------------
    const std::string& getTicker()      const { return ticker; }
    const std::string& getCompanyName() const { return companyName; }
    double             getPrice()       const { return price; }
    double             getOpenPrice()   const { return openPrice; }
    double             getVolatility()  const { return volatility; }
    const std::deque<double>& getHistory() const { return history; }

    // -----------------------------------------------------------
    //  displayRow()  --  single line in market table
    // -----------------------------------------------------------
    void displayRow(int index) const {
        double chg = dailyChangePercent();
        const char* arrow = (chg >= 0) ? "+" : "-";
        const char* color = (chg >= 0) ? "\033[32m" : "\033[31m";
        std::cout
            << "  [" << index << "] "
            << std::left  << std::setw(6)  << ticker
            << std::left  << std::setw(22) << companyName
            << std::right << std::setw(8)  << std::fixed << std::setprecision(2) << price
            << "   " << color
            << arrow << std::setw(5) << std::fixed << std::setprecision(2) << std::abs(chg) << "%"
            << "\033[0m"
            << "  [" << sparkline() << "]"
            << "\n";
    }
};
