#ifndef CRYPT_TRANSACTION_INPUT_H
#define CRYPT_TRANSACTION_INPUT_H

#include <string>

class TransactionInput
{
public:
  uint32_t reference_transaction_hash;
  uint8_t utxo_index;
  uint32_t signature;

  TransactionInput(
      uint32_t reference_transaction_hash_, uint8_t utxo_index_,
      uint32_t signature_);
  static std::string serialize(const TransactionInput &transaction_input);
  static std::unique_ptr<TransactionInput> deserialize(const std::string &serialized_transaction_input);
};

#endif
