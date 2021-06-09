#include <block.h>

Block::Block(
    std::unique_ptr<BlockHeader> block_header_,
    std::vector<std::unique_ptr<Transaction>> transactions_)
    : block_header(std::move(block_header_)),
      transactions(std::move(transactions_)) {}

void Block::set_block_header(std::unique_ptr<BlockHeader> block_header_)
{
  block_header = std::move(block_header_);
}

void Block::set_transactions(std::vector<std::unique_ptr<Transaction>> transactions_)
{
  transactions = std::move(transactions_);
}

std::vector<std::unique_ptr<Transaction>> Block::get_transactions()
{
  std::vector<std::unique_ptr<Transaction>> result;
  std::vector<std::unique_ptr<TransactionInput>> transactionInputs;
  std::vector<std::unique_ptr<TransactionOutput>> transactionOutputs;
  for (auto &transaction : transactions)
  {
    for (auto &transactionInput : transaction->transaction_inputs)
    {
      transactionInputs.push_back(std::make_unique<TransactionInput>(transactionInput->reference_transaction_hash,
                                                                     transactionInput->utxo_index,
                                                                     transactionInput->signature));
    }
    for (auto &transactionOutput : transaction->transaction_outputs)
    {
      transactionOutputs.push_back(std::make_unique<TransactionOutput>(transactionOutput->amount,
                                                                       transactionOutput->public_key));
    }
    result.push_back(std::make_unique<Transaction>(std::move(transactionInputs), std::move(transactionOutputs)));
  }

  return result;
}

std::string Block::serialize(const Block &block)
{
  PBlock pblock = PBlock();
  // Block Header
  PBlockHeader *pblock_header = new PBlockHeader();
  pblock_header->set_version(block.block_header->version);
  pblock_header->set_previous_block_hash(block.block_header->previous_block_hash);
  pblock_header->set_merkle_root(block.block_header->merkle_root);
  pblock_header->set_difficulty_target(block.block_header->difficulty_target);
  pblock_header->set_nonce(block.block_header->nonce);
  pblock_header->set_timestamp(block.block_header->timestamp);
  pblock.set_allocated_block_header(pblock_header);

  // Add all Transactions
  for (int i = 0; i < block.transactions.size(); i++)
  {
    PTransaction *ptransaction = pblock.add_transactions();
    ptransaction->set_version(block.transactions[i]->version);
    for (int j = 0; j < block.transactions[i]->transaction_inputs.size(); j++)
    {
      PTransactionInput *ptxi = ptransaction->add_transaction_inputs();
      ptxi->set_reference_transaction_hash(block.transactions[i]->transaction_inputs[j]->reference_transaction_hash);
      ptxi->set_signature(block.transactions[i]->transaction_inputs[j]->signature);
      ptxi->set_utxo_index(block.transactions[i]->transaction_inputs[j]->utxo_index);
    }
    for (int j = 0; j < block.transactions[i]->transaction_outputs.size(); j++)
    {
      PTransactionOutput *ptxo = ptransaction->add_transaction_outputs();
      ptxo->set_amount(block.transactions[i]->transaction_outputs[j]->amount);
      ptxo->set_public_key(block.transactions[i]->transaction_outputs[j]->public_key);
    }
    ptransaction->set_lock_time(block.transactions[i]->lock_time);
  }
  // Serialize
  std::string serialized_block;
  pblock.SerializeToString(&serialized_block);
  return serialized_block;
}

std::unique_ptr<Block> Block::deserialize(const std::string &serialized_block)
{
  if (serialized_block == "")
  {
    return nullptr;
  }

  PBlock pblock = PBlock();
  pblock.ParseFromString(serialized_block);
  // BlockHeader
  std::unique_ptr<BlockHeader> block_header_ = std::make_unique<BlockHeader>(pblock.block_header().version(),
                                                                             pblock.block_header().previous_block_hash(),
                                                                             pblock.block_header().merkle_root(),
                                                                             pblock.block_header().difficulty_target(),
                                                                             pblock.block_header().nonce(),
                                                                             pblock.block_header().timestamp());

  // Transactions
  std::vector<std::unique_ptr<Transaction>> transactions_;
  for (int i = 0; i < pblock.transactions_size(); i++)
  {
    std::vector<std::unique_ptr<TransactionInput>> txis;
    for (int j = 0; j < pblock.transactions(i).transaction_inputs_size(); j++)
    {
      PTransactionInput ptxi = pblock.transactions(i).transaction_inputs(j);
      std::unique_ptr<TransactionInput> txi = std::make_unique<TransactionInput>(ptxi.reference_transaction_hash(),
                                                                                 ptxi.utxo_index(),
                                                                                 ptxi.signature());
      txis.push_back(std::move(txi));
    }
    std::vector<std::unique_ptr<TransactionOutput>> txos;
    for (int j = 0; j < pblock.transactions(i).transaction_outputs_size(); j++)
    {
      PTransactionOutput ptxo = pblock.transactions(i).transaction_outputs(j);
      std::unique_ptr<TransactionOutput> txo = std::make_unique<TransactionOutput>(ptxo.amount(), ptxo.public_key());
      txos.push_back(std::move(txo));
    }
    std::unique_ptr<Transaction> tx = std::make_unique<Transaction>(std::move(txis), std::move(txos), pblock.transactions(i).version(),
                                                                    pblock.transactions(i).lock_time());
    transactions_.push_back(std::move(tx));
  }

  std::unique_ptr<Block> block = std::unique_ptr<Block>(new Block(std::move(block_header_), std::move(transactions_)));
  return std::move(block);
}
