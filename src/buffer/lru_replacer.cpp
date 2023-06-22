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
LRUReplacer::LRUReplacer(size_t num_pages) {
  capacity = num_pages;
  pinned_list = {};
  unpinned_list = {};
}

LRUReplacer::~LRUReplacer() = default;

// Victim set frame_id with victim's frame_id.
// RETURN VALUE: bool
// If Victim doesn't exist, return false, else return true.
auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
  if (unpinned_list.size() == 0) return false;
  *frame_id = unpinned_list.back().frame_id;
  unpinned_list.pop_back();
  entries.erase(*frame_id);
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  // if doesn't exist
  auto en_it = entries.find(frame_id);
  if (en_it == entries.end()) {
    return;
  }
  entry en = en_it->second;
  if (en.pinned) {
    // ref_cnt++
    en.it->ref_cnt++;
  } else {
    // move from unpinned_list to pinned_list

    pinned_list.splice(pinned_list.begin(), unpinned_list, en.it, ++(en.it));
    en.it = pinned_list.begin();
    en.it->ref_cnt++;
    en.pinned = true;
  }
}

void LRUReplacer::Debug() {
  LOG_INFO("capacity=[%ld], size=[%ld]", capacity, this->Size());

  for (auto it = entries.begin(); it != entries.end(); it++) {
    LOG_INFO("entries[%d] = {pinned: %d, ref_cnt: %ld}", it->first, it->second.pinned, it->second.it->ref_cnt);
  }

  for (const auto &n : unpinned_list) {
    LOG_INFO("unpinned_list node: {frame_id: %d, ref_cnt: %ld}", n.frame_id, n.ref_cnt);
  }

  for (const auto &n : pinned_list) {
    LOG_INFO("pinned_list node: {frame_id: %d, ref_cnt: %ld}", n.frame_id, n.ref_cnt);
  }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  // lookup frame_id in entries
  auto pair = entries.find(frame_id);
  if (pair != entries.end()) {
    entry en = pair->second;
    auto old_node = *en.it;
    en.it->ref_cnt--;
    if (old_node.ref_cnt == 0) {
      unpinned_list.push_front(old_node);
      en.it = unpinned_list.begin();
      entries[frame_id] = en;
    }
  } else {
    node new_node = {.frame_id = frame_id, .ref_cnt = 0};
    unpinned_list.push_front(new_node);
    entries[frame_id] = {.frame_id = frame_id, .pinned = false, .it = unpinned_list.begin()};
  }
}

auto LRUReplacer::Size() -> size_t { return unpinned_list.size() + pinned_list.size(); }

}  // namespace bustub
