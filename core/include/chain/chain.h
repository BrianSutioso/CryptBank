#ifndef CRYPTBANK_CHAIN_H
#define CRYPTBANK_CHAIN_H

#include "../primitives/block.h"
#include <block_info_database.h>
#include <chain_writer.h>
#include <coin_database.h>

class Chain
{
private:
  uint32_t _active_chain_length;
  std::unique_ptr<Block> _active_chain_last_block;
  std::unique_ptr<BlockInfoDatabase> _block_info_database;
  std::unique_ptr<ChainWriter> _chain_writer;
  std::unique_ptr<CoinDatabase> _coin_database;
  static std::unique_ptr<Block> construct_genesis_block();
  std::vector<std::shared_ptr<Block>> get_forked_blocks_stack(uint32_t starting_hash);
  std::vector<std::unique_ptr<UndoBlock>> get_undo_blocks_queue(uint32_t branching_height);

public:
  Chain();

  void handle_block(std::unique_ptr<Block> block);
  void handle_transaction(std::unique_ptr<Transaction> transaction);
  uint32_t get_chain_length(uint32_t block_hash);
  std::unique_ptr<Block> get_block(uint32_t block_hash);
  std::vector<std::unique_ptr<Block>> get_active_chain(uint32_t start, uint32_t end);
  std::vector<uint32_t> get_active_chain_hashes(uint32_t start, uint32_t end);
  std::unique_ptr<Block> get_last_block();
  uint32_t get_last_block_hash();
  uint32_t get_active_chain_length() const;
  std::vector<std::pair<uint32_t, uint8_t>> get_all_utxo(uint32_t public_key);
  Chain(Chain &&other) = delete;

  Chain &operator=(const Chain &other) = delete;
};

#endif
