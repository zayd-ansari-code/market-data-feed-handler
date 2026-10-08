#pragma once

#include <functional>
#include <map>
#include <optional>
#include <unordered_map>

#include "message.hpp"

namespace feed {

struct BookLevel {
    Price price;
    Qty totalQty;
};

class OrderBook {
public:
    void onAdd(OrderId id, SymbolId symbol, Side side, Price price, Qty qty);
    void onCancel(OrderId id);
    void onTrade(OrderId id, Qty execQty);

    std::optional<BookLevel> bestBid(SymbolId symbol) const;
    std::optional<BookLevel> bestAsk(SymbolId symbol) const;

private:
    struct RestingOrder {
        SymbolId symbol;
        Side side;
        Price price;
        Qty qty;
    };
    struct PriceLevel {
        Qty totalQty = 0; // dont need to know which orders make up this qty, just the sum
    };

    // bids sorted highest first, asks sorted lowest first, so begin() is always best price
    using BidLevels = std::map<Price, PriceLevel, std::greater<Price>>;
    using AskLevels = std::map<Price, PriceLevel>;

    std::unordered_map<SymbolId, BidLevels> bids_;
    std::unordered_map<SymbolId, AskLevels> asks_;
    std::unordered_map<OrderId, RestingOrder> orders_;

    void reduce(const RestingOrder& order, Qty qty);
};

} // namespace feed
