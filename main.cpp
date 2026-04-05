// ============================================================
//  main.cpp  --  Stock Market Simulator (OOP / C++)
//  Modules: Stock | Portfolio | Market | TradingBot | FileManager
// ============================================================

#include "Market.h"
#include "Portfolio.h"
#include "TradingBot.h"
#include "FileManager.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <chrono>
#ifdef _WIN32
  #include <conio.h>    // _kbhit(), _getch()
  #include <windows.h>  // HANDLE, SetConsoleCursorPosition
#endif

// -- Helpers --------------------------------------------------

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnter() {
    std::cout << "\n  \033[90mPress ENTER to continue...\033[0m";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

int readInt(const std::string& prompt, int lo, int hi) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= lo && val <= hi) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  \033[31mInvalid. Enter a number between "
                  << lo << " and " << hi << ".\033[0m\n";
    }
}

// -- Banner ---------------------------------------------------

void printBanner() {
    std::cout << "\033[1;33m";
    std::cout << R"(
  +==============================================================+
  |                                                              |
  |    ███████╗ ████████╗ ██████╗  ██████╗██╗  ██╗             |
  |    ██╔════╝ ╚══██╔══╝██╔═══██╗██╔════╝██║ ██╔╝             |
  |    ███████╗    ██║   ██║   ██║██║     █████╔╝              |
  |    ╚════██║    ██║   ██║   ██║██║     ██╔═██╗              |
  |    ███████║    ██║   ╚██████╔╝╚██████╗██║  ██╗             |
  |    ╚══════╝    ╚═╝    ╚═════╝  ╚═════╝╚═╝  ╚═╝             |
  |          MARKET SIMULATOR  .  OOP Edition  .  C++           |
  +==============================================================+
)" << "\033[0m\n";
    std::cout << "\033[36m  10 Stocks  |  Live Feed  |  AI Bot  |  2 Strategies\033[0m\n\n";
}

// -- Main Menu ------------------------------------------------

void printMenu(int day, double cash) {
    std::cout << "\033[1;37m  +------------------------------------------+\033[0m\n";
    std::cout << "\033[1;37m  |        MAIN MENU  (Day " << std::setw(3) << day
              << ")              |\033[0m\n";
    std::cout << "\033[1;37m  +------------------------------------------+\033[0m\n";
    std::cout << "\033[1;37m  |  [1]  View Market (snapshot)             |\033[0m\n";
    std::cout << "\033[1;37m  |  [2]  Buy Stock                          |\033[0m\n";
    std::cout << "\033[1;37m  |  [3]  Sell Stock                         |\033[0m\n";
    std::cout << "\033[1;37m  |  [4]  View Portfolio                     |\033[0m\n";
    std::cout << "\033[1;37m  |  [5]  LIVE Market Feed (real-time)  ***  |\033[0m\n";
    std::cout << "\033[1;37m  |  [6]  Next Day (simulate)                |\033[0m\n";
    std::cout << "\033[1;37m  |  [7]  Run AI Trading Bot                 |\033[0m\n";
    std::cout << "\033[1;37m  |  [8]  View Stock Detail                  |\033[0m\n";
    std::cout << "\033[1;37m  |  [9]  Save + Export CSV                  |\033[0m\n";
    std::cout << "\033[1;37m  |  [0]  Exit                               |\033[0m\n";
    std::cout << "\033[1;37m  +------------------------------------------+\033[0m\n";
    std::cout << "\033[90m  Cash: $" << std::fixed << std::setprecision(2)
              << cash << "\033[0m\n\n";
}

