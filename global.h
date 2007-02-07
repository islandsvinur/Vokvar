#ifndef GLOBAL_H
#define GLOBAL_H

#define DEBUG(title,...) do { fprintf(stderr, "%s: ", title); \
                              fprintf(stderr, __VA_ARGS__); \
                              fprintf(stderr, "\n"); \
                         } while(0)

#endif /* GLOBAL_H */
