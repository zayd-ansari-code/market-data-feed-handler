Feed Handler + Order Book

Small C++20 project that reads a stream of add/cancel/trade messages and applies them to a live order book, tracked separately per symbol.

To build: cmake -B build -S . then cmake --build build

To run it: ./build/generate_feed build/feed.bin makes a test feed, ./build/feed_handler_main build/feed.bin runs it and prints the resulting book, ./build/book_tests runs the test suite.

message.hpp has the binary message format. order_book.hpp/.cpp is the book itself, bids sorted highest price first, asks lowest first, with order lookup by id for cancels and fills. feed_handler.cpp reads the message stream and applies it to a book. generate_feed.cpp makes a synthetic feed to test against. book_tests.cpp has the correctness checks.
