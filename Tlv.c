#include "Tlv.h"

void printOperation(uint32_t TLVvalue)
{
    if(TLVvalue==0)        {printf("The amplifier is off\n");}
    else if(TLVvalue==1)   {printf("The amplifier is on\n");}
    else if(TLVvalue==rx)  {printf("Reciver mode\n");}
    else if(TLVvalue==tx)  {printf("Transmitter mode\n");}
    else if(TLVvalue==rxtx){printf("Duplex mode\n");}

}
uint8_t TlvValueDecode(struct TLVProtocol * TLV,char*data,enum byteOrder byte_order,int position)
{
    // check TLV value
    if(byte_order==LE)
    {
        TLV->currentPosition=&data[position+headerLen+TLV->len-1];
    }
    else if(byte_order==BE)
    {
        TLV->currentPosition=&data[position+headerLen];
    }
    bytesToValue conversion={};
    for(int i=0;i<TLV->len;++i)
    {
        if(byte_order==LE)
        {
            conversion.byte[i]=*TLV->currentPosition;
            TLV->currentPosition--;
        }
        else if(byte_order==BE)
        {
            conversion.byte[i]=*TLV->currentPosition;
            TLV->currentPosition++;
        }
    }

    if((conversion.value==rx&&TLV->tag==1) || (conversion.value==tx&&TLV->tag==1) || (conversion.value==rxtx&&TLV->tag==1) ||
       (conversion.value==0&&TLV->tag==0 ) || (conversion.value==1 &&TLV->tag==0))
    {
        printOperation(conversion.value);
        return 1;
    }

    else
    {

        return 0;
    }
}

uint8_t TlvHeaderDecode(struct TLVProtocol * TLV,const char*data,enum byteOrder byte_order,int position)
{
     // check tag and len into TLV
    if(byte_order==LE)
    {
        TLV->tag= ((uint16_t)((data[position+1]&0xff)|(data[position]&0xff)<<8));
        TLV->len= ((uint16_t)((data[position+3]&0xff)|(data[position+2]&0xff)<<8));
    }
    else if(byte_order==BE)
    {
        TLV->tag= ((uint16_t)((data[position]&0xff)|(data[position+1]&0xff)<<8));
        TLV->len= ((uint16_t)((data[position+2]&0xff)|(data[position+3]&0xff)<<8));
    }
    if((TLV->tag==0 || TLV->tag==1) && (TLV->len<65535)){ return 1;} else {return 0;}
}

enum Error_m TlvDecode(const struct Esocket * sock,char * data,int size,enum byteOrder byte_order,enum Error_m prevError)
{
    // check TLV into recived data
   struct TLVProtocol TLV={};
   prevError=NonError;
   for(uint32_t i=0;i<size-headerLen;)
   {
       if((TlvHeaderDecode(&TLV,data,byte_order,i)==1)&&((i+TLV.len+headerLen-1)<size) && (TlvValueDecode(&TLV,data,byte_order,i)==1)) //  parcing ok
       {
           sendDatagramm(&TLV,sock,data,i);
           i+=TLV.len+headerLen;
       }
       else
       {
           ++i;
           prevError=dataError;
       }
   }
   return prevError;
}

int sockState(int sock ,fd_set * readfds)
{
    if(FD_ISSET(sock,readfds)){return 1;} else {perror("ERROR");return 0;}
}


uint8_t initSocket(struct Esocket* sock,int protocolFamily,int socketType,int protocol,int port,int mode,struct timeval timeout)
{
    // initializing the socket
    FD_ZERO(&sock->readfds);
    sock->timeout=timeout;
    sock->reciveSocket=socket(protocolFamily,socketType,protocol);
    if(sock->reciveSocket<0) {return 0;}

    sock->serverAddr.sin_family=protocolFamily;
    sock->serverAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    sock->serverAddr.sin_port=htons(port);
    if(fcntl(sock->reciveSocket,F_SETFL,mode)<0){perror("ERROR");return 0;}

    if(bind(sock->reciveSocket,(struct sockaddr*)&sock->serverAddr,sizeof(sock->serverAddr))<0){perror("ERROR");return 0;}
    sock->readyRead=sockState;
    return 1;
}


int sockselect(struct Esocket * sock)
{
    FD_SET(sock->reciveSocket,&sock->readfds);
    if(select(sock->reciveSocket+1,&sock->readfds,NULL,NULL,&sock->timeout)<=0){return 0;}else{return 1;}
}

size_t readDatagramm(const struct Esocket * sock,char * data,int size)
{
    // reading data from the socket
    socklen_t len=sizeof(sock->clientAddr);
    size_t datagrammSize=recvfrom(sock->reciveSocket,data,size,0,(struct sockaddr*)&sock->clientAddr,&len);
    return datagrammSize;
}

void printState(struct Esocket *sock)
{
    // print to console
    int addrlen=0;
    if(sock->clientAddr.sin_family==AF_INET6){addrlen=INET6_ADDRSTRLEN;}
    else{addrlen=INET_ADDRSTRLEN;}
    char address[addrlen];
    inet_ntop(sock->clientAddr.sin_family,&sock->clientAddr.sin_addr,address,addrlen);
    printf("Data sending to %s",address);
    printf(" port %hu\n", ntohs(sock->clientAddr.sin_port));
}

void sendDatagramm(const struct TLVProtocol * TLV, const struct Esocket *echoS, char *data, int position)
{
    size_t size=TLV->len+headerLen;
    char* buffer=(char*)malloc(size);
    if(malloc_usable_size(buffer)==size)
    {
        memcpy(buffer,data+position,size);
        sendto(echoS->reciveSocket,buffer,size,0,(struct sockaddr*)&echoS->clientAddr,sizeof(echoS->clientAddr));
    }
}
