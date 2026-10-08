#include <cassert>
#include <iostream>

#include "order_book.hpp"

using namespace feed;

namespace {

// not using a real test framework, just asserts. good enough for now

void addThenCancelRemovesLevel() {
    OrderBook book;
    book.onAdd(1, 1, Side::Buy, 100, 10);

    auto bid = book.bestBid(1);
    assert(bid.has_value());
    assert(bid->price == 100);
    assert(bid->totalQty == 10);

    book.onCancel(1);
    assert(!book.bestBid(1).has_value());
}

void bestBidTracksHighestPrice() {
    OrderBook book;
    book.onAdd(1, 1, Side::Buy, 100, 10);
    book.onAdd(2, 1, Side::Buy, 105, 5);

    auto bid = book.bestBid(1);
    assert(bid->price == 105);
    assert(bid->totalQty == 5);

    book.onCancel(2);
    bid = book.bestBid(1);
    assert(bid->price == 100);
    assert(bid->totalQty == 10);
}

void bestAskTracksLowestPrice() {
    OrderBook book;
    book.onAdd(1, 1, Side::Sell, 110, 10);
    book.onAdd(2, 1, Side::Sell, 108, 5);

    auto ask = book.bestAsk(1);
    assert(ask->price == 108);
    assert(ask->totalQty == 5);
}

void partialTradeReducesQtyWithoutRemovingOrder() {
    OrderBook book;
    book.onAdd(1, 1, Side::Buy, 100, 10);
    book.onTrade(1, 4);

    auto bid = book.bestBid(1);
    assert(bid->totalQty == 6);

    book.onTrade(1, 6);
    assert(!book.bestBid(1).has_value());
}

void tradeExceedingResidualQtyClampsAndRemovesOrder() {
    OrderBook book;
    book.onAdd(1, 1, Side::Buy, 100, 5);
    book.onTrade(1, 1000);
    assert(!book.bestBid(1).has_value());
}

void symbolsAreIsolated() {
    OrderBook book;
    book.onAdd(1, 1, Side::Buy, 100, 10);
    book.onAdd(2, 2, Side::Buy, 200, 20);

    assert(book.bestBid(1)->price == 100);
    assert(book.bestBid(2)->price == 200);
}

} // namespace

int main() {
    addThenCancelRemovesLevel();
    bestBidTracksHighestPrice();
    bestAskTracksLowestPrice();
    partialTradeReducesQtyWithoutRemovingOrder();
    tradeExceedingResidualQtyClampsAndRemovesOrder();
    symbolsAreIsolated();

    std::cout << "all tests passed\n";
    return 0;
}
