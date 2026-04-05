#pragma once
#include "Market.h"
#include "Portfolio.h" 
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

// ============================================================
//  TradingBot.h  —  AI agent that trades on your behalf
// ============================================================

enum class BotStrategy {
    THRESHOLD,      // Buy on 5% drop, Sell on 8% rise
    MOVING_AVERAGE  // Buy below SMA-5, Sell above SMA-5
};

struct BotAction {
    std::string ticker;
    std::string action;  // "BUY" | "SELL" | "HOLD"
    int         qty;
    std::string reason;
};

class TradingBot {
private:
    BotStrategy strategy;
    int         sharesPerTrade;  // How many shares the bot buys/sells at once

    // ── Threshold strategy ────────────────────────────────────
    BotAction analyzeThreshold(const Stock& s, const Portfolio& p) const {
        double chg = s.dailyChangePercent();
        bool hasShares = p.getShares(s.getTicker()) >= sharesPerTrade;

        if (chg <= -5.0 && p.getCash() >= s.getPrice() * sharesPerTrade) {
            return {s.getTicker(), "BUY",  sharesPerTrade,
                    "price dropped " + formatPct(chg) + " (≤ -5%)"};
        }
        if (chg >=  8.0 && hasShares) {
            return {s.getTicker(), "SELL", sharesPerTrade,
                    "price surged +" + formatPct(chg) + " (≥ +8%)"};
        }
        return {s.getTicker(), "HOLD", 0, "no signal"};
    }

    // ── Moving average strategy ───────────────────────────────
    BotAction analyzeMovingAverage(const Stock& s, const Portfolio& p) const {
        double sma  = s.movingAverage(5);
        double cur  = s.getPrice();
        bool hasShares = p.getShares(s.getTicker()) >= sharesPerTrade;

        if (cur < sma * 0.98 && p.getCash() >= cur * sharesPerTrade) {
            // Price dipped >2% below SMA → BUY
            return {s.getTicker(), "BUY",  sharesPerTrade,
                    "price $" + fmt(cur) + " < SMA5 $" + fmt(sma)};
        }
        if (cur > sma * 1.02 && hasShares) {
            // Price rose >2% above SMA → SELL
            return {s.getTicker(), "SELL", sharesPerTrade,
                    "price $" + fmt(cur) + " > SMA5 $" + fmt(sma)};
        }
        return {s.getTicker(), "HOLD", 0,
                "price $" + fmt(cur) + " near SMA5 $" + fmt(sma)};
    }

    // ── Helpers ───────────────────────────────────────────────
    static std::string fmt(double v) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(2) << v;
        return os.str();
    }
    static std::string formatPct(double v) {
        std::ostringstream os;
        os << std::fixed << std::setprecision(2) << v << "%";
        return os.str();
    }


public:
    explicit TradingBot(BotStrategy strat = BotStrategy::MOVING_AVERAGE,
                        int sharesPerTrade = 2)
        : strategy(strat), sharesPerTrade(sharesPerTrade) {}

    // ── Run one full market scan ──────────────────────────────
    void run(Market& market, Portfolio& portfolio) {
        std::cout << "\n\033[1;35m╔══════════════════════════════════════════════╗\033[0m\n";
        std::cout << "\033[1;35m║   🤖  AI TRADING BOT — SCAN RESULTS          ║\033[0m\n";
        std::cout << "\033[1;35m╚══════════════════════════════════════════════╝\033[0m\n";

        std::string stratName = (strategy == BotStrategy::THRESHOLD)
                              ? "Threshold (−5%/+8%)"
                              : "Moving Average (SMA-5)";
        std::cout << "  Strategy : " << stratName << "\n";
        std::cout << "  Qty/trade: " << sharesPerTrade << " shares\n\n";

        int buys = 0, sells = 0, holds = 0;

        for (auto& s : market.getStocks()) {
            BotAction act = (strategy == BotStrategy::THRESHOLD)
                          ? analyzeThreshold(s, portfolio)
                          : analyzeMovingAverage(s, portfolio);

            std::string icon, color;
            if (act.action == "BUY") {
                icon = "💚 BUY "; color = "\033[32m"; ++buys;
                portfolio.buyShares(s, act.qty);
            } else if (act.action == "SELL") {
                icon = "🔴 SELL"; color = "\033[31m"; ++sells;
                portfolio.sellShares(s, act.qty);
            } else {
                icon = "⚪ HOLD"; color = "\033[37m"; ++holds;
            }

            std::cout << "  " << color
                      << std::left << std::setw(6) << s.getTicker()
                      << " → " << icon
                      << "  " << act.reason
                      << "\033[0m\n";
        }

        std::cout << "\n  Summary: " << buys << " buys | "
                  << sells << " sells | " << holds << " holds\n\n";
    }

    // ── Strategy toggle ───────────────────────────────────────
    void setStrategy(BotStrategy s) { strategy = s; }
    BotStrategy getStrategy() const  { return strategy; }
};
