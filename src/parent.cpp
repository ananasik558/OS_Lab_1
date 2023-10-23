#include <iostream>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <fcntl.h>
#include <random>
#include <time.h>

int main() {
    std::string FileName1 = "", FileName2 = "";
    char c;

    while(read(STDIN_FILENO, &c, sizeof(char)) != 0) {
        if(c != '\n') FileName1.push_back(c);
        else break;
    }
    while(read(STDIN_FILENO, &c, sizeof(char)) != 0) {
        if(c != '\n') FileName2.push_back(c);
        else break;
    }
    char File_Name1[FileName1.size() + 1];
    for (int i = 0; i < FileName1.size(); ++i) {
        File_Name1[i] = FileName1[i];
    }
    File_Name1[FileName2.size()] = '\0';
    char File_Name2[FileName2.size() + 1];
    for (int i = 0; i < FileName2.size(); ++i) {
        File_Name2[i] = FileName2[i];
    }
    File_Name2[FileName2.size()] = '\0';

    int pipe1_fd[2];
    int err1 = pipe(pipe1_fd);
    if(err1 == -1) {
        perror("pipe");
        return 1;
    }
    int pipe2_fd[2];
    int err2 = pipe(pipe2_fd);
    if(err2 == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork1");
        return -1;
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork2");
        return -1;
    }

    int x, start = 1, end = 100;
    srand(time(NULL));
    std::string s, s1, s2;
    if(pid1 > 0 && pid2 > 0) {
        while(read(STDIN_FILENO, &c, sizeof(c))) {
            if(c == '\n' || c == ' ' || c == '\t') {
                reverse(s.begin(), s.end());
                s += '\n';
                x = rand() % (end - start + 1) + start;
                if(x <= 80) {
                    s1 += s;
                } else {
                    s2 += s;
                }
                s = "";
            }
            s += c;
        }
        close(pipe1_fd[0]);
        close(pipe2_fd[0]);
        write(pipe1_fd[1], s1.c_str(), s1.length());
        write(pipe2_fd[1], s2.c_str(), s2.length());
        close(pipe1_fd[1]);
        close(pipe2_fd[1]);
    }
    if (pid1 == 0) {
        close(pipe1_fd[1]);
        dup2(pipe1_fd[0], STDIN_FILENO);
        close(pipe1_fd[0]);
        execl("child1.out", "child1.out", File_Name1, NULL);
    } else if (pid2 == 0) {
        close(pipe2_fd[1]);
        dup2(pipe2_fd[0], STDIN_FILENO);
        close(pipe2_fd[0]);
        execl("child2.out", "child2.out", File_Name2, NULL);
    }
    
    return 0;
}