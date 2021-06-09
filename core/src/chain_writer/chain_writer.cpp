#include <iostream>
#include <fstream>
#include <vector>
#include <chain_writer.h>
#include <cryptcrypto.h>
#include <undo_coin_record.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <filesystem>

const std::string ChainWriter::_file_extension = "data";
const std::string ChainWriter::_block_filename = "blocks";
const uint16_t ChainWriter::_max_block_file_size = 1000;
const std::string ChainWriter::_undo_filename = "undo_blocks";
const uint16_t ChainWriter::_max_undo_file_size = 1000;
const std::string ChainWriter::_data_directory = "data";

ChainWriter::ChainWriter()
    : _current_block_file_number(0),
      _current_block_offset(0),
      _current_undo_file_number(0),
      _current_undo_offset(0) {}

std::unique_ptr<UndoBlock> make_undo_block(const Block &block)
{
  std::vector<uint32_t> transaction_hashes;
  std::vector<std::unique_ptr<UndoCoinRecord>> undo_coin_records;
  std::vector<uint32_t> utxo_index;
  std::vector<uint32_t> amounts;
  std::vector<uint32_t> public_keys;

  for (auto &transaction : block.transactions)
  {
    transaction_hashes.push_back(CryptCrypto::hash(Transaction::serialize(*transaction)));

    for (auto &transactionInput : transaction->transaction_inputs)
    {
      utxo_index.push_back(transactionInput->utxo_index);
    }

    for (auto &transactionOutput : transaction->transaction_outputs)
    {
      amounts.push_back(transactionOutput->amount);
      public_keys.push_back(transactionOutput->public_key);
    }

    std::unique_ptr<UndoCoinRecord> undoRecord = std::make_unique<UndoCoinRecord>(transaction->version, utxo_index, amounts, public_keys);
    undo_coin_records.push_back(std::move(undoRecord));
  }

  return std::make_unique<UndoBlock>(std::move(transaction_hashes), std::move(undo_coin_records));
}

std::unique_ptr<BlockRecord> ChainWriter::store_block(const Block &block, uint32_t height)
{
  std::unique_ptr<UndoBlock> undoBlock = make_undo_block(block);
  std::unique_ptr<FileInfo> blockInfo = write_block(Block::serialize(block));
  std::unique_ptr<FileInfo> undoInfo = write_undo_block(UndoBlock::serialize(*undoBlock));

  return std::make_unique<BlockRecord>(std::make_unique<BlockHeader>(block.block_header->version,
                                                                     block.block_header->previous_block_hash,
                                                                     block.block_header->merkle_root,
                                                                     block.block_header->difficulty_target,
                                                                     block.block_header->nonce,
                                                                     block.block_header->timestamp),
                                       block.transactions.size(), height, *blockInfo, *undoInfo);
}

std::unique_ptr<FileInfo> ChainWriter::write_block(std::string serialized_block)
{
  FILE *fp;
  std::string filePath;
  uint32_t N = serialized_block.size() + 1;

  if (_max_block_file_size - _current_block_offset <= N)
  { // checks if serialized block fits in the file
    _current_block_offset = 0;
    _current_block_file_number += 1;
  }

  if (!std::filesystem::is_directory(get_data_directory()))
  { // checks if the directory exists
    std::filesystem::create_directory(get_data_directory());
  }

  filePath = get_data_directory() + "/" + get_block_filename() + std::to_string(_current_block_file_number) + "." + get_file_extension();
  fp = fopen(filePath.c_str(), "ab");

  if (fp != nullptr)
  {
    fwrite(serialized_block.c_str(), sizeof(char), N, fp);
    fclose(fp);

    uint16_t start = _current_block_offset;
    uint16_t end = _current_block_offset + N;
    _current_block_offset = end;

    return std::make_unique<FileInfo>(filePath, start, end);
  }
  return nullptr;
}

std::unique_ptr<FileInfo> ChainWriter::write_undo_block(std::string serialized_block)
{
  FILE *fp;
  std::string filePath;
  uint32_t N = serialized_block.size() + 1;

  if (_max_undo_file_size - _current_undo_offset < N)
  { // checks if serialized block fits in the file
    _current_undo_offset = 0;
    _current_undo_file_number += 1;
  }

  filePath = get_data_directory() + "/" + get_undo_filename() + std::to_string(_current_undo_file_number) + "." + get_file_extension();
  fp = fopen(filePath.c_str(), "ab");

  if (fp != nullptr)
  {
    fwrite(serialized_block.c_str(), sizeof(char), N, fp);
    fclose(fp);

    uint16_t start = _current_undo_offset;
    uint16_t end = _current_undo_offset + N;
    _current_undo_offset = end;

    return std::make_unique<FileInfo>(filePath, start, end);
  }

  return nullptr;
}

std::string ChainWriter::read_block(const FileInfo &block_location)
{
  FILE *fp;
  std::string filePath = block_location.file_name;
  int length = block_location.end - block_location.start;
  char buffer[length];

  fp = fopen(block_location.file_name.c_str(), "rb");
  
  if (fp != nullptr)
  {
    fseek(fp, block_location.start, SEEK_SET);
    fread(buffer, sizeof(char), length, fp);
    fclose(fp);
    std::string result(buffer, length - 1);

    return result;
  }

  return "";
}

std::string ChainWriter::read_undo_block(const FileInfo &undo_block_location)
{
  return read_block(undo_block_location);
}
