#include <iostream>
#include <string>

#include "./manager.h"

int main(int argc, char** argv) {
  std::string ethernet_name = "enp6s0";

  int slave_number = 3;
  int sdo_data_index = 6000;
  std::vector<double> ft_buffer;
  ft_buffer.resize(6);

  elfin_ethercat_driver_v2::EtherCatManager ethercat_manager(ethernet_name);

  std::cout << "Start publishing wrench messages" << std::endl;

  std::map<std::string, int> name_index_map{{"Fx", 1}, {"Fy", 2}, {"Fz", 3},
                                            {"Tx", 4}, {"Ty", 5}, {"Tz", 6}};

  while (1) {
    std::cout << "\rDATA: ";
    for (const auto& name_index : name_index_map) {
      const int index = name_index.second;
      const std::string& name = name_index.first;
      int32_t FT = ethercat_manager.readSDO<int32_t>(slave_number,
                                                     sdo_data_index, index);
      std::cout << name << "=" << FT / 10000.0 << " , ";
    }
    usleep(0.1 * 1e6);
  }

  return 0;
}
