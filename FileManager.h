#pragma once
#include "Market.h"
#include "Portfolio.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

// ============================================================
//  FileManager.h  —  Save and load portfolio to/from disk
// ============================================================

class FileManager {
private:
    static std::string timestamp() {
        std::time_t now = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

public:
    // ── Save portfolio state ──────────────────────────────────
    static bool savePortfolio(const Portfolio& p,
                              const std::vector<Stock>& stocks,
                              int day,
                              const std::string& filename = "save.txt") {
        std::ofstream f(filename);
        if (!f.is_open()) return false;

        f << "# Stock Market Simulator — Save File\n";
        f << "# Saved: " << timestamp() << "\n";
        f << "DAY=" << day << "\n";
        f << "CASH=" << std::fixed << std::setprecision(4) << p.getCash() << "\n";
        f << "INITIAL=" << p.getInitialInvestment() << "\n";

        for (const auto& [ticker, qty] : p.getAllShares()) {
            f << "SHARES " << ticker << " " << qty << "\n";
        }

        f << "# Prices\n";
        for (const auto& s : stocks) {
            f << "PRICE " << s.getTicker() << " "
              << std::fixed << std::setprecision(4) << s.getPrice() << "\n";
        }

        f.close();
        return true;
    }

    // ── Load portfolio state ──────────────────────────────────
    static bool loadPortfolio(Portfolio& p,
                              int& day,
                              const std::string& filename = "save.txt") {
        std::ifstream f(filename);
        if (!f.is_open()) return false;

        std::string line;
        double cash = 10000.0, initial = 10000.0;
        day = 1;

        while (std::getline(f, line)) {
            if (line.empty() || line[0] == '#') continue;

            std::istringstream ss(line);
            std::string key;
            ss >> key;

            if (key == "DAY")     { ss >> day; }
            else if (key == "CASH")    { ss >> cash; }
            else if (key == "INITIAL") { ss >> initial; }
            // Shares loading is omitted for brevity (extend as needed)
        }
        // Re-create portfolio with loaded cash
        p = Portfolio(cash);
        f.close();
        return true;
    }

    // ── Export history as CSV ─────────────────────────────────
    static bool exportCsv(const std::vector<Stock>& stocks,
                           const std::string& filename = "history.csv") {
        std::ofstream f(filename);
        if (!f.is_open()) return false;

        // Header
        f << "Ticker";
        for (size_t i = 1; i <= 20; ++i) f << ",Day-" << i;
        f << "\n";

        for (const auto& s : stocks) {
            f << s.getTicker();
            for (const auto& p : s.getHistory()) {
                f << "," << std::fixed << std::setprecision(2) << p;
            }
            f << "\n";
        }

        f.close();
        return true;
    }
};
