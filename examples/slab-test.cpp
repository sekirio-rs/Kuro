#include "slab.h"
#include <iostream>

class Element {
public:
  int val;
  Element() {}
  Element(int val) : val(val) {}
};

template class Slab<Element>;

int main() {
  Slab<Element> slab(10);
  
  std::cout << "len of slab: " << slab.len() << std::endl;
  std::cout << "capacity of slab: " << slab.capacity() << std::endl;

  Element e0 = Element(0);
  Element e1 = Element(1);
  Element e2 = Element(2);

  auto token0 = slab.insert(std::move(e0));
  auto token1 = slab.insert(std::move(e1));
  auto token2 = slab.insert(std::move(e2));

  
  std::cout << "len of slab: " << slab.len() << std::endl;
  std::cout << "capacity of slab: " << slab.capacity() << std::endl;

  slab.remove(token1);

  std::cout << "len of slab: " << slab.len() << std::endl;
  std::cout << "capacity of slab: " << slab.capacity() << std::endl;
  
  auto token3 = slab.insert(std::move(Element(3)));

  std::cout << "token0: " << token0 << std::endl;
  std::cout << "token1: " << token1 << std::endl;
  std::cout << "token2: " << token2 << std::endl;
  std::cout << "token3: " << token3 << std::endl;

  return 0;
}