// -- Live Market Feed -----------------------------------------
// Prices tick every 500 ms. Press any key to return to menu.
void liveMarketFeed(Market& market) {
#ifdef _WIN32
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize   = 1;
    ci.bVisible = FALSE;
    SetConsoleCursorInfo(hCon, &ci); // hide cursor for clean animation
#endif

    clearScreen();
    // Print static header (7 lines)
    std::cout << "\033[1;36m";
    std::cout << "  +----------------------------------------------------------------------+\n";
    std::cout << "  |   LIVE MARKET FEED  --  prices tick every 500 ms                    |\n";
    std::cout << "  |   Press ANY KEY to return to menu                                   |\n";
    std::cout << "  +----------------------------------------------------------------------+\n";
    std::cout << "\033[0m\n";
    std::cout << "  #   Ticker  Company               Price      Change    Trend(10d)\n";
    std::cout << "  " << std::string(68, '-') << "\n";

    // Print initial rows so cursor is positioned correctly
    for (int i = 0; i < (int)market.getStocks().size(); ++i)
        market.getStocks()[i].displayRow(i + 1);

    const int HEADER_LINES = 7; // lines before the stock table rows

    while (true) {
#ifdef _WIN32
        if (_kbhit()) { _getch(); break; }
#endif
        // Update prices with small live ticks
        market.liveTick();

        // Move ANSI cursor back to row HEADER_LINES+1 (1-indexed) to redraw
        std::cout << "\033[" << (HEADER_LINES + 1) << ";1H";

        for (int i = 0; i < (int)market.getStocks().size(); ++i)
            market.getStocks()[i].displayRow(i + 1);

        std::cout << "\n  \033[90m[Live ticking... Day " << market.getDay()
                  << " | press any key to stop]\033[0m    \r";
        std::cout.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

#ifdef _WIN32
    ci.bVisible = TRUE;
    SetConsoleCursorInfo(hCon, &ci); // restore cursor
#endif
    clearScreen();
}

// -- Stock Detail View ----------------------------------------

void viewStockDetail(Market& market) {
    market.display();
    int choice = readInt("  Select stock [1-10]: ", 1, 10);
    Stock* s = market.getByIndex(choice);
    if (!s) return;

    std::cout << "\n\033[1;36m  -- " << s->getTicker() << "  /  "
              << s->getCompanyName() << " --\033[0m\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Current Price : $" << s->getPrice()    << "\n";
    std::cout << "  Open Price    : $" << s->getOpenPrice()<< "\n";
    std::cout << "  Daily Change  : " << std::showpos
              << s->dailyChangePercent() << "%" << std::noshowpos << "\n";
    std::cout << "  Volatility    : +/-$" << s->getVolatility() << " per day\n";
    std::cout << "  SMA-5         : $" << s->movingAverage(5)  << "\n";
    std::cout << "  SMA-10        : $" << s->movingAverage(10) << "\n";

    auto& hist = s->getHistory();
    std::cout << "\n  Price History (" << hist.size() << " days):\n  ";
    for (double p : hist)
        std::cout << "$" << std::setprecision(0) << p << " ";
    std::cout << "\n\n  Sparkline : " << s->sparkline() << "\n";
}

// -- Buy Flow -------------------------------------------------

void buyStock(Market& market, Portfolio& portfolio) {
    market.display();
    std::cout << "  Your cash: $" << std::fixed << std::setprecision(2)
              << portfolio.getCash() << "\n\n";
    int idx = readInt("  Select stock to BUY [1-10] (0=cancel): ", 0, 10);
    if (idx == 0) return;

    Stock* s = market.getByIndex(idx);
    if (!s) return;

    int maxQty = static_cast<int>(portfolio.getCash() / s->getPrice());
    if (maxQty == 0) {
        std::cout << "\033[31m  Not enough cash to buy even 1 share ($"
                  << s->getPrice() << ").\033[0m\n";
        return;
    }

    std::cout << "  " << s->getTicker() << " @ $" << s->getPrice()
              << "  (max: " << maxQty << " shares)\n";
    int qty = readInt("  Quantity to buy (0=cancel): ", 0, maxQty);
    if (qty == 0) return;

    if (portfolio.buyShares(*s, qty)) {
        std::cout << "\033[32m  Bought " << qty << " share(s) of "
                  << s->getTicker() << " for $"
                  << std::fixed << std::setprecision(2) << s->getPrice() * qty
                  << "\033[0m\n";
    } else {
        std::cout << "\033[31m  Purchase failed.\033[0m\n";
    }
}

// -- Sell Flow ------------------------------------------------

void sellStock(Market& market, Portfolio& portfolio) {
    market.display();
    int idx = readInt("  Select stock to SELL [1-10] (0=cancel): ", 0, 10);
    if (idx == 0) return;

    Stock* s = market.getByIndex(idx);
    if (!s) return;

    int owned = portfolio.getShares(s->getTicker());
    if (owned == 0) {
        std::cout << "\033[31m  You don't own any shares of "
                  << s->getTicker() << ".\033[0m\n";
        return;
    }

    std::cout << "  " << s->getTicker() << " @ $" << std::fixed
              << std::setprecision(2) << s->getPrice()
              << "  (you own: " << owned << " shares)\n";
    int qty = readInt("  Quantity to sell (0=cancel): ", 0, owned);
    if (qty == 0) return;

    if (portfolio.sellShares(*s, qty)) {
        std::cout << "\033[32m  Sold " << qty << " share(s) of "
                  << s->getTicker() << " for $"
                  << std::fixed << std::setprecision(2) << s->getPrice() * qty
                  << "\033[0m\n";
    } else {
        std::cout << "\033[31m  Sale failed.\033[0m\n";
    }
}

// -- Bot Strategy Chooser ------------------------------------

BotStrategy chooseBotStrategy() {
    std::cout << "\n  Choose AI Strategy:\n";
    std::cout << "  [1] Threshold   -- Buy if -5%, Sell if +8%\n";
    std::cout << "  [2] Moving Avg  -- SMA-5 crossover signal\n";
    int c = readInt("  Strategy: ", 1, 2);
    return (c == 1) ? BotStrategy::THRESHOLD : BotStrategy::MOVING_AVERAGE;
}

// -- Entry Point ----------------------------------------------

int main() {
#ifdef _WIN32
    system("chcp 65001 > nul");
    system(""); // enable ANSI/VT100 on Windows 10+
#endif

    clearScreen();
    printBanner();
    std::cout << "  Starting cash: $10,000  |  10 stocks loaded\n\n";
    pressEnter();

    Market    market;
    Portfolio portfolio(10000.0);
    TradingBot bot(BotStrategy::MOVING_AVERAGE, 2);

    bool running = true;
    while (running) {
        clearScreen();
        printMenu(market.getDay(), portfolio.getCash());

        int choice = readInt("  Your choice: ", 0, 9);

        switch (choice) {

        case 1: // View Market snapshot
            clearScreen();
            market.display();
            pressEnter();
            break;

        case 2: // Buy
            clearScreen();
            buyStock(market, portfolio);
            pressEnter();
            break;

        case 3: // Sell
            clearScreen();
            sellStock(market, portfolio);
            pressEnter();
            break;

        case 4: // Portfolio
            clearScreen();
            portfolio.display(market.getStocks());
            pressEnter();
            break;

        case 5: // Live Market Feed
            liveMarketFeed(market);
            break;

        case 6: { // Next Day
            clearScreen();
            std::cout << "\033[1;33m  Simulating Day "
                      << market.getDay() + 1 << "...\033[0m\n\n";
            std::string news = market.nextDay();
            if (!news.empty()) {
                std::cout << "  \033[1;34m*** BREAKING NEWS *****************************\033[0m\n";
                std::cout << "  " << news << "\n";
                std::cout << "  \033[1;34m**********************************************\033[0m\n\n";
            }
            market.display();
            pressEnter();
            break;
        }

        case 7: { // AI Bot
            clearScreen();
            BotStrategy strat = chooseBotStrategy();
            bot.setStrategy(strat);
            bot.run(market, portfolio);
            pressEnter();
            break;
        }

        case 8: // Stock Detail
            clearScreen();
            viewStockDetail(market);
            pressEnter();
            break;

        case 9: { // Save + Export
            clearScreen();
            bool ok1 = FileManager::savePortfolio(portfolio,
                                                   market.getStocks(),
                                                   market.getDay());
            bool ok2 = FileManager::exportCsv(market.getStocks());
            if (ok1) std::cout << "\033[32m  Portfolio saved to save.txt\033[0m\n";
            else     std::cout << "\033[31m  Save failed!\033[0m\n";
            if (ok2) std::cout << "\033[32m  History exported to history.csv\033[0m\n";
            else     std::cout << "\033[31m  Export failed!\033[0m\n";
            pressEnter();
            break;
        }

        case 0: { // Exit
            clearScreen();
            std::cout << "\n\033[1;33m  Thanks for playing! Final results:\033[0m\n";
            portfolio.display(market.getStocks());

            std::string grade;
            double pct = portfolio.profitLossPercent(market.getStocks());
            if      (pct >= 20)  grade = "S  -- Wall Street Legend! [Trophy]";
            else if (pct >= 10)  grade = "A  -- Excellent Trader!";
            else if (pct >= 5)   grade = "B  -- Good Job!";
            else if (pct >= 0)   grade = "C  -- Break-even";
            else if (pct >= -10) grade = "D  -- Lost some money";
            else                 grade = "F  -- Rough market";

            std::cout << "  \033[1mYour Grade: " << grade << "\033[0m\n\n";
            running = false;
            break;
        }

        } // end switch
    }

    return 0;
}
