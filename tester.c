#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"

void test_single_file(const char *filename, int buffer_size) {
    printf("\n%sTesting file: %s with BUFFER_SIZE=%d%s\n", CYAN, filename, buffer_size, RESET);
    
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("%sFailed to open file: %s%s\n", RED, filename, RESET);
        return;
    }
    
    char *line;
    int line_count = 0;
    
    while ((line = get_next_line(fd)) != NULL) {
        printf("%sLine %d: %s%s", GREEN, ++line_count, RESET, line);
        free(line);
    }
    
    close(fd);
    printf("%sTotal lines read: %d%s\n", MAGENTA, line_count, RESET);
}

void test_multiple_files(const char **filenames, int num_files, int buffer_size) {
    printf("\n%sTesting multiple files (%d) with BUFFER_SIZE=%d%s\n", YELLOW, num_files, buffer_size, RESET);
    
    int *fds = malloc(num_files * sizeof(int));
    if (!fds) {
        printf("%sMemory allocation failed%s\n", RED, RESET);
        return;
    }
    
    // Open all files
    for (int i = 0; i < num_files; i++) {
        fds[i] = open(filenames[i], O_RDONLY);
        if (fds[i] == -1) {
            printf("%sFailed to open file: %s%s\n", RED, filenames[i], RESET);
            // Close already opened files
            for (int j = 0; j < i; j++) {
                close(fds[j]);
            }
            free(fds);
            return;
        }
    }
    
    char *line;
    int active_fd = 0;
    int lines_read = 0;
    int total_lines = 0;
    int files_completed = 0;
    
    // Read lines from files in round-robin fashion
    while (files_completed < num_files) {
        line = get_next_line(fds[active_fd]);
        
        if (line != NULL) {
            printf("%sFile %d, Line %d: %s%s", BLUE, active_fd + 1, ++lines_read, RESET, line);
            free(line);
            total_lines++;
        } else {
            files_completed++;
            printf("%sFinished reading from file %d (%s). Total lines: %d%s\n", 
                   MAGENTA, active_fd + 1, filenames[active_fd], lines_read, RESET);
            lines_read = 0;
        }
        
        active_fd = (active_fd + 1) % num_files;
    }
    
    printf("%sAll files completed. Total lines read from all files: %d%s\n", 
           YELLOW, total_lines, RESET);
    
    // Close all files
    for (int i = 0; i < num_files; i++) {
        close(fds[i]);
    }
    free(fds);
}

void test_stdin(int buffer_size) {
    printf("\n%sTesting STDIN with BUFFER_SIZE=%d%s\n", CYAN, buffer_size, RESET);
    printf("%sType some text (press Ctrl+D to end):%s\n", GREEN, RESET);
    
    char *line;
    int line_count = 0;
    
    while ((line = get_next_line(STDIN_FILENO)) != NULL) {
        printf("%sYou typed (line %d): %s%s", BLUE, ++line_count, RESET, line);
        free(line);
    }
    
    printf("%sTotal lines read from STDIN: %d%s\n", MAGENTA, line_count, RESET);
}

void test_edge_cases(int buffer_size) {
    printf("\n%sTesting edge cases with BUFFER_SIZE=%d%s\n", YELLOW, buffer_size, RESET);
    
    // Test empty file
    printf("\n%s1. Testing empty file...%s\n", CYAN, RESET);
    int fd = open("empty.txt", O_CREAT | O_RDONLY | O_TRUNC, 0644);
    if (fd == -1) {
        printf("%sFailed to create empty file%s\n", RED, RESET);
    } else {
        char *line = get_next_line(fd);
        if (line == NULL) {
            printf("%sOK: NULL returned for empty file%s\n", GREEN, RESET);
        } else {
            printf("%sFAIL: Expected NULL for empty file%s\n", RED, RESET);
            free(line);
        }
        close(fd);
    }
    
    // Test file without newline at end
    printf("\n%s2. Testing file without newline at end...%s\n", CYAN, RESET);
    fd = open("no_newline.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        printf("%sFailed to create test file%s\n", RED, RESET);
    } else {
        write(fd, "This file has no newline at end", 30);
        close(fd);
        
        fd = open("no_newline.txt", O_RDONLY);
        line = get_next_line(fd);
        if (line != NULL && strchr(line, '\n') == NULL) {
            printf("%sOK: Line without newline returned correctly%s\n", GREEN, RESET);
            printf("%sContent: %s%s", BLUE, line, RESET);
        } else {
            printf("%sFAIL: Expected line without newline%s\n", RED, RESET);
        }
        free(line);
        
        // Should return NULL on next call
        line = get_next_line(fd);
        if (line == NULL) {
            printf("%sOK: NULL returned at EOF%s\n", GREEN, RESET);
        } else {
            printf("%sFAIL: Expected NULL at EOF%s\n", RED, RESET);
            free(line);
        }
        close(fd);
    }
    
    // Test very long line
    printf("\n%s3. Testing very long line...%s\n", CYAN, RESET);
    fd = open("long_line.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1) {
        printf("%sFailed to create test file%s\n", RED, RESET);
    } else {
        for (int i = 0; i < 1000; i++) {
            write(fd, "This is a very long line that should test buffer handling. ", 56);
        }
        write(fd, "\n", 1);
        close(fd);
        
        fd = open("long_line.txt", O_RDONLY);
        line = get_next_line(fd);
        if (line != NULL) {
            printf("%sOK: Very long line read successfully%s\n", GREEN, RESET);
            printf("%sLine length: %zu%s\n", BLUE, strlen(line), RESET);
            free(line);
        } else {
            printf("%sFAIL: Could not read very long line%s\n", RED, RESET);
        }
        
        // Should return NULL on next call
        line = get_next_line(fd);
        if (line == NULL) {
            printf("%sOK: NULL returned after very long line%s\n", GREEN, RESET);
        } else {
            printf("%sFAIL: Expected NULL after very long line%s\n", RED, RESET);
            free(line);
        }
        close(fd);
    }
    
    // Test binary file (undefined behavior, but shouldn't crash)
    printf("\n%s4. Testing binary file (shouldn't crash)...%s\n", CYAN, RESET);
    fd = open("/bin/ls", O_RDONLY);
    if (fd == -1) {
        printf("%sFailed to open binary file%s\n", RED, RESET);
    } else {
        line = get_next_line(fd);
        printf("%sBinary file test completed without crashing%s\n", 
               (line == NULL) ? GREEN : YELLOW, RESET);
        if (line != NULL) free(line);
        close(fd);
    }
}

void cleanup_test_files() {
    remove("empty.txt");
    remove("no_newline.txt");
    remove("long_line.txt");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("%sUsage: %s <buffer_size> [file1 file2 ...]%s\n", RED, argv[0], RESET);
        printf("%sExample: %s 42 test1.txt test2.txt%s\n", YELLOW, argv[0], RESET);
        return 1;
    }
    
    int buffer_size = atoi(argv[1]);
    if (buffer_size <= 0) {
        printf("%sInvalid buffer size: %d. Using default 42%s\n", YELLOW, buffer_size, RESET);
        buffer_size = 42;
    }
    
    // Test single file
    if (argc == 2) {
        test_stdin(buffer_size);
        test_edge_cases(buffer_size);
    } else if (argc == 3) {
        test_single_file(argv[2], buffer_size);
        test_edge_cases(buffer_size);
    } else {
        // Test multiple files
        const char **files = (const char **)&argv[2];
        test_multiple_files(files, argc - 2, buffer_size);
    }
    
    cleanup_test_files();
    return 0;
}
