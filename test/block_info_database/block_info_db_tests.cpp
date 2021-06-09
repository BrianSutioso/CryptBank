#include <gtest/gtest.h>
#include <block_record.h>
#include <undo_block.h>
#include <block_info_database.h>

TEST(BlockRecord, Serializaiton)
{
  std::unique_ptr<BlockHeader> block_header = std::make_unique<BlockHeader>();
  FileInfo file_info = FileInfo("a", 1, 2);
  BlockRecord block_record = BlockRecord(std::move(block_header), 41, 0, file_info, file_info);
  std::string serialized_block_record = BlockRecord::serialize(block_record);
  std::unique_ptr<BlockRecord> deserialized_block_record = BlockRecord::deserialize(serialized_block_record);
  EXPECT_EQ(block_record.num_transactions, deserialized_block_record->num_transactions);
  EXPECT_EQ(deserialized_block_record->num_transactions, 41);
}

TEST(UndoBlock, Serializaiton)
{
  std::vector<uint32_t> transaction_hashes = {9999};
  std::vector<uint32_t> utxo = {99};
  std::vector<uint32_t> amounts = {12};
  std::vector<uint32_t> public_keys = {14};
  std::unique_ptr<UndoCoinRecord> undo_coin_record = std::make_unique<UndoCoinRecord>(32, std::move(utxo), std::move(amounts),
                                                                                      std::move(public_keys));
  std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;
  undo_coin_records.push_back(std::move(undo_coin_record));
  UndoBlock undo_block = UndoBlock(std::move(transaction_hashes), std::move(undo_coin_records));
  std::string serialized_undo_block = UndoBlock::serialize(undo_block);
  std::unique_ptr<UndoBlock> deserialized_undo_block = UndoBlock::deserialize(serialized_undo_block);
  EXPECT_EQ(undo_block.transaction_hashes.at(0), deserialized_undo_block->transaction_hashes.at(0));
  EXPECT_EQ(deserialized_undo_block->transaction_hashes.at(0), 9999);
}

TEST(BlockInfoDatabase, WriteAndRead)
{
  auto database = std::make_unique<BlockInfoDatabase>();
  auto record = std::make_unique<BlockRecord>(std::make_unique<BlockHeader>(1, 1, 1, 1, 1, 1), 1, 1, *std::make_unique<FileInfo>("File", 1, 1), *std::make_unique<FileInfo>("File", 1, 1));
  database->store_block_record(1, *record);
  auto fetch = database->get_block_record(1);

  EXPECT_EQ(BlockRecord::serialize(*fetch), BlockRecord::serialize(*record));
}

TEST(BlockInfoDatabase, WriteOverAndRead)
{
  auto database = std::make_unique<BlockInfoDatabase>();
  auto record = std::make_unique<BlockRecord>(std::make_unique<BlockHeader>(1, 1, 1, 1, 1, 1), 1, 1, *std::make_unique<FileInfo>("File", 1, 1), *std::make_unique<FileInfo>("File", 1, 1));
  auto record2 = std::make_unique<BlockRecord>(std::make_unique<BlockHeader>(1, 1, 7777, 1, 1, 9999), 999, 999, *std::make_unique<FileInfo>("File", 1, 1), *std::make_unique<FileInfo>("File", 1, 1));

  database->store_block_record(1, *record);
  database->store_block_record(1, *record2);
  auto fetch = database->get_block_record(1);

  EXPECT_EQ(BlockRecord::serialize(*fetch), BlockRecord::serialize(*record2));
}