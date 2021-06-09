#include <gtest/gtest.h>
#include <chain.h>
#include <cryptcrypto.h>
#include <filesystem>
#include <memory>

std::unique_ptr<Block> make_blockd(std::unique_ptr<Block> block, int tx_index, int out_index)
{
  std::vector<std::unique_ptr<Transaction>> transactions;
  std::vector<std::unique_ptr<TransactionInput>> transaction_inputs;
  transaction_inputs.push_back(std::make_unique<TransactionInput>(
      CryptCrypto::hash(Transaction::serialize(*block->transactions.at(tx_index))),
      out_index,
      99));
  std::vector<std::unique_ptr<TransactionOutput>> transaction_outputs;
  transaction_outputs.push_back(std::make_unique<TransactionOutput>(
      block->transactions.at(tx_index)->transaction_outputs.at(out_index)->amount,
      block->transactions.at(tx_index)->transaction_outputs.at(out_index)->public_key));
  transactions.push_back(std::make_unique<Transaction>(std::move(transaction_inputs), std::move(transaction_outputs)));
  std::unique_ptr<BlockHeader> block_header = std::make_unique<BlockHeader>(1, CryptCrypto::hash(Block::serialize(*block)), 3, 4, 5, 6);
  std::unique_ptr<Block> b = std::make_unique<Block>(std::move(block_header), std::move(transactions));
  return std::move(b);
}

TEST(Chain, GetActiveHashes)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  auto hashes = chain.get_active_chain_hashes(1, 1);
  EXPECT_EQ(hashes.size(), 1);
}

TEST(Chain, GetActiveChainLength)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  auto result = chain.get_active_chain(1, 1);
  EXPECT_EQ(result.size(), 1);
}

TEST(Chain, HandleTransaction)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  auto block = make_blockd(chain.get_last_block(), 0, 0);
  auto blockHash = CryptCrypto::hash(Block::serialize(*block));
  auto hash = CryptCrypto::hash(Transaction::serialize(*block->transactions[0]));
  chain.handle_block(std::move(block));
  // Fetch previously added block
  std::unique_ptr<Block> newBlock = chain.get_last_block();
  auto newBlockHash = CryptCrypto::hash(Block::serialize(*newBlock));
  auto hash2 = CryptCrypto::hash(Transaction::serialize(*newBlock->transactions[0]));

  EXPECT_EQ(blockHash, newBlockHash);
  EXPECT_EQ(hash, hash2);
}

TEST(Chain, HandleForkNoTakeOverOccurence)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  auto genesisBlock = chain.get_last_block();
  auto block1 = make_blockd(chain.get_last_block(), 0, 0);
  auto block1Hash = CryptCrypto::hash(Block::serialize(*block1));
  chain.handle_block(std::move(block1));
  auto forkedBlock1 = make_blockd(std::move(genesisBlock), 0, 0);
  auto forkedBlock1Copy = Block::deserialize(Block::serialize(*forkedBlock1));
  auto forkedBlock1Hash = CryptCrypto::hash(Block::serialize(*forkedBlock1));
  chain.handle_block(std::move(forkedBlock1));

  EXPECT_EQ(chain.get_last_block_hash(), block1Hash);
  EXPECT_EQ(chain.get_active_chain_length(), 2);
}

TEST(Chain, GetGenesisBlockHash0)
{
  // test setup - remove past data
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  std::unique_ptr<Block> genesis_block = chain.get_last_block();
  // check that genesis block has at least 1 transaction
  EXPECT_TRUE(genesis_block->transactions.size() >= 1);
  const std::vector<std::unique_ptr<TransactionOutput>> &transaction_outputs = genesis_block->transactions.at(0)->transaction_outputs;
  // check that the first transaction has at least 3 outputs
  EXPECT_TRUE(transaction_outputs.size() >= 3);
  EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
  EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
  EXPECT_EQ(transaction_outputs.at(2)->amount, 300);

  uint32_t hash0 = CryptCrypto::hash(Block::serialize(*genesis_block));
  EXPECT_EQ(hash0, chain.get_last_block_hash());

  std::filesystem::remove_all(ChainWriter::get_data_directory());
}

