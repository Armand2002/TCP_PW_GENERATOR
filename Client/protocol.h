// protocol.h
#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define PROTO_PORT 60000
#define BUFFER_SIZE 64
#define QLEN 5

typedef struct {
    char type;
    int length;
    char password[BUFFER_SIZE];
} msg;

#endif // PROTOCOL_H_
