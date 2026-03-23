#pragma once

#include "Event.hpp"

// region Definitions

class Layer : public std::enable_shared_from_this<Layer> {
public:
    virtual ~Layer() = default;

    Layer(const Layer&) = delete;
    Layer& operator=(const Layer&) = delete;

    Layer(Layer&&) = default;
    Layer& operator=(Layer&&) = default;

    friend class LayerManager;

protected:
    virtual void OnAttach() {};
    virtual void OnDetach() {};
    virtual void OnUpdate() {};
    virtual void OnRender() {};

private:
    explicit Layer(const EventManager&);
};

class LayerManager {
public:
    void push(std::shared_ptr<Layer>);
    void pop();

private:
    std::vector<std::shared_ptr<Layer>> m_layers;
};

// endregion

// region Implementations

inline Layer::Layer(const EventManager& eventManager) {

}

inline void LayerManager::push(std::shared_ptr<Layer> layer) {
    m_layers.push_back(std::move(layer));
    m_layers.back()->OnAttach();
}

inline void LayerManager::pop() {
    m_layers.back()->OnDetach();
    m_layers.pop_back();
}

// endregion
