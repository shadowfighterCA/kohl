# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/advait/esp/esp-idf/components/bootloader/subproject"
  "/home/advait/Downloads/esp-idf-master/examples/bluetooth/esp_ble_mesh/ble_mesh_node/HSL_SERVER/build/bootloader"
  "/home/advait/Downloads/esp-idf-master/examples/bluetooth/esp_ble_mesh/ble_mesh_node/HSL_SERVER/build/bootloader-prefix"
  "/home/advait/Downloads/esp-idf-master/examples/bluetooth/esp_ble_mesh/ble_mesh_node/HSL_SERVER/build/bootloader-prefix/tmp"
  "/home/advait/Downloads/esp-idf-master/examples/bluetooth/esp_ble_mesh/ble_mesh_node/HSL_SERVER/build/bootloader-prefix/src/bootloader-stamp"
  "/home/advait/Downloads/esp-idf-master/examples/bluetooth/esp_ble_mesh/ble_mesh_node/HSL_SERVER/build/bootloader-prefix/src"
  "/home/advait/Downloads/esp-idf-master/examples/bluetooth/esp_ble_mesh/ble_mesh_node/HSL_SERVER/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/advait/Downloads/esp-idf-master/examples/bluetooth/esp_ble_mesh/ble_mesh_node/HSL_SERVER/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
