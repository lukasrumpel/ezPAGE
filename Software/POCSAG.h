//(2020) DO4LR based on the library by ON1ARF
//80 characters

#ifndef POCSAG_H
#define POCSAG_H

#define sync_cw0 0x7c
#define sync_cw1 0xd2
#define sync_cw2 0x15
#define sync_cw3 0xd8
#define sync_inv 0xff
#define idle_cw 0x7a89c197

class Pocsag {
public:

// enum's:
typedef enum {
	POCSAGRC_UNDETERMINED = 0,
	POCSAGRC_UNGUELTIGEADRESSE,
	POCSAGRC_UNGUELTIGEQUELLE,
	POCSAGRC_UNGUELTIGEBATCH2OPT,
	POCSAGRC_UNGUELTIGEINVERTOPT
} Pocsag_error;

typedef enum {
	POCSAG_GESCHEITERT = 0, // failed
	POCSAG_ERFOLG // success
} Pocsag_rc;


// structures
// a 2-batch pocsag message
typedef struct {
	unsigned char sync[72];

	// batch 1
	unsigned char synccw1[4];
	uint32_t batch1[16];

	// batch 2
	unsigned char synccw2[4];
	uint32_t batch2[16];

	//batch 3
	unsigned char synccw3[4];
	uint32_t batch3[16];

    //batch 4
	unsigned char synccw4[4];
	uint32_t batch4[16];

} Pocsagmsg_s; // end struct


// data
	Pocsagmsg_s Pocsagmsg;

// methodes

	Pocsag();


	int CreatePocsag (long int, int, char *);


	int GetState ();


	int GetSize ();


	int GetError ();


	void * GetMsgPointer();

private:
	// vars
	int state;
	int size;
	int error;

	uint32_t createcrc(uint32_t);
	unsigned char flip7charbitorder (unsigned char);
	void replaceline (Pocsag::Pocsagmsg_s *, int, uint32_t);

};
#endif