TEST(Chain, HandleValidBlock23)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  // 1 = add valid to genesis
  std::unique_ptr<Block> genesis_block = chain.get_last_block();
  // check that genesis block has at least 1 transaction
  EXPECT_TRUE(genesis_block->transactions.size() >= 1);
  const std::vector<std::unique_ptr<TransactionOutput>> &transaction_outputs = genesis_block->transactions.at(0)->transaction_outputs;
  // check that the first transaction has at least 3 outputs
  EXPECT_TRUE(transaction_outputs.size() >= 3);
  EXPECT_EQ(transaction_outputs.at(0)->amount, 100);
  EXPECT_EQ(transaction_outputs.at(1)->amount, 200);
  EXPECT_EQ(transaction_outputs.at(2)->amount, 300);
  std::unique_ptr<Block> block = make_blockd(std::move(genesis_block), 0, 0);
  uint32_t block_hash = CryptCrypto::hash(Block::serialize(*block));
  chain.handle_block(std::move(block));

  EXPECT_EQ(block_hash, chain.get_last_block_hash());
  std::unique_ptr<Block> ret_block = chain.get_last_block();
  EXPECT_EQ(ret_block->transactions.size(), 1);
  EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.size(), 1);
  EXPECT_EQ(ret_block->transactions.at(0)->transaction_outputs.at(0)->amount, 100);
}

TEST(Chain, HandleForkTakeOverOccurence)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  auto genesisBlock = chain.get_last_block();
  auto block1 = make_blockd(chain.get_last_block(), 0, 0);
  auto block1Hash = CryptCrypto::hash(Block::serialize(*block1));
  chain.handle_block(std::move(block1));
  auto forkedBlock1 = make_blockd(std::move(genesisBlock), 0, 0);
  auto forkedBlock1Copy = Block::deserialize(Block::serialize(*forkedBlock1));
  auto forkedBlock1Hash = CryptCrypto::hash(Block::serialize(*forkedBlock1));
  chain.handle_block(std::move(forkedBlock1));
  auto forkedBlock2 = make_blockd(std::move(forkedBlock1Copy), 0, 0);
  auto forkedBlock2Hash = CryptCrypto::hash(Block::serialize(*forkedBlock2));
  chain.handle_block(std::move(forkedBlock2));

  EXPECT_EQ(chain.get_last_block_hash(), forkedBlock2Hash);
  EXPECT_EQ(chain.get_active_chain_length(), 3);
}

TEST(Chain, GetActiveChain)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  auto block1 = make_blockd(chain.get_last_block(), 0, 0);
  auto block1Copy = Block::deserialize(Block::serialize(*block1));
  auto block1Hash = CryptCrypto::hash(Block::serialize(*block1));
  chain.handle_block(std::move(block1));
  auto block2 = make_blockd(std::move(block1Copy), 0, 0);
  auto block2Copy = Block::deserialize(Block::serialize(*block2));
  auto block2Hash = CryptCrypto::hash(Block::serialize(*block2));
  chain.handle_block(std::move(block2));

  auto activeChain = chain.get_active_chain(1, 3);
  EXPECT_EQ(block1Hash, CryptCrypto::hash(Block::serialize(*activeChain[1])));
  EXPECT_EQ(block2Hash, CryptCrypto::hash(Block::serialize(*activeChain[2])));
}

TEST(Chain, MultipleHandleBlocks)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  Chain chain = Chain();

  auto genesisBlock = chain.get_last_block();

  for (int i = 0; i <= 40; i++)
  {
    std::unique_ptr<Block> prevBlock = chain.get_last_block();
    std::unique_ptr<Block> addedBlock = make_blockd(std::move(prevBlock), 0, 0);
    chain.handle_block(std::move(addedBlock));
  }

  EXPECT_EQ(chain.get_active_chain_length(), 42);
}