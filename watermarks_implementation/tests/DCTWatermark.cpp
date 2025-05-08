#include <boost/test/unit_test.hpp>

#include <memory>

#include <opencv2/opencv.hpp>

#include "Utils.h"
#include "VideoFrame.h"
#include "DCTWatermark.h"

BOOST_AUTO_TEST_SUITE(dct_watermark);

BOOST_AUTO_TEST_CASE(dct_reference)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 64;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  std::size_t widthInBlocks = pframe->width() / DCTWatermark::BLOCK_SIZE;
  std::size_t heightInBlocks = pframe->height() / DCTWatermark::BLOCK_SIZE;
  std::size_t expectedSize = (DCTWatermark::BLOCK_SIZE * DCTWatermark::BLOCK_SIZE - skipCount) * widthInBlocks * heightInBlocks;

  BOOST_CHECK_EQUAL(expectedSize, reference.size());

  bool maxValueCorrect = true;
  for (std::size_t i = 0; i < reference.size(); i++)
  {
    if (reference[i] >= maxValue)
      maxValueCorrect = false;
  }
  BOOST_CHECK(maxValueCorrect);
}

BOOST_AUTO_TEST_CASE(dct_reference_store_load)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 64;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  DCTWatermark::referenceStore(reference, getSourceDir(__FILE__) + "out/dct_reference_store_load");
  std::vector<int> loaded = DCTWatermark::referenceLoad(getSourceDir(__FILE__) + "out/dct_reference_store_load");

  BOOST_CHECK_EQUAL_COLLECTIONS(reference.begin(), reference.end(), loaded.begin(), loaded.end());
}

BOOST_AUTO_TEST_CASE(dct_watermark)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(*pframe);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 64;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  DCTWatermark watermark(0.3, 0.1, skipCount, reference, DCTWatermark::LEGACY_CC);
  watermark.embed(pframeTrue, true);
  watermark.embed(pframeFalse, false);

  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
  BOOST_CHECK_EQUAL(watermark.detect(pframeFalse), Watermark::DetectResult::FALSE);
  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);
}

BOOST_AUTO_TEST_CASE(dct_watermark_crop)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg");
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(*pframe);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 64;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  DCTWatermark watermark(0.3, 0.1, skipCount, reference, DCTWatermark::LEGACY_CC);
  watermark.embed(pframeTrue, true);
  watermark.embed(pframeFalse, false);

  std::string fileName = getSourceDir(__FILE__) + "out/dct_watermark_crop_with_watermark_true.png";
  pframeTrue->save(fileName);

  cv::Mat image;
  image = cv::imread(fileName, cv::IMREAD_COLOR);
  cv::Rect cropRegion(128, 48, 300, 451);
  cv::Mat croppedImage = image(cropRegion);

  std::string fileNameCropped = getSourceDir(__FILE__) + "out/dct_watermark_crop_cropped_true.png";
  cv::imwrite(fileNameCropped, croppedImage);

  std::vector<int> referenceCropped = DCTWatermark::cropReference(reference, skipCount, 128, 48, 300, 451, pframe->width(), pframe->height());

  std::shared_ptr<VideoFrame> pframeCropped = std::make_shared<VideoFrame>(fileNameCropped);
  DCTWatermark watermarkCropped(0.3, 0.1, skipCount, referenceCropped, DCTWatermark::LEGACY_CC);

  BOOST_CHECK_EQUAL(watermarkCropped.detect(pframeCropped), Watermark::DetectResult::TRUE);


  fileName = getSourceDir(__FILE__) + "out/dct_watermark_crop_with_watermark_false.png";
  pframeFalse->save(fileName);

  image = cv::imread(fileName, cv::IMREAD_COLOR);
  cropRegion = cv::Rect(64, 128, 200, 150);
  croppedImage = image(cropRegion);

  fileNameCropped = getSourceDir(__FILE__) + "out/dct_watermark_crop_cropped_false.png";
  cv::imwrite(fileNameCropped, croppedImage);

  referenceCropped = DCTWatermark::cropReference(reference, skipCount, 64, 128, 200, 150, pframe->width(), pframe->height());

  pframeCropped = std::make_shared<VideoFrame>(fileNameCropped);
  DCTWatermark watermarkCroppedFalse(0.3, 0.1, skipCount, referenceCropped, DCTWatermark::LEGACY_CC);

  BOOST_CHECK_EQUAL(watermarkCroppedFalse.detect(pframeCropped), Watermark::DetectResult::FALSE);
}

