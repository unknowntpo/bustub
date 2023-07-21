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

void LRUKReplacer::Debug() {
  LOG_INFO("capacity=[%ld], size=[%ld]", this->replacer_size_, this->Size());

  for (auto it = this->node_store_.begin(); it != this->node_store_.end(); it++) {
    LOG_INFO("node_store_[%d] = {k_: %ld, is_evictable: %d}", it->first, it->second.k_, it->second.is_evictable_);
  }

  for (const auto &n : this->hist_list_) {
    LOG_INFO("hist_list node: {fid_: %d}", n.fid_);
  }

  LOG_INFO("size hist_list node: %ld", this->hist_list_.size());

  LOG_INFO("size cache list : %ld", this->cache_list_.size());

  for (auto it = this->cache_list_.begin(); it != this->cache_list_.end(); it++) {
    LOG_INFO("cache_list node: {fid_: %d}", it->fid_);
  }
}

// if frame_id doesn't exist, return false
// if frame_id is not evictable, return false
// look for frame_id from map, and delete it from hist_list or cache_list,
auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  // search hist_list, if exist victim, then evict it
  for (auto rit = this->hist_list_.rbegin(); rit != this->hist_list_.rend(); rit++) {
    auto it = --rit.base();
    ListNode n;
    auto map_node_it = it->map_node_it_;
    if (it->map_node_it_->second.is_evictable_) {
      *frame_id = it->fid_;
      this->hist_list_.erase(it);
      this->node_store_.erase(map_node_it);
      this->curr_size_--;
      return true;
    }
  }

  // if hist_list has no victim, then get victim from cache_list
  for (auto rit = this->cache_list_.rbegin(); rit != this->cache_list_.rend(); rit++) {
    auto it = --rit.base();
    ListNode n;
    auto map_node_it = it->map_node_it_;
    if (it->map_node_it_->second.is_evictable_) {
      *frame_id = it->fid_;
      this->cache_list_.erase(it);
      this->node_store_.erase(map_node_it);
      this->curr_size_--;
      return true;
    }
  }

  // victim not found
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
    MapNode new_node = MapNode(frame_id, this->hist_list_.begin(), false, 1);
    this->node_store_.emplace(frame_id, new_node);
    this->hist_list_.begin()->map_node_it_ = this->node_store_.find(frame_id);
    return;
  }

  // if not exceed k: move it to head of hist list, and incr k
  // if k-1 -> k: move it to cache list, and incr k
  //(o) if k -> inf: move it to head of cache list, and incr k
  //
  //
  //

  LOG_INFO("before check, frame_id: %d, k: %ld, it->second.K(): %ld, exceed k: %d", frame_id, it->second.k_,
           it->second.K(), it->second.ExceedK(this->k_));
  if (it->second.K() == this->k_ - 1) {
    // FIXME: Why this check not true for frame_id = 1
    // if ((!it->second.ExceedK(this->k_)) && it->second.K() == this->k_ - 1) {
    LOG_INFO("need to move from hist list to cache list frame_id: %d", frame_id);
    it->second.k_++;
    LOG_INFO("frame_id: %d mapnode.k is %ld", frame_id, it->second.k_);
    auto l_it = it->second.it_;
    // move from hist list to cache list
    this->cache_list_.splice(this->cache_list_.begin(), this->hist_list_, l_it, std::next(l_it));
    it->second.it_ = this->cache_list_.begin();
    return;
  }
  if (it->second.ExceedK(this->k_)) {
    LOG_INFO("need to move from cache list to head of cache list frame_id: %d", frame_id);
    it->second.k_++;
    auto l_it = it->second.it_;
    // move to head of cache_list
    auto node = *l_it;
    this->cache_list_.erase(l_it);
    this->cache_list_.push_front(node);
    it->second.it_ = this->cache_list_.begin();
  } else {
    it->second.k_++;
    auto l_it = it->second.it_;
    auto node = *l_it;
    this->hist_list_.erase(l_it);
    this->hist_list_.push_front(node);
    it->second.it_ = this->hist_list_.begin();
    return;
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

bool MapNode::ExceedK(size_t k) { return this->k_ >= k; }
size_t MapNode::K() { return this->k_; }

// MapNode::MapNode(size_t frame_id) : fid_(frame_id) {}

}  // namespace bustub
   //
