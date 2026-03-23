#pragma once

#include <algorithm>
#include <functional>
#include <vector>
#include <memory>

// region Definitions

template<typename... Args>
class Handle;

using Id = uint64_t;
static constexpr Id INVALID_EVENT_ID{0};

template<typename... Args>
class Event : public std::enable_shared_from_this<Event<Args...>> {
public:
    Event() = default;
    ~Event() = default;

    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event(Event&&) noexcept;
    Event& operator=(Event&&) noexcept;

    [[nodiscard]] std::shared_ptr<Handle<Args...>> subscribe(std::function<void(Args...)>);
    void unsubscribe(Id);
    void fire(Args...);

private:
    struct Subscription {
        Id m_eventID{};
        std::function<void(Args...)> m_callback{};
    };

    std::vector<Subscription> m_subscriptions;
    Id m_maxEventId{1};
};

class EventManager {
public:
};

template <typename... Args>
class Handle {
public:
    Handle() = delete;
    Handle(std::weak_ptr<Event<Args...>>, Id) noexcept;
    ~Handle() noexcept;

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    Handle(Handle&&) noexcept;
    Handle& operator=(Handle&&) noexcept;

private:
    void unsubscribe();

    std::weak_ptr<Event<Args...>> m_parent{};
    Id m_ID{};
};

// endregion

// region Implementations

template <typename... Args>
Event<Args...>::Event(Event&& other) noexcept {
    m_subscriptions = std::move(other.m_subscriptions);
}

template <typename... Args>
Event<Args...>& Event<Args...>::operator=(Event&& other) noexcept {
    if (this == &other) { return *this; }
    m_subscriptions = std::move(other.m_subscriptions);
    return *this;
}

template <typename... Args>
std::shared_ptr<Handle<Args...>> Event<Args...>::subscribe(std::function<void(Args...)> callable) {
    const auto eventId = m_maxEventId++;
    m_subscriptions.emplace_back(Subscription{eventId, std::move(callable)});
    return std::make_shared<Handle<Args...>>(this->weak_from_this(), eventId);
}

template <typename... Args>
void Event<Args...>::unsubscribe(Id eventId) {
    auto removeStart = std::remove_if(m_subscriptions.begin(), m_subscriptions.end(), [eventId](auto& subscription) {
        return eventId == subscription.m_eventID;
    });
    m_subscriptions.erase(removeStart, m_subscriptions.end());
}

template <typename... Args>
void Event<Args...>::fire(Args... args) {
    for (auto& subscription : m_subscriptions) {
        subscription.m_callback(args...);
    }
}

template <typename... Args>
Handle<Args...>::Handle(std::weak_ptr<Event<Args...>> weak_ptr, Id eventId) noexcept
    : m_parent(weak_ptr), m_ID(eventId) {}

template <typename... Args>
Handle<Args...>::~Handle() noexcept {
    unsubscribe();
}

template <typename... Args>
Handle<Args...>::Handle(Handle&& other) noexcept {
    m_ID = other.m_ID;
    m_parent = std::move(other.m_parent);
    other.m_ID = INVALID_EVENT_ID;
}

template <typename... Args>
Handle<Args...>& Handle<Args...>::operator=(Handle&& other) noexcept {
    if (this == &other) { return *this; }
    unsubscribe();
    m_ID = other.m_ID;
    m_parent = std::move(other.m_parent);
    other.m_ID = INVALID_EVENT_ID;
    return *this;
}

template <typename... Args>
void Handle<Args...>::unsubscribe() {
    if (m_ID == INVALID_EVENT_ID) { return; }
    if (auto parent = m_parent.lock()) {
        parent->unsubscribe(m_ID);
    }
}

// endregion

