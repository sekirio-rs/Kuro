// todo: ref
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
