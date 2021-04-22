// !TODO: All set
//
#pragma once
#include "style.C"
#include "Datasheet.h"
//
//>>    Set Instrumental Errors on IV curves
TGraphErrors   *fSetErrorsOnIV          ( TGraphErrors *,    Double_t);
TGraphErrors   *fSetErrorsOnIV          ( const char   *fFileName,  Double_t fScale =   1. )            { return fSetErrorsOnIV(new TGraphErrors(fFileName), fScale); }
TGraphErrors   *IV                      ( TGraphErrors *fTarget,    Double_t fScale =   1. )            { return fSetErrorsOnIV(fTarget, fScale); }
TGraphErrors   *IV                      ( const char   *fFileName,  Double_t fScale =   1. )            { return fSetErrorsOnIV(new TGraphErrors(fFileName), fScale); }
//
//>>    Get a copy of the Graph with the sqrt of Y-values
TGraphErrors   *fGetYsqrtOnIV           ( const TGraphErrors * );
TGraphErrors   *fGetYsqrtOnIV           ( const char   *fFileName )                                     { return fGetYsqrtOnIV(fSetErrorsOnIV(fFileName)); }
TGraphErrors   *sqrtIV                  ( TGraphErrors *fTarget )                                       { return fGetYsqrtOnIV(fTarget); }
TGraphErrors   *sqrtIV                  ( const char   *fFileName )                                     { return fGetYsqrtOnIV(fSetErrorsOnIV(fFileName)); }
//
//>>    Get a copy of the Graph with the derivative of Y-values
TGraphErrors   *fGetYderivativeOnIV     ( const TGraphErrors * );
TGraphErrors   *fGetYderivativeOnIV     ( const char   *fFileName )                                     { return fGetYderivativeOnIV(fSetErrorsOnIV(fFileName)); }
TGraphErrors   *D                       ( TGraphErrors *fTarget )                                       { return fGetYderivativeOnIV(fTarget); }
TGraphErrors   *D                       ( const char   *fFileName   )                                   { return fGetYderivativeOnIV(fSetErrorsOnIV(fFileName)); }
//
//>>    Get a copy of the Graph with the log derivative of Y-values
TGraphErrors   *fGetYlogDerivOnIV       ( const TGraphErrors *, Bool_t );
TGraphErrors   *fGetYlogDerivOnIV       ( const char   *fFileName, Bool_t fInverse = false )            { return fGetYlogDerivOnIV(fSetErrorsOnIV(fFileName),fInverse); }
TGraphErrors   *LD                      ( const TGraphErrors *fTarget )                                 { return fGetYlogDerivOnIV(fTarget,false); }
TGraphErrors   *LD                      ( const char *fFileName )                                       { return fGetYlogDerivOnIV(fSetErrorsOnIV(fFileName),false); }
TGraphErrors   *ILD                     ( const TGraphErrors *fTarget )                                 { return fGetYlogDerivOnIV(fTarget,true); }
TGraphErrors   *ILD                     ( const char *fFileName )                                       { return fGetYlogDerivOnIV(fSetErrorsOnIV(fFileName),true); }
//
//>>    Get a copy of the Graph with the derivative of Y-values
Double_t        fFindBreakDownV         ( const TGraphErrors *,     int,            double);
Double_t        fFindBreakDownV         ( const char   *fFileName,  int method = 3, double vbd_ini = 24.5){ return fFindBreakDownV(fSetErrorsOnIV(fFileName),method,vbd_ini); }
Double_t        Vdb                     ( TGraphErrors *fTarget,    int method = 3, double vbd_ini = 24.5){ return fFindBreakDownV(fTarget, method, vbd_ini); }
Double_t        Vdb                     ( const char   *fFileName,  int method = 3, double vbd_ini = 24.5){ return fFindBreakDownV(new TGraphErrors(fFileName), method, vbd_ini); }
//
//>>    Get a copy of the Graph with the log derivative of Y-values
TGraphErrors   *fGetAverage             ( const TGraphErrors *, Int_t );
TGraphErrors   *fGetAverage             ( const char   *fFileName, Int_t fNpoints ) { return fGetAverage(fSetErrorsOnIV(fFileName),fNpoints); }
TGraphErrors   *average                 ( const char   *fFileName, Int_t fNpoints ) { return fGetAverage(fSetErrorsOnIV(fFileName),fNpoints); }
//
//>>    Get a copy of the Graph with the log derivative of Y-values
TGraphErrors   *fGetMovingAvg           ( const TGraphErrors *, Int_t );
TGraphErrors   *fGetMovingAvg           ( const char   *fFileName, Int_t fNpoints ) { return fGetMovingAvg(fSetErrorsOnIV(fFileName),fNpoints); }
TGraphErrors   *moving_average          ( const char   *fFileName, Int_t fNpoints ) { return fGetMovingAvg(fSetErrorsOnIV(fFileName),fNpoints); }
//
//>>    Draw a bunch of histos
void            drawA1                  ();
void            drawA1B1                ();
//
std::pair<double,double> fFindQuenchingResist (TGraphErrors *gIV, double fitmin, double fitmax);
std::pair<double,double> Rq(TGraphErrors *gIV, double fitmin, double fitmax)                { return fFindQuenchingResist(gIV,fitmin,fitmax); }
std::pair<double,double> Rq(const char *fFileName, double fitmin = 2, double fitmax = 3)    { return fFindQuenchingResist(fSetErrorsOnIV(fFileName),fitmin,fitmax); }
