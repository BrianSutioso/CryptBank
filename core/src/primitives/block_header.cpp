#include <block_header.h>

BlockHeader::BlockHeader(
    uint8_t version_, uint32_t previous_block_hash_,
    uint32_t merkle_root_, uint32_t difficulty_target_,
    uint32_t nonce_, uint32_t timestamp_) : version(version_), previous_block_hash(previous_block_hash_),
                                            merkle_root(merkle_root_), difficulty_target(difficulty_target_),
                                            nonce(nonce_), timestamp(timestamp_) {}

inline void BlockHeader::set_version(uint8_t version_)
{
  version = version_;
}

inline void BlockHeader::set_previous_block_hash(uint32_t previous_block_hash_)
{
  previous_block_hash = previous_block_hash_;
}

inline void BlockHeader::set_merkle_root(uint32_t merkle_root_)
{
  merkle_root = merkle_root_;
}

inline void BlockHeader::set_difficulty_target(uint32_t difficulty_target_)
{
  difficulty_target = difficulty_target_;
}

inline void BlockHeader::set_nonce(uint32_t nonce_)
{
  nonce = nonce_;
}

inline void BlockHeader::set_timestamp(uint32_t timestamp_)
{
  timestamp = timestamp_;
}

std::string BlockHeader::serialize(const BlockHeader &block_header)
{
  PBlockHeader protobuf_block_header = PBlockHeader();
  protobuf_block_header.set_version(block_header.version);
  protobuf_block_header.set_nonce(block_header.nonce);
  protobuf_block_header.set_merkle_root(block_header.merkle_root);
  protobuf_block_header.set_difficulty_target(block_header.difficulty_target);
  protobuf_block_header.set_previous_block_hash(block_header.previous_block_hash);
  protobuf_block_header.set_timestamp(block_header.timestamp);
  return std::move(protobuf_block_header.SerializeAsString());
}

std::unique_ptr<BlockHeader> BlockHeader::deserialize(const std::string &serialized_block_header)
{
  PBlockHeader protobuf_block_header = PBlockHeader();
  protobuf_block_header.ParseFromString(serialized_block_header);
  std::unique_ptr<BlockHeader> block_header = std::make_unique<BlockHeader>(0);
  block_header->set_version(protobuf_block_header.version());
  block_header->set_nonce(protobuf_block_header.nonce());
  block_header->set_merkle_root(protobuf_block_header.merkle_root());
  block_header->set_difficulty_target(protobuf_block_header.difficulty_target());
  block_header->set_previous_block_hash(protobuf_block_header.previous_block_hash());
  block_header->set_timestamp(protobuf_block_header.timestamp());
  return std::move(block_header);
}
