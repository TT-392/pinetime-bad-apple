// generic saveram code

#include <string.h>

#define TIME_OF_WRITING 1577836800
#define SAVERAM_MAGICK  0x5AB3BEEF
#define SAVERAM_VERSION 0x00010008

struct saveram 
{
	unsigned int magick;   // magick number 
	unsigned int version;  // version
	unsigned int length;   // size of this saveram struct
	
	long long int clock;   // time in seconds, live updated by interrupt
};

extern struct saveram saveram[1];

void saveram_format(void);

int saveram_setup(void)
{
	if( ( saveram->magick  != SAVERAM_MAGICK   ) ||
	    ( saveram->version != SAVERAM_VERSION  ) ||
	    ( saveram->length  != sizeof(saveram)  ) ) // SIMPLE VALIDATION CHECK
	{
		saveram_format();
	}
	return 0;
}

void saveram_format(void)
{
	memset(saveram,0xf0,sizeof(saveram)); // check we can write to all of it
	for(int i=0;i<sizeof(saveram);i++) {
		if( ((char*)saveram)[i]!=0xf0 ) {
			while(1);
		}
	}
	memset(saveram,0x00,sizeof(saveram));
	
	saveram->magick  = SAVERAM_MAGICK;
	saveram->version = SAVERAM_VERSION;
	saveram->length  = sizeof(saveram);

	saveram->clock=((long long int)(TIME_OF_WRITING))*65536; // it will be at least 2020

}

