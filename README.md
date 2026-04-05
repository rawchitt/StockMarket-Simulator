# 📊 Stock Market Simulator — OOP C++ Project

A full-featured console-based stock market simulator built with **modern C++ (C++17)** using **Object-Oriented Programming** principles.

---

## 🗂️ Project Structure

```
OOPs Stock Market Project/
├── Stock.h          ← Module 1: Stock class (price, history, sparkline)
├── Portfolio.h      ← Module 2: Portfolio (cash, shares, P&L)
├── Market.h         ← Module 3: Market engine (stocks, time, news events)
├── TradingBot.h     ← Module 4: AI Trading Bot (2 strategies)
├── FileManager.h    ← Utility: Save/load & CSV export
├── main.cpp         ← Main app: console UI & game loop
├── compile.bat      ← One-click Windows build script
└── README.md        ← This file
```

---

## 🏗️ OOP Design

| Class | Responsibility |
|---|---|
| `Stock` | Stores price, volatility, history; updates price each day |
| `Portfolio` | Tracks cash, shares per stock, computes P&L |
| `Market` | Manages the stock universe, day simulation, news events |
| `TradingBot` | Two AI strategies: Threshold & Moving Average |
| `FileManager` | Saves portfolio to file, exports price history to CSV |

---

## ✨ Features

### 📦 Module 1 — Stock
- Ticker symbol + company name
- Daily price update via Mersenne-Twister random engine
- Per-stock volatility parameter
- 20-day price history via `std::deque`
- **SMA-5 / SMA-10** moving average calculation
- **ASCII sparkline** (▁▂▃▄▅▆▇█) for last 10 days

### 💼 Module 2 — Portfolio
- Starting cash: **$10,000**
- `map<string, int>` — efficient ticker→shares lookup
- Real-time P&L = (Total Value − Initial Investment)
- Colour-coded profit (🟢) / loss (🔴) display

### 📊 Module 3 — Market Engine
- **10 stocks** across tech, energy, pharma, consumer
- Day-by-day simulation with `nextDay()`
- **30% chance** of random news event each day  
  (Fed hikes, market crash, drug approvals, AI surge, etc.)
- ANSI-coloured live market board with trend arrows (▲▼)

### 🤖 Module 4 — AI Trading Bot
| Strategy | Logic |
|---|---|
| **Threshold** | Buy if daily drop ≥ 5%, Sell if daily rise ≥ 8% |
| **Moving Average** | Buy if price <98% of SMA-5, Sell if price >102% of SMA-5 |

### 💾 File Handling
- `Save Portfolio` → writes `save.txt`
- `Export History` → writes `history.csv` (importable in Excel)

---

## ⚙️ Data Structures Used

| Structure | Where Used |
|---|---|
| `class` | All four main modules |
| `std::vector<Stock>` | Market stock universe |
| `std::deque<double>` | Price history (auto-trims to 20 entries) |
| `std::map<string,int>` | Portfolio shares ledger |
| `std::mt19937` | Gaussian-quality randomness |
| `enum class` | Bot strategy type |

---

## 🔁 Price Fluctuation Formula

```
new_price = max(1.0, old_price + Uniform(−volatility, +volatility))
```

Using `std::uniform_real_distribution` with `std::mt19937` (seeded from `std::random_device`).

---

## 🖥️ Menu Options

```
[1]  📊  View Market          — live price board with sparklines
[2]  💚  Buy Stock            — guided buy flow
[3]  🔴  Sell Stock           — guided sell flow
[4]  📁  View Portfolio       — holdings + P&L
[5]  ⏭   Next Day            — price update + possible news event
[6]  🤖  Run AI Trading Bot   — choose strategy, auto-trade
[7]  📰  View Stock Detail    — full stats + SMA + history
[8]  💾  Save Portfolio       — write save.txt
[9]  📤  Export History       — write history.csv
[0]  🚪  Exit                 — final performance grade
```

---

## 🚀 Build & Run

### Requirements
- **g++** with C++17 support (MinGW / MSYS2 on Windows, or any Linux/macOS toolchain)

### Compile
```bash
# Windows (double-click or run in terminal)
compile.bat

# Or manually
g++ -std=c++17 -O2 -Wall -o StockMarket.exe main.cpp
```

### Run
```bash
.\StockMarket.exe     # Windows
./StockMarket         # Linux / macOS
```

---

## 📚 OOP Concepts Demonstrated

- ✅ **Encapsulation** — private data, public interface in every class
- ✅ **Abstraction** — `Market::nextDay()` hides all simulation internals
- ✅ **Composition** — `Portfolio` holds a `map` of shares; `Market` owns a `vector<Stock>`
- ✅ **Templates/STL** — `vector`, `deque`, `map`, `mt19937`, distributions
- ✅ **Static members** — `getRng()` singleton pattern inside `Stock`
- ✅ **const correctness** — all read-only methods marked `const`
- ✅ **enum class** — type-safe `BotStrategy` enum

---

## 🏆 Performance Grading

| Grade | P&L |
|---|---|
| S — Wall Street Legend 🏆 | +20%+ |
| A — Excellent Trader 🎯 | +10% to +20% |
| B — Good Job 📈 | +5% to +10% |
| C — Break-even 😐 | 0% to +5% |
| D — Lost Some Money 😬 | −10% to 0% |
| F — Rough Market 😢 | Below −10% |
