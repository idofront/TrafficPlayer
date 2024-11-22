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
        CLI::App app{"TrafficPlayer"};

        // General options
        std::string pcap_file;
        app.add_option("--pcap,-p", pcap_file, "Path to the pcap file")->required();

        std::string network_interface;
        app.add_option("--interface,-i", network_interface, "Network interface to send packets")->required();

        std::string log_level = "info";
        app.add_option("--log-level", log_level, "Log level (trace, debug, info, warn, error, critical)");

        auto reportIntervalSec = (double)0.0;
        app.add_option("--report-interval", reportIntervalSec, "Interval to show reports in seconds")->default_val(1.0);

        app.add_option("--repeat", _RepeatCount, "Number of times to repeat the traffic. 0 means infinite repeat")
            ->default_val(1);

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
            _Mode = ::Mode::Throughput;
            _ThroughputMbps = throughput_mbps;

            if (throughput_mbps <= 0)
            {
                throw std::runtime_error("Throughput must be greater than 0");
            }
        }
        else if (speedScale->parsed())
        {
            spdlog::info("SpeedScale: factor {}", speedScaleFactor);
            _Mode = ::Mode::SpeedScale;
            _SpeedScaleFactor = speedScaleFactor;
        }
        else if (duration->parsed())
        {
            spdlog::info("Duration: {} seconds", duration_time);
            _Mode = ::Mode::Duration;
            _DurationTime = duration_time;
        }
        else
        {
            throw std::runtime_error("No mode specified");
        }

        _InterfaceName = network_interface;
        _PcapFilePath = pcap_file;
        _LogLevel = spdlog::level::from_str(log_level);
        _ReportIntervalUsec = std::chrono::milliseconds(static_cast<long long>(reportIntervalSec * 1e3));
    }

    /// @brief Network interface name
    const std::string InterfaceName() const
    {
        return _InterfaceName;
    }

    /// @brief Pcap file path
    const std::filesystem::path PcapFilePath() const
    {
        return _PcapFilePath;
    }

    /// @brief Mode
    const ::Mode Mode() const
    {
        return _Mode;
    }

    /// @brief Throughput in Mbps
    const double ThroughputMbps() const
    {
        return _ThroughputMbps;
    }

    /// @brief Speed scale factor
    const double SpeedScaleFactor() const
    {
        return _SpeedScaleFactor;
    }

    /// @brief Duration time in seconds
    const double DurationTime() const
    {
        return _DurationTime;
    }

    /// @brief Log level
    const spdlog::level::level_enum LogLevel() const
    {
        return _LogLevel;
    }

    /// @brief Interval to show reports in microseconds
    const std::chrono::milliseconds ReportIntervalUsec() const
    {
        return _ReportIntervalUsec;
    }

    /// @brief Number of times to repeat the traffic
    const uint64_t RepeatCount() const
    {
        return _RepeatCount;
    }

  private:
    ::Mode _Mode;
    std::string _InterfaceName;
    std::filesystem::path _PcapFilePath;
    double _ThroughputMbps;
    double _SpeedScaleFactor;
    double _DurationTime;
    spdlog::level::level_enum _LogLevel;
    std::chrono::milliseconds _ReportIntervalUsec;
    uint64_t _RepeatCount;
};

#endif
