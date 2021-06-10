# CryptBank

CryptBank is a storage system that serves as a database for distributed blockchain applications, including cryptocurrencies. Its main component is the chain which involves a coin database, chain writer, and a block info database. 

## Prerequisites

* Latest version of [`Google Protobuf`](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md) installed
* Latest version of [`GoogleTest`](https://github.com/google/googletest/blob/master/README.md) installed
* Latest version of [`CMake`](https://cmake.org/cmake/help/latest/command/cmake.html) installed (on MacOS, you can install via Homebrew with `brew install cmake`)

## Building Manually

```bash
rm -rf build && mkdir build && cd build
cmake ..
make
cd ..
```

## Running Tests
```bash
cd build
make test
```

## Structural Details

### Coin Database

The coin database is a database that stores all the `Coin` objects, manages the storage between RAM and disk by using a cache, and keeps track of which TXOs are spent. A `Coin` is an object that contains Transaction Outputs (TXOs) and whether it is spent or not.

### Chain Writer

The chain writer is an interface that is used to perform read-write disk opertions on the blocks and undo blocks of the chain. When a block is written to disk, the chain writer is responsible for writing the block to the disk and the undo block to the disk. A `BlockRecord` will be used to track where the block and undo block are stored. This is constructed by passing in two `FileInfo` objects that represents the location of the block and undo block.

The whole idea of the chain writer revolves around how blocks are rarely accessed by users but take up a lot of space. So, rather than storing it in RAM, It is stored on disk.


### Block Info Database

The block info database is a database that stores all the `BlockRecord` objects mentioned above. It is used to keep track of where the blocks and undo blocks are stored, the current height of the chain, and the essential information regarding the block which is stored in the `BlockHeader` object.
