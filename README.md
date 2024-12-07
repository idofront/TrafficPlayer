# TrafficPlayer

TrafficPlayer is an application designed to replay network traffic based on different strategies, providing flexibility in testing and analyzing network behaviors. This project supports various replay modes, including custom duration, speed scaling, and uniform throughput.

## Features

- **CustomDuration Replay**: Replay traffic for a specified duration, giving users control over timing.
- **SpeedScaled Replay**: Adjust the replay speed of captured traffic, scaling it up or down as needed.
- **UniformThroughput Replay**: Replay traffic with a constant throughput, maintaining uniform data rates.

## Requirements

- **C++ Compiler**: Requires a C++17 or newer compatible compiler.
- **CPU**: It is recommended to use a CPU with at least 3 threads for optimal performance.
- **CMake**: Used for managing the build process.

## Installation

1. Clone the repository:
   ```sh
   git clone https://github.com/idofront/TrafficPlayer
   cd TrafficPlayer
   ```
2. Create a build directory and configure the project:
   ```sh
   mkdir build && cd build
   cmake ..
   ```
3. Build the project:
   ```sh
   make
   ```

## Usage

After building the project, you can execute TrafficPlayer with different options:

```sh
./build/TrafficPlayer [OPTIONS] [SUBCOMMAND]
```

Options:

- `-h, --help`: Print this help message and exit.
- `-p, --pcap <path>`: Path to the pcap file (required).
- `-i, --interface <name>`: Network interface to send packets (required).
- `--log-level <level>`: Log level (trace, debug, info, warn, error, critical, off).
- `--report-interval <seconds>`: Interval to show reports in seconds (default: 1).
- `--repeat <times>`: Number of times to repeat the traffic. 0 means infinite repeat (default: 1).

Subcommands:

- `throughput`: Throughput mode - replay at a specified throughput.
- `scale`: Speed scale mode - adjust replay speed by a factor.
- `duration`: Custom duration mode - replay all packets within a specified duration.
- `pps`: Packets per second mode: Replay at a specified rate

## License

This project is licensed under the MIT License.
