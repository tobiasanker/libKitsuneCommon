﻿/**
 *  @file    binary_file_test.cpp
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
 */

#include "binary_file_without_directIO_test.h"

#include <libKitsunemimiCommon/files/binary_file.h>

namespace Kitsunemimi
{

struct TestStruct
{
    uint8_t a = 0;
    uint8_t b = 0;
    uint64_t c = 0;
} __attribute__((packed));

BinaryFile_withoutDirectIO_Test::BinaryFile_withoutDirectIO_Test()
    : Kitsunemimi::CompareTestHelper("BinaryFile_withoutDirectIO_Test")
{
    initTest();
    closeFile_test();
    allocateStorage_test();
    writeSegment_test();
    readSegment_test();
    writeCompleteFile_test();
    readCompleteFile_test();
    writeDataIntoFile_test();
    readDataFromFile_test();
    closeTest();
}

/**
 * initTest
 */
void
BinaryFile_withoutDirectIO_Test::initTest()
{
    m_filePath = "/tmp/binaryFile_test.bin";
    deleteFile();
}

/**
 * closeFile_test
 */
void
BinaryFile_withoutDirectIO_Test::closeFile_test()
{
    // init buffer and file
    DataBuffer buffer;
    BinaryFile binaryFile(m_filePath, false);

    // test close
    TEST_EQUAL(binaryFile.closeFile(), true);
    TEST_EQUAL(binaryFile.closeFile(), false);

    deleteFile();
}

/**
 * @brief BinaryFile_Test::updateFileSize_test
 */
void
BinaryFile_withoutDirectIO_Test::updateFileSize_test()
{
    // init buffer and file
    DataBuffer buffer(5);
    BinaryFile binaryFile(m_filePath, false);
    binaryFile.allocateStorage(4, 4000);
    binaryFile.closeFile();

    BinaryFile binaryFileNew(m_filePath, false);
    TEST_EQUAL(binaryFileNew.updateFileSize(), true);
    TEST_EQUAL(binaryFileNew.m_totalFileSize, 4*4096);

    TEST_EQUAL(binaryFileNew.m_totalFileSize, binaryFileNew.m_totalFileSize);
}

/**
 * allocateStorage_test
 */
void
BinaryFile_withoutDirectIO_Test::allocateStorage_test()
{
    // init buffer and file
    DataBuffer buffer;
    BinaryFile binaryFile(m_filePath, false);

    // test allocation
    TEST_EQUAL(binaryFile.allocateStorage(4, 4000), true);
    TEST_EQUAL(binaryFile.allocateStorage(4, 4000), true);
    TEST_EQUAL(binaryFile.allocateStorage(0, 4000), false);

    // check meta-data
    TEST_EQUAL(binaryFile.m_totalFileSize, 8*4000);

    binaryFile.closeFile();

    // negative test
    TEST_EQUAL(binaryFile.allocateStorage(4, 4000), false);

    deleteFile();
}

/**
 * writeSegment_test
 */
void
BinaryFile_withoutDirectIO_Test::writeSegment_test()
{
    // init buffer and file
    DataBuffer buffer(5);
    BinaryFile binaryFile(m_filePath, false);
    binaryFile.allocateStorage(4, 4096);

    // prepare test-buffer
    TestStruct testStruct;
    testStruct.a = 42;
    testStruct.c = 1337;
    addObject_DataBuffer(buffer, &testStruct);
    buffer.usedBufferSize = 2000;
    addObject_DataBuffer(buffer, &testStruct);

    // write-tests
    TEST_EQUAL(binaryFile.writeSegment(buffer, 1000, 1000, 0), true);
    TEST_EQUAL(binaryFile.writeSegment(buffer, 2000, 1000, 2000), true);

    // negative tests
    TEST_EQUAL(binaryFile.writeSegment(buffer, 2000, 0, 3000), false);
    TEST_EQUAL(binaryFile.writeSegment(buffer, 42000, 1000, 3000), false);
    TEST_EQUAL(binaryFile.writeSegment(buffer, 2000, 42000, 3000), false);
    TEST_EQUAL(binaryFile.writeSegment(buffer, 2000, 1000, 42000), false);

    // cleanup
    TEST_EQUAL(binaryFile.closeFile(), true);
    deleteFile();
}

/**
 * readSegment_test
 */
void
BinaryFile_withoutDirectIO_Test::readSegment_test()
{
    // init buffer and file
    DataBuffer buffer(5);
    BinaryFile binaryFile(m_filePath, false);
    binaryFile.allocateStorage(4, 4096);

    // prepare test-buffer
    TestStruct testStruct;
    testStruct.a = 42;
    testStruct.c = 1337;
    addObject_DataBuffer(buffer, &testStruct);
    testStruct.a = 10;
    testStruct.c = 1234;
    buffer.usedBufferSize = 2000;
    addObject_DataBuffer(buffer, &testStruct);

    // write the two blocks of the buffer
    TEST_EQUAL(binaryFile.writeSegment(buffer, 1000, 1000, 0), true);
    TEST_EQUAL(binaryFile.writeSegment(buffer, 2000, 1000, 2000), true);

    // clear orinial buffer
    memset(buffer.data, 0, buffer.totalBufferSize);
    testStruct.a = 0;
    testStruct.c = 0;

    // read the two blocks back
    TEST_EQUAL(binaryFile.readSegment(buffer, 1000, 1000, 1000), true);
    TEST_EQUAL(binaryFile.readSegment(buffer, 2000, 1000, 3000), true);

    // negative tests
    TEST_EQUAL(binaryFile.readSegment(buffer, 2000, 0, 3000), false);
    TEST_EQUAL(binaryFile.readSegment(buffer, 42000, 1000, 3000), false);
    TEST_EQUAL(binaryFile.readSegment(buffer, 2000, 42000, 3000), false);
    TEST_EQUAL(binaryFile.readSegment(buffer, 2000, 1000, 42000), false);

    // copy and check the first block
    mempcpy(&testStruct,
            static_cast<uint8_t*>(buffer.data) + 1000,
            sizeof(TestStruct));
    TEST_EQUAL(testStruct.a, 42);
    TEST_EQUAL(testStruct.c, 1337);

    // copy and check the second block
    mempcpy(&testStruct,
            static_cast<uint8_t*>(buffer.data) + 3000,
            sizeof(TestStruct));
    TEST_EQUAL(testStruct.a, 10);
    TEST_EQUAL(testStruct.c, 1234);

    // cleanup
    TEST_EQUAL(binaryFile.closeFile(), true);
    deleteFile();
}

/**
 * writeCompleteFile_test
 */
void
BinaryFile_withoutDirectIO_Test::writeCompleteFile_test()
{
    // init buffer and file
    DataBuffer buffer(5);
    BinaryFile binaryFile(m_filePath, false);

    // prepare test-buffer
    TestStruct testStruct;
    testStruct.a = 42;
    testStruct.c = 1337;
    addObject_DataBuffer(buffer, &testStruct);
    testStruct.a = 10;
    testStruct.c = 1234;
    addObject_DataBuffer(buffer, &testStruct);

    TEST_EQUAL(binaryFile.writeCompleteFile(buffer), true);
    TEST_EQUAL(std::filesystem::file_size(m_filePath), 2 * sizeof(TestStruct));

    // cleanup
    TEST_EQUAL(binaryFile.closeFile(), true);
    deleteFile();
}

/**
 * readCompleteFile_test
 */
void
BinaryFile_withoutDirectIO_Test::readCompleteFile_test()
{
    // init buffer and file
    DataBuffer sourceBuffer(5);
    DataBuffer targetBuffer(5);
    BinaryFile binaryFile(m_filePath, false);

    // prepare test-buffer
    TestStruct testStruct;
    testStruct.a = 42;
    testStruct.c = 1337;
    addObject_DataBuffer(sourceBuffer, &testStruct);
    testStruct.a = 10;
    testStruct.c = 1234;
    addObject_DataBuffer(sourceBuffer, &testStruct);

    // test with buffer-size unequal a multiple of the block-size
    sourceBuffer.usedBufferSize = 2 * sourceBuffer.blockSize + 1;
    targetBuffer.usedBufferSize = 2 * targetBuffer.blockSize + 1;

    binaryFile.writeCompleteFile(sourceBuffer);
    TEST_EQUAL(binaryFile.readCompleteFile(targetBuffer), true);

    // check if source and target-buffer are
    int ret = memcmp(sourceBuffer.data,
                     targetBuffer.data,
                     2 * sourceBuffer.blockSize + 1);
    TEST_EQUAL(ret, 0);

    // cleanup
    TEST_EQUAL(binaryFile.closeFile(), true);
    deleteFile();
}

/**
 * @brief writeDataIntoFile_test
 */
void
BinaryFile_withoutDirectIO_Test::writeDataIntoFile_test()
{
    DataBuffer targetBuffer(5);

    // init buffer and file
    DataBuffer sourceBuffer(5);
    BinaryFile binaryFile(m_filePath, false);
    binaryFile.allocateStorage(4, 4096);

    // prepare test-buffer
    TestStruct testStruct;
    testStruct.a = 42;
    testStruct.c = 1337;
    addObject_DataBuffer(sourceBuffer, &testStruct);
    sourceBuffer.usedBufferSize = 2000;
    addObject_DataBuffer(sourceBuffer, &testStruct);

    // write-tests
    TEST_EQUAL(binaryFile.writeDataIntoFile(sourceBuffer.data, 0, 4000), true);

    // negative tests
    TEST_EQUAL(binaryFile.writeDataIntoFile(sourceBuffer.data, 42000, 1000), false);
    TEST_EQUAL(binaryFile.writeDataIntoFile(sourceBuffer.data, 2000, 42000), false);

    TEST_EQUAL(binaryFile.readCompleteFile(targetBuffer), true);

    // check if source and target-buffer are
    int ret = memcmp(sourceBuffer.data,
                     targetBuffer.data,
                     2 * sourceBuffer.blockSize + 1);
    TEST_EQUAL(ret, 0);

    // cleanup
    TEST_EQUAL(binaryFile.closeFile(), true);
    deleteFile();
}

/**
 * @brief readDataFromFile_test
 */
void
BinaryFile_withoutDirectIO_Test::readDataFromFile_test()
{
    DataBuffer targetBuffer(5);

    // init buffer and file
    DataBuffer sourceBuffer(5);
    BinaryFile binaryFile(m_filePath, false);
    binaryFile.allocateStorage(4, 4096);

    // prepare test-buffer
    TestStruct testStruct;
    testStruct.a = 42;
    testStruct.c = 1337;
    addObject_DataBuffer(sourceBuffer, &testStruct);
    sourceBuffer.usedBufferSize = 2000;
    addObject_DataBuffer(sourceBuffer, &testStruct);

    // write-tests
    binaryFile.writeDataIntoFile(sourceBuffer.data, 0, 4000);
    TEST_EQUAL(binaryFile.readDataFromFile(targetBuffer.data, 0, 4000), true);

    // negative tests
    TEST_EQUAL(binaryFile.readDataFromFile(targetBuffer.data, 42000, 1000), false);
    TEST_EQUAL(binaryFile.readDataFromFile(targetBuffer.data, 2000, 42000), false);

    // check if source and target-buffer are
    int ret = memcmp(sourceBuffer.data,
                     targetBuffer.data,
                     2 * sourceBuffer.blockSize + 1);
    TEST_EQUAL(ret, 0);

    // cleanup
    TEST_EQUAL(binaryFile.closeFile(), true);
    deleteFile();
}

/**
 * closeTest
 */
void
BinaryFile_withoutDirectIO_Test::closeTest()
{
    deleteFile();
}

/**
 * common usage to delete test-file
 */
void
BinaryFile_withoutDirectIO_Test::deleteFile()
{
    std::filesystem::path rootPathObj(m_filePath);
    if(std::filesystem::exists(rootPathObj)) {
        std::filesystem::remove(rootPathObj);
    }
}

} // namespace Kitsunemimi

