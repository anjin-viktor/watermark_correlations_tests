#include "VideoFrame.h"
#include "EBlindDLC.h"


#include <boost/program_options.hpp>

#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

int main(int argc, char** argv)
{
	namespace po = boost::program_options;

	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("gen_reference", "generate reference pattern (call with reference_max and output file as arguments)")
		("embed", "embed watermark reference pattern (call with alpha and input, reference and output files as arguments)")
		("detect", "detect watermark reference pattern (call with threshold and input and reference files as arguments)")
		("reference_max", po::value<int>()->default_value(50), "reference pattern maximum value (from 0 to 255)")
		("alpha", po::value<double>()->default_value(1.0), "reference pattern embedding multiplier")
		("threshold", po::value<double>()->default_value(0.01), "detector threshold value")
		("value,v", po::value<bool>()->default_value(true), "embedded value")
		("in,i", po::value<std::string>(), "input file")
		("out,o", po::value<std::string>(), "output file")
		("reference,r", po::value<std::string>(), "reference file")
		("get_correlation", "extract collelation beetwen input and reference (call input and reference files as arguments)")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);

	if (vm.count("gen_reference"))
	{
		if (!vm.count("in") || !vm.count("out"))
		{
			std::cout << "Arguments `in` and `out` are expected. Use help for additional info";
			return 1;
		}
		std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		srand(ms.count());

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		auto preference = EBlindDLC::createReference(pframe, vm["reference_max"].as<int>());
		preference->save(vm["out"].as<std::string>());
	}
	else if (vm.count("embed"))
	{
		if (!vm.count("in") || !vm.count("out") || !vm.count("reference"))
		{
			std::cout << "Arguments `in`, `out` and `reference` are expected. Use help for additional info";
			return 1;
		}

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		std::shared_ptr<VideoFrame> preference = std::make_shared<VideoFrame>(vm["reference"].as<std::string>());
		if (!preference)
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		EBlindDLC watermark(vm["alpha"].as<double>(), -1, preference);

		watermark.embed(pframe, vm["value"].as<bool>());
		pframe->save(vm["out"].as<std::string>());
	}
	else if (vm.count("detect"))
	{
		if (!vm.count("in") || !vm.count("reference"))
		{
			std::cout << "Arguments `in`, `out` and `reference` are expected. Use help for additional info";
			return 1;
		}

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		std::shared_ptr<VideoFrame> preference = std::make_shared<VideoFrame>(vm["reference"].as<std::string>());
		if (!preference)
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		EBlindDLC watermark(-1, vm["threshold"].as<double>(), preference);
		Watermark::DetectResult res = watermark.detect(pframe);

		if (res == Watermark::DetectResult::TRUE)
			std::cout << "TRUE" << std::endl;
		else if (res == Watermark::DetectResult::FALSE)
			std::cout << "FALSE" << std::endl;
		else
			std::cout << "NOT DETECTED" << std::endl;
	}
	else if (vm.count("get_correlation"))
	{
		if (!vm.count("in") || !vm.count("reference"))
		{
			std::cout << "Arguments `in`, `out` and `reference` are expected. Use help for additional info";
			return 1;
		}

		std::shared_ptr<VideoFrame> pframe = std::make_shared<VideoFrame>(vm["in"].as<std::string>());
		if (!pframe)
		{
			std::cout << "Error opening input file `" + vm["in"].as<std::string>() + "`";
			return 1;
		}

		std::shared_ptr<VideoFrame> preference = std::make_shared<VideoFrame>(vm["reference"].as<std::string>());
		if (!preference)
		{
			std::cout << "Error opening reference file `" + vm["reference"].as<std::string>() + "`";
			return 1;
		}

		EBlindDLC watermark(-1, vm["threshold"].as<double>(), preference);
		double correlation = -1;
		watermark.getCorrelation(pframe, correlation);
		std::cout << std::setprecision(10) << correlation << std::endl;
	}
	else
	{
		std::cout << desc << "\n";
	}

	return 0;
}
