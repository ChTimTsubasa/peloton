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
class TransactionRawData : public AbstractRawData {
  friend class TransactionMetric;
 public:
  void Aggregate(AbstractRawData &other) override {
    TransactionRawData
        &other_txn_data = dynamic_cast<TransactionRawData &>(other);
    for (int i = 0; i < other_txn_data.txns_.size(); ++i) {
      txns_.push_back(other_txn_data.txns_[i]);
      reads_.push_back(other_txn_data.reads_[i]);
      durations_.push_back(other_txn_data.durations_[i]);
      commits_.push_back(other_txn_data.commits_[i]);
    }
  }

  void UpdateAndPersist() override {
    auto txn =
        concurrency::TransactionManagerFactory::GetInstance().BeginTransaction();
    auto *catalog = catalog::TransactionMetricsCatalog::GetInstance(txn);
    for (int i = 0; i < txns_.size(); ++i)
      catalog->InsertTxnMetric(txns_[i],
                               reads_[i],
                               durations_[i],
                               commits_[i],
                               nullptr,
                               txn);
  }

  const std::string GetInfo() const override {
    return "";
  }

 private:
  std::vector<txn_id_t> txns_;
  std::vector<uint64_t> reads_;
  std::vector<double> durations_;
  std::vector<bool> commits_;

};
class TransactionMetric : public AbstractMetric<TransactionRawData> {
 public:
  void OnTransactionBegin(const concurrency::TransactionContext *context) override {
    auto tid = context->GetTransactionId();
    timers_[tid].Reset();
    timers_[tid].Start();
  }
  void OnTransactionCommit(const concurrency::TransactionContext *context,
                           oid_t oid) override {
    WriteToRawData(context->GetTransactionId(), true);
  }

  void OnTransactionAbort(const concurrency::TransactionContext *context,
                          oid_t oid) override {
    WriteToRawData(context->GetTransactionId(), false);
  }

  void OnTupleRead(const concurrency::TransactionContext *context,
                   oid_t oid) override {
    auto tid = context->GetTransactionId();
    counts_[tid]++;
  }

 private:
  void WriteToRawData(txn_id_t tid, bool commit) {
    timers_[tid].Stop();
    auto raw_data = GetRawData();
    raw_data->txns_.push_back(tid);
    raw_data->reads_.push_back(counts_[tid]);
    raw_data->durations_.push_back(timers_[tid].GetDuration());
    raw_data->commits_.push_back(true);
    counts_.erase(tid);
    timers_.erase(tid);
  }

  std::unordered_map<txn_id_t, uint64_t> counts_;
  std::unordered_map<txn_id_t, Timer<std::ratio<1, 1000>>> timers_;
};
} // namespace stats
} // namespace peloton