#ifndef CRYPT_TRANSACTION_OUTPUT_H
#define CRYPT_TRANSACTION_OUTPUT_H

#include <string>

class TransactionOutput
{
public:
  uint32_t amount;
  uint32_t public_key;

  TransactionOutput(uint32_t amount_, uint32_t public_key_);
  static std::string serialize(const TransactionOutput &transaction_input);
  static std::unique_ptr<TransactionOutput> deserialize(const std::string &serialized_transaction_input);
};

#endif
