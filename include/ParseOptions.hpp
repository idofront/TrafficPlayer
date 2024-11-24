#ifndef PARSE_OPTIONS_HPP
#define PARSE_OPTIONS_HPP

#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <spdlog/spdlog.h>

enum Mode
{
    Throughput,
    SpeedScale,
    Duration
};

class ParseOptions
{
  public:
    ParseOptions(int argc, char *argv[])
    {
        CLI::App app{"PCAP Replayer"};

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
        throughput->add_option("Mbps", throughput_mbps, "Throughput value in Mbps")->default_val(0);

        auto speedScale = app.add_subcommand("scale", "Speed scale mode: Adjust replay speed by a factor");
        double speedScaleFactor;
        speedScale->add_option("factor", speedScaleFactor, "Speed scale factor multiplier")->default_val(1);

        auto duration =
            app.add_subcommand("duration", "Custom duration mode: Replay all packets within a specified duration");
        double duration_time;
        duration->add_option("time", duration_time, "Duration time in seconds")->default_val(0);

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
        else if (speedScale->parsed())
        {
            spdlog::info("SpeedScale: factor {}", speedScaleFactor);
            _mode = ::Mode::SpeedScale;
            _SpeedScaleFactor = speedScaleFactor;
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

    const double SpeedScaleFactor() const
    {
        return _SpeedScaleFactor;
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
    double _SpeedScaleFactor;
    double _durationTime;
    spdlog::level::level_enum _logLevel;
};

#endif
