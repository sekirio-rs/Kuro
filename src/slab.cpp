#include "slab.h"

#include <algorithm>

template <typename T>
Entry<T>::Entry(T&& val) : val(val) {
  type_ = ENTRY_VAL;
}

template <typename T>
Entry<T>::Entry(usize next) : next(next) {
  type_ = ENTRY_NEXT;
}

template <typename T>
Slab<T>::Slab(usize capacity) {
  entries.assign(capacity, Entry<T>(0));
  entries.clear();
  len_ = 0;
  next = 0;
}

template <typename T>
usize Slab<T>::capacity() {
  return entries.capacity();
}

template <typename T>
usize Slab<T>::len() {
  return len_;
}

template <typename T>
void Slab<T>::clear() {
  entries.clear();
  len_ = 0;
  next = 0;
}

template <typename T>
bool Slab<T>::is_empty() {
  return len_ == 0;
}

template <typename T>
std::optional<T*> Slab<T>::get(usize key) {
  if (key >= len_) return {};

  auto entry = entries.at(key);

  if (entry.type_ == ENTRY_VAL) return &entry.val;
  return {};
}

template <typename T>
usize Slab<T>::insert(T&& val) {
  auto key = next;

  this->insert_at(key, std::move(val));

  return key;
}

template <typename T>
void Slab<T>::insert_at(usize key, T&& val) {
  len_++;
  if (key == entries.size()) {
    entries.push_back(Entry<T>(std::move(val)));

    next = key + 1;
  } else {
    auto entry = entries.at(key);
    if (entry.type_ == ENTRY_NEXT) {
      next = entry.next;
    } else {
      throw "unreachable";
      exit(1);
    }

    entries[key] = Entry<T>(std::move(val));
  }
}

template <typename T>
std::optional<T> Slab<T>::remove(usize key) {
  auto prev = Entry<T>(next);

  std::swap(prev, entries[key]);

  if (prev.type_ == ENTRY_VAL) {
    len_--;
    next = key;

    return std::move(prev.val);
  } else {
    entries[key] = prev;

    return {};
  }
}
