#pragma once

#include <cstdint>

namespace feed {

using OrderId = std::uint64_t;
using Price = std::int64_t;
using Qty = std::uint32_t;
using SymbolId = std::uint16_t;

enum class MsgType : std::uint8_t { Add = 1, Cancel = 2, Trade = 3 };
enum class Side : std::uint8_t { Buy = 0, Sell = 1 };

// packed so the struct is exactly 24 bytes on disk, no padding
// side/price dont mean anything for cancel/trade msgs but its simpler
// to just use one struct for everything instead of 3 different ones
#pragma pack(push, 1)
struct WireMessage {
    MsgType type;
    OrderId orderId;
    SymbolId symbol;
    Side side;
    Price price;
    Qty qty;
};
#pragma pack(pop)

static_assert(sizeof(WireMessage) == 24, "wire format layout changed");

} // namespace feed
