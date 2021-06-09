#include <transaction_input.h>
#include <crypt.pb.h>

TransactionInput::TransactionInput(
    uint32_t reference_transaction_hash, uint8_t utxo_index,
    uint32_t signature) : reference_transaction_hash(reference_transaction_hash),
                          utxo_index(utxo_index), signature(signature) {}

std::string TransactionInput::serialize(const TransactionInput &transaction_input)
{
  PTransactionInput protobuf_transaction_input = PTransactionInput();
  protobuf_transaction_input.set_reference_transaction_hash(transaction_input.reference_transaction_hash);
  protobuf_transaction_input.set_signature(transaction_input.signature);
  protobuf_transaction_input.set_utxo_index(transaction_input.utxo_index);
  std::string serialized_transaction_input = protobuf_transaction_input.SerializeAsString();
  return serialized_transaction_input;
}

std::unique_ptr<TransactionInput> TransactionInput::deserialize(const std::string &serialized_transaction_input)
{
  PTransactionInput protobuf_transaction_input = PTransactionInput();
  protobuf_transaction_input.ParseFromString(serialized_transaction_input);
  std::unique_ptr<TransactionInput> transaction_input = std::make_unique<TransactionInput>(protobuf_transaction_input.reference_transaction_hash(),
                                                                                           protobuf_transaction_input.utxo_index(),
                                                                                           protobuf_transaction_input.signature());
  return std::move(transaction_input);
}
