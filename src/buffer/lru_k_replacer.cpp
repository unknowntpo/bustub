//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include <sys/fcntl.h>
#include <set>
#include "common/config.h"
#include "common/exception.h"
#include "common/logger.h"

namespace bustub {

LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

// if frame_id doesn't exist, return false
// if frame_id is not evictable, return false
// look for frame_id from map, and delete it from hist_list or cache_list,
auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  LOG_INFO("Evicted frame_id: %d", *frame_id);

  /*
  // search hist_list, if exist victim, then evict it
  for (auto it = this->hist_list_.begin(); it != this->hist_list_.end(); it++) {
    if (it->map_node_ptr_->is_evictable_) {
      // trim
    }
  }
  */
  // if hist_list has not victim, search hist_list
  return false;
}

void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
  LOG_INFO("RecordAccess frame_id: %d", frame_id);

  auto it = this->node_store_.find(frame_id);
  if (it == this->node_store_.end()) {
    ListNode list_node = {
        .fid_ = frame_id,
    };
    this->hist_list_.push_front(list_node);
    MapNode new_node = MapNode(frame_id, this->hist_list_.begin(), false, 0);
    this->hist_list_.begin()->map_node_ptr_ = &new_node;
    this->node_store_.emplace(frame_id, new_node);
    return;
  }

  if (it->second.ExceedK(this->k_)) {
    it->second.k_++;
    // move to cache_list
    auto l_it = it->second.it_;
    this->cache_list_.splice(this->cache_list_.begin(), this->hist_list_, l_it, (++l_it));
    it->second.it_ = this->cache_list_.begin();
  } else {
    it->second.k_++;
  }
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  LOG_INFO("SetEvictable frame_id: %d", frame_id);
  auto it = this->node_store_.find(frame_id);
  if (it == this->node_store_.end()) {
    LOG_ERROR("in SetEvictable, frame_id: %d does not exist", frame_id);
    return;
  }
  if (it->second.is_evictable_ == set_evictable) {
    // no change
    return;
  }
  if (it->second.is_evictable_ && !set_evictable) {
    this->curr_size_--;
  }
  if (!it->second.is_evictable_ && set_evictable) {
    this->curr_size_++;
  }
  it->second.is_evictable_ = set_evictable;
}

void LRUKReplacer::Remove(frame_id_t frame_id) { LOG_INFO("Remove frame_id: %d", frame_id); }

auto LRUKReplacer::Size() -> size_t {
  LOG_INFO("size is called");
  return this->curr_size_;
}

/*
MapNode::MapNode(size_t frame_id, std::list<ListNode>::iterator it, bool is_evictable, size_t k)
    : fid_(frame_id), it_(it), is_evictable_(is_evictable), k_(k){};
    */

MapNode::MapNode(size_t frame_id, std::list<ListNode>::iterator it, bool is_evictable, size_t k) {
  fid_ = frame_id;
  it_ = it;
  is_evictable_ = is_evictable;
  k_ = k;
}

bool MapNode::ExceedK(size_t k) { return k >= this->k_; }

// MapNode::MapNode(size_t frame_id) : fid_(frame_id) {}

}  // namespace bustub
   //
