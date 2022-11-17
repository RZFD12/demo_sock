#include "Tlv.h"
#include "Test.h"
#include <assert.h>


void testHeaderDeocodeTag(int16_t tag,uint16_t len)
{
    char data[sizeof (tag)+sizeof (len)];
    unsigned int tmp=0;
    tmp=tag;
    data[1] =tmp&0xff;
    data[0] =(tmp>>8)&0xff;
    tmp=len;
    data[3] =tmp&0xff;
    data[2] =(tmp>>8)&0xff;
    struct TLVProtocol protocol={};
    assert(1==TlvHeaderDecode(&protocol,data,LE,0));

}

void testValueDecode(uint16_t tag, uint16_t len)
{
    struct TLVProtocol protocol;
    protocol.tag=tag;
    protocol.len=len;
    char data[sizeof (protocol.tag)+sizeof(protocol.len)+len+1];
    unsigned int tmp=0;
    tmp=tag;
    // header
    data[1] =tmp&0xff;
    data[0] =(tmp>>8)&0xff;
    tmp=len;
    data[3] =tmp&0xff;
    data[2] =(tmp>>8)&0xff;
    //
    if(tag==1)
    {
        char goodValue[5]="rxtx";
        char badValue[5]="rxds";
        data[4]=goodValue[0];
        data[5]=goodValue[1];
        data[6]=goodValue[2];
        data[7]=goodValue[3];
        assert(1==TlvValueDecode(&protocol,data,LE,0)); // good value for tag 1

        data[4]=badValue[0];
        data[5]=badValue[1];
        data[6]=badValue[2];
        data[7]=badValue[3];
        assert(0==TlvValueDecode(&protocol,data,LE,0)); // bad value for tag 0
    }
    if(tag==0)
    {
        uint8_t goodValue=0;
        data[4]=goodValue;
        assert(1==TlvValueDecode(&protocol,data,LE,0)); //good value for tag 0
        uint8_t badValue=12;
        data[4]=badValue;
        assert(0==TlvValueDecode(&protocol,data,LE,0)); //bad value for tag 1
    }
}



