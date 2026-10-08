#include <iostream>
#include <vector>

#include "feed_handler.hpp"
#include "order_book.hpp"

using namespace feed;

namespace {

void printLevel(const char* label, SymbolId symbol, const std::optional<BookLevel>& level) {
    std::cout << "  symbol " << symbol << " " << label << ": ";
    if (level) {
        std::cout << "price=" << level->price << " qty=" << level->totalQty << "\n";
    } else {
        std::cout << "(empty)\n";
    }
}

} // namespace

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "feed.bin";

    const std::vector<WireMessage> messages = loadFeed(path);
    if (messages.empty()) {
        std::cerr << "no messages loaded from " << path << "\n";
        return 1;
    }

    OrderBook book;
    for (const WireMessage& msg : messages) {
        applyMessage(book, msg); // just replay the whole feed in order
    }

    std::cout << "processed " << messages.size() << " messages\n\n";
    std::cout << "final book state:\n";
    for (SymbolId symbol : {SymbolId{1}, SymbolId{2}, SymbolId{3}}) {
        printLevel("bid", symbol, book.bestBid(symbol));
        printLevel("ask", symbol, book.bestAsk(symbol));
    }

    return 0;
}
