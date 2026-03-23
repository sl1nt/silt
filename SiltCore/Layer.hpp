#pragma once

#include "Event.hpp"

// region Definitions

class Layer {
public:
    virtual ~Layer() = default;

    Layer(const Layer&) = delete;
    Layer& operator=(const Layer&) = delete;
    Layer(Layer&&) = delete;
    Layer& operator=(Layer&&) = delete;

    virtual void OnAttach() {};
    virtual void OnDetach() {};
};

class LayerManager {
public:
    void push(std::unique_ptr<Layer>);
    void pop();

    private:
    std::vector<std::unique_ptr<Layer>> m_layers;
};

// endregion

// region Implementations

inline void LayerManager::push(std::unique_ptr<Layer> layer) {
    m_layers.push_back(std::move(layer));
    m_layers.back()->OnAttach();
}

inline void LayerManager::pop() {
    m_layers.back()->OnDetach();
    m_layers.pop_back();
}

// endregion
