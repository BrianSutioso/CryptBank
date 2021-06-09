#include <gtest/gtest.h>
#include <chain_writer.h>
#include <file_info.h>
#include <cryptcrypto.h>
#include <filesystem>
#include <memory>

TEST(ChainWriter, TestWriteReadSingleEntry)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  ChainWriter chainWriter = ChainWriter();
  std::string testString = "serialized_block";

  FileInfo resultingFile = *chainWriter.write_block(testString);
  std::string fetchedString = chainWriter.read_block(resultingFile);

  EXPECT_EQ(testString, fetchedString);

  std::filesystem::remove_all(ChainWriter::get_data_directory());
}

TEST(ChainWriter, TestWriteReadMultipleEntries)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  ChainWriter chainWriter = ChainWriter();
  std::string testString1 = "serialized_block1";
  std::string testString2 = "serialized_block2";
  std::string testString3 = "serialized_block3";
  std::string testString4 = "serialized_block4";
  std::string testString5 = "serialized_block5";
  std::string testString6 = "serialized_block6";
  std::string testString7 = "serialized_block7";
  std::string testString8 = "serialized_block8";
  std::string testString9 = "serialized_block9";
  std::string testString10 = "serialized_block10";
  std::string testString11 = "serialized_block11";
  std::string testString12 = "serialized_block12";
  std::string testString13 = "serialized_block13";
  std::string testString14 = "serialized_block14";
  std::string testString15 = "serialized_block15";
  std::string testString16 = "serialized_block16";
  std::string testString17 = "serialized_block17";
  std::string testString18 = "serialized_block18";
  std::string testString19 = "serialized_block19";
  std::string testString20 = "serialized_block20";
  std::string testString21 = "serialized_block21";
  std::string testString22 = "serialized_block22";
  std::string testString23 = "serialized_block23";
  std::string testString24 = "serialized_block24";
  std::string testString25 = "serialized_block25";

  FileInfo resultingFile1 = *chainWriter.write_block(testString1);
  FileInfo resultingFile2 = *chainWriter.write_block(testString2);
  FileInfo resultingFile3 = *chainWriter.write_block(testString3);
  FileInfo resultingFile4 = *chainWriter.write_block(testString4);
  FileInfo resultingFile5 = *chainWriter.write_block(testString5);
  FileInfo resultingFile6 = *chainWriter.write_block(testString6);
  FileInfo resultingFile7 = *chainWriter.write_block(testString7);
  FileInfo resultingFile8 = *chainWriter.write_block(testString8);
  FileInfo resultingFile9 = *chainWriter.write_block(testString9);
  FileInfo resultingFile10 = *chainWriter.write_block(testString10);
  FileInfo resultingFile11 = *chainWriter.write_block(testString11);
  FileInfo resultingFile12 = *chainWriter.write_block(testString12);
  FileInfo resultingFile13 = *chainWriter.write_block(testString13);
  FileInfo resultingFile14 = *chainWriter.write_block(testString14);
  FileInfo resultingFile15 = *chainWriter.write_block(testString15);
  FileInfo resultingFile16 = *chainWriter.write_block(testString16);
  FileInfo resultingFile17 = *chainWriter.write_block(testString17);
  FileInfo resultingFile18 = *chainWriter.write_block(testString18);
  FileInfo resultingFile19 = *chainWriter.write_block(testString19);
  FileInfo resultingFile20 = *chainWriter.write_block(testString20);
  FileInfo resultingFile21 = *chainWriter.write_block(testString21);
  FileInfo resultingFile22 = *chainWriter.write_block(testString22);
  FileInfo resultingFile23 = *chainWriter.write_block(testString23);
  FileInfo resultingFile24 = *chainWriter.write_block(testString24);
  FileInfo resultingFile25 = *chainWriter.write_block(testString25);
  std::string fetchedString1 = chainWriter.read_block(resultingFile1);
  std::string fetchedString2 = chainWriter.read_block(resultingFile2);
  std::string fetchedString3 = chainWriter.read_block(resultingFile3);
  std::string fetchedString4 = chainWriter.read_block(resultingFile4);
  std::string fetchedString5 = chainWriter.read_block(resultingFile5);
  std::string fetchedString6 = chainWriter.read_block(resultingFile6);
  std::string fetchedString7 = chainWriter.read_block(resultingFile7);
  std::string fetchedString8 = chainWriter.read_block(resultingFile8);
  std::string fetchedString9 = chainWriter.read_block(resultingFile9);
  std::string fetchedString10 = chainWriter.read_block(resultingFile10);
  std::string fetchedString11 = chainWriter.read_block(resultingFile11);
  std::string fetchedString12 = chainWriter.read_block(resultingFile12);
  std::string fetchedString13 = chainWriter.read_block(resultingFile13);
  std::string fetchedString14 = chainWriter.read_block(resultingFile14);
  std::string fetchedString15 = chainWriter.read_block(resultingFile15);
  std::string fetchedString16 = chainWriter.read_block(resultingFile16);
  std::string fetchedString17 = chainWriter.read_block(resultingFile17);
  std::string fetchedString18 = chainWriter.read_block(resultingFile18);
  std::string fetchedString19 = chainWriter.read_block(resultingFile19);
  std::string fetchedString20 = chainWriter.read_block(resultingFile20);
  std::string fetchedString21 = chainWriter.read_block(resultingFile21);
  std::string fetchedString22 = chainWriter.read_block(resultingFile22);
  std::string fetchedString23 = chainWriter.read_block(resultingFile23);
  std::string fetchedString24 = chainWriter.read_block(resultingFile24);
  std::string fetchedString25 = chainWriter.read_block(resultingFile25);

  EXPECT_EQ(testString25, fetchedString25);

  std::filesystem::remove_all(ChainWriter::get_data_directory());
}

TEST(ChainWriter, TestReadNonEntry)
{
  std::filesystem::remove_all(ChainWriter::get_data_directory());

  ChainWriter chainWriter = ChainWriter();

  FileInfo fileInfo = FileInfo("", 0, 0);
  std::string fetchedString = chainWriter.read_block(fileInfo);

  EXPECT_EQ(nullptr, Block::deserialize(fetchedString));

  std::filesystem::remove_all(ChainWriter::get_data_directory());
}
