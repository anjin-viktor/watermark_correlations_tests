#include <boost/test/unit_test.hpp>

#include <memory>

#include "Utils.h"
#include "VideoFrame.h"
#include "EPercShape.h"

BOOST_AUTO_TEST_SUITE(e_perc_shape);

BOOST_AUTO_TEST_CASE(epercshape_embed_detect_grayscale)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg", VideoFrame::Grayscale);

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);

  std::shared_ptr<VideoFrame> preference = EPercShape::createReference(pframe, 0.25);

  EPercShape watermark(0.5, pframe, preference);
  watermark.embed(pframeTrue, true);

//  preference->save(getSourceDir(__FILE__) + "out/reference.png");
//  pframeTrue->save(getSourceDir(__FILE__) + "out/frameTrue.png");

  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);
  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
}

BOOST_AUTO_TEST_CASE(epercshape_embed_detect_color)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg");

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);

  std::shared_ptr<VideoFrame> preference = EPercShape::createReference(pframe, 0.25);

  EPercShape watermark(0.5, pframe, preference);
  watermark.embed(pframeTrue, true);

//  preference->save(getSourceDir(__FILE__) + "out/reference.png");
//  pframeTrue->save(getSourceDir(__FILE__) + "out/frameTrue.png");

  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);
  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
}


BOOST_AUTO_TEST_CASE(epercshape_field_sd_gray)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/field-sd-gray.jpg");

  BOOST_CHECK_EQUAL(pframe->width(), 640);
  BOOST_CHECK_EQUAL(pframe->height(), 480);

  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);

  std::shared_ptr<VideoFrame> preference = EPercShape::createReference(pframe, 0.5);

  EPercShape watermark(0.5, pframe, preference);
  watermark.embed(pframeTrue, true);

//  preference->save(getSourceDir(__FILE__) + "out/reference.png");
//  pframeTrue->save(getSourceDir(__FILE__) + "out/frameTrue.png");

  BOOST_CHECK_EQUAL(watermark.detect(pframe), Watermark::DetectResult::NO_WATERMARK);
  BOOST_CHECK_EQUAL(watermark.detect(pframeTrue), Watermark::DetectResult::TRUE);
}

BOOST_AUTO_TEST_CASE(epercshape_correlation_color)
{
  std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(getSourceDir(__FILE__) + "images/sea_640.jpg");

  BOOST_CHECK_EQUAL(pframe->width(), 480);
  BOOST_CHECK_EQUAL(pframe->height(), 640);

  std::shared_ptr<VideoFrame> pframeTrue = std::make_shared<VideoFrame>(*pframe);

  std::shared_ptr<VideoFrame> preference = EPercShape::createReference(pframe, 0.25);

  EPercShape watermark(0.5, pframe, preference);
  watermark.embed(pframeTrue, true);

  double correlationEmpty = -1;
  double correlationWatermark = -1;
  watermark.getCorrelation(pframe, correlationEmpty);
  watermark.getCorrelation(pframeTrue, correlationWatermark);

  BOOST_CHECK(correlationEmpty < 0.001);
  BOOST_CHECK(correlationWatermark > 1 - 0.001);

}


BOOST_AUTO_TEST_SUITE_END();
