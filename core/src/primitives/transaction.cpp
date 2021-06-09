#include <transaction.h>
#include <crypt.pb.h>

Transaction::Transaction(
    std::vector<std::unique_ptr<TransactionInput>> transaction_inputs,
    std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs, uint8_t version,
    uint32_t lock_time) : transaction_inputs(std::move(transaction_inputs)),
                          transaction_outputs(std::move(transaction_outputs)), version(version),
                          lock_time(lock_time) {}

std::string Transaction::serialize(const Transaction &transaction)
{
  PTransaction ptransaction = PTransaction();
  ptransaction.set_version(transaction.version);
  for (int j = 0; j < transaction.transaction_inputs.size(); j++)
  {
    PTransactionInput *ptxi = ptransaction.add_transaction_inputs();
    ptxi->set_reference_transaction_hash(transaction.transaction_inputs[j]->reference_transaction_hash);
    ptxi->set_signature(transaction.transaction_inputs[j]->signature);
    ptxi->set_utxo_index(transaction.transaction_inputs[j]->utxo_index);
  }
  for (int j = 0; j < transaction.transaction_outputs.size(); j++)
  {
    PTransactionOutput *ptxo = ptransaction.add_transaction_outputs();
    ptxo->set_amount(transaction.transaction_outputs[j]->amount);
    ptxo->set_public_key(transaction.transaction_outputs[j]->public_key);
  }
  ptransaction.set_lock_time(transaction.lock_time);
  std::string serialized_transaction;
  ptransaction.SerializeToString(&serialized_transaction);
  return serialized_transaction;
}

std::unique_ptr<Transaction> Transaction::deserialize(const std::string &serialized_transaction)
{
  PTransaction ptransaction = PTransaction();
  ptransaction.ParseFromString(serialized_transaction);
  std::vector<std::unique_ptr<TransactionInput>> txis;
  for (int j = 0; j < ptransaction.transaction_inputs_size(); j++)
  {
    PTransactionInput ptxi = ptransaction.transaction_inputs(j);
    std::unique_ptr<TransactionInput> txi = std::make_unique<TransactionInput>(ptxi.reference_transaction_hash(),
                                                                               ptxi.utxo_index(),
                                                                               ptxi.signature());
    txis.push_back(std::move(txi));
  }
  std::vector<std::unique_ptr<TransactionOutput>> txos;
  for (int j = 0; j < ptransaction.transaction_outputs_size(); j++)
  {
    PTransactionOutput ptxo = ptransaction.transaction_outputs(j);
    std::unique_ptr<TransactionOutput> txo = std::make_unique<TransactionOutput>(ptxo.amount(), ptxo.public_key());
    txos.push_back(std::move(txo));
  }
  return std::make_unique<Transaction>(std::move(txis), std::move(txos), ptransaction.version(),
                                       ptransaction.lock_time());
}