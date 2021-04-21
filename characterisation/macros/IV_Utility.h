// !TODO: All set
//
#pragma once
#include "style.C"
#include "Datasheet.h"
//
//>>    Reference Errors from Ferrara
//
double belowI[6] = {2.e-9, 20.e-9, 200.e-9, 2.e-6, 20.e-6, 200.e-6};
double percI[6] = {0.003, 0.002, 0.0015, 0.0015, 0.001, 0.001};
double fixedI[6] = {400.e-15, 1.e-12, 10.e-12, 100.e-12, 1.e-9, 10.e-9};

double percV[1] = {0.001};
double fixedV[1] = {4.e-3};

//>>    Errors on Keithley #####


TGraphErrors *IV(const char *fname, bool invert = false);

TGraphErrors *sqrtIV(const TGraphErrors *gIV);
TGraphErrors *sqrtIV(const char *fname);
TGraphErrors *D(const TGraphErrors *gIV);
TGraphErrors *D(const char *fname);
TGraphErrors *LD(const TGraphErrors *gIV);
TGraphErrors *LD(const char *fname);
TGraphErrors *ILD(const TGraphErrors *gIV);
TGraphErrors *ILD(const char *fname);
double Vbd(const TGraphErrors *gIV, int method = 3, double vbd_ini = 24.5);
double Vbd(const char *fname, int method = 3, double vbd_ini = 24.5);
std::pair<double,double> Rq(TGraphErrors *gIV, double fitmin, double fitmax);
std::pair<double,double> Rq(const char *fname, double fitmin = 2, double fitmax = 3);
TGraphErrors *sqrtIV(const TGraphErrors *gIV);

std::pair<double,double> Rq(TGraphErrors *gIV, double fitmin, double fitmax);

TGraphErrors *D(const TGraphErrors *gIV);

TGraphErrors *LD(const TGraphErrors *gIV);
TGraphErrors *ILD(const TGraphErrors *gIV);

void
drawA1();
void
drawA1B1();


//TGraphErrors *
//moving_average(TGraphErrors *gin, int n = 2);
TGraphErrors *
average(TGraphErrors *gin, int n = 2);
double
Vbd(const TGraphErrors *gIV, int method, double vbd_ini);
