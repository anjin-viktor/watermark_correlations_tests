#include <boost/test/unit_test.hpp>

#include <memory>

#include "VideoFrame.h"
#include "VideoFrameBlockProcessor.h"
#include "Utils.h"

BOOST_AUTO_TEST_SUITE(video_frame_block_processor);

BOOST_AUTO_TEST_CASE(block_count)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  std::size_t widthInBlocks = pframe->width() / 8;
  std::size_t heightInBlocks = pframe->height() / 8;

  VideoFrameBlockProcessor processor(pframe, 8);
  std::size_t count = processor.getBlockCount();
  BOOST_CHECK_EQUAL(widthInBlocks * heightInBlocks, count);

  pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Color);
  VideoFrameBlockProcessor processorColor(pframe, 8);
  count = processorColor.getBlockCount();
  BOOST_CHECK_EQUAL(widthInBlocks * heightInBlocks * 3, count);
}

BOOST_AUTO_TEST_CASE(block_get_gray)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/field-sd-gray.jpg", VideoFrame::Grayscale);

  BOOST_CHECK_EQUAL(pframe->width(), 640);
  BOOST_CHECK_EQUAL(pframe->height(), 480);

  VideoFrameBlockProcessor processor(pframe, 8);
  std::vector<uint8_t> blockData = processor.getBlock(0);

  BOOST_CHECK_EQUAL(blockData[0], 99);
  BOOST_CHECK_EQUAL(blockData[63], 96);

  blockData = processor.getBlock(80);
  BOOST_CHECK_EQUAL(blockData[0], 97);
  BOOST_CHECK_EQUAL(blockData[63], 95);

  blockData = processor.getBlock(2440);
  BOOST_CHECK_EQUAL(blockData[0], 155);
  BOOST_CHECK_EQUAL(blockData[63], 162);
}

BOOST_AUTO_TEST_CASE(block_get_color)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Color);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  VideoFrameBlockProcessor processor(pframe, 8);
  std::vector<uint8_t> blockData = processor.getBlock(0);

  BOOST_CHECK_EQUAL(blockData[0], 92);
  BOOST_CHECK_EQUAL(blockData[1], 95);
  BOOST_CHECK_EQUAL(blockData[2], 96);
  BOOST_CHECK_EQUAL(blockData[63], 103);

  blockData = processor.getBlock(1);
  BOOST_CHECK_EQUAL(blockData[0], 83);
  BOOST_CHECK_EQUAL(blockData[1], 86);
  BOOST_CHECK_EQUAL(blockData[2], 87);
  BOOST_CHECK_EQUAL(blockData[63], 97);

  blockData = processor.getBlock(2);
  BOOST_CHECK_EQUAL(blockData[0], 0);
  BOOST_CHECK_EQUAL(blockData[1], 0);
  BOOST_CHECK_EQUAL(blockData[2], 0);
  BOOST_CHECK_EQUAL(blockData[63], 0);


  blockData = processor.getBlock(14397);
  BOOST_CHECK_EQUAL(blockData[0], 61);
  BOOST_CHECK_EQUAL(blockData[1], 44);
  BOOST_CHECK_EQUAL(blockData[2], 37);
  BOOST_CHECK_EQUAL(blockData[63], 63);

  blockData = processor.getBlock(14398);
  BOOST_CHECK_EQUAL(blockData[0], 54);
  BOOST_CHECK_EQUAL(blockData[1], 36);
  BOOST_CHECK_EQUAL(blockData[2], 29);
  BOOST_CHECK_EQUAL(blockData[63], 57);

  blockData = processor.getBlock(14399);
  BOOST_CHECK_EQUAL(blockData[0], 57);
  BOOST_CHECK_EQUAL(blockData[1], 37);
  BOOST_CHECK_EQUAL(blockData[2], 30);
  BOOST_CHECK_EQUAL(blockData[63], 58);
}


