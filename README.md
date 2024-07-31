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

## Features

- File Reading: Reads the firmware file in chunks.
- Data Transfer Simulation: Uses a placeholder function to simulate sending data to the device.
- Progress Indication: Displays the progress percentage of the firmware update process.
- Checksum Calculation: Ensures data integrity through checksum calculation.
