![image](https://github.com/user-attachments/assets/f7f264fc-328f-47e3-89e3-d214dd60cb89)

# Firmware Update Utility

This project is a firmware update utility designed to simulate the process of updating firmware with progress indication. It reads a firmware file, calculates a checksum, and simulates the data transfer to the device.

## Features

- Reads firmware file in chunks
- Simulates data transfer
- Displays progress percentage
- Verifies data integrity through checksum calculation

## Prerequisites

- GCC (GNU Compiler Collection)

## Installation

### Step 1: Install GCC

Ensure that GCC is installed on your system. On Arch Linux, you can install it using:

```sh
sudo pacman -Syu gcc
```

## Step 2: Create the Firmware File

Create a dummy firmware file using the dd command:

```sh
dd if=/dev/urandom of=firmware_file.bin bs=1k count=64

```

## Compile the code using gcc:

```sh
gcc -o firmware_upudate_utils main.c
```

Step 4: Run the Program
Execute the compiled program with the program name and the firmware file as arguments:

```sh
./firmware_update_utils mark_1 firmware_file.bin

```

## ESP32 Support

The new functionality is contained in the main_esp32.c file.

## Instructions for ESP32

Compile the ESP32 Firmware Update Utility:

```sh

gcc -o fuu main_esp32.c
```

## Prepare the ESP32:

Connect your ESP32 to your computer via USB. Put the ESP32 into bootloader mode by holding down the BOOT button and pressing and releasing the RESET button.

Run the Firmware Update Utility for ESP32:

```sh
./fuu mark_1 firmware_file.bin

```

## Features

- File Reading: Reads the firmware file in chunks.
- Data Transfer Simulation: Uses a placeholder function to simulate sending data to the device.
- Progress Indication: Displays the progress percentage of the firmware update process.
- Checksum Calculation: Ensures data integrity through checksum calculation.
