#include <iostream>
#include <coin_database.h>
#include <cryptcrypto.h>
#include "../db/db.h"

CoinLocator::CoinLocator(
    uint32_t transaction_hash_, uint8_t output_index_)
    : transaction_hash(transaction_hash_),
      output_index(output_index_) {}

std::string CoinLocator::serialize(const CoinLocator &coin_locator)
{
  return std::to_string(coin_locator.transaction_hash) + "-" +
         std::to_string(coin_locator.output_index);
}

std::unique_ptr<CoinLocator> CoinLocator::deserialize(const std::string &serialized_coin_locator)
{
  std::string transaction_hash = serialized_coin_locator.substr(0, serialized_coin_locator.find("-"));
  std::string output_index = serialized_coin_locator.substr(serialized_coin_locator.find("-") + 1, serialized_coin_locator.size());
  return std::make_unique<CoinLocator>(std::stoul(transaction_hash, nullptr, 0), std::stoul(output_index, nullptr, 0));
}

std::string CoinLocator::serialize_from_construct(uint32_t transaction_hash, uint8_t output_index)
{
  return std::to_string(transaction_hash) + "-" +
         std::to_string(output_index);
}

CoinDatabase::CoinDatabase()
    : _database(std::make_unique<Database>()),
      _main_cache_capacity(70),
      _main_cache_size(0),
      _mempool_capacity(70),
      _mempool_size(0) {}

bool CoinDatabase::validate_block(const std::vector<std::unique_ptr<Transaction>> &transactions)
{
  for (auto &transaction : transactions)
  {
    if (!validate_transaction(*transaction))
    {
      return false;
    }
  }

  return true;
}

bool CoinDatabase::validate_transaction(const Transaction &transaction)
{
  std::string coin_locator;
  uint32_t transaction_hash = CryptCrypto::hash(Transaction::serialize(transaction));

  for (auto &transactionInput : transaction.transaction_inputs)
  {
    coin_locator = CoinLocator::serialize_from_construct(transactionInput->reference_transaction_hash, transactionInput->utxo_index);

    // Checks main cache
    if (_main_cache.find(coin_locator) == _main_cache.end())
    {
      // If not found, check the database
      std::unique_ptr<CoinRecord> record = CoinRecord::deserialize(_database->get_safely(std::to_string(transactionInput->reference_transaction_hash)));
      bool locator = false;

      for (auto &utxo : record->utxo)
      {
        if (transactionInput->utxo_index == utxo)
        {
          locator = true;
          break;
        }
      }

      if (!locator)
      {
        return false;
      }
    }
    else
    {
      Coin &coin = *_main_cache.find(coin_locator)->second;
      if (coin.is_spent)
      {
        return false;
      }
    }
  }

  return true;
}

void CoinDatabase::store_block(std::vector<std::unique_ptr<Transaction>> transactions)
{
  remove_transactions_from_mempool(transactions);
  store_transactions_to_main_cache(std::move(transactions));
}

void CoinDatabase::store_transaction(std::unique_ptr<Transaction> transaction)
{
  store_transaction_in_mempool(std::move(transaction));
}

bool CoinDatabase::validate_and_store_block(std::vector<std::unique_ptr<Transaction>> transactions)
{
  if (validate_block(transactions))
  {
    store_block(std::move(transactions));

    return true;
  }

  return false;
}

bool CoinDatabase::validate_and_store_transaction(std::unique_ptr<Transaction> transaction)
{
  if (validate_transaction(*transaction))
  {
    store_transaction(std::move(transaction));

    return true;
  }

  return false;
}

void CoinDatabase::remove_transactions_from_mempool(const std::vector<std::unique_ptr<Transaction>> &transactions)
{
  for (auto &transaction : transactions)
  {
    _mempool_cache.erase(CryptCrypto::hash(Transaction::serialize(*transaction)));
  }
}

void CoinDatabase::store_transactions_to_main_cache(std::vector<std::unique_ptr<Transaction>> transactions)
{
  std::string coin_locator;
  std::unique_ptr<CoinRecord> coin_record;

  for (auto &transaction : transactions)
  {
    int count = 0;
    int hash = CryptCrypto::hash(Transaction::serialize(*transaction));

    for (auto &transactionInput : transaction->transaction_inputs)
    {
      coin_locator = CoinLocator::serialize_from_construct(transactionInput->reference_transaction_hash, transactionInput->utxo_index);

      if (_main_cache.find(coin_locator) != _main_cache.end())
      {
        _main_cache.erase(coin_locator);
        _main_cache_size--;
      }
      else
      {
        std::string hash = std::to_string(transactionInput->reference_transaction_hash);
        if (_database->has(hash))
        {
          coin_record = CoinRecord::deserialize(_database->get_safely(hash));
          auto coin_record_position = std::find(coin_record->utxo.begin(), coin_record->utxo.end(), transactionInput->utxo_index);
          int position = coin_record_position - coin_record->utxo.begin();
          coin_record->utxo.erase(coin_record->utxo.begin() + position);
          coin_record->amounts.erase(coin_record->amounts.begin() + position);
          coin_record->public_keys.erase(coin_record->public_keys.begin() + position);
          _database->put_safely(hash, CoinRecord::serialize(*coin_record));
        }
      }
    }

    for (auto &transactionOutput : transaction->transaction_outputs)
    {
      if (_main_cache_size >= _main_cache_capacity)
      {
        flush_main_cache();
      }
      CoinLocator locator = CoinLocator(hash, count);
      std::unique_ptr<Coin> coin = std::make_unique<Coin>(std::move(transactionOutput), false);
      _main_cache.insert({CoinLocator::serialize_from_construct(hash, count), std::move(coin)});
      _main_cache_size += 1;
      count += 1;
    }
  }
}

