#ifndef TOOLS_HPP
#define TOOLS_HPP


void print_fd_set(fd_set *set) {
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, set)) {
            printf("%d ", i);
        }
    }
    printf("\n");
}

std::string trimWhitespace(std::string s) {
    // Find the first non-whitespace character
    size_t first = s.find_first_not_of(" \t\r\n");

    // If no non-whitespace character is found, return an empty string
    if (first == std::string::npos) return "";

    // Find the last non-whitespace character
    size_t last = s.find_last_not_of(" \t\r\n");

    // Erase the whitespace characters from the beginning and end of the string
    s.erase(0, first);
    s.erase(last - first + 1);

    return s;
}


#endif