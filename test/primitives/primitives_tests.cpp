#include <gtest/gtest.h>
#include <block.h>

TEST(Block, Serialization)
{
  std::vector<std::unique_ptr<TransactionInput>> transaction_inputs;
  std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs;
  std::unique_ptr<Transaction> transaction = std::make_unique<Transaction>(
      std::move(transaction_inputs), std::move(transaction_outputs), 9);
  std::vector<std::unique_ptr<Transaction>> transactions;
  transactions.push_back(std::move(transaction));
  std::unique_ptr<BlockHeader> block_header = std::make_unique<BlockHeader>(
      1, 2, 3, 4, 5, 6);
  Block block = Block(std::move(block_header),
                      std::move(transactions));
  std::string serialized_block = Block::serialize(block);
  std::unique_ptr<Block> deserialized_block = Block::deserialize(serialized_block);
  EXPECT_EQ(block.block_header->previous_block_hash, deserialized_block->block_header->previous_block_hash);
  EXPECT_EQ(deserialized_block->block_header->previous_block_hash, 2);
}

TEST(BlockHeader, Serialization)
{
  std::vector<std::unique_ptr<TransactionInput>> transaction_inputs;
  std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs;
  Transaction transaction = Transaction(
      std::move(transaction_inputs), std::move(transaction_outputs), 20);
  std::string serialized_transaction = Transaction::serialize(transaction);
  std::unique_ptr<Transaction> deserialized_transaction = Transaction::deserialize(serialized_transaction);
  EXPECT_EQ(transaction.version, deserialized_transaction->version);
  EXPECT_EQ(deserialized_transaction->version, 20);
}

TEST(Transaction, Serialization)
{
  BlockHeader block_header = BlockHeader(1, 3, 3, 4, 5, 6);
  std::string serialized_block_header = BlockHeader::serialize(block_header);
  std::unique_ptr<BlockHeader> deserialized_block_header = BlockHeader::deserialize(serialized_block_header);
  EXPECT_EQ(block_header.previous_block_hash, deserialized_block_header->previous_block_hash);
  EXPECT_EQ(deserialized_block_header->previous_block_hash, 3);
}

TEST(TransactionInput, Serialization)
{
  TransactionInput transaction_input = TransactionInput(9, 2, 3);
  std::string serialized_transaction_input = TransactionInput::serialize(transaction_input);
  std::unique_ptr<TransactionInput> deserialized_transaction_input = TransactionInput::deserialize(serialized_transaction_input);
  EXPECT_EQ(transaction_input.reference_transaction_hash, deserialized_transaction_input->reference_transaction_hash);
  EXPECT_EQ(deserialized_transaction_input->reference_transaction_hash, 9);
}

TEST(TransactionOutput, Serialization)
{
  TransactionOutput transaction_output = TransactionOutput(300, 2);
  std::string serialized_transaction_output = TransactionOutput::serialize(transaction_output);
  std::unique_ptr<TransactionOutput> deserialized_transaction_output = TransactionOutput::deserialize(serialized_transaction_output);
  EXPECT_EQ(transaction_output.amount, deserialized_transaction_output->amount);
  EXPECT_EQ(deserialized_transaction_output->amount, 300);
}
