#ifndef TLV_H
#define TLV_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <malloc.h>
#include <string.h>
#define  rx 0x7278
#define  tx 0x7478
#define  rxtx 0x72787478
#define tagLen 2
#define valLen 2
#define headerLen 4
typedef union
{
    uint32_t value;
    char byte[1024];

} bytesToValue;
struct TLVProtocol
{
    uint16_t tag;
    uint16_t len;
    uint32_t value;
    char *currentPosition;
};
struct Esocket
{
    int reciveSocket;
    fd_set readfds;
    struct timeval timeout;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int (*readyRead)(int sock ,fd_set * readfds);
};
enum byteOrder {LE,BE};
enum Error_m {decodeError,dataError,NonError};



uint8_t TlvValueDecode(struct TLVProtocol * TLV,char*data,enum byteOrder byte_order,int position);

uint8_t TlvHeaderDecode(struct TLVProtocol * TLV,const char*data,enum byteOrder byte_order,int position)__attribute__((pure));

enum Error_m TlvDecode(const struct Esocket *sock, char * data, int size, enum byteOrder byte_order, enum Error_m prevError);

int sockState(int sock ,fd_set * readfds)__attribute__((pure));

uint8_t initSocket(struct Esocket* sock,int protocolFamily,int socketType,int protocol,int port,int mode,struct timeval timeout);

int sockselect(struct Esocket * sock);

size_t readDatagramm(const struct Esocket * sock,char * data,int size)__attribute__((pure));

void sendDatagramm(const struct TLVProtocol *TLV, const struct Esocket * echoS, char * data, int position);

void printState(struct Esocket * sock);


#endif // TLV_H
