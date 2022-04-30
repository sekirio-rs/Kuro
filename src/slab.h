/*
 * Copyright (C) 2022 SKTT1Ryze. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under License.
 * */

#include <optional>
#include <vector>

typedef unsigned long usize;

#define ENTRY_VAL 0
#define ENTRY_NEXT 1

template <typename T>
class Entry {
 public:
  int type_;

  union {
    T val;
    usize next;
  };

  Entry(T&& val);
  Entry(usize next);
};

template <typename T>
class slab {
 public:
  slab(usize capacity);

  usize capacity();

  usize len();

  void clear();

  bool is_empty();

  std::optional<T*> get(usize key);

  usize insert(T&& val);

  std::optional<T> remove(usize key);

 private:
  std::vector<Entry<T>> entries;
  usize len_;
  usize next;

  void insert_at(usize key, T&& val);
};
