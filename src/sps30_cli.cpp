

extern "C" {
#include "sensirion_uart.h"
#include "sensirion_uart_port.h"
#include "sps30.h"
}

#include "CLI/CLI.hpp"

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>


class SensirionUart {
public:
  SensirionUart(std::string s) : 
    dev(s)
  {
    sensirion_uart_set_dev(dev.c_str());
  }
  int16_t open() {
    return sensirion_uart_open();
  }
  ~SensirionUart() {
    if (sensirion_uart_close() != 0) {
      std::cerr << "Closing UART failed\n";
    }
  }
private:
  std::string const dev;
};


int main(int argc, char** argv) {
  int16_t ret = 1;

  CLI::App app("SPS30 client");

  std::string serial_port;
  app.add_option("uart_port", serial_port, std::string("path to the device file, e.g. ") + sensirion_uart_get_dev(), true)
    ->required()
    ->check(CLI::ExistingPath)
    ;

  bool read_serial;
  app.add_flag("-s,--serial", read_serial, "read the device serial number");

  bool read_autoclean;
  app.add_flag("-a,--read-autoclean", read_autoclean, "read the autoclean interval in seconds");
  uint32_t set_autoclean_seconds{3600*24*4};
  app.add_option("-A,--set-autoclean", set_autoclean_seconds, "set the autoclean interval in seconds");

  CLI11_PARSE(app, argc, argv);

  SensirionUart uart(serial_port);

  if (uart.open() != 0) {
    std::cerr << "UART initialisation failed, device: " << serial_port << "\n";
    return EXIT_FAILURE;
  }

  uint32_t retries = 3;
  while (sps30_probe() != 0 && retries-- > 0) {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
  if (retries == 0) {
    std::cerr << "SPS sensor probing failed\n";
    return EXIT_FAILURE;
  }

  if (read_serial) { 
    char serial[SPS_MAX_SERIAL_LEN];
    ret = sps30_get_serial(serial);
    if (ret) {
      std::cerr << "Error reading SPS serial\n";
      return EXIT_FAILURE;
    } else {
      std::cout << serial << "\n";
      return 0;
    }
  } else if (read_autoclean) {
    uint32_t auto_clean_interval;
    ret = sps30_get_fan_auto_cleaning_interval(&auto_clean_interval);
    if (ret) {
      std::cerr << "Error retrieving the auto-clean interval\n";
      return EXIT_FAILURE;
    } else {
      std::cout << auto_clean_interval << "\n";
      return 0;
    }
  } else if (app.count("-A") > 0) {
    ret = sps30_set_fan_auto_cleaning_interval(set_autoclean_seconds);
    if (ret) {
      std::cerr << "Error setting the auto-clean interval #" << ret;
      return EXIT_FAILURE;
    }
    return 0;
  }

  struct sps30_measurement m;
  ret = sps30_start_measurement();
  if (ret < 0) {
    std::cerr << "Error starting measurements\n";
    return EXIT_FAILURE;
  }

  retries = 3;
  do {
    ret = sps30_read_measurement(&m);
    if (ret >= 0) break;
    std::this_thread::sleep_for (std::chrono::seconds(5));
  } while (retries--);
  if (retries == 0) {
    std::cerr << "Error reading measurements\n";
    return EXIT_FAILURE;
  }

  if (SPS_IS_ERR_STATE(ret)) {
    std::cout << "SPS chip state: " << SPS_GET_ERR_STATE(ret) << " - measurements may not be accurate\n";
  }

  std::cout << "pm1.0,pm2.5,pm4.0,pm10.0,nc0.5,nc1.0,nc2.5,nc4.5,nc10.0,typical particle size\n";
  std::cout << std::fixed << std::setprecision(5);
  std::cout << m.mc_1p0 << ",";
  std::cout << m.mc_2p5 << ",";
  std::cout << m.mc_4p0 << ",";
  std::cout << m.mc_10p0 << ",";
  std::cout << m.nc_0p5 << ",";
  std::cout << m.nc_1p0 << ",";
  std::cout << m.nc_2p5 << ",";
  std::cout << m.nc_4p0 << ",";
  std::cout << m.nc_10p0 << ",";
  std::cout << m.typical_particle_size << "\n";

  return 0;
}
