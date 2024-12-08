#ifndef PARSE_OPTIONS_HPP
#define PARSE_OPTIONS_HPP

#include <CLI/CLI.hpp>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <spdlog/common.h>

enum Mode
{
    Throughput,
    SpeedScale,
    Duration,
    PacketsPerSecond
};

class ParseOptions
{
  public:
    ParseOptions(int argc, char *argv[])
    {
        CLI::App app{"TrafficPlayer"};

        // General options
        app.add_option("--pcap,-p", _PcapFilePath, "Path to the pcap file.")->required();
        app.add_option("--interface,-i", _InterfaceName, "Network interface to send packets.")->default_val("");
        app.add_option("--log-level", _LogLevelString, "Log level")
            ->check(CLI::IsMember({"trace", "debug", "info", "warn", "error", "critical", "off"}))
            ->default_val("info");
        app.add_option("--report-interval", _ReportIntervalSec, "Interval to show reports in seconds.")
            ->default_val(1.0);
        app.add_option("--repeat", _RepeatCount, "Number of times to repeat the traffic. 0 means infinite repeat.")
            ->default_val(1);
        app.add_flag("--dry-run", _DryRun,
                     "Dry run mode: Do not send packets, just show reports. No network interface "
                     "is required. No root permission is required.")
            ->default_val(false);

        // Subcommands for different modes
        auto throughput = app.add_subcommand("throughput", "Throughput mode: Replay at a specified throughput");
        throughput->add_option("Mbps", _ThroughputMbps, "Throughput value in Mbps")->default_val(0);

        auto speedScale = app.add_subcommand("scale", "Speed scale mode: Adjust replay speed by a factor");
        speedScale->add_option("factor", _SpeedScaleFactor, "Speed scale factor multiplier")->default_val(1);

        auto duration =
            app.add_subcommand("duration", "Custom duration mode: Replay all packets within a specified duration");
        duration->add_option("time", _DurationTime, "Duration time in seconds")->default_val(0);

        auto packetsPerSecond = app.add_subcommand("pps", "Packets per second mode: Replay at a specified rate");
        packetsPerSecond->add_option("pps", _PacketsPerSecond, "Packets per second value")->default_val(1);

        // Parse command line
        try
        {
            app.parse(argc, argv);
        }
        catch (const CLI::ParseError &e)
        {
            app.exit(e);
            throw std::runtime_error(e.what());
        }

        // If help flag was specified, display help
        if (app.get_help_ptr() && app.get_help_ptr()->count())
        {
            std::cout << app.help() << "\n";
            throw std::runtime_error("Help requested");
        }

        // Handle general options
        HandleGeneralOptions();

        // Handle subcommands
        auto modeSelecters = std::vector<std::tuple<CLI::App *, ::Mode, std::function<void()>>>{
            {throughput, ::Mode::Throughput, std::bind(&ParseOptions::HandleThroughputSubcommand, this)},
            {speedScale, ::Mode::SpeedScale, std::bind(&ParseOptions::HandleSpeesdScaleSubcommand, this)},
            {duration, ::Mode::Duration, std::bind(&ParseOptions::HandleDurationSubcommand, this)},
            {packetsPerSecond, ::Mode::PacketsPerSecond,
             std::bind(&ParseOptions::HandlePacketsPerSecondSubcommand, this)}};
        auto enableModeSelecters = std::vector<std::tuple<CLI::App *, ::Mode, std::function<void()>>>();
        std::copy_if(modeSelecters.begin(), modeSelecters.end(), std::back_inserter(enableModeSelecters),
                     [](std::tuple<CLI::App *, ::Mode, std::function<void()>> &modeSelecter) {
                         return std::get<0>(modeSelecter)->parsed();
                     });
        if (enableModeSelecters.size() != 1)
        {
            throw std::runtime_error("Exactly one subcommand must be specified");
        }
        auto modeSelecter = enableModeSelecters[0];
        _Mode = std::get<1>(modeSelecter);
        auto subcommandHandler = std::get<2>(modeSelecter);
        subcommandHandler();
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

    /// @brief Packet per second
    const double PacketsPerSecond() const
    {
        return _PacketsPerSecond;
    }

    /// @brief Log level
    const spdlog::level::level_enum LogLevel() const
    {
        return spdlog::level::from_str(_LogLevelString);
    }

    /// @brief Interval to show reports in microseconds
    const std::chrono::nanoseconds ReportIntervalNsec() const
    {
        return std::chrono::nanoseconds(static_cast<long long>(_ReportIntervalSec * 1e9));
    }

    /// @brief Number of times to repeat the traffic
    const uint64_t RepeatCount() const
    {
        return _RepeatCount;
    }

    /// @brief Dry run mode
    const bool DryRun() const
    {
        return _DryRun;
    }

  private:
    ::Mode _Mode;
    std::string _InterfaceName;
    std::filesystem::path _PcapFilePath;
    double _ThroughputMbps;
    double _SpeedScaleFactor;
    double _DurationTime;
    double _PacketsPerSecond;
    std::string _LogLevelString;
    double _ReportIntervalSec;
    int64_t _RepeatCount;
    bool _DryRun;

    void HandleGeneralOptions()
    {
        if (_RepeatCount < 0)
        {
            throw std::runtime_error("Repeat count must be greater than or equal to 0");
        }

        if (_InterfaceName.empty() && !_DryRun)
        {
            throw std::runtime_error("--interface must be specified unless dry run mode is enabled.");
        }
    }

    void HandleThroughputSubcommand()
    {
        if (_ThroughputMbps <= 0)
        {
            throw std::runtime_error("Throughput must be greater than 0");
        }
    }
    void HandleSpeesdScaleSubcommand()
    {
        if (_SpeedScaleFactor <= 0)
        {
            throw std::runtime_error("Speed scale factor must be greater than 0");
        }
    }
    void HandleDurationSubcommand()
    {
        if (_DurationTime <= 0)
        {
            throw std::runtime_error("Duration time must be greater than 0");
        }
    }
    void HandlePacketsPerSecondSubcommand()
    {
        if (_PacketsPerSecond <= 0)
        {
            throw std::runtime_error("Packets per second must be greater than 0");
        }
    }
};

#endif
