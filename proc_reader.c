#include "proc_reader.h"

int list_process_directories(void) {
    DIR *proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("ERROR: Failed to open /proc");
        return -1;
    }

    struct dirent *entry;
    int process_count = 0;

    printf("Process directories in /proc:\n");
    printf("%-8s %-20s\n", "PID", "Type");
    printf("%-8s %-20s\n", "---", "----");

    while ((entry = readdir(proc_dir)) != NULL) {
        if (is_number(entry->d_name)) {
            printf("%-8s %-20s\n", entry->d_name, "process");
            process_count++;
        }
    }

    if (closedir(proc_dir) == -1) {
        perror("ERROR: Failed to close /proc");
        return -1;
    }

    printf("Found %d process directories\n", process_count);

    return 0;
}

int read_process_info(const char* pid) {
    char filepath[256];

    snprintf(filepath, sizeof(filepath), "/proc/%s/status", pid);

    printf("\n--- Process Information for PID %s ---\n", pid);

    if (read_file_with_syscalls(filepath) != 0) {
        perror("ERROR: Failed to read process status");
        return -1;
    }

    snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", pid);

    printf("\n--- Command Line ---\n");

    if (read_file_with_syscalls(filepath) != 0) {
        perror("ERROR: Failed to read process cmdline");
        return -1;
    }

    printf("\n");

    return 0;
}

int show_system_info(void) {
    int line_count = 0;
    const int MAX_LINES = 10;

    printf("\n--- CPU Information (first %d lines) ---\n", MAX_LINES);

    char line[256];
    FILE *file;

    file = fopen("/proc/cpuinfo", "r");
    if (file == NULL) {
        perror("Failed to open /proc/cpuinfo");
        return -1;
    }

    while (line_count < MAX_LINES && fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
        line_count++;
    }
    fclose(file);

    line_count = 0;

    printf("\n--- Memory Information (first %d lines) ---\n", MAX_LINES);

    file = fopen("/proc/meminfo", "r");
    if (file == NULL) {
        perror("Failed to open /proc/meminfo");
        return -1;
    }

    while (line_count < MAX_LINES && fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
        line_count++;
    }
    fclose(file);

    return 0;
}

void compare_file_methods(void) {
    const char* test_file = "/proc/version";

    printf("Comparing file reading methods for: %s\n\n", test_file);

    printf("=== Method 1: Using System Calls ===\n");
    read_file_with_syscalls(test_file);

    printf("\n=== Method 2: Using Library Functions ===\n");
    read_file_with_library(test_file);

    printf("\nNOTE: Run this program with strace to see the difference!\n");
    printf("Example: strace -e trace=openat,read,write,close ./lab2\n");
}

int read_file_with_syscalls(const char* filename) {
    int fd;
    char buffer[1024];
    ssize_t bytes_read;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("ERROR: Failed to open file with syscalls");
        return -1;
    }

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }

    if (bytes_read == -1) {
        perror("ERROR: Failed to read file with syscalls");
        close(fd);
        return -1;
    }

    if (close(fd) == -1) {
        perror("ERROR: Failed to close file with syscalls");
        return -1;
    }

    return 0;
}

int read_file_with_library(const char* filename) {
    FILE *file;
    char buffer[256];

    file = fopen(filename, "r");
    if (file == NULL) {
        perror("ERROR: Failed to open file with library functions");
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }

    if (fclose(file) != 0) {
        perror("ERROR: Failed to close file with library functions");
        return -1;
    }

    return 0;
}

int is_number(const char* str) {
    if (str == NULL || *str == '\0') {
        return 0;
    }

    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}