//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// transaction_metric.cpp
//
// Identification: src/catalog/transaction_metric.cpp
//
// Copyright (c) 2015-18, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//
#include <vector>
#include <unordered_map>
#include "common/timer.h"
#include "statistics/abstract_raw_data.h"
#include "statistics/abstract_metric.h"
#include "concurrency/transaction_context.h"
#include "concurrency/transaction_manager_factory.h"
#include "concurrency/transaction_manager.h"
#include "catalog/transaction_metrics_catalog.h"

namespace peloton {
namespace stats {
class TransactionMetricRawData : public AbstractRawData {
  friend class TransactionMetric;
 public:
  const std::string GetInfo() const override {
    // TODO(Tianyu): Pretty print
    return "placeholder";
  }

  void Aggregate(AbstractRawData &other) override {
  }

  void UpdateAndPersist() override {
  }

 private:
  template<typename T>
  static inline void Concat(std::vector<T> &a, std::vector<T> &b) {
    for (size_t i = 0; i < b.size(); i++)
      a.push_back(b[i]);
  }
  std::vector<txn_id_t> txns_;
  std::vector<uint64_t> counts_;
  std::vector<double> durations_;
  std::vector<bool> commits_;
};

class TransactionMetric : public AbstractMetric<TransactionMetricRawData> {
 public:
  void OnTransactionBegin(const concurrency::TransactionContext *context) override {
  }

  void OnTransactionCommit(const concurrency::TransactionContext *context, oid_t) override {
  }

  void OnTransactionAbort(const concurrency::TransactionContext *context, oid_t) override {
  }

  void OnTupleRead(const concurrency::TransactionContext *context, oid_t) override {
  }

 private:
  void WriteEntry(txn_id_t tid, bool committed) {
  }

  std::unordered_map<txn_id_t, uint64_t> counts_;
  std::unordered_map<txn_id_t, Timer<std::ratio<1, 1000>>> timers_;
};
} // namespace stats
} // namespace peloton