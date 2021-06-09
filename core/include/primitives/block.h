#ifndef CRYPT_BLOCK_H
#define CRYPT_BLOCK_H

#include <transaction.h>
#include <block_header.h>
#include <vector>
#include <crypt.pb.h>
#include <string>

class Block
{
public:
  std::unique_ptr<BlockHeader> block_header;
  std::vector<std::unique_ptr<Transaction>> transactions;

  std::vector<std::unique_ptr<Transaction>> get_transactions();

  void set_block_header(std::unique_ptr<BlockHeader> block_header_);
  void set_transactions(std::vector<std::unique_ptr<Transaction>> transactions_);

  Block(std::unique_ptr<BlockHeader> block_header_, std::vector<std::unique_ptr<Transaction>> transactions_);

  static std::string serialize(const Block &block);
  static std::unique_ptr<Block> deserialize(const std::string &serialized_block);
};

#endif
