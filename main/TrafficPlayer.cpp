#include <Dealer.hpp>
#include <Producer.hpp>
#include <ThreadSafeQueue.hpp>
#include <TrafficMaker/UniformTrafficMaker.hpp>
#include <TrafficPlayer.hpp>
#include <TrafficRecord.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <filesystem>
#include <iostream>
#include <pcap.h>
#include <spdlog/spdlog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

class ParseOptions
{
  public:
    ParseOptions(int argc, char *argv[])
    {
        auto desc = boost::program_options::options_description("Allowed options");

        try
        {
            desc.add_options()("help,h", "produce help message")(
                "pcap,p", boost::program_options::value<std::filesystem::path>(), "A pcap file for replaying.")(
                "interface,i", boost::program_options::value<std::string>(), "An interface for replaying.");

            boost::program_options::variables_map vm;
            boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
            boost::program_options::notify(vm);

            if (vm.count("help"))
            {
                throw std::runtime_error("A show option is specified.");
            }

            if (vm.count("pcap"))
            {
                auto value = vm["pcap"].as<std::filesystem::path>();
                _pcapFilePath = vm["pcap"].as<std::filesystem::path>();
            }
            else
            {
                throw std::runtime_error("A pcap file must be speficied");
            }

            if (vm.count("interface"))
            {
                auto value = vm["interface"].as<std::string>();
                _interfaceName = value;
            }
            else
            {
                throw std::runtime_error("An interface must be specified.");
            }
        }
        catch (const std::runtime_error &e)
        {
            desc.print(std::cout);
            throw;
        }
        catch (const std::exception &e)
        {
            SPDLOG_ERROR("Error: {}", e.what());
            throw std::runtime_error("Failed to parse options");
        }
        catch (...)
        {
            SPDLOG_ERROR("Unknown error!");
            throw std::runtime_error("Failed to parse options");
        }
    }

    const std::string InterfaceName() const
    {
        return _interfaceName;
    }

    const std::filesystem::path PcapFilePath() const
    {
        return _pcapFilePath;
    }

  private:
    std::string _interfaceName;
    std::filesystem::path _pcapFilePath;
};

int main(int argc, char *argv[])
{
    auto options = ParseOptions(argc, argv);

    auto interface = options.InterfaceName();
    auto pcapFile = options.PcapFilePath();

    auto queuePtr = std::make_shared<TrafficPlayer::ThreadSafeQueue<TrafficRecord>>();

    auto producer = Producer(queuePtr);
    auto consumer = Dealer(queuePtr, interface);

    std::thread consumer_thread(consumer);

    // Read pcap file
    auto TrafficMaker = TrafficMaker::UniformTrafficMaker(pcapFile, 0.001);
    auto trafficRecords = TrafficMaker.Make();

    std::for_each(trafficRecords.begin(), trafficRecords.end(),
                  [&producer](const TrafficRecord &trafficRecord) { producer.Produce(trafficRecord); });

    while (!queuePtr->empty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    consumer_thread.detach();

    return 0;
}
