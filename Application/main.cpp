#include <iostream>
#include "../SiltCore/ResourceManager.hpp"
#include "../SiltCore/Event.hpp"
#include <print>

#include "../SiltCore/Layer.hpp"

int main() {
    Event<int> e;

    auto h = e.subscribe([](int value) {
        std::println("{}", value + 10);
        return value;
    });

    e.fire(42);

    LayerManager layerManager;
    EventManager eventManager;

    while (!WindowShouldClose()) {
        eventManager.m_onUpdate.fire(50.0);
        eventManager.m_onRender.fire();
    }

    return 0;
}
