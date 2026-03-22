#pragma once

#include <algorithm>
#include <functional>
#include <vector>
#include <memory>

// region Definitions

template<typename... Args>
class Handle;

using EventId = uint64_t;
static constexpr EventId INVALID_EVENT_ID{0};

template<typename... Args>
class Event : public std::enable_shared_from_this<Event<Args...>> {
public:
    Event() = default;
    ~Event() = default;

    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    Event(Event&&) noexcept;
    Event& operator=(Event&&) noexcept;

    std::shared_ptr<Handle<Args...>> subscribe(std::function<void(Args...)>);
    void unsubscribe(EventId);
    void fire(Args...);

private:
    struct Subscription {
        EventId m_eventID{};
        std::function<void(Args...)> m_callback{};
    };

    std::vector<Subscription> m_subscriptions;
    EventId m_maxEventId{1};
};

template <typename... Args>
class Handle {
public:
    Handle() = delete;
    Handle(std::weak_ptr<Event<Args...>>, EventId) noexcept;
    ~Handle() noexcept;

    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;

    Handle(Handle&&) noexcept;
    Handle& operator=(Handle&&) noexcept;

private:
    void unsubscribe();

    std::weak_ptr<Event<Args...>> m_parent{};
    EventId m_eventID{};
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
void Event<Args...>::unsubscribe(EventId eventId) {
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
Handle<Args...>::Handle(std::weak_ptr<Event<Args...>> weak_ptr, EventId eventId) noexcept
    : m_parent(weak_ptr), m_eventID(eventId) {}

template <typename... Args>
Handle<Args...>::~Handle() noexcept {
    unsubscribe();
}

template <typename... Args>
Handle<Args...>::Handle(Handle&& other) noexcept {
    m_eventID = other.m_eventID;
    m_parent = std::move(other.m_parent);
    other.m_eventID = INVALID_EVENT_ID;
}

template <typename... Args>
Handle<Args...>& Handle<Args...>::operator=(Handle&& other) noexcept {
    if (this == &other) { return *this; }
    unsubscribe();
    m_eventID = other.m_eventID;
    m_parent = std::move(other.m_parent);
    other.m_eventID = INVALID_EVENT_ID;
    return *this;
}

template <typename... Args>
void Handle<Args...>::unsubscribe() {
    if (m_eventID == INVALID_EVENT_ID) { return; }
    if (auto parent = m_parent.lock()) {
        parent->unsubscribe(m_eventID);
    }
}

// endregion