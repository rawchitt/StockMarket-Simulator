#pragma once
#include "Stock.h"   // Stock.h already includes Rng.h
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <random>
#include <sstream>

// ============================================================
//  Market.h  --  Manages the list of stocks, time, and events
// ============================================================

struct NewsEvent {
    std::string headline;
    std::string ticker;    // empty = affects all stocks
    double      impact;    // percent change applied
};

class Market {
private:
    std::vector<Stock> stocks;
    int                day;
    bool               marketOpen;

    std::vector<NewsEvent> eventPool;

    void buildEventPool() {
        eventPool = {
            {"Fed raises interest rates -- markets nervous",       "",     -3.0},
            {"Recession fears grip Wall Street",                   "",     -5.0},
            {"Tech sector surges on AI optimism",                  "TECH", +8.0},
            {"Pharma giant announces blockbuster drug approval",   "PHMA", +12.0},
            {"Oil prices spike -- energy stocks rally",            "ENRG", +7.0},
            {"Green energy bill passed -- renewables soar",        "GRNE", +10.0},
            {"Major smartphone launch drives consumer excitement", "TECH", +5.0},
            {"Market crash: panic selling across all sectors",     "",    -10.0},
            {"GDP beats expectations -- bull market continues",    "",     +4.0},
            {"Cybersecurity breach shakes confidence in tech",     "TECH", -6.0},
            {"Earnings season: most stocks beat estimates",        "",     +3.0},
            {"Natural disaster disrupts supply chains",            "",     -4.0},
            {"Central bank cuts rates -- stocks rally hard",       "",     +6.0},
            {"Trade war escalates -- broad market selloff",        "",     -7.0},
            {"Merger mania: buyout fever boosts sectors",          "",     +5.0},
        };
    }

public:
    // ----------------------------------------------------------
    //  Constructor
    // ----------------------------------------------------------
    Market() : day(1), marketOpen(true) {
        buildEventPool();
        stocks.emplace_back("AAPL",  "Apple Inc.",        175.0,  4.5);
        stocks.emplace_back("GOOGL", "Alphabet Inc.",     140.0,  5.0);
        stocks.emplace_back("TSLA",  "Tesla Inc.",        250.0,  9.0);
        stocks.emplace_back("MSFT",  "Microsoft Corp.",   320.0,  4.0);
        stocks.emplace_back("AMZN",  "Amazon.com Inc.",   185.0,  5.5);
        stocks.emplace_back("NVDA",  "NVIDIA Corp.",      480.0,  8.0);
        stocks.emplace_back("ENRG",  "GreenEnergy Ltd.",   55.0,  3.5);
        stocks.emplace_back("PHMA",  "PharmaCo Inc.",      90.0,  4.0);
        stocks.emplace_back("GRNE",  "SolarTech Corp.",    42.0,  3.0);
        stocks.emplace_back("TECH",  "NextGen Tech Inc.", 120.0,  6.0);
    }

    // ----------------------------------------------------------
    //  nextDay()  --  advance one simulated trading day
    //  Returns a news headline if a random event fires
    // ----------------------------------------------------------
    std::string nextDay() {
        ++day;
        for (auto& s : stocks)
            s.updatePrice();

        // 35% chance of a news event each day
        std::uniform_int_distribution<int> chance(1, 100);
        if (chance(globalRng()) <= 35) {
            std::uniform_int_distribution<size_t> pick(0, eventPool.size() - 1);
            const NewsEvent& ev = eventPool[pick(globalRng())];
            for (auto& s : stocks) {
                if (ev.ticker.empty() || ev.ticker == s.getTicker())
                    s.applyEvent(ev.impact);
            }
            return ev.headline;
        }
        return "";
    }

    // ----------------------------------------------------------
    //  liveTick()  --  for real-time feed mode
    // ----------------------------------------------------------
    void liveTick() {
        for (auto& s : stocks)
            s.liveTick();
    }

    // ----------------------------------------------------------
    //  display()  --  print market snapshot
    // ----------------------------------------------------------
    void display() const {
        std::cout << "\n\033[1;33m+============================================================+\033[0m\n";
        std::cout << "\033[1;33m|      STOCK MARKET SIMULATOR   --   Day "
                  << std::setw(3) << day
                  << "                  |\033[0m\n";
        std::cout << "\033[1;33m+============================================================+\033[0m\n";
        std::cout << "  #   Ticker  Company               Price      Chg       Chart\n";
        std::cout << "  " << std::string(66, '-') << "\n";
        for (size_t i = 0; i < stocks.size(); ++i)
            stocks[i].displayRow(static_cast<int>(i + 1));
        std::cout << "\n";
    }

    // ----------------------------------------------------------
    //  Getters
    // ----------------------------------------------------------
    std::vector<Stock>&       getStocks()       { return stocks; }
    const std::vector<Stock>& getStocks() const  { return stocks; }
    int                       getDay()    const  { return day; }

    Stock* findStock(const std::string& ticker) {
        for (auto& s : stocks)
            if (s.getTicker() == ticker) return &s;
        return nullptr;
    }

    Stock* getByIndex(int idx) {
        if (idx < 1 || idx > static_cast<int>(stocks.size())) return nullptr;
        return &stocks[idx - 1];
    }
};
