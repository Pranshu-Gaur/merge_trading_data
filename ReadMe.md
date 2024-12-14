# Pranshu Gaur - BestEx Research Trading Assignment

## Overview

This project merges multiple sorted data files into a single merged output. Initially, the `data` directory is empty. If data is already available, just run the merge. A unit test executable can also run a full end-to-end test including generating dummy_data, running the merge and producing analytics for the unit test.

## File Structure:
- **Makefile**: Instructions to build the main merge executable (`market_data_merger`) and a unit test executable (`unit_test`)
- **src/main.cpp**: Main entry point for the merging logic
- **src/DataMerger.cpp/h**: Implements the merging functionality using a priority queue
- **src/DataLine.h**: Header file for data line parsing
- **unit_test.cpp**: Generates dummy data, runs merge, and outputs analytics report

## Makefile Insructions
Steps to run the Makefile and executables:

1. To build the main merge executable [if data is available]
    - In a powershell/cmd terminal, run ```make```<br>
    This poduces the `market_data_merger` executable
    - Execute the merge, ```.\market_data_merger.exe```
<br><br>
2. To run a unit test [if data is not available]
    - In a powershell/cmd terminal, run ```make test```<br> This poduces the `unit_test` executable
    - Execute the unit test, ```.\unit_test.exe```
    - This will automatically create dummy data in `data` directory, run the merge internally, and produce `analytics.txt` containing performance and output details.
    * **Note**: You can set the number of files (`numFiles`) and lines per file (`linesPerFile`) for generating the dummy data in unit_test.cpp before producing the executable.
<br><br>
3. To clean up:
    - Run ```make clean```<br> This removes executables and resets the environment for a fresh build.
