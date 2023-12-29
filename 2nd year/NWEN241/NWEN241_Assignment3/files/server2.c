#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

/**
 * Takes a output string  (c_fd is client file descriptor
 * sends a message to the client
 * if the sending failes then inform of error and disconect from client
 * otherwise do a successful return
 */
int sendMsg(char* str, int c_fd) {
    ssize_t s = send(c_fd, str, strlen(str), 0);
    if(s < 0) {
        printf("Error sending message");
        close(c_fd);
        exit(0);
        return -1;
    }
    return 0;
}

/**
 * Takes a buffer string alongside its length (c_fd is client file descriptor
 * waits to recieve message from client and stores it into the buffer
 * if the reading failes then inform of error and disconect from client
 * otherwise add escape char and do a successful return
 */
int recieveMsg(char* buf, int buf_len, int c_fd) {
    ssize_t r = recv(c_fd, buf, buf_len - 1, 0);
    if(r <= 0) {
        printf("Error receiving message");
        close(c_fd);
        exit(0);
        return -1;
    }
    buf[r] = '\0';
    return 0;
}
/**
 * file read takes a file name
 * attempts to open it
 *  if opened then prints the contents in a formatted manner\
 * close the file
 */
int readFromFile(int c_fd, char* f_name) {
    FILE *in; // use for handling input file
    char* read_buf;
    // Open raw.txt for reading
    printf("Received file: %s\n", f_name);

    // check if another token exists
    char* file_check =  strtok(f_name, " \n\r");
    file_check =  strtok(NULL, " \n\r");

    // stop the command if any of these flags have failed
    if (f_name == NULL || file_check != NULL)  {
        sendMsg("SERVER 500 Get Error\n", c_fd);
        return 1;
    }

    // otherwise carry on as normal
    in = fopen(f_name, "r");
    if (in == NULL) {
        printf("input file failed to open\n");
        sendMsg("SERVER 404 Not Found\n", c_fd);
        return 1;
    }
    printf("Openning: %s\n", f_name);
        // determine file size
    fseek(in, 0L, SEEK_END);
    long file_size = ftell(in);
    rewind(in);

    // allocate buffer and read file contents
    read_buf = (char *)malloc(file_size * sizeof(char));
    fread(read_buf, sizeof(char), file_size, in);

    // close file and print contents of buffer
    fclose(in);

    sendMsg("SERVER 200 OK\n\n", c_fd);
    char* line = strtok(read_buf, "\n");
    while(line != NULL){
        sendMsg(line, c_fd);
        sendMsg("\n", c_fd);
        line = strtok(NULL, "\n");
    }
    sendMsg("\n\n", c_fd);
    free(read_buf);
    return 0;
}

/**
 * file writer takes a file name
 * opens the file for editing while checking its not the server name
 * if opened then keep taking user inputs until double \n
 * then close the file
 */
int writeToFile(int c_fd, char* f_name) {
    // check if another token exists
    char* file_check =  strtok(f_name, " \n\r");
    file_check =  strtok(NULL, " \n\r");

    // stop the command if any of these flags have failed
    if (f_name == NULL || file_check != NULL)  {
        sendMsg("SERVER 501 Put Error\n", c_fd);
        return 1;
    }

    FILE* out = fopen(f_name, "w"); // Open file for writing, truncating it if it already exists
    if (out == NULL) {
        printf("Failed to open output file\n");
        sendMsg("SERVER 501 Put Error\n", c_fd);
        return 1;
    }

    printf("Opened %s for writing\n", f_name);
    sendMsg("SERVER 201 Created\n", c_fd);

    char input_buf[1024];
    int input_len;

    // Keep reading user inputs until they enter two consecutive blank lines
    int nl = 0; //keeps track of consecutive \n
    while (nl < 3) {
        //safely stop if failed to read
        if (recieveMsg(input_buf, sizeof(input_buf), c_fd)) {
            fclose(out);
            return 1;
        }
        // reset counter if the its not a standalone \n (\r is used for telnet)
        if (*input_buf != '\n' && *input_buf != '\r')
            nl = 0;

        fprintf(out, "%s", input_buf);
        nl++;
    }
    fclose(out);
    printf("File written successfully\n");
    return 0;
}

/**
 * function that converts input string to a server command
 * if Bye - then returns 0
 * otherwise try to read command and filename
 * commands (Bye, Get, Put)
 * if the command is Get, it runs the file reader method
 * if the command is put it runs the file writer method
 * then returns 1 for succes
 */
int getCommand(char* in_string, int c_fd) {
    char *cmds[] =  {"bye", "get", "put"};
    int num_cmds = 3;

    enum sel {inv = -1, bye, get, add};
    enum sel c = inv;

    printf("Received message: %s\n", in_string);
    char* cmd = strtok(in_string, " \n\r");    //(\r is used for telnet compatability)

    for (int i = 0; i < num_cmds; i++) {
        c = (strcasecmp(cmd, cmds[i]) == 0) ? i : c;
        // skip all next checks if comman already found
        if(c != inv)
            i = 3;
    }
    // couldnt find a cleaner way other than using single line if statements
    if (c == inv)
        sendMsg("SERVER 502 Command Error\n", c_fd);
    if (c > bye) {
        // potentially add a check for the filename
        cmd = strtok(NULL, "\n\r");    //(\r is used for telnet compatability)
        if(c == get)
            readFromFile(c_fd, cmd);
        else
            writeToFile(c_fd, cmd);
    }
    return (c);
}


/**
 * The main function should be able to accept a command-line argument
 * argv[0]: program name
 * argv[1]: port number
 *
 * Read the assignment handout for more details about the server program
 * design specifications.
 *
 * testing
 * nc localhost port
 */

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("No port selected!\n");
        return -1;
    }
    const int MAX_PORT= 65535;
    const int MIN_PORT = 1024;
    int port;

    sscanf(argv[1], "%d", &port);
    if (port > MAX_PORT || port < MIN_PORT) {
        printf("Invalid port value selected! Terminating program\n");
        return -1;
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("Failed to create socket\n");
        exit(0);
        return -1;
    }
    printf("Socket created\n");

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY; // any address
    addr.sin_port = htons(port); // port
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("Error binding socket\n");
        exit(0);
    }
    printf("Socket bound\n");

    int connected = 0;
    int pid = 1;
    int client_fd;

    // keep the server constantly looking for a connectiong if its ever disconected
    for(;;) {
        // attempt to search for a client block
        if (pid) {
            if(listen(fd, SOMAXCONN) < 0) {
                printf("Error listening for connections\n");
                exit(0);
            }
            struct sockaddr_in client_addr;
            int addrlen = sizeof(client_addr);
            client_fd = accept(fd, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen);
            if(client_fd < 0) {
                printf("Error accepting connection\n");
                exit(0);
            }
            printf("Client connected\n");
            pid = fork();
            if (pid <0) {
                printf("Failed to fork process\n");
            } else if (pid == 0) {
                char msg[] = "HELLO\n";
                sendMsg(msg, client_fd);
            }  else {
                printf("Forking success\n");
                close(client_fd);
                client_fd = -1;
            }

        }

        //Connected to client block
        if (pid == 0) {
            int b_len = 100;
            char incoming[b_len];
            recieveMsg(incoming, b_len, client_fd);

            // Do something with receiving message
            if (getCommand(incoming, client_fd) == 0) {
                printf("Client requested to disconect\n");
                // close connection and exit loop
                close(client_fd);
                exit(0);
            }
        }
    }
    return 0;
}

