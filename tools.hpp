#ifndef TOOLS_HPP
#define TOOLS_HPP


inline void print_fd_set(fd_set *set) {
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (FD_ISSET(i, set)) {
            printf("%d ", i);
        }
    }
    printf("\n");
}

inline std::string trimWhitespace(std::string s) {
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

inline std::string removeLeadingSpace(std::string str)
{
    std::size_t firstNonSpace = str.find_first_not_of(' ');
    if (firstNonSpace == 0)
        return str;
    str.erase(0, firstNonSpace);
    return str;
}

inline bool is_char_or_digit(const std::string &str) {
  return str.length() > 4 ;
}

inline int substr_to_first_space_or_end(const std::string &str) {
  std::size_t pos = str.find(' ');
  return pos != std::string::npos ? pos : 0;
}


#endif