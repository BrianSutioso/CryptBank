#ifndef CRYPT_TRANSACTION_H
#define CRYPT_TRANSACTION_H

#include <transaction_input.h>
#include <transaction_output.h>
#include <vector>

class Transaction
{
public:
  uint8_t version;
  std::vector<std::unique_ptr<TransactionInput>> transaction_inputs;
  std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs;
  uint32_t lock_time;

  Transaction(std::vector<std::unique_ptr<TransactionInput>> transaction_inputs_,
              std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs_,
              uint8_t version_ = 0, uint32_t lock_time_ = 0);

  static std::string serialize(const Transaction &transaction);
  static std::unique_ptr<Transaction> deserialize(const std::string &serialized_transaction);
};

#endif
