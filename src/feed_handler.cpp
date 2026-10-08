#include "feed_handler.hpp"

#include <fstream>
#include <stdexcept>

namespace feed {

// just loads the whole feed into memory, fine for a file this size
std::vector<WireMessage> loadFeed(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) throw std::runtime_error("could not open feed file: " + path);

    std::vector<WireMessage> messages;
    WireMessage msg;
    while (in.read(reinterpret_cast<char*>(&msg), sizeof(msg))) {
        messages.push_back(msg);
    }
    return messages;
}

void applyMessage(OrderBook& book, const WireMessage& msg) {
    // only 3 message types right now so a switch is fine
    switch (msg.type) {
        case MsgType::Add:
            book.onAdd(msg.orderId, msg.symbol, msg.side, msg.price, msg.qty);
            break;
        case MsgType::Cancel:
            book.onCancel(msg.orderId);
            break;
        case MsgType::Trade:
            book.onTrade(msg.orderId, msg.qty);
            break;
    }
}

} // namespace feed
