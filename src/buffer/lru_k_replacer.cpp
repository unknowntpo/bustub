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
#include "common/exception.h"
#include "common/logger.h"

namespace bustub {

LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

/*

auto LRUKReplacer::Evict(frame_id_t *frame_id) -> bool {
  bool p_in_buffer;
  auto now = get_now();
  LOG_INFO("Evicted frame_id: %d", *frame_id);

  if (p_in_buffer) {
    if (now - Last(p) > coor_ref_period) {
      co_period_of_ref_page = Last(p) - Hist(p, 1);
      for (size_t i = 2; i <= k; i++) {
        Hist(p, i) = Hist(p, i - 1) + co_period_of_ref_page;
      }
      Hist(p, 1) = now;
      Last(p) = now;
    } else {
      Last(p) = t;
    }
  } else {
    auto min = now;
    fram_id_t victim;

    for (const q : all_pages) {
      if (now - Last(p) > coor_ref_period && Hist(q, k) < min) {
        victim = q;
        min = Hist(q, k);
      }
    }
    bool victim_is_dirty;
    if (victim_is_dirty) {
      write_victim_to_db(victim);
    }
    // now fetch the reference_page
    fetch_p_into_buf_prev_held_by_victim();
    if (exist(Hist(p)) == false) {
      allocate_hist_p();
      for (auto i = 2; i <= k; i++) {
        Hist(p, i) = 0;
      }
    } else {
      for (auto i = 2; i <= k; i++) {
        Hist(p, i) = Hist(p, i - 1);
      }
    }
    Hist(p, 1) = now;
    Last(p) = now;
  }
  return false;
}
*/

void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
  LOG_INFO("RecordAccess frame_id: %d", frame_id);
}

void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
  LOG_INFO("SetEvictable frame_id: %d", frame_id);
}

void LRUKReplacer::Remove(frame_id_t frame_id) { LOG_INFO("Remove frame_id: %d", frame_id); }

auto LRUKReplacer::Size() -> size_t {
  LOG_INFO("size is called");
  return 0;
}

}  // namespace bustub
