#include "order_book.hpp"

namespace feed {

void OrderBook::onAdd(OrderId id, SymbolId symbol, Side side, Price price, Qty qty) {
    // keep a copy of the order so cancel/trade can look it up by id later
    orders_[id] = RestingOrder{symbol, side, price, qty};
    if (side == Side::Buy) {
        bids_[symbol][price].totalQty += qty;
    } else {
        asks_[symbol][price].totalQty += qty;
    }
}

void OrderBook::onCancel(OrderId id) {
    auto it = orders_.find(id);
    if (it == orders_.end()) return; // already gone, nothing to do
    reduce(it->second, it->second.qty);
    orders_.erase(it);
}

void OrderBook::onTrade(OrderId id, Qty execQty) {
    auto it = orders_.find(id);
    if (it == orders_.end()) return;
    RestingOrder& order = it->second;
    const Qty applied = execQty < order.qty ? execQty : order.qty; // just in case the feed is bad
    reduce(order, applied);
    order.qty -= applied;
    if (order.qty == 0) orders_.erase(it);
}

// shared by cancel and trade, just pulls qty off whatever side/price the order sits at
void OrderBook::reduce(const RestingOrder& order, Qty qty) {
    auto reduceLevels = [&](auto& levelsBySymbol) {
        auto symIt = levelsBySymbol.find(order.symbol);
        if (symIt == levelsBySymbol.end()) return;
        auto& levels = symIt->second;
        auto levelIt = levels.find(order.price);
        if (levelIt == levels.end()) return;
        levelIt->second.totalQty -= qty;
        if (levelIt->second.totalQty == 0) levels.erase(levelIt); // dont leave empty levels lying around
    };
    if (order.side == Side::Buy) {
        reduceLevels(bids_);
    } else {
        reduceLevels(asks_);
    }
}

std::optional<BookLevel> OrderBook::bestBid(SymbolId symbol) const {
    auto it = bids_.find(symbol);
    if (it == bids_.end() || it->second.empty()) return std::nullopt;
    const auto& [price, level] = *it->second.begin(); // map sorted greatest first
    return BookLevel{price, level.totalQty};
}

std::optional<BookLevel> OrderBook::bestAsk(SymbolId symbol) const {
    auto it = asks_.find(symbol);
    if (it == asks_.end() || it->second.empty()) return std::nullopt;
    const auto& [price, level] = *it->second.begin(); // lowest ask is first here
    return BookLevel{price, level.totalQty};
}

} // namespace feed
