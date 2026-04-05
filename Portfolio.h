#pragma once
#include "Stock.h"
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

// ============================================================
//  Portfolio.h  —  Tracks cash, shares, and P&L
// ============================================================

class Portfolio {
private:
    double cash;
    double initialInvestment;
    std::map<std::string, int> shares;  // ticker -> numShares

public:
    // ── Constructor ───────────────────────────────────────────
    explicit Portfolio(double startingCash = 10000.0)
        : cash(startingCash), initialInvestment(startingCash) {}

    // ── Core trading operations ───────────────────────────────

    // Returns true on success, false if insufficient funds / shares
    bool buyShares(const Stock& stock, int qty) {
        double cost = stock.getPrice() * qty;
        if (cost > cash) return false;
        cash -= cost;
        shares[stock.getTicker()] += qty;
        return true;
    }

    bool sellShares(const Stock& stock, int qty) {
        auto it = shares.find(stock.getTicker());
        if (it == shares.end() || it->second < qty) return false;
        cash += stock.getPrice() * qty;
        it->second -= qty;
        if (it->second == 0) shares.erase(it);
        return true;
    }

    // ── Valuation ─────────────────────────────────────────────

    // Total portfolio value given current market snapshot
    double totalValue(const std::vector<Stock>& market) const {
        double val = cash;
        for (const auto& [ticker, qty] : shares) {
            for (const auto& s : market) {
                if (s.getTicker() == ticker) {
                    val += s.getPrice() * qty;
                    break;
                }
            }
        }
        return val;
    }

    double profitLoss(const std::vector<Stock>& market) const {
        return totalValue(market) - initialInvestment;
    }

    double profitLossPercent(const std::vector<Stock>& market) const {
        if (initialInvestment == 0.0) return 0.0;
        return (profitLoss(market) / initialInvestment) * 100.0;
    }

    // ── Display ───────────────────────────────────────────────
    void display(const std::vector<Stock>& market) const {
        double total = totalValue(market);
        double pl    = profitLoss(market);
        bool   profit = pl >= 0;

        std::cout << "\n\033[1;36m╔══════════════════════════════════════════════╗\033[0m\n";
        std::cout << "\033[1;36m║           📁  YOUR PORTFOLIO                 ║\033[0m\n";
        std::cout << "\033[1;36m╚══════════════════════════════════════════════╝\033[0m\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  💵 Cash Balance  : $" << cash << "\n";
        std::cout << "  📊 Total Value   : $" << total << "\n";

        std::string plColor = profit ? "\033[32m" : "\033[31m";
        std::string plSign  = profit ? "+" : "";
        std::cout << "  " << plColor
                  << "📈 Profit / Loss : " << plSign << "$" << pl
                  << "  (" << plSign << profitLossPercent(market) << "%)\033[0m\n";

        if (shares.empty()) {
            std::cout << "\n  (No stock positions held)\n";
        } else {
            std::cout << "\n  Holdings:\n";
            std::cout << "  " << std::string(50, '-') << "\n";
            std::cout << "  " << std::left << std::setw(8) << "Ticker"
                      << std::setw(8)  << "Shares"
                      << std::setw(12) << "Cur.Price"
                      << std::setw(12) << "Position" << "\n";
            std::cout << "  " << std::string(50, '-') << "\n";
            for (const auto& [ticker, qty] : shares) {
                double curPrice = 0.0;
                for (const auto& s : market)
                    if (s.getTicker() == ticker) { curPrice = s.getPrice(); break; }
                double posVal = curPrice * qty;
                std::cout << "  " << std::left << std::setw(8) << ticker
                          << std::setw(8)  << qty
                          << "$" << std::setw(11) << curPrice
                          << "$" << posVal << "\n";
            }
        }
        std::cout << "\n";
    }

    // ── Getters ───────────────────────────────────────────────
    double getCash() const { return cash; }
    int getShares(const std::string& ticker) const {
        auto it = shares.find(ticker);
        return (it != shares.end()) ? it->second : 0;
    }
    const std::map<std::string, int>& getAllShares() const { return shares; }
    double getInitialInvestment() const { return initialInvestment; }
};
