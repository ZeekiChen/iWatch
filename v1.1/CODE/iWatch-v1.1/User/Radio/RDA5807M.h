#ifndef __RDA5807M_H_
#define __RDA5807M_H_

#define RDA5807M_IIC_ADDR	0x20

#define	DMUTE			0x4000
#define SEEK_UP		0x0200
#define	SEEK			0x0100
#define	SOFT_RST	0x0002
#define	POWER_EN	0x0001
#define	TUNE			0x0010

#define	UPWARD		1
#define	DOWNWARD	0

void RDA5807MWriteRegs(void);
void RDA5807MReadRges(void);
unsigned char RDA5807MInit(void);
void RDA5807MSetVOLUME(unsigned int VOLUME);
float RDA5807MSEEK(unsigned char direction);
void RDA5807MSetFq(float fq);
float RDA5807MGetCurrentFq(void);
unsigned char RDA5807MGetCurrentRSSI(void);
void RDA5807MSetMute(void);
void RDA5807MPowerUp(void);
void RDA5807MPowerDown(void);

#endif