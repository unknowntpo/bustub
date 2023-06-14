//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"
#include <list>
#include <unordered_map>
#include "common/config.h"
#include "common/logger.h"

namespace bustub {
LRUReplacer::LRUReplacer(size_t num_pages) { capacity = num_pages; }

LRUReplacer::~LRUReplacer() = default;

auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
  auto it = um.find(*frame_id);
  if (it == um.end()) {
    LOG_INFO("frame_id: %d", it->second->frame_id);
    return false;
  }
  dl.splice(dl.begin(), dl, um[*frame_id]);
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) { LOG_INFO("Pin is called for frame_id: %d", frame_id); }

void LRUReplacer::Unpin(frame_id_t frame_id) { LOG_INFO("Unpin is called for frame_id: %d", frame_id); }

auto LRUReplacer::Size() -> size_t {
  LOG_INFO("Size is called");
  return 33;
}

}  // namespace bustub

/*

class LRUCache {
 private:
  int capacity;
  list<pair<int, int>> dl;
  unordered_map<int, list<pair<int, int>>::iterator> um;

 public:
  LRUCache(int capacity) { this->capacity = capacity; }

  int get(int key) {
    if (um.find(key) == um.end()) {
      return -1;
    }
    dl.splice(dl.begin(), dl, um[key]);
    return um[key]->second;
  }

  void put(int key, int value) {
    if (um.find(key) != um.end()) {
      dl.erase(um[key]);
    } else if (dl.size() == capacity) {
      int delKey = dl.back().first;
      dl.pop_back();
      um.erase(delKey);
    }
    dl.push_front({key, value});
    um[key] = dl.begin();
  }
};
*/
