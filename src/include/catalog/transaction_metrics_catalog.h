//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// transaction_metrics_catalog.h
//
// Identification: src/include/catalog/transaction_metrics_catalog.h
//
// Copyright (c) 2015-18, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include "catalog/abstract_catalog.h"
#include "concurrency/transaction_context.h"

#define TXN_METRICS_CATALOG_NAME "txn_metrics_catalog"

namespace peloton {
namespace catalog {

class TransactionMetricsCatalog : public AbstractCatalog {
 public:

  static TransactionMetricsCatalog *GetInstance(concurrency::TransactionContext *txn) {
    static TransactionMetricsCatalog catalog{txn};
    return &catalog;
  }

  TransactionMetricsCatalog(concurrency::TransactionContext *txn);

  ~TransactionMetricsCatalog() = default;

  inline std::string GetName() const override {
    return TXN_METRICS_CATALOG_NAME;
  }

  //===--------------------------------------------------------------------===//
  // Write Related API
  //===--------------------------------------------------------------------===//
  bool InsertTxnMetric(txn_id_t tid,
                       uint64_t reads,
                       double duration,
                       bool committed,
                       type::AbstractPool *pool,
                       concurrency::TransactionContext *txn);
 private:
  enum ColumnId {
    TXN_ID = 0,
    READS = 1,
    DURATION = 2,
    COMMITTED = 3
  };
  std::vector<oid_t> all_column_ids_ = {0, 1, 2, 3};

  enum IndexId {
    PRIMARY_KEY = 0,
    // under new hierarchical catalog design, each database has its own table
    // catalogs, so table_oid is a valid primary key
  };
};

}  // namespace catalog
}  // namespace peloton
