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

// Victim set frame_id with victim's frame_id.
// RETURN VALUE: bool
// If Victim doesn't exist, return false, else return true.
auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
  auto it = dl.begin();
  if (it == dl.end()) {
    return false;
  }
  // get the first node which it->ref_cnt > 0
  size_t iter_count = 0;
  while (it->ref_cnt > 0 && iter_count < 1000) {
    LOG_INFO("ref_cnt: %ld", it->ref_cnt);
    it++;
    iter_count++;
  }

  *frame_id = it->frame_id;
  dl.erase(it);
  um.erase(*frame_id);
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  LRUReplacer::Debug();
  auto pair = um.find(frame_id);
  if (pair != um.end()) {
    LOG_INFO("pair: first:  %d, second: %d", pair->first, pair->second->frame_id);
    auto it = pair->second;
    it->ref_cnt++;
    um[frame_id] = it;
    return;
  }
  LOG_WARN("Pin is called with non-exist frame_id: %d", frame_id);
}

void LRUReplacer::Debug() {
  for (auto it = um.begin(); it != um.end(); it++) {
    LOG_INFO("um[%d] = {frame_id: %d, ref_cnt: %ld}", it->first, it->second->frame_id, it->second->ref_cnt);
  }

  for (const auto &n : dl) {
    LOG_INFO("n: {frame_id: %d, ref_cnt: %ld}", n.frame_id, n.ref_cnt);
  }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  //  // if exist -> move frame_id to front
  // frame_id not exist, create new entry
  if (um.find(frame_id) == um.end()) {
    // add node to head of dl
    node new_node;
    new_node.frame_id = frame_id;
    new_node.ref_cnt = 0;
    dl.push_back(new_node);
    LOG_INFO("new_node: frame_id: %d, ref_cnt: %ld", new_node.frame_id, new_node.ref_cnt);
    um[frame_id] = dl.rbegin().base();
  } else {
    // move target node to front
    dl.splice(dl.begin(), dl, um[frame_id]);
  }
  // if doesn't -> add it
  LOG_INFO("Unpin is called for frame_id: %d", frame_id);
}

auto LRUReplacer::Size() -> size_t { return dl.size(); }

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
