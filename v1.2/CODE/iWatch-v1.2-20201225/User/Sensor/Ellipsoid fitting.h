#ifndef _ELLIPSOID_FITTING_H
#define _ELLIPSOID_FITTING_H

struct cal_data{
	int X0;
	int Y0;
	int Z0;
	float A;
	float B;
	float C;
};

void ResetMatrix();
void CalcData_Input(float x, float y, float z);
unsigned char Ellipsoid_fitting_Process(struct cal_data *dat);

#endif