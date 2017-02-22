/*
 * untitled
 * Exported from happybday.gbt by GBA Tracker
 * Mon Nov 09 02:28:42 EST 2015
 */

// Defines for the interrupt handler
#define REG_IE        *(volatile unsigned short*) 0x4000200
#define REG_IF        *(volatile unsigned short*) 0x4000202
#define REG_IME       *(volatile unsigned short*) 0x4000208
#define IRQ_ENABLE    1
#define IRQ_VBLANK    (1 << 0)
#define IRQ_HBLANK    (1 << 1)
#define IRQ_VCOUNT    (1 << 2)
#define IRQ_TIMER(n)  (1 << (3 + (n)))
#define IRQ_COM       (1 << 7)
#define IRQ_DMA(n)    (1 << (8 + (n)))
#define IRQ_KEYPAD    (1 << 12)
#define IRQ_CARTRIDGE (1 << 13)
typedef void (*irqptr)(void);
#define REG_ISR_MAIN *(irqptr*) 0x3007FFC

// Defines for the timer
#define REG_TMD(n)   *(volatile unsigned short*) (0x4000100 + ((n) << 2))
#define REG_TMCNT(n) *(volatile unsigned short*) (0x4000102 + ((n) << 2))
#define TM_FREQ_1    0
#define TM_FREQ_64   1
#define TM_FREQ_256  2
#define TM_FREQ_1024 3
#define TM_CASCADE   (1 << 2)
#define TM_IRQ       (1 << 6)
#define TM_ENABLE    (1 << 7)

// Defines for sound control
#define REG_SND1SWP   *(volatile unsigned short*) 0x4000060 
#define REG_SND1ENV   *(volatile unsigned short*) 0x4000062
#define REG_SND1FRQ   *(volatile unsigned short*) 0x4000064
#define REG_SND2ENV   *(volatile unsigned short*) 0x4000068
#define REG_SND2FRQ   *(volatile unsigned short*) 0x400006C
#define REG_SND4ENV   *(volatile unsigned short*) 0x4000078
#define REG_SND4FRQ   *(volatile unsigned short*) 0x400007C
#define REG_SNDDMGCNT *(volatile unsigned short*) 0x4000080
#define REG_SNDSTAT   *(volatile unsigned short*) 0x4000084
#define MASTER_SND_EN (1 << 7)
#define MASTER_VOL25  0
#define MASTER_VOL50  1
#define MASTER_VOL100 2
#define LEFT_VOL(n)   (n)
#define RIGHT_VOL(n)  ((n) << 4)
#define CHAN_EN_L(n)  (1 << ((n) + 7))
#define CHAN_EN_R(n)  (1 << ((n) + 11))

#define BPM_QUAN_TRANSFORM -(0x14000/50)

// Defines and global variables used by the music handler
typedef struct {
	unsigned short CH1_SWP;
	unsigned short CH1_ENV;
	unsigned short CH1_FRQ;
	unsigned short CH2_ENV;
	unsigned short CH2_FRQ;
	unsigned short CH4_ENV;
	unsigned short CH4_FRQ;
	unsigned short LEN;
} note_t;

static void untitled_step();
static const note_t sound_data[12];
static int index, wait;

// Begin playing the music
void untitled_play() {

	REG_IME = 0;
	REG_ISR_MAIN = untitled_step;
	REG_IE = IRQ_TIMER(0);

	REG_TMD(0) = BPM_QUAN_TRANSFORM;
	REG_TMCNT(0) = TM_ENABLE | TM_IRQ | TM_FREQ_1024;

	REG_SNDSTAT = MASTER_SND_EN;
	REG_SNDDMGCNT = CHAN_EN_L(1) |
	                CHAN_EN_R(1) |
	                CHAN_EN_L(2) |
	                CHAN_EN_R(2) |
	                CHAN_EN_L(4) |
	                CHAN_EN_R(4) |
	                LEFT_VOL(7)  |
	                RIGHT_VOL(7);

	index = 0;
	wait = 1;
	
	REG_IME = IRQ_ENABLE;
}

// Stop playing the music
void untitled_stop() {
	REG_IME &= ~IRQ_ENABLE;
	REG_SNDSTAT &= ~MASTER_SND_EN;
}

// Play one 48th of a measure of music
static void untitled_step() {
	REG_IME = 0;
	int halt = 0;
	switch(REG_IF) {
	case IRQ_TIMER(0) :

		if(!--wait) {

			if(sound_data[index].CH1_FRQ) {
				REG_SND1SWP = sound_data[index].CH1_SWP;
				REG_SND1ENV = sound_data[index].CH1_ENV;
				REG_SND1FRQ = sound_data[index].CH1_FRQ;
			}
	
			if(sound_data[index].CH2_FRQ) {
				REG_SND2ENV = sound_data[index].CH2_ENV;
				REG_SND2FRQ = sound_data[index].CH2_FRQ;
			}
	
			if(sound_data[index].CH4_FRQ) {
				REG_SND4ENV = sound_data[index].CH4_ENV;
				REG_SND4FRQ = sound_data[index].CH4_FRQ;
			}
			
			wait = sound_data[index].LEN;
	
			if(++index == 12) {
				if(1) {
					index = 0;
				} else {
					halt = 1;
				}
			}
		}

		break;
	default:
		break;
	}

	REG_IF = REG_IF;
	if(!halt) {
		REG_IME = IRQ_ENABLE;
	}
}

// Note data
static const note_t sound_data[12] = {
	{0x0008,0xF140,0x860C,0x0000,0x0000,0x0000,0x0000,0x0006},
	{0x0008,0xF140,0x860C,0x0000,0x0000,0x0000,0x0000,0x0006},
	{0x0008,0xF340,0x860C,0x0000,0x0000,0x0000,0x0000,0x000C},
	{0x0008,0xF340,0x860C,0x0000,0x0000,0x0000,0x0000,0x000C},
	{0x0008,0xF340,0x8589,0x0000,0x0000,0x0000,0x0000,0x000C},
	{0x0008,0xF340,0x853C,0x0000,0x0000,0x0000,0x0000,0x000C},
	{0x0008,0xF340,0x86C5,0x0000,0x0000,0x0000,0x0000,0x0006},
	{0x0008,0xF340,0x86C5,0x0000,0x0000,0x0000,0x0000,0x0006},
	{0x0008,0xF340,0x86C5,0x0000,0x0000,0x0000,0x0000,0x000C},
	{0x0008,0xF340,0x86C5,0x0000,0x0000,0x0000,0x0000,0x000C},
	{0x0008,0xF340,0x86E7,0x0000,0x0000,0x0000,0x0000,0x000C},
	{0x0008,0xF340,0x85CE,0x0000,0x0000,0x0000,0x0000,0x0006}
};
