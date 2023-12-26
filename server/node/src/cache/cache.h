#pragma once

#include <unordered_map>
#include <mutex>
#include <iostream>

using sys_clock = std::chrono::system_clock;

template <typename V>
struct CacheEntry {
    V value;
    sys_clock::time_point expiration_time;
};

template <typename K, typename V>
class Cache {
    private:
        std::unordered_map<K, CacheEntry<V>> cache_store_;

        std::mutex cache_mutex_;

        static const int MAX_CACHE_SIZE = 10000;

    public:
        Cache();

        V get(K);
        void set(K, V);
        bool isEntryExpired(CacheEntry<V> &);
        std::unordered_map<K, CacheEntry<V>> & getCacheStore();
};

/*
** Class Source
*/
template <typename K, typename V>
Cache<K, V>::Cache() {
    cache_store_.clear();
}

template <typename K, typename V>
V Cache<K, V>::get(K key) {
    // lock only the write operation while reading
    std::unique_lock<std::mutex> lock(cache_mutex_);
    if (cache_store_.count(key) == 0) {
        return V();
    }

    CacheEntry<V>& entry = cache_store_[key];

    if (isEntryExpired(entry)) {
        cache_store_.erase(key);
        return V();
    }
    return entry.value;
}

template <typename K, typename V>
void Cache<K, V>::set(K key, V value) {
    // lock the access to cache
    std::unique_lock<std::mutex> lock(cache_mutex_);

    // capacity check
    if (cache_store_.size() >= MAX_CACHE_SIZE) {
        // evict(load_factor=0.2);
    }

    // expiration time
    std::chrono::hours expiration_duration(1);
    sys_clock::time_point current_time = sys_clock::now();
    sys_clock::time_point expiration_time = current_time + expiration_duration;

    cache_store_[key] = CacheEntry<V>{value, expiration_time};
}

template <typename K, typename V>
bool Cache<K, V>::isEntryExpired(CacheEntry<V>& entry) {
    return sys_clock::now() > entry.expiration_time;
}

template <typename K, typename V>
std::unordered_map<K, CacheEntry<V>> & Cache<K, V>::getCacheStore() {
    return cache_store_;
}