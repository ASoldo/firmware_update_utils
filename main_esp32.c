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
    int start = IMAGE_START;
    size_t len = 0;
    int timeout = 50; // Example timeout value in milliseconds

    printf("Opening port...\n");
    int fd = open_port("/dev/ttyUSB0"); // Adjust to your port
    if (fd == -1) {
        exit(1);
    }
    configure_port(fd);

    printf("Computing checksum...\n");
    // Compute image checksum
    cksumlen = (start - len < 0xcc00) ? len : 0xcc00;

    // Setup send buffer
    send[0] = 0x01; // example command
    send[1] = 0x02; // example data
    send[2] = 0x03; // example data
    send[3] = 0x04; // example data
    send[4] = 0x05; // example data
    send[5] = 0x75; // example data

    printf("Sending start firmware download command...\n");
    // Send command and check response
    if (rcx_sendrecv(fd, send, 6, recv, 2, 50, RETRIES) != 2) {
        fprintf(stderr, "%s: start firmware download failed\n", progname);
        exit(1); // Start firmware
    }

    printf("Transferring data...\n");
    addr = 0;
    idx = 0;

    unsigned char msg[BUFFERSIZE];
    int msglen = 0;
    int pos = 0;

    // Initialize starting address
    start = IMAGE_START;
    size_t count = 0;
    size_t total_size = 0;

    if ((file = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "%s: failed to open file %s\n", progname, filename);
        exit(1);
    }

    // Get total size of the file
    fseek(file, 0, SEEK_END);
    total_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    printf("Starting file transfer...\n");
    clock_t start_time = clock();

    while ((len = fread(buf, 1, BUFFERSIZE, file)) > 0) {
        count += len;

        // Print progress
        printf("\rProgress: %.2f%%", (count / (float)total_size) * 100);
        fflush(stdout);

        if (len == sizeof(msg) && !memcmp(buf, msg, sizeof(msg))) {
            printf("\nFirmware update completed successfully.\n");
            return; /* success */
        }
        while (timer_read(start_time) < (float)(timeout / 1000.0f)) {
            // Waiting
        }

        // Checksum calculation
        int sum = 0;
        for (pos = 0; pos < sizeof(buf); pos++) {
            sum += buf[pos];
        }

        // Send data
        if (rcx_sendrecv(fd, buf, len, recv, 2, 50, RETRIES) != 2) {
            fprintf(stderr, "%s: data transfer failed\n", progname);
            exit(1);
        }
    }

    printf("\nFirmware update completed successfully.\n");
    fclose(file);
    close(fd);

    // Example data array for fast download
    unsigned char fastdl_image[] = {
        121, 6, 0, 15, 107, 134, 238, 128, 121,
        238, 116, 94, 0, 59, 154, 11, 135, 121,
        127, 216, 114, 80, 254, 103, 62, 217, 24,
        104, 142, 159, 6, 254, 13, 106, 142, 233
    };
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
    if (send_data(fd, sendbuf, sendlen) != sendlen) {
        return -1;
    }
    usleep(timeout * 1000);
    if (receive_data(fd, recvbuf, recvlen) != recvlen) {
        return -1;
    }
    return recvlen;
}

float timer_read(clock_t start_time) {
    // Calculate elapsed time in seconds
    return (float)(clock() - start_time) / CLOCKS_PER_SEC;
}