BOOST_AUTO_TEST_CASE(dct_watermark_crop_gray)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 16;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  DCTWatermark watermark(0.5, 0.1, skipCount, reference);
  watermark.embed(pframeTrue, true);

  std::string fileName = getSourceDir(__FILE__) + "out/dct_watermark_crop_gray_true.png";
  pframeTrue->save(fileName);

  cv::Mat image;
  image = cv::imread(fileName, cv::IMREAD_COLOR);
  cv::Rect cropRegion(256, 256, 128, 128);
  cv::Mat croppedImage = image(cropRegion);

  std::string fileNameCropped = getSourceDir(__FILE__) + "out/dct_watermark_crop_gray_cropped_true.png";
  cv::imwrite(fileNameCropped, croppedImage);

  std::vector<int> referenceCropped = DCTWatermark::cropReference(reference, skipCount, 256, 256, 128, 128, pframe->width(), pframe->height());

  std::shared_ptr<VideoFrame> pframeCropped = std::make_shared<VideoFrame>(fileNameCropped, VideoFrame::Grayscale);
  DCTWatermark watermarkCropped(0.3, 0.1, skipCount, referenceCropped, DCTWatermark::LEGACY_CC);

  BOOST_CHECK_EQUAL(watermarkCropped.detect(pframeCropped), Watermark::DetectResult::TRUE);
}

BOOST_AUTO_TEST_CASE(dct_watermark_get_avg)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 8;
  int maxValue = 255;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  DCTWatermark watermark(0.003125, 0.1, skipCount, reference, DCTWatermark::BARNI);
  watermark.embed(pframeTrue, true);

  double avgDctTrue = 0;
  watermark.getAverageDCT(pframeTrue, avgDctTrue);
  BOOST_CHECK(avgDctTrue > 10.5 && avgDctTrue < 11);
}

BOOST_AUTO_TEST_CASE(dct_watermark_normalized)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(*pframe);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 64;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  DCTWatermark watermark(0.5, 0.000002, skipCount, reference, DCTWatermark::DEFAULT, Detector::NORMALIZED);
  watermark.embed(pframeTrue, true);
  watermark.embed(pframeFalse, false);

  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);

  std::vector<int> reference2 = DCTWatermark::createReference(pframe, maxValue, skipCount);
  DCTWatermark watermark2(0.3, 0.01, skipCount, reference2, DCTWatermark::DEFAULT, Detector::NORMALIZED);
  std::shared_ptr<VideoFrame> pframeReference2 = std::make_shared<VideoFrame>(*pframe);
  watermark2.embed(pframeReference2, true);
  BOOST_CHECK_EQUAL(watermark.detect(pframeReference2), Watermark::DetectResult::NO_WATERMARK);

}

BOOST_AUTO_TEST_CASE(dct_watermark_cc)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(*pframe);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 64;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  DCTWatermark watermark(0.3, 100, skipCount, reference, DCTWatermark::DEFAULT, Detector::CORRELATION_COEFFICIENT);
  watermark.embed(pframeTrue, true);
  watermark.embed(pframeFalse, false);

  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
  BOOST_CHECK_EQUAL(watermark.detect(pframeFalse), Watermark::DetectResult::FALSE);
  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);
}

BOOST_AUTO_TEST_CASE(dct_watermark_pearson)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(*pframe);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 64;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount);

  DCTWatermark watermark(0.3, 0.1, skipCount, reference, DCTWatermark::DEFAULT, Detector::PEARSON_COEFFICIENT);
  watermark.embed(pframeTrue, true);
  watermark.embed(pframeFalse, false);

  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
  BOOST_CHECK_EQUAL(watermark.detect(pframeFalse), Watermark::DetectResult::FALSE);
  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);
}

BOOST_AUTO_TEST_CASE(dct_watermark_middle)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);
  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);
  std::shared_ptr<VideoFrame> pframeFalse = std::make_shared<VideoFrame>(*pframe);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  int skipCount = 16;
  int maxValue = 64;
  std::vector<int> reference = DCTWatermark::createReference(pframe, maxValue, skipCount, false, DCTWatermark::MIDDLE_FREQUENCY);

  DCTWatermark watermark(0.3, 100, skipCount, reference, DCTWatermark::DEFAULT, Detector::CORRELATION_COEFFICIENT, DCTWatermark::MIDDLE_FREQUENCY);
  watermark.embed(pframeTrue, true);
  watermark.embed(pframeFalse, false);

  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
  BOOST_CHECK_EQUAL(watermark.detect(pframeFalse), Watermark::DetectResult::FALSE);
  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);
}

BOOST_AUTO_TEST_SUITE_END();
