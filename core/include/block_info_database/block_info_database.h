#ifndef CRYPTBANK_BLOCK_INFO_DATABASE_H
#define CRYPTBANK_BLOCK_INFO_DATABASE_H

#include <../db/db.h>
#include <block_record.h>

class BlockInfoDatabase
{
private:
  std::unique_ptr<Database> _database;

public:
  BlockInfoDatabase();

  void store_block_record(uint32_t hash, const BlockRecord &record);
  std::unique_ptr<BlockRecord> get_block_record(uint32_t block_hash);

  BlockInfoDatabase(BlockInfoDatabase &&other) = delete;
  BlockInfoDatabase &operator=(const BlockInfoDatabase &other) = delete;

  BlockInfoDatabase(const BlockInfoDatabase &other) = delete;
  BlockInfoDatabase &operator=(BlockInfoDatabase &&other) = delete;
};

#endif
