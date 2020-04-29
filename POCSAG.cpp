#include <string.h>
#include <stdint.h>
#include <POCSAG.h>

Pocsag::Pocsag(){
state = 0;
size = 0;
};

int Pocsag::GetState(){
return(state);
};

int Pocsag::GetSize(){
	return(size);
};

int Pocsag::GetError(){
	return(error);
};

void * Pocsag::GetMsgPointer(){
return ((void*) &Pocsagmsg);
};

int Pocsag::CreatePocsag(long int adresse, int quelle, char *text){
	int txtlen;
	unsigned char c; // buffer for generated characters
	int stop; 
	
	char lastchar;
	
	unsigned char txtcoded[112]; // generated Text up to 56 octets, equals 8 frames to 7 bit characterencoding (4 Frames per batch, 4 x batch x 7)
	int txtcodedlen;	
	
	int currentframe;
	uint32_t adressline;
	
	int bitcount_in, bitcount_out, bytecount_in, bytecount_out; //Counters for textencoding
	
	const unsigned char size2mask[7] = {0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f}; //encodingtable
	
	txtlen = strlen(text);
	if(txtlen > 80){ 
	txtlen = 80; //max. size of transmitted TEXT (practical 76 characters, limited by the datasize of the SPI Bus
	};
	
	state = 0;
	
	error = POCSAGRC_UNDETERMINED;
	size = 0;
	
	if((adresse > 0x1FFFFF) || (adresse <= 0)){
	error = POCSAGRC_UNGUELTIGEADRESSE;
	return(POCSAG_GESCHEITERT);
	};
	
	if((quelle < 0) || (quelle> 3)){
		error=POCSAGRC_UNGUELTIGEQUELLE;
		return(POCSAG_GESCHEITERT);
	};
	
	
	lastchar = text[txtlen]; // change \0 with EOT
	text[txtlen] = 0x04; //EOT
	txtlen++; //final EOT
	 
	state = 1; 
	
	memset(Pocsagmsg.sync, 0x55, 72);
	
	//generating of synchronization word
	Pocsagmsg.synccw1[0] = sync_cw0;
	Pocsagmsg.synccw1[1] = sync_cw1;
	Pocsagmsg.synccw1[2] = sync_cw2;
	Pocsagmsg.synccw1[3] = sync_cw3;

	Pocsagmsg.synccw2[0] = sync_cw0;
	Pocsagmsg.synccw2[1] = sync_cw1;
	Pocsagmsg.synccw2[2] = sync_cw2;
	Pocsagmsg.synccw2[3] = sync_cw3;

	Pocsagmsg.synccw3[0] = sync_cw0;
	Pocsagmsg.synccw3[1] = sync_cw1;
	Pocsagmsg.synccw3[2] = sync_cw2;
	Pocsagmsg.synccw3[3] = sync_cw3;

	Pocsagmsg.synccw4[0] = sync_cw0;
	Pocsagmsg.synccw4[1] = sync_cw1;
	Pocsagmsg.synccw4[2] = sync_cw2;
	Pocsagmsg.synccw4[3] = sync_cw3;


	
	Pocsagmsg.synccw1[0] ^= sync_inv;
	Pocsagmsg.synccw1[1] ^= sync_inv;
	Pocsagmsg.synccw1[2] ^= sync_inv;
	Pocsagmsg.synccw1[3] ^= sync_inv;

	Pocsagmsg.synccw2[0] ^= sync_inv;
	Pocsagmsg.synccw2[1] ^= sync_inv;
	Pocsagmsg.synccw2[2] ^= sync_inv;
	Pocsagmsg.synccw2[3] ^= sync_inv;

	Pocsagmsg.synccw3[0] ^= sync_inv;
	Pocsagmsg.synccw3[1] ^= sync_inv;
	Pocsagmsg.synccw3[2] ^= sync_inv;
	Pocsagmsg.synccw3[3] ^= sync_inv;

	Pocsagmsg.synccw4[0] ^= sync_inv;
	Pocsagmsg.synccw4[1] ^= sync_inv;
	Pocsagmsg.synccw4[2] ^= sync_inv;
	Pocsagmsg.synccw4[3] ^= sync_inv;
	

	//fill all batches with IDLE Pattern
	for(int l = 0; l < 16; l++){
	Pocsagmsg.batch1[l] = idle_cw;
	Pocsagmsg.batch2[l] = idle_cw;
	Pocsagmsg.batch3[l] = idle_cw;
	Pocsagmsg.batch4[l] = idle_cw;
	};


	//adresscodeword generating

	currentframe = ((adresse &0x7)<<1);
	adressline=adresse >> 3;
	adressline <<=2;
	adressline += quelle;

	replaceline(&Pocsagmsg, currentframe, createcrc(adressline<<11));

	//coding of the text

	memset(txtcoded, 0x00, 112);

	bitcount_out = 7;
	bytecount_out = 0;

	bitcount_in = 7;
	bytecount_in = 0;
	c = flip7charbitorder(text[0]);

	txtcodedlen = 0;
	txtcoded[0] = 0x80;

	stop = 0;

	while(!stop){
	int bits2copy;
	unsigned char t;

	if(bitcount_in > bitcount_out){
	bits2copy = bitcount_out;
	}
	else{
		bits2copy = bitcount_in;
	};

	t = c & (size2mask[bits2copy-1] << (bitcount_in - bits2copy));

	if(bitcount_in > bitcount_out){
	t >>= (bitcount_in - bitcount_out);
	}
	else if(bitcount_in < bitcount_out){
		t <<= (bitcount_out - bitcount_in);
	};

	txtcoded[txtcodedlen] |= t;

	bitcount_in -= bits2copy;
	bitcount_out -= bits2copy;

	if(bitcount_out == 0){
		bytecount_out++;
		txtcodedlen++;

		if(bytecount_out == 1){
			txtcoded[txtcodedlen] = 0x00;
			bitcount_out = 8;
		}
		else if(bytecount_out == 2){
		txtcoded[txtcodedlen] = 0x00;
		bitcount_out = 5;
		}
		else if(bytecount_out >= 3){
		txtcoded[txtcodedlen]=0x80;
		bitcount_out = 7;
		bytecount_out = 0;
		};
	};

	if (bitcount_in == 0) {
    bytecount_in++;
	
	
	if(bytecount_in < txtlen){
		c = flip7charbitorder(text[bytecount_in]);
		bitcount_in = 7;
	}
	else{
	stop = 1;
	continue;
	};
	};

	}

	txtcodedlen++;

	for(int l = 0; l < txtcodedlen; l += 3){
	uint32_t t;
	currentframe++;

	t = (uint32_t) txtcoded[l] << 24;
	t |= (uint32_t) txtcoded[l+1] << 16;
	t |= (uint32_t) txtcoded[l+2] << 11;

	replaceline(&Pocsagmsg, currentframe, createcrc(t));
	};

	
	for(int l = 0; l < 16; l++){
		Pocsagmsg.batch1[l] ^= 0xffffffff;
	};
	for(int l = 0; l < 16; l++){
		Pocsagmsg.batch2[l] ^= 0xffffffff;
	};
	for(int l = 0; l < 16; l++){
		Pocsagmsg.batch3[l] ^= 0xffffffff;
	};
	for(int l = 0; l < 16; l++){
		Pocsagmsg.batch4[l] ^= 0xffffffff;
	};
		
	

	for(int l= 0; l< 16; l++){
	int32_t t1;

	struct int32_4char{
	union{
	int32_t i;
	unsigned char c[4];
	};
	}t2;

	t1 = Pocsagmsg.batch1[l];

	t2.c[0] = (t1>>24)&0xff;
	t2.c[1] = (t1>>16)&0xff;
	t2.c[2] = (t1>>8)&0xff;
	t2.c[3] = t1&0xff;

	Pocsagmsg.batch1[l] = t2.i;

	t1 = Pocsagmsg.batch2[l];

	t2.c[0] = (t1>>24)&0xff;
	t2.c[1] = (t1>>16)&0xff;
	t2.c[2] = (t1>>8)&0xff;
	t2.c[3] = t1&0xff;

	Pocsagmsg.batch2[l] = t2.i;

	t1 = Pocsagmsg.batch3[l];

	t2.c[0] = (t1>>24)&0xff;
	t2.c[1] = (t1>>16)&0xff;
	t2.c[2] = (t1>>8)&0xff;
	t2.c[3] = t1&0xff;

	Pocsagmsg.batch3[l] = t2.i;
	
	t1 = Pocsagmsg.batch4[l];

	t2.c[0] = (t1>>24)&0xff;
	t2.c[1] = (t1>>16)&0xff;
	t2.c[2] = (t1>>8)&0xff;
	t2.c[3] = t1&0xff;

	Pocsagmsg.batch4[l] = t2.i;

	};

	text[txtlen] = lastchar;

	if(currentframe < 16){
		
		size = 248; /////////////////////////////// max. buffersize limited by FIFO of ezRadio
		return(POCSAG_ERFOLG);
	};

	size = 248; /////////////////////////////// 208 (two Batches)+ 8 * number of aditional characters
	return(POCSAG_ERFOLG);
};

void Pocsag::replaceline(Pocsag::Pocsagmsg_s *msg, int line, uint32_t val){

if((line < 0) || (line > 32)){
	return;
};

if(line < 16){
msg->batch1[line] = val;
}
else{
msg ->batch2[line-16] = val;
};

}

unsigned char Pocsag::flip7charbitorder(unsigned char c_in){
int i;
char c_out;

c_out = 0x00;

for(int l=0; l<6;l++){
	if(c_in & 0x01){
	c_out |= 1;
	};

	c_out <<= 1;
	c_in >>= 1;
};
if(c_in &0x01){
c_out |= 1;
};

return(c_out);
}


uint32_t Pocsag::createcrc(uint32_t in){

uint32_t cw;
uint32_t local_cw = 0;
uint32_t parity = 0;

cw = in;

local_cw = in;

for(int bit = 1; bit <= 21; bit++, cw <<=1){
if(cw & 0x80000000){
 cw ^= 0xED200000;
};
};

local_cw |= (cw >> 21);

cw = local_cw;

for(int bit = 1; bit <= 32; bit++, cw <<=1){
if (cw & 0x80000000) {
    parity++;
  };
};

if(parity % 2){
local_cw++;
};

return(local_cw);

};
