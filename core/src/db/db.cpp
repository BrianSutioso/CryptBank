#include <db.h>

BatchOperation::BatchOperation(HashTableWriteOperation operation_, std::string key_,
                               std::string value_) : operation(operation_), key(key_), value(value_) {}

Batch::Batch() {}

void Batch::delete_(std::string key)
{
  _batch.push_back(BatchOperation(DELETE, key, ""));
}

void Batch::put(std::string key, std::string value)
{
  _batch.push_back(BatchOperation(PUT, key, value));
}

Database::Database() : _store(std::unordered_map<std::string, std::string>()) {}

std::string Database::get(std::string key)
{
  return _store[key];
}

std::string Database::get_safely(std::string key)
{
  std::lock_guard<std::mutex> guard(_mutex);
  return _store[key];
}

void Database::batch_write_safely(Batch batch)
{
  std::lock_guard<std::mutex> guard(_mutex);
  for (const BatchOperation &batch_operation : batch._batch)
  {
    if (batch_operation.operation == DELETE)
    {
      _store.erase(batch_operation.key);
    }
    else if (batch_operation.operation == PUT)
    {
      _store[batch_operation.key] = batch_operation.value;
    }
  }
}

std::vector<std::string> Database::batch_read(std::vector<std::string> keys)
{
  std::vector<std::string> reads;
  for (const std::string &key : keys)
  {
    reads.push_back(_store[key]);
  }
  return std::move(reads);
}

std::vector<std::string> Database::batch_read_safely(std::vector<std::string> keys)
{
  std::vector<std::string> reads;
  std::lock_guard<std::mutex> guard(_mutex);
  for (const std::string &key : keys)
  {
    reads.push_back(_store[key]);
  }
  return std::move(reads);
}

void Database::put_safely(std::string key, std::string value)
{
  std::lock_guard<std::mutex> guard(_mutex);
  _store[key] = value;
}

void Database::delete_safely(std::string key)
{
  std::lock_guard<std::mutex> guard(_mutex);
  _store.erase(key);
}

bool Database::has(std::string key)
{
  return _store.find(key) != _store.end();
}
