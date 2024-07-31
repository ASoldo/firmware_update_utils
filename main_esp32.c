#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

#define BUFFERSIZE 1024
#define IMAGE_START 0x1000
#define RETRIES 3

unsigned char recv[BUFFERSIZE];
int addr, idx, size, i;
int cksumlen;
unsigned char send[6];
unsigned char buf[BUFFERSIZE];
FILE *file;

int open_port(const char *port);
void configure_port(int fd);
int send_data(int fd, unsigned char *data, int length);
int receive_data(int fd, unsigned char *buffer, int length);
int rcx_sendrecv(int fd, unsigned char *sendbuf, int sendlen, unsigned char *recvbuf, int recvlen, int timeout, int retries);
float timer_read(clock_t start_time);

void install_firmware(const char *progname, const char *filename) {
    printf("Uploading firmware using esptool.py...\n");
    
    char command[512];
    snprintf(command, sizeof(command), "esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 write_flash -z 0x10000 %s", filename);
    
    int ret = system(command);
    if (ret != 0) {
        fprintf(stderr, "%s: firmware upload failed\n", progname);
        exit(1);
    }

    printf("\nFirmware update completed successfully.\n");

    // Add a small delay to ensure everything is written properly
    usleep(100000);
    printf("Resetting the ESP32...\n");
    system("esptool.py --port /dev/ttyUSB0 --before default_reset run");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <program_name> <firmware_file>\n", argv[0]);
        return 1;
    }

    install_firmware(argv[1], argv[2]);
    return 0;
}

int open_port(const char *port) {
    int fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1) {
        perror("open_port: Unable to open port ");
    } else {
        fcntl(fd, F_SETFL, 0);
    }
    return fd;
}

void configure_port(int fd) {
    struct termios options;
    tcgetattr(fd, &options);
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    options.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(fd, TCSANOW, &options);
}

int send_data(int fd, unsigned char *data, int length) {
    int n = write(fd, data, length);
    if (n < 0) {
        perror("send_data: Failed to write to port ");
    }
    return n;
}

int receive_data(int fd, unsigned char *buffer, int length) {
    int n = read(fd, buffer, length);
    if (n < 0) {
        perror("receive_data: Failed to read from port ");
    }
    return n;
}

int rcx_sendrecv(int fd, unsigned char *sendbuf, int sendlen, unsigned char *recvbuf, int recvlen, int timeout, int retries) {
    int attempts = 0;
    int bytes_sent, bytes_received, total_bytes_received;

    while (attempts < retries) {
        printf("Sending data, attempt %d...\n", attempts + 1);
        bytes_sent = send_data(fd, sendbuf, sendlen);
        if (bytes_sent != sendlen) {
            perror("Failed to send data");
            attempts++;
            continue;
        }

        printf("Waiting for response...\n");
        usleep(timeout * 1000);

        total_bytes_received = 0;
        while (total_bytes_received < recvlen) {
            bytes_received = receive_data(fd, recvbuf + total_bytes_received, recvlen - total_bytes_received);
            if (bytes_received < 0) {
                perror("Failed to receive data");
                attempts++;
                break;
            }
            total_bytes_received += bytes_received;
        }

        if (total_bytes_received != recvlen) {
            fprintf(stderr, "Incomplete response: expected %d bytes, received %d bytes\n", recvlen, total_bytes_received);
            attempts++;
            continue;
        }

        printf("Data received successfully.\n");
        return total_bytes_received;
    }

    printf("Max retries reached. Data transfer failed.\n");
    return -1; // Indicate failure after retries
}

float timer_read(clock_t start_time) {
    // Calculate elapsed time in seconds
    return (float)(clock() - start_time) / CLOCKS_PER_SEC;
}
