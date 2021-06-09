#ifndef CRYPT_DB_H
#define CRYPT_DB_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <deque>
#include <vector>

enum HashTableWriteOperation
{
  DELETE,
  PUT
};

class BatchOperation
{
public:
  HashTableWriteOperation operation;
  std::string key;
  std::string value;

  BatchOperation() = delete;
  BatchOperation(HashTableWriteOperation operation_, std::string key_, std::string value_);
};

class Database;

class Batch
{
  friend Database;

private:
  std::deque<BatchOperation> _batch;

public:
  Batch();
  void delete_(std::string key);
  void put(std::string key, std::string value);
};

class Database
{
private:
  std::unordered_map<std::string, std::string> _store;
  std::mutex _mutex;

public:
  Database();
  std::string get(std::string key);
  std::string get_safely(std::string key);
  void batch_write_safely(Batch batch);
  std::vector<std::string> batch_read(std::vector<std::string> keys);
  std::vector<std::string> batch_read_safely(std::vector<std::string> keys);
  void put_safely(std::string key, std::string value);
  void delete_safely(std::string key);
  bool has(std::string key);
};

#endif