BOOST_AUTO_TEST_CASE(block_set_gray)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/field-sd-gray.jpg", VideoFrame::Grayscale);

  BOOST_CHECK_EQUAL(pframe->width(), 640);
  BOOST_CHECK_EQUAL(pframe->height(), 480);

  VideoFrameBlockProcessor processor(pframe, 8);
  std::vector<uint8_t> blockData(64);
  uint8_t* pdata = pframe->data(0);
  std::size_t stride = pframe->stride(0);

  std::fill(blockData.begin(), blockData.end(), 0xFF);

  processor.setBlock(blockData, 0);

  BOOST_CHECK_EQUAL(pdata[0], 0xFF);
  BOOST_CHECK_EQUAL(pdata[7*stride + 7], 0xFF);
  BOOST_CHECK(pdata[7 * stride + 8] != 0xFF);

  processor.setBlock(blockData, 80);
  BOOST_CHECK_EQUAL(pdata[8 * stride], 0xFF);
  BOOST_CHECK_EQUAL(pdata[15 * stride + 7], 0xFF);
  BOOST_CHECK(pdata[15 * stride + 8] != 0xFF);

  processor.setBlock(blockData, 2440);
  BOOST_CHECK_EQUAL(pdata[240 * stride + 40*8], 0xFF);
  BOOST_CHECK_EQUAL(pdata[247 * stride + 40 * 8 + 7], 0xFF);
  BOOST_CHECK(pdata[247 * stride + 40 * 8 + 8] != 0xFF);
}

BOOST_AUTO_TEST_CASE(block_set_color)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Color);

  VideoFrameBlockProcessor processor(pframe, 8);
  std::vector<uint8_t> blockDataB(64);
  std::vector<uint8_t> blockDataG(64);
  std::vector<uint8_t> blockDataR(64);

  std::fill(blockDataB.begin(), blockDataB.end(), 0xFF);
  std::fill(blockDataG.begin(), blockDataG.end(), 0x50);
  std::fill(blockDataR.begin(), blockDataR.end(), 0x15);

  processor.setBlock(blockDataB, 0);
  processor.setBlock(blockDataG, 1);
  processor.setBlock(blockDataR, 2);

  uint8_t* pdata = pframe->data(0);
  std::size_t stride = pframe->stride(0);
  BOOST_CHECK_EQUAL(pdata[0], 0xFF);
  BOOST_CHECK_EQUAL(pdata[1], 0x50);
  BOOST_CHECK_EQUAL(pdata[2], 0x15);

  BOOST_CHECK_EQUAL(pdata[7*stride + 7*3], 0xFF);
  BOOST_CHECK_EQUAL(pdata[7 * stride + 7 * 3 + 1], 0x50);
  BOOST_CHECK_EQUAL(pdata[7 * stride + 7 * 3 + 2], 0x15);

  BOOST_CHECK(pdata[7 * stride + 8 * 3] != 0xFF);
  BOOST_CHECK(pdata[7 * stride + 8 * 3 + 1] != 0x50);
  BOOST_CHECK(pdata[7 * stride + 8 * 3 + 2] != 0x15);


  processor.setBlock(blockDataB, 14397);
  processor.setBlock(blockDataG, 14398);
  processor.setBlock(blockDataR, 14399);

  BOOST_CHECK_EQUAL(pdata[632 * stride + 472 * 3], 0xFF);
  BOOST_CHECK_EQUAL(pdata[632 * stride + 472 * 3 + 1], 0x50);
  BOOST_CHECK_EQUAL(pdata[632 * stride + 472 * 3 + 2], 0x15);

  BOOST_CHECK_EQUAL(pdata[639 * stride + 479 * 3], 0xFF);
  BOOST_CHECK_EQUAL(pdata[639 * stride + 479 * 3 + 1], 0x50);
  BOOST_CHECK_EQUAL(pdata[639 * stride + 479 * 3 + 2], 0x15);
}

BOOST_AUTO_TEST_SUITE_END();
