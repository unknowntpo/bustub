//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.h
//
// Identification: src/include/buffer/lru_replacer.h
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <mutex>  // NOLINT
#include <unordered_map>
#include <vector>

#include "buffer/replacer.h"
#include "common/config.h"

namespace bustub {

struct node {
  frame_id_t frame_id;
  size_t ref_cnt;
};

struct entry {
  frame_id_t frame_id;
  bool pinned;
  std::list<node>::iterator it;
};

/**
 * LRUReplacer implements the Least Recently Used replacement policy.
 */
class LRUReplacer : public Replacer {
 public:
  /**
   * Create a new LRUReplacer.
   * @param num_pages the maximum number of pages the LRUReplacer will be required to store
   */
  explicit LRUReplacer(size_t num_pages);

  /**
   * Destroys the LRUReplacer.
   */
  ~LRUReplacer() override;

  /**
   * Remove the victim frame as defined by the replacement policy.
   * @param[out] frame_id id of frame that was removed, nullptr if no victim was found
   * @return true if a victim frame was found, false otherwise
   */
  auto Victim(frame_id_t *frame_id) -> bool override;

  /**
   * Pins a frame, indicating that it should not be victimized until it is unpinned.
   * @param frame_id the id of the frame to pin
   */
  void Pin(frame_id_t frame_id) override;

  /**
   * Unpins a frame, indicating that it can now be victimized.
   * @param frame_id the id of the frame to unpin
   */
  void Unpin(frame_id_t frame_id) override;

  void Debug();

  /** @return the number of elements in the replacer that can be victimized */
  auto Size() -> size_t override;

 private:
  // TODO(student): implement me!
  size_t capacity;
  // push from head, pop from back
  std::list<node> unpinned_list;
  std::list<node> pinned_list;
  std::unordered_map<frame_id_t, entry> entries;
};
}  // namespace bustub
