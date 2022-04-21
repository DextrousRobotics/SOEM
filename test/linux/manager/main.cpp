/*
Created on Mon Dec 17 09:02:30 2018
@author: Cong Liu
 Software License Agreement (BSD License)
 Copyright (c) 2018, Han's Robot Co., Ltd.
 All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.
  * Neither the name of the copyright holders nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/
/*
From https://www.ati-ia.com/app_content/documents/9610-05-1032.pdf:
6.2.6.1 Converting Force/Torque Counts to Units
  The data in the register for this object is in counts. Therefore, the F/T counts
  must be converted to a value in units.

  To convert the SDO counts into units, complete the following steps:
    1. Read the Counts per Force SDO register (refer to Section 6.2.2—Object
      0x2040: Calibration subindex 0x31).
    2. Read the Counts per Torque SDO register (refer to Section 6.2.2—Object
      0x2040: Calibration subindex 0x32).
    3. Verify the units of force (refer to Section 6.2.2—Object 0x2040:
      Calibration subindex 0x29).
    4. Verify the units of torque (refer to Section 6.2.2—Object 0x2040:
      Calibration subindex 0x2a).
    5. Read the F/T counts for force (refer to Section 6.2.6—Object 0x6000:
      Reading Data).
    6. Read the F/T counts for torque (refer to Section 6.2.6—Object 0x6000:
      Reading Data).
    7. Convert the counts to units.
      a. For force, divide the register from step 5 by the register from step 1.
      b. For torque, divide the register from step 6 by the register from step 2.

  See 'test/linux/manager/slave_info_sdo.txt' for the values of the described
  registers.
*/
#include <iostream>
#include <string>

#include "./manager.h"

int32_t readInput_unit(elfin_ethercat_driver_v2::EtherCatManager *manager,
                       int slave_no, uint8_t channel) {
  uint8_t map[4];
  for (int i = 0; i < 4; i++) {
    map[i] = manager->readInput(slave_no, channel + i);
  }
  int32_t value_tmp = *(int32_t *)(map);
  return value_tmp;
}

int main(int argc, char **argv) {
  std::string ethernet_name = "enp6s0";

  int slave_number = 3;
  int sdo_data_index = 6000;
  std::vector<double> ft_buffer;
  ft_buffer.resize(6);

  elfin_ethercat_driver_v2::EtherCatManager ethercat_manager(ethernet_name);

  std::cout << "Start publishing wrench messages" << std::endl;

  std::map<std::string, int> name_index_map{
      {"Fx", 1}, {"Fy", 2}, {"Fz", 3}, {"Tx", 4}, {"Ty", 5}, {"Tz", 6},
  };

  while (1) {
    std::cout << "\rDATA: ";
    int i = 0;
    for (const auto &name_index : name_index_map) {
      const int index = name_index.second;
      const std::string &name = name_index.first;
      int32_t FT = readInput_unit(&ethercat_manager, slave_number, i++ * 4);

      printf("%s=%-5.9f, ", name.c_str(), FT / 100000.0);
    }
    usleep(0.01 * 1e6);
  }

  return 0;
}
