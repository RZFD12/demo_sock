#include "Tlv.h"
#include "Test.h"
#define recivePort 12344
int main()
{
    setvbuf(stdout,NULL,_IOLBF,0);
    //testHeaderDeocodeTag(15,65); debug
    //testValueDecode(0,1);
    struct timeval timeout={};
    timeout.tv_sec=0;
    timeout.tv_usec=3000;
    struct Esocket echoS={};
    if(initSocket(&echoS,AF_INET6,SOCK_DGRAM,IPPROTO_IPV6,recivePort,O_NONBLOCK,timeout)==0)
    {
        perror("socket failed to get socket descriptor ipv6");
    }
    if(initSocket(&echoS,AF_INET,SOCK_DGRAM,IPPROTO_UDP,recivePort,O_NONBLOCK,timeout)==0)
    {
       perror("socket failed to get socket descriptor ipv4"); exit(EXIT_FAILURE);
    }
    while(1)
    {
        if((sockselect(&echoS)==1)&&(echoS.readyRead(echoS.reciveSocket,&echoS.readfds)))
        {

            int datagrammSize=0;
            ioctl(echoS.reciveSocket,FIONREAD,&datagrammSize);
            if(datagrammSize>0)
            {
                char * buffer=NULL;
                buffer=(char*)malloc(datagrammSize*sizeof(char));
                if((malloc_usable_size(buffer)==datagrammSize)&&(readDatagramm(&echoS,buffer,datagrammSize)>0) && (TlvDecode(&echoS,buffer,datagrammSize,LE,NonError))==NonError)
                {
                    sendto(echoS.reciveSocket,buffer,datagrammSize,0,(struct sockaddr*)&echoS.clientAddr,sizeof(echoS.clientAddr));
                    printState(&echoS);
                }
             }
        }
    }
    return 0;
}
