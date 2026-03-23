#pragma once

#include <any>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <raylib.h>

// region Definitions

using OuterMap = std::unordered_map<std::type_index, std::any>;
template <typename T>
using InnerMap = std::unordered_map<std::string, std::shared_ptr<T>>;

class ResourceManager {
public:
    explicit ResourceManager(std::string resourcePath) : m_resourcePath(std::move(resourcePath)) {}

    template<typename T>
    std::shared_ptr<T> Get(const std::string&);

    template<typename T>
    std::shared_ptr<T> Load(const std::string&);

private:
    template<typename T>
    InnerMap<T>& getInnerMap() { return std::any_cast<InnerMap<T>&>(m_outerMap.at(std::type_index(typeid(T)))); }

private:
    std::string m_resourcePath{};
    OuterMap m_outerMap{};
};

// endregion

//region Implementations

template <typename T>
std::shared_ptr<T> ResourceManager::Get(const std::string& key) {
    m_outerMap.try_emplace(std::type_index(typeid(T)), InnerMap<T>{});
    auto& innerMap = getInnerMap<T>();
    if (!innerMap.contains(key)) {
        innerMap[key] = Load<T>(key);
    }
    return innerMap[key];
}

template <typename T>
std::shared_ptr<T> ResourceManager::Load(const std::string&) {
    static_assert(false, "No Load specialization for this type");
    // UNREACHABLE
    return nullptr;
}

template <>
inline std::shared_ptr<Texture2D> ResourceManager::Load(const std::string& path) {
    const std::string fullPath = m_resourcePath + path;
    return std::shared_ptr<Texture2D>(new Texture2D(LoadTexture(fullPath.c_str())), [](Texture2D* texture) {
        UnloadTexture(*texture);
        delete texture;
    });
}

//endregion
