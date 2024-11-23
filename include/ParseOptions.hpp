#ifndef PARSE_OPTIONS_HPP
#define PARSE_OPTIONS_HPP

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <spdlog/spdlog.h>

enum Mode
{
    Throughput,
    Interval,
    Duration
};

class ParseOptions
{
  public:
    ParseOptions(int argc, char *argv[])
    {
        CLI::App app{"PCAP Replay Tool"};

        // General options
        std::string pcap_file;
        app.add_option("--pcap,-p", pcap_file, "Path to the pcap file")->required();

        std::string network_interface;
        app.add_option("--interface,-i", network_interface, "Network interface to send packets")->required();

        std::string log_level = "info";
        app.add_option("--log-level", log_level, "Log level (trace, debug, info, warn, error, critical)");

        // Subcommands for different modes
        auto throughput = app.add_subcommand("throughput", "Throughput mode: Replay at a specified throughput");
        double throughput_mbps;
        throughput->add_option("Mbps", throughput_mbps, "Throughput value in Mbps")->required();

        auto interval = app.add_subcommand("interval", "Interval mode: Adjust packet interval by a factor");
        double interval_factor;
        interval->add_option("factor", interval_factor, "Interval factor multiplier")->required();

        auto duration = app.add_subcommand("duration", "Duration mode: Replay all packets within a specified duration");
        double duration_time;
        duration->add_option("time", duration_time, "Duration time in seconds")->required();

        // Parse command line
        try
        {
            app.parse(argc, argv);
        }
        catch (const CLI::ParseError &e)
        {
            app.exit(e);
        }

        // If help flag was specified, display help
        if (app.get_help_ptr() && app.get_help_ptr()->count())
        {
            std::cout << app.help() << "\n";
            throw std::runtime_error("Help requested");
        }

        // Handle subcommands
        if (throughput->parsed())
        {
            spdlog::info("Throughput: {} Mbps", throughput_mbps);
            _mode = ::Mode::Throughput;
            _throughputMbps = throughput_mbps;

            if (throughput_mbps <= 0)
            {
                throw std::runtime_error("Throughput must be greater than 0");
            }
        }
        else if (interval->parsed())
        {
            spdlog::info("Interval: factor {}", interval_factor);
            _mode = ::Mode::Interval;
            _intervalFactor = interval_factor;
        }
        else if (duration->parsed())
        {
            spdlog::info("Duration: {} seconds", duration_time);
            _mode = ::Mode::Duration;
            _durationTime = duration_time;
        }
        else
        {
            throw std::runtime_error("No mode specified");
        }

        _interfaceName = network_interface;
        _pcapFilePath = pcap_file;
        _logLevel = spdlog::level::from_str(log_level);
    }

    const std::string InterfaceName() const
    {
        return _interfaceName;
    }

    const std::filesystem::path PcapFilePath() const
    {
        return _pcapFilePath;
    }

    const ::Mode Mode() const
    {
        return _mode;
    }

    const double ThroughputMbps() const
    {
        return _throughputMbps;
    }

    const double IntervalFactor() const
    {
        return _intervalFactor;
    }

    const double DurationTime() const
    {
        return _durationTime;
    }

    const spdlog::level::level_enum LogLevel() const
    {
        return _logLevel;
    }

  private:
    ::Mode _mode;
    std::string _interfaceName;
    std::filesystem::path _pcapFilePath;
    double _throughputMbps;
    double _intervalFactor;
    double _durationTime;
    spdlog::level::level_enum _logLevel;
};

#endif
