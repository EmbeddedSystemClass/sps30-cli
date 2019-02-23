


#include "sensirion_uart.h"
#include "sensirion_uart_port.h"
#include "sps30.h"

#include "CLI/CLI.hpp"

#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>


int main(int argc, char** argv) {

  CLI::App app("SPS30 client");


  std::string serial_port;
  app.add_option("uart_port", serial_port, std::string("path to the device file, e.g. ") + sensirion_uart_get_dev(), true)->required()->check(CLI::ExistingPath);

  bool opt_serial;
  app.add_flag("-s,--serial", opt_serial, "read the device serial number");

  uint32_t opt_autoclean;
  app.add_flag("-a,--autoclean", opt_autoclean, "read or set the autoclean interval");


  CLI11_PARSE(app, argc, argv);

  sensirion_uart_set_dev(serial_port.c_str());


  uint32_t retries = 3;
  while (sensirion_uart_open() != 0 && retries-- > 0) {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
  if (retries == 0) {
    std::cerr << "UART initialisation failed, device: " << serial_port << "\n";
    return EXIT_FAILURE;
  }
  std::cerr << "UART initialisation ok, device: " << serial_port << "\n";

  retries = 3;
  while (sps30_probe() != 0 && retries-- > 0) {
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
  if (retries == 0) {
    std::cerr << "SPS sensor probing failed\n";
    return EXIT_FAILURE;
  }
  std::cerr << "SPS sensor probing ok\n";


  
  char serial[SPS_MAX_SERIAL_LEN];

  struct sps30_measurement m;
  uint16_t ret = 1;
  retries = 3;
  while (ret != 0 && retries-- > 0) {
    ret = sps30_start_measurement();
    std::this_thread::sleep_for (std::chrono::seconds(1));
  }
  if (retries == 0) {
    std::cerr << "Error starting measurements\n";
    return EXIT_FAILURE;
  }
  if (SPS_IS_ERR_STATE(ret)) {
    std::cout << "SPS chip state: " << SPS_GET_ERR_STATE(ret) << " - measurements may not be accurate\n";
  }

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

  if (sensirion_uart_close() != 0) {
    std::cout << "Closing UART failed\n";
    return EXIT_FAILURE;
  }
  
  return 0;
}
