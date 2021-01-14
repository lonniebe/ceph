// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#pragma once
#include <boost/intrusive_ptr.hpp>
#include <boost/smart_ptr/intrusive_ref_counter.hpp>
#include <seastar/core/future.hh>

#include "include/ceph_assert.h"
#include "crimson/osd/exceptions.h"

#include "crimson/os/seastore/omap_manager.h"
#include "crimson/os/seastore/omap_manager/btree/omap_btree_node.h"
#include "crimson/os/seastore/seastore_types.h"
#include "crimson/os/seastore/transaction_manager.h"

namespace crimson::os::seastore::omap_manager {
/**
 * BtreeOMapManager
 *
 * Uses a btree to track :
 * string -> string mapping for each onode omap
 */

class BtreeOMapManager : public OMapManager {
  TransactionManager &tm;

  omap_context_t get_omap_context(Transaction &t) {
    return omap_context_t{tm, t};
  }

  /* get_omap_root
   *
   * load omap tree root node
   */
  using get_root_ertr = base_ertr;
  using get_root_ret = get_root_ertr::future<OMapNodeRef>;
  get_root_ret get_omap_root(const omap_root_t &omap_root, Transaction &t);

  /* handle_root_split
   *
   * root has been splitted and need update omap_root_t
   */
  using handle_root_split_ertr = base_ertr;
  using handle_root_split_ret = handle_root_split_ertr::future<bool>;
  handle_root_split_ret handle_root_split(omap_root_t &omap_root, omap_context_t oc,
                                          OMapNode:: mutation_result_t mresult);

  /* handle_root_merge
   *
   * root node has only one item and it is not leaf node, need remove a layer
   */
  using handle_root_merge_ertr = base_ertr;
  using handle_root_merge_ret = handle_root_merge_ertr::future<bool>;
  handle_root_merge_ret handle_root_merge(omap_root_t &omap_root, omap_context_t oc,
                                          OMapNode:: mutation_result_t mresult);

public:
  explicit BtreeOMapManager(TransactionManager &tm);

  initialize_omap_ret initialize_omap(Transaction &t) final;

  omap_get_value_ret omap_get_value(const omap_root_t &omap_root, Transaction &t,
                                    const std::string &key) final;

  omap_set_key_ret omap_set_key(omap_root_t &omap_root, Transaction &t,
                                const std::string &key, const std::string &value) final;

  omap_rm_key_ret omap_rm_key(omap_root_t &omap_root, Transaction &t,
                              const std::string &key) final;

  omap_list_keys_ret omap_list_keys(const omap_root_t &omap_root, Transaction &t,
                                    std::string &start,
                                    size_t max_result_size = MAX_SIZE) final;

  omap_list_ret omap_list(const omap_root_t &omap_root, Transaction &t,
                          std::string &start,
                          size_t max_result_size = MAX_SIZE) final;

  omap_clear_ret omap_clear(omap_root_t &omap_root, Transaction &t) final;

};
using BtreeOMapManagerRef = std::unique_ptr<BtreeOMapManager>;

}
