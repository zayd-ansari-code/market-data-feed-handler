#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "message.hpp"

using namespace feed;

namespace {

constexpr int kMessageCount = 20000;
constexpr SymbolId kSymbols[] = {1, 2, 3};
constexpr Price kBasePrices[] = {10000, 25000, 5000};

struct LiveOrder {
    SymbolId symbol;
    Side side;
    Price price;
    Qty qty;
};

} // namespace

int main(int argc, char** argv) {
    const std::string outPath = argc > 1 ? argv[1] : "feed.bin";

    std::ofstream out(outPath, std::ios::binary);
    if (!out) {
        std::cerr << "could not open output file: " << outPath << "\n";
        return 1;
    }

    std::mt19937 rng(42); // fixed seed so the feed is reproducible between runs
    std::uniform_int_distribution<int> actionDist(0, 99);
    std::uniform_int_distribution<int> symbolDist(0, 2);
    std::uniform_int_distribution<int> sideDist(0, 1);
    std::uniform_int_distribution<int> offsetDist(1, 50);
    std::uniform_int_distribution<Qty> qtyDist(1, 500);

    std::unordered_map<OrderId, LiveOrder> live;
    OrderId nextId = 1;

    for (int i = 0; i < kMessageCount; ++i) {
        const int action = live.empty() ? 0 : actionDist(rng);
        WireMessage msg{};

        // mostly adds, some cancels, some trades. not super realistic but good enough
        if (action < 60 || live.empty()) {
            const int symIdx = symbolDist(rng);
            const SymbolId symbol = kSymbols[symIdx];
            const Side side = sideDist(rng) == 0 ? Side::Buy : Side::Sell;
            const int offset = offsetDist(rng);
            const Price price = kBasePrices[symIdx] + (side == Side::Buy ? -offset : offset);
            const Qty qty = qtyDist(rng);
            const OrderId id = nextId++;

            msg.type = MsgType::Add;
            msg.orderId = id;
            msg.symbol = symbol;
            msg.side = side;
            msg.price = price;
            msg.qty = qty;

            live[id] = LiveOrder{symbol, side, price, qty};
        } else {
            // grab a random order that's still live and either cancel or trade against it
            auto it = live.begin();
            std::advance(it, std::uniform_int_distribution<size_t>(0, live.size() - 1)(rng));
            const OrderId id = it->first;

            if (action < 80) {
                msg.type = MsgType::Cancel;
                msg.orderId = id;
                live.erase(it);
            } else {
                const Qty remaining = it->second.qty;
                const Qty execQty = std::uniform_int_distribution<Qty>(1, remaining)(rng);
                msg.type = MsgType::Trade;
                msg.orderId = id;
                msg.qty = execQty;

                if (execQty >= remaining) {
                    live.erase(it);
                } else {
                    it->second.qty -= execQty;
                }
            }
        }

        out.write(reinterpret_cast<const char*>(&msg), sizeof(msg));
    }

    std::cout << "wrote " << kMessageCount << " messages to " << outPath << "\n";
    return 0;
}
