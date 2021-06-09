#include <chain.h>
#include <cryptcrypto.h>
#include <algorithm>

Chain::Chain()
    : _active_chain_last_block(construct_genesis_block()),
      _active_chain_length(1),
      _block_info_database(std::make_unique<BlockInfoDatabase>()),
      _chain_writer(std::make_unique<ChainWriter>()),
      _coin_database(std::make_unique<CoinDatabase>())
{
  std::unique_ptr<BlockRecord> record = _chain_writer->store_block(*_active_chain_last_block, 1);
  _block_info_database->store_block_record(CryptCrypto::hash(Block::serialize(*_active_chain_last_block)), *record);
  _coin_database->store_block(_active_chain_last_block->get_transactions());
}

std::unique_ptr<Block> Chain::construct_genesis_block()
{
  std::unique_ptr<BlockHeader> header = std::make_unique<BlockHeader>(1, 0, 0, 0, 0, 0);
  std::vector<std::unique_ptr<Transaction>> transactions;
  std::vector<std::unique_ptr<TransactionInput>> transactionInput;
  std::vector<std::unique_ptr<TransactionOutput>> transactionOutput;
  transactionOutput.push_back(std::make_unique<TransactionOutput>(100, 12345));
  transactionOutput.push_back(std::make_unique<TransactionOutput>(200, 67891));
  transactionOutput.push_back(std::make_unique<TransactionOutput>(300, 23456));
  transactions.push_back(std::make_unique<Transaction>(std::move(transactionInput), std::move(transactionOutput)));
  std::unique_ptr<Block> genesisBlock = std::make_unique<Block>(std::move(header), std::move(transactions));

  return genesisBlock;
}

std::vector<std::shared_ptr<Block>> Chain::get_forked_blocks_stack(uint32_t starting_hash)
{
  std::vector<uint32_t> forkedBlocks;
  std::vector<uint32_t> activeBlocks = get_active_chain_hashes(1, _active_chain_length);
  std::vector<std::shared_ptr<Block>> result;
  std::unique_ptr<BlockRecord> record;
  uint32_t forkedPrevHash = starting_hash;
  uint32_t blockHeight = _block_info_database->get_block_record(starting_hash)->height;
  std::pair<std::vector<uint32_t>::iterator, std::vector<uint32_t>::iterator> firstMismatch;
  uint32_t activeFirstMismatchHash;

  for (int i = blockHeight; i >= 1; i--)
  {
    forkedBlocks.insert(forkedBlocks.begin(), forkedPrevHash);
    record = _block_info_database->get_block_record(forkedPrevHash);
    forkedPrevHash = record->block_header->previous_block_hash;
  }

  firstMismatch = std::mismatch(activeBlocks.begin(), activeBlocks.end(), forkedBlocks.begin());
  activeFirstMismatchHash = *firstMismatch.first;

  record = _block_info_database->get_block_record(activeFirstMismatchHash);
  uint32_t commonAncestorHash = record->block_header->previous_block_hash;
  forkedPrevHash = record->block_header->previous_block_hash;

  for (int i = blockHeight; i >= 1; i--)
  {
    if (forkedPrevHash == commonAncestorHash)
    {
      result.push_back(get_block(forkedPrevHash));
      break;
    }

    result.push_back(get_block(forkedPrevHash));
    record = _block_info_database->get_block_record(forkedPrevHash);
    forkedPrevHash = record->block_header->previous_block_hash;
  }

  return result;
}

std::vector<std::unique_ptr<UndoBlock>> Chain::get_undo_blocks_queue(uint32_t branching_height)
{
  std::vector<std::unique_ptr<UndoBlock>> result;
  uint32_t prev_block_hash = get_last_block_hash();
  std::unique_ptr<BlockRecord> record;

  if (branching_height == 0 || branching_height > _active_chain_length)
  {
    return result;
  }

  for (int i = 0; i < _active_chain_length - branching_height; i++)
  {
    record = _block_info_database->get_block_record(prev_block_hash);
    std::unique_ptr<FileInfo> block_location = std::make_unique<FileInfo>(record->undo_file_stored, record->undo_offset_start, record->undo_offset_end);
    std::string block = _chain_writer->read_undo_block(*block_location);
    prev_block_hash = record->block_header->previous_block_hash;
    result.push_back(UndoBlock::deserialize(block));
  }

  return result;
}

