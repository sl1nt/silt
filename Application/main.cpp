#include <iostream>
#include "../SiltCore/ResourceManager.hpp"
#include "../SiltCore/Event.hpp"
#include <print>

int main() {
    Event<int> e;

    auto h = e.subscribe([](int value) {
        std::println("{}", value + 10);
    });

    e.fire(42);

    return 0;
}