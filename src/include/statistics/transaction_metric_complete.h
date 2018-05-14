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
    auto &other_data = dynamic_cast<TransactionMetricRawData &>(other);
    Concat(txns_, other_data.txns_);
    Concat(counts_, other_data.counts_);
    Concat(durations_, other_data.durations_);
    Concat(commits_, other_data.commits_);
  }

  void UpdateAndPersist() override {
    auto txn =
        concurrency::TransactionManagerFactory::GetInstance().BeginTransaction();
    auto catalog = catalog::TransactionMetricsCatalog::GetInstance(txn);
    for (size_t i = 0; i < txns_.size(); i++)
      catalog->InsertTxnMetric(txns_[i],
                               counts_[i],
                               durations_[i],
                               commits_[i],
                               nullptr,
                               txn);
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
    timers_[context->GetTransactionId()].Start();
  }

  void OnTransactionCommit(const concurrency::TransactionContext *context, oid_t) override {
    WriteEntry(context->GetTransactionId(), true);
  }

  void OnTransactionAbort(const concurrency::TransactionContext *context, oid_t) override {
    WriteEntry(context->GetTransactionId(), false);
  }

  void OnTupleRead(const concurrency::TransactionContext *context, oid_t) override {
    counts_[context->GetTransactionId()]++;
  }

 private:
  void WriteEntry(txn_id_t tid, bool committed) {
    auto raw_data = GetRawData();
    timers_[tid].Stop();
    raw_data->txns_.push_back(tid);
    raw_data->counts_.push_back(counts_[tid]);
    raw_data->durations_.push_back(timers_[tid].GetDuration());
    raw_data->commits_.push_back(committed);
    counts_.erase(tid);
    timers_.erase(tid);
  }

  std::unordered_map<txn_id_t, uint64_t> counts_;
  std::unordered_map<txn_id_t, Timer<std::ratio<1, 1000>>> timers_;
};
} // namespace stats
} // namespace peloton