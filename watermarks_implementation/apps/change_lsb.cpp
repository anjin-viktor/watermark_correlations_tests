#include "VideoFrame.h"
#include "WatermarkReference.h"
#include "Detector.h"

#include <boost/program_options.hpp>

#include <iostream>


int main(int argc, char** argv)
{
	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("in,i", po::value<std::string>(), "input file")
		("out,o", po::value<std::string>(), "output file")
		("strength,s", po::value<double>()->default_value(0.25), "percentage of pixels to change (from 0 to 1)")
		("value,v", po::value<int>()->default_value(0), "value used in changing(0 - set all values to 0, 1 - set all values to 1, 2 use random value for every pixel)")
		("mask,m", po::value<int>()->default_value(1), "mask of bits for spicifying changed values of pixels (1 - one last bit, 3(11b) - last 2 bits, etc)")
		("grayscale", "use grayscale mode")

		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);

	if (!vm.count("in") || !vm.count("out") || vm.count("help"))
	{
		std::cout << desc << "\n";
		return 0;
	}

	int mask = vm["mask"].as<int>();
	if (mask <= 0 || mask > 255)
	{
		std::cout << "Wrong `mask` value'\n";
		std::cout << desc << "\n";
		return 0;
	}

	int value = vm["value"].as<int>();
	if (value < 0 || value > 2)
	{
		std::cout << "Wrong value of `value` argument'\n";
		std::cout << desc << "\n";
		return 0;
	}

	std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>(), vm.count("grayscale") ? VideoFrame::ColorFormat::Grayscale : VideoFrame::ColorFormat::Color);

	std::size_t valuesCnt = pframe->width() * pframe->height() * pframe->colorFormat() == VideoFrame::ColorFormat::Color ? 3 : 1;

	uint8_t *pdata = pframe->data(0);
	std::size_t stride = pframe->stride(0);
	std::size_t colorComponents = pframe->colorFormat() == VideoFrame::ColorFormat::Color ? 3 : 1;

	for (std::size_t i = 0; i < pframe->height(); i++)
	{
		for (std::size_t j = 0; j < pframe->width(); j++)
		{
			for (std::size_t k = 0; k < colorComponents; k++)
			{
				bool changeValue = (rand() / RAND_MAX) <= vm["strength"].as<double>();
				if (changeValue)
				{
					std::size_t valueIdx = i * stride + j * colorComponents + k;
					uint8_t value = pdata[valueIdx];

					if (vm["value"].as<int>() == 0)
					{
						value &= ~mask;
					}
					else if (vm["value"].as<int>() == 1)
					{
						value |= mask;
					}
					else
					{
						value &= ~mask;
						value |= rand() & mask;
					}

					pdata[valueIdx] = value;
				}
			}
		}
	}

	pframe->save(vm["out"].as<std::string>());

	return 0;
}
