# VIHAL Support for ESP32 (Xtensa)

The ESP32 is a very strong and affordable microcontroller, with a quite special Xtensa LX16 CPU using Windowed ABI.
I've invested a lot of time to find the necessary low-level Xtensa CPU initialization codes and linker requirements.
Fortunately it seems to work now. 

Only exececution from RAM is supported so far. XIP is not planned by me (but is not so hard to implement).

__WARNING: Wifi/Bluetooth support is not included and probably they won't ever work with VIHAL__


## Eclipse CDT Project Setup for the ESP32 Vihal Development

### One-Time Eclipse CDT Preparation
- Acquire the Xtensa ESP32 GCC tools (actually the esp-idf is not required, but it is easier to have a complete
  esp-idf setup wicht at some point installs the necessary tools as well)
- Open Eclipse Preferences / C/C++ / Build / Build Variables: Add the following variable:
  XTENSA_ESP32_GCC_PATH to point to the Xtensa ESP32 GCC (with the .../bin directory at the end)
  
### Project Setup
Unfortunately there is no Xtrensa support by the Eclipse Embedded-CDT plugin, so the the (ESP32) setup takes slightly more
steps.
- Add a new build configuration like BOARD_ESP32_DEVKIT
- At the Tool Chain Editor select "Cross GCC"
- In C/C++ General
  - Paths and Symbols / Symbols: define the board for the "GNU C++" like BOARD_ESP32_DEVKIT (no value required)
  - Paths and Symbols / Includes / GNU C++: 
    - src
    - /${ProjName}/vihal/core/src
    - /${ProjName}/vihal/xtensa/core
    - /${ProjName}/vihal/xtensa/ESP/src
    - /${ProjName}/vihal/xtensa/ESP/ESP32
  - Paths and Symbols / Includes / Assembly: 
    - /${ProjName}/vihal/xtensa/ESP/ESP32
- Activate (include into the build) the following directories from the vihal:
  - vihal/core/src (this must be always there)
  - vihal/xtensa/core
  - vihal/xtensa/ESP/src



