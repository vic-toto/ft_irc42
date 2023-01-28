#ifndef TOOLS_HPP
#define TOOLS_HPP

#include "Server.hpp"

void print_fd_set(fd_set *set) {
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, set)) {
            printf("%d ", i);
        }
    }
    printf("\n");
}

#endif