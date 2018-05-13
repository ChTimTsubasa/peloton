//===----------------------------------------------------------------------===//
//
//                         Peloton
//
// transaction_metrics_catalog.cpp
//
// Identification: src/catalog/transaction_metrics_catalog.cpp
//
// Copyright (c) 2015-18, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "catalog/transaction_metrics_catalog.h"

#include <vector>
#include "executor/logical_tile.h"
#include "storage/data_table.h"
#include "type/value_factory.h"

namespace peloton {
namespace catalog {

TransactionMetricsCatalog::TransactionMetricsCatalog(concurrency::TransactionContext *txn)
    : AbstractCatalog("CREATE TABLE "  CATALOG_DATABASE_NAME
                      "." CATALOG_SCHEMA_NAME "." TXN_METRICS_CATALOG_NAME
                      " ("
                      "txn_id      BIGINT NOT NULL PRIMARY KEY, "
                      "reads       BIGINT NOT NULL, "
                      "duration    DECIMAL NOT NULL, "
                      "committed   BOOL NOT NULL);",
                      txn) {}

bool TransactionMetricsCatalog::InsertTxnMetric(peloton::txn_id_t tid,
                                                uint64_t reads,
                                                double duration,
                                                bool committed,
                                                peloton::type::AbstractPool *pool,
                                                peloton::concurrency::TransactionContext *txn){
  std::unique_ptr<storage::Tuple>
      tuple(new storage::Tuple(catalog_table_->GetSchema(), true));

  tuple->SetValue(ColumnId::TXN_ID,
                  type::ValueFactory::GetBigIntValue(tid),
                  pool);
  tuple->SetValue(ColumnId::READS,
                  type::ValueFactory::GetBigIntValue(reads),
                  pool);
  tuple->SetValue(ColumnId::DURATION,
                  type::ValueFactory::GetDecimalValue(duration),
                  pool);
  tuple->SetValue(ColumnId::COMMITTED,
                  type::ValueFactory::GetBooleanValue(committed),
                  pool);
  return InsertTuple(std::move(tuple), txn);
}

}  // namespace catalog
}  // namespace peloton