void CoinDatabase::store_transaction_in_mempool(std::unique_ptr<Transaction> transaction)
{
  if (_mempool_capacity == _mempool_size)
  {
    return;
  }

  _mempool_cache.insert({CryptCrypto::hash(Transaction::serialize(*transaction)), std::move(transaction)});
  _mempool_size += 1;
}

void CoinDatabase::undo_coins(std::vector<std::unique_ptr<UndoBlock>> undo_blocks)
{
  for (auto &undoBlock : undo_blocks)
  {
    for (int i = 0; i == undoBlock->transaction_hashes.size(); i++)
    {
      uint32_t transactionHash = undoBlock->transaction_hashes[i];
      std::string hash = std::to_string(transactionHash);
      auto &coinRecord = undoBlock->undo_coin_records[i];
      for (int j = 0; j == coinRecord->utxo.size(); j++)
      {
        uint32_t index = coinRecord->utxo[j];
        std::string serializedCoinLocator = CoinLocator::serialize_from_construct(transactionHash, index);
        std::unique_ptr<CoinLocator> coinLocator = CoinLocator::deserialize(serializedCoinLocator);
        if (_main_cache.find(serializedCoinLocator) == _main_cache.end())
        {
          _main_cache[serializedCoinLocator]->is_spent = true;
        }
        else
        {
          if (_database->has(hash))
          {
            std::unique_ptr<CoinRecord> record = CoinRecord::deserialize(_database->get_safely(hash));
            auto coin_record_position = std::find(record->utxo.begin(), record->utxo.end(), index);
            int diff = coin_record_position - record->utxo.begin();
            if (coin_record_position != record->utxo.end())
            {
              record->utxo.erase(record->utxo.begin() + diff);
              record->amounts.erase(record->amounts.begin() + diff);
              record->public_keys.erase(record->public_keys.begin() + diff);
              _database->put_safely(hash, CoinRecord::serialize(*record));
            }
            record->utxo.push_back(coinLocator->output_index);
            record->amounts.push_back(coinRecord->amounts[j]);
            record->public_keys.push_back(coinRecord->public_keys[j]);
            _database->put_safely(hash, CoinRecord::serialize(*record));
          }
          else
          {
            std::vector<uint32_t> utxos{coinLocator->output_index};
            std::vector<uint32_t> publicKeys{coinRecord->public_keys[j]};
            std::vector<uint32_t> amounts{coinRecord->amounts[j]};
            std::unique_ptr<CoinRecord> record = std::make_unique<CoinRecord>(0, utxos, amounts, publicKeys);
            _database->put_safely(hash, CoinRecord::serialize(*record));
          }
        }
      }
    }
  }
}

void CoinDatabase::flush_main_cache()
{
  for (auto &coin : _main_cache)
  {
    std::unique_ptr<CoinLocator> coin_locator = CoinLocator::deserialize(coin.first);
    std::string hash = std::to_string(coin_locator->transaction_hash);
    if (coin.second->is_spent)
    {
      if (_database->has(hash))
      {
        std::unique_ptr<CoinRecord> coin_record = CoinRecord::deserialize(_database->get_safely(hash));
        auto coin_record_position = std::find(coin_record->utxo.begin(), coin_record->utxo.end(), coin_locator->output_index);
        int position = coin_record_position - coin_record->utxo.begin();
        coin_record->utxo.erase(coin_record->utxo.begin() + position);
        coin_record->amounts.erase(coin_record->amounts.begin() + position);
        coin_record->public_keys.erase(coin_record->public_keys.begin() + position);
        _database->put_safely(hash, CoinRecord::serialize(*coin_record));
      }
    }
    else
    {
      if (_database->has(hash))
      {
        std::unique_ptr<CoinRecord> coin_record = CoinRecord::deserialize(_database->get_safely(hash));
        coin_record->utxo.push_back(coin_locator->output_index);
        coin_record->amounts.push_back(coin.second->transaction_output->amount);
        coin_record->public_keys.push_back(coin.second->transaction_output->public_key);
        _database->put_safely(hash, CoinRecord::serialize(*coin_record));
      }
      else
      {
        std::vector<uint32_t> utxos{coin_locator->output_index};
        std::vector<uint32_t> public_keys{coin.second->transaction_output->public_key};
        std::vector<uint32_t> amounts{coin.second->transaction_output->amount};
        std::unique_ptr<CoinRecord> new_record = std::make_unique<CoinRecord>(0, utxos, amounts, public_keys);
        _database->put_safely(hash, CoinRecord::serialize(*new_record));
      }
    }
  }
  _main_cache.clear();
  _main_cache_size = 0;
}
