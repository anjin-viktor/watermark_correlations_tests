#ifndef VIDEO_FRAME_H_
#define VIDEO_FRAME_H_

#include <cstddef>
#include <vector>
#include <string>
#include <memory>

#include "ThreadPool.h"

class VideoFrame
{
public:
  enum Optimization
  {
    Auto,
    C, 
    SSE,
    AVX
  };

  enum ThreadingType
  {
    Rows,
    Collumns
  };

  enum ColorFormat
  {
    Color, //bgr
    Grayscale
  };


  VideoFrame(std::size_t width = 0, std::size_t height = 0, ColorFormat colorFormat = ColorFormat::Color);
  VideoFrame(const std::string& fileName, ColorFormat colorFormat = ColorFormat::Color);
  VideoFrame(const VideoFrame&);

  void save(const std::string& fileName);

  bool applyWR(std::shared_ptr<VideoFrame> preference, double alpha, bool key, Optimization optimization = Auto);
  bool applyWR(std::shared_ptr<VideoFrame> preference, double alpha, bool key, ThreadPool &threadPool, Optimization optimization = Auto, ThreadingType threading  = Rows);
  bool applyWR(std::vector<double> &reference, double alpha, bool key);

  std::size_t width() const;
  std::size_t height() const;
  std::size_t stride(int plane) const;
  uint8_t* data(int plane);
  ColorFormat colorFormat() const;

  std::vector<float> fDCT();
  static VideoFrame iDCT(std::vector<float> dctData, std::size_t width, std::size_t height);
  std::shared_ptr<VideoFrame> sub(std::shared_ptr<VideoFrame> pframeExtracted);
  std::shared_ptr<VideoFrame> add(std::shared_ptr<VideoFrame> pframeAdd);

  void DCTSharpening(float threshold, int referenceMax);

private:
  std::size_t                       m_width;
  std::size_t                       m_height;
  std::vector<std::size_t>          m_strides;
  std::vector<std::vector<uint8_t>> m_data;
  ColorFormat                       m_colorFormat;
};


#endif
