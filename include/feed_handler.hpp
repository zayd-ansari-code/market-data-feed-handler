#pragma once

#include <string>
#include <vector>

#include "message.hpp"
#include "order_book.hpp"

namespace feed {

std::vector<WireMessage> loadFeed(const std::string& path);
void applyMessage(OrderBook& book, const WireMessage& msg);

} // namespace feed