void Chain::handle_block(std::unique_ptr<Block> block)
{
  int blockHeight;
  uint32_t currentHash = CryptCrypto::hash(Block::serialize(*block));
  uint32_t prevHash = block->block_header->previous_block_hash;

  if (_coin_database->validate_block(block->get_transactions()) || prevHash != get_last_block_hash())
  { // Allowing for forks
    if (prevHash == get_last_block_hash())
    { // on the active chain
      _coin_database->store_block(block->get_transactions());
      blockHeight = _active_chain_length + 1;
      std::unique_ptr<BlockRecord> newRecord = _chain_writer->store_block(*block, blockHeight);
      _block_info_database->store_block_record(currentHash, *newRecord);
      _active_chain_length += 1;
      _active_chain_last_block = std::move(block);
    }
    else
    {
      std::unique_ptr<BlockRecord> prevBlockRecord = _block_info_database->get_block_record(prevHash);
      uint32_t blockHeight = prevBlockRecord->height + 1;
      uint32_t numOfTransactions = block->transactions.size();
      std::unique_ptr<FileInfo> blockFileInfo = _chain_writer->write_block(Block::serialize(*block));
      std::unique_ptr<FileInfo> dummyUndoBlockFileInfo = std::make_unique<FileInfo>("", 0, 0);
      std::string rawHeader = BlockHeader::serialize(*block->block_header);
      std::unique_ptr<BlockRecord> resultBlockRecord = std::make_unique<BlockRecord>(BlockHeader::deserialize(rawHeader), numOfTransactions, blockHeight, *blockFileInfo, *dummyUndoBlockFileInfo);
      _block_info_database->store_block_record(currentHash, *resultBlockRecord);

      if (blockHeight > _active_chain_length)
      {
        std::vector<std::shared_ptr<Block>> forkedBlocks = get_forked_blocks_stack(currentHash);
        std::vector<std::unique_ptr<UndoBlock>> undoBlocks = get_undo_blocks_queue(forkedBlocks.size());
        _coin_database->undo_coins(std::move(undoBlocks));
        
        for (auto &fetchedBlock : forkedBlocks)
        {
          _coin_database->store_block(fetchedBlock->get_transactions());
        }
        _active_chain_length++;
        _active_chain_last_block = std::move(block);
      }
    }
  }
}

void Chain::handle_transaction(std::unique_ptr<Transaction> transaction)
{
  _coin_database->validate_and_store_transaction(std::make_unique<Transaction>(std::move(transaction->transaction_inputs), std::move(transaction->transaction_outputs)));
}

uint32_t Chain::get_chain_length(uint32_t block_hash)
{
  std::unique_ptr<BlockRecord> record = _block_info_database->get_block_record(block_hash);

  if (!record)
  {
    return 0;
  }

  return record->height;
}

std::unique_ptr<Block> Chain::get_block(uint32_t block_hash)
{
  std::unique_ptr<BlockRecord> record = _block_info_database->get_block_record(block_hash);

  if (!record)
  {
    return nullptr;
  }

  std::unique_ptr<FileInfo> block_location = std::make_unique<FileInfo>(record->block_file_stored, record->block_offset_start, record->block_offset_end);
  std::string block = _chain_writer->read_block(*block_location);

  return Block::deserialize(block);
}

std::vector<std::unique_ptr<Block>> Chain::get_active_chain(uint32_t start, uint32_t end)
{
  std::vector<std::unique_ptr<Block>> result;

  if (end < start)
  {
    return result;
  }

  if (start == 0)
  {
    start = 1;
  }

  if (end > _active_chain_length)
  {
    end = _active_chain_length;
  }

  uint32_t prev_block_hash = get_last_block_hash();

  for (int i = _active_chain_length; i >= 1; i--)
  {
    if (i < start)
    {
      break;
    }

    if (i <= end)
    {
      result.insert(result.begin(), get_block(prev_block_hash));
    }

    std::unique_ptr<BlockRecord> record = _block_info_database->get_block_record(prev_block_hash);
    prev_block_hash = record->block_header->previous_block_hash;
  }

  return result;
}

std::vector<uint32_t> Chain::get_active_chain_hashes(uint32_t start, uint32_t end)
{
  std::vector<uint32_t> result;

  if (end < start)
  {
    return result;
  }

  if (start == 0)
  {
    start = 1;
  }

  if (end > _active_chain_length)
  {
    end = _active_chain_length;
  }

  uint32_t prev_block_hash = get_last_block_hash();
  for (int i = _active_chain_length; i >= 1; i--)
  {
    if (i < start)
    {
      break;
    }

    if (i <= end)
    {
      result.insert(result.begin(), prev_block_hash);
    }

    std::unique_ptr<BlockRecord> record = _block_info_database->get_block_record(prev_block_hash);
    prev_block_hash = record->block_header->previous_block_hash;
  }

  return result;
}

std::unique_ptr<Block> Chain::get_last_block()
{
  std::string rawInfo = Block::serialize(*_active_chain_last_block);
  return Block::deserialize(rawInfo);
}

uint32_t Chain::get_last_block_hash()
{
  std::unique_ptr<Block> block = get_last_block();
  return CryptCrypto::hash(Block::serialize(*block));
}

uint32_t Chain::get_active_chain_length() const
{
  return _active_chain_length;
}
