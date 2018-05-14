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

 private:
  std::vector<txn_id_t> txns_;
  std::vector<uint64_t> counts_;
  std::vector<double> durations_;
  std::vector<bool> commits_;
};

class TransactionMetric : public AbstractMetric<TransactionMetricRawData> {
 public:


 private:
  void WriteEntry(txn_id_t tid, bool committed) {
    timers_[tid].Stop();
    auto raw_data = GetRawData();
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