//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// buffer_pool_manager.cpp
//
// Identification: src/buffer/buffer_pool_manager.cpp
//
// Copyright (c) 2015-2021, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/buffer_pool_manager.h"

#include "common/exception.h"
#include "common/logger.h"
#include "common/macros.h"
#include "storage/page/page_guard.h"

namespace bustub {

BufferPoolManager::BufferPoolManager(size_t pool_size, DiskManager *disk_manager, size_t replacer_k,
                                     LogManager *log_manager)
    : pool_size_(pool_size), disk_manager_(disk_manager), log_manager_(log_manager) {
  // TODO(students): remove this line after you have implemented the buffer pool manager
  // throw NotImplementedException(
  //     "BufferPoolManager is not implemented yet. If you have finished implementing BPM, please remove the throw "
  //     "exception line in `buffer_pool_manager.cpp`.");

  // we allocate a consecutive memory space for the buffer pool
  pages_ = new Page[pool_size_];
  replacer_ = std::make_unique<LRUKReplacer>(pool_size, replacer_k);
  // FIXME: should I use this->free_list_ ?

  // Initially, every page is in the free list.
  for (size_t i = 0; i < pool_size_; ++i) {
    free_list_.emplace_back(static_cast<int>(i));
  }
}

BufferPoolManager::~BufferPoolManager() { delete[] pages_; }

/**
 * TODO(P1): Add implementation
 *
 * @brief Create a new page in the buffer pool. Set page_id to the new page's id, or nullptr if all frames
 * are currently in use and not evictable (in another word, pinned).
 *
 * You should pick the replacement frame from either the free list or the replacer (always find from the free list
 * first), and then call the AllocatePage() method to get a new page id. If the replacement frame has a dirty page,
 * you should write it back to the disk first. You also need to reset the memory and metadata for the new page.
 *
 * Remember to "Pin" the frame by calling replacer.SetEvictable(frame_id, false)
 * so that the replacer wouldn't evict the frame before the buffer pool manager "Unpin"s it.
 * Also, remember to record the access history of the frame in the replacer for the lru-k algorithm to work.
 *
 * @param[out] page_id id of created page
 * @return nullptr if no new pages could be created, otherwise pointer to new page
 */
auto BufferPoolManager::NewPage(page_id_t *page_id) -> Page * {
  LOG_INFO("NewPage is called");
  const std::lock_guard<std::mutex> lock(latch_);

  frame_id_t frame_id;
  if (free_list_.begin() != free_list_.end()) {
    frame_id = *free_list_.begin();
    free_list_.pop_front();
  } else {
    // pick from the replacer
    bool ok = replacer_->Evict(&frame_id);
    if (!ok) {
      LOG_INFO("no page can be created since replacer is full");
      return nullptr;
    }
    Page *page = &pages_[frame_id];
    if (page->IsDirty()) {
      if (!this->FlushPage(page->GetPageId())) {
        LOG_ERROR("failed to flush the dirty page");
        // FIXME: should I add back page_id to replacer ?
        replacer_->RecordAccess(frame_id);
        return nullptr;
      }
    }
  }

  Page *page = new Page();
  *page_id = this->AllocatePage();
  page->page_id_ = *page_id;
  page_table_.emplace(*page_id, frame_id);
  replacer_->RecordAccess(frame_id);
  replacer_->SetEvictable(frame_id, false);

  return page;
}

// TODO: impl
auto BufferPoolManager::FetchPage(page_id_t page_id, [[maybe_unused]] AccessType access_type) -> Page * {
  const std::lock_guard<std::mutex> lock(latch_);

  auto it = page_table_.find(page_id);
  if (it == page_table_.end()) {
    // not found
    return nullptr;
  }
  frame_id_t id = it->second;
  return &pages_[id];
}

// TODO: impl
auto BufferPoolManager::UnpinPage(page_id_t page_id, bool is_dirty, [[maybe_unused]] AccessType access_type) -> bool {
  std::cout << "UnpinPage is called" << std::endl;
  const std::lock_guard<std::mutex> lock(latch_);

  // return false;
  auto it = page_table_.find(page_id);
  if (it == page_table_.end()) {
    // not found
    return false;
  }
  frame_id_t id = it->second;
  Page *page = &pages_[id];
  if (page->pin_count_ <= 0) return false;
  page->pin_count_--;
  return true;
}

// TODO: impl
auto BufferPoolManager::FlushPage(page_id_t page_id) -> bool {
  std::cout << "FlushPage is called" << std::endl;
  const std::lock_guard<std::mutex> lock(latch_);

  if (page_id == INVALID_PAGE_ID) return false;
  auto it = page_table_.find(page_id);
  if (it == page_table_.end()) {
    // not found
    return false;
  }
  frame_id_t frame_id = it->second;
  Page *page = &pages_[frame_id];
  if (page->pin_count_ > 0) return false;

  // Now, page can be flushed.
  page_table_.erase(page_id);

  // destruct page, re-construct new blank page.
  pages_[frame_id].~Page();
  new (&pages_[frame_id]) Page();

  free_list_.push_back(frame_id);
  return true;
}

// TODO: impl
void BufferPoolManager::FlushAllPages() {}

// TODO: impl
auto BufferPoolManager::DeletePage(page_id_t page_id) -> bool { return false; }

auto BufferPoolManager::AllocatePage() -> page_id_t { return next_page_id_++; }

auto BufferPoolManager::FetchPageBasic(page_id_t page_id) -> BasicPageGuard { return {this, nullptr}; }

auto BufferPoolManager::FetchPageRead(page_id_t page_id) -> ReadPageGuard { return {this, nullptr}; }

auto BufferPoolManager::FetchPageWrite(page_id_t page_id) -> WritePageGuard { return {this, nullptr}; }

auto BufferPoolManager::NewPageGuarded(page_id_t *page_id) -> BasicPageGuard { return {this, nullptr}; }

}  // namespace bustub
