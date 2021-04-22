// !TODO: Take a look at the functions with the TODO tag "// ! TODO !"
//
#include "IV_Utility.h"
using namespace datasheet;
//
TGraphErrors   *fSetErrorsOnIV          ( TGraphErrors *fTarget,    Double_t fScale =   1. )            {
    //>>  Looping over points
    for ( Int_t iPnt = 0; iPnt < fTarget->GetN(); ++iPnt ) {
        double fCurrentError = 0., fVoltageError = 0.;
        //
        //>>    Current Error evaluation
        for ( Int_t iRng = 0; iRng < 6; ++iRng ) {
            if ( fTarget->GetY()[iPnt] < belowI[iRng]) {
                fCurrentError = percI[iRng] * fTarget->GetY()[iPnt] + fixedI[iRng];
                break;
            }
        }
        //
        //>>    Voltage Error evaluation
        fVoltageError = percV[0] * fTarget->GetX()[iPnt] + fixedV[0];
        //
        //>>    Setting Errors
        fTarget ->  SetPointError( iPnt, fVoltageError, fCurrentError );
        //
        //>>    Rescaling points
        fTarget ->  GetX()[iPnt] = fScale * fTarget->GetX()[iPnt];
        fTarget ->  GetY()[iPnt] = fScale * fTarget->GetY()[iPnt];
    }
    return fTarget;
}
//
TGraphErrors   *fGetYsqrtOnIV           ( const TGraphErrors *fTarget )                                 {
    auto fResult  = new TGraphErrors();
    for ( Int_t iPnt = 0; iPnt < fTarget->GetN(); ++iPnt )   {
        auto    fXvalue =   fTarget->GetPointX(iPnt);
        auto    fYvalue =   TMath::Sqrt(fTarget->GetPointY(iPnt));
        fResult ->  SetPoint( iPnt, fXvalue, fYvalue );
        // TODO: 1. Protection against negative Y values
        // TODO: 2. Correct error propagation
    }
    return fResult;
}      // ! TODO !
//
TGraphErrors   *fGetYderivativeOnIV     ( const TGraphErrors *fTarget )                                 {

    auto fResult  = new TGraphErrors();
    for ( Int_t iPnt = 0; iPnt < fTarget->GetN() -1; ++iPnt )   {
        auto    fXvalMin    =   fTarget->GetPointX(iPnt);
        auto    fYvalMin    =   fTarget->GetPointY(iPnt);
        auto    fXvalMax    =   fTarget->GetPointX(iPnt +1);
        auto    fYvalMax    =   fTarget->GetPointY(iPnt +1);
        auto    fXcenter    =   0.5*( fXvalMax + fXvalMin );
        auto    fYslop_     =   ( fYvalMax - fYvalMin )/( fXvalMax - fXvalMin );
        fResult ->  SetPoint( iPnt, fXcenter, fYslop_ );
        // TODO: 1. Protection against 0 divisor
        // TODO: 2. Correct error propagation
    }
    return fResult;
}      // ! TODO !
//
TGraphErrors   *fGetYlogDerivOnIV       ( const TGraphErrors *fTarget, Bool_t fInverse  =  false )      {
    auto fResult  = new TGraphErrors();
    for ( Int_t iPnt = 0; iPnt < fTarget->GetN() -1; ++iPnt )   {
        auto    fXvalMin    =   fTarget->GetPointX(iPnt);
        auto    fYvalMin    =   std::log(fTarget->GetPointY(iPnt));
        auto    fXerrMin    =   fTarget->GetErrorX(iPnt);
        auto    fYerrMin    =   fTarget->GetErrorY(iPnt);
        auto    fXvalMax    =   fTarget->GetPointX(iPnt +1);
        auto    fYvalMax    =   std::log(fTarget->GetErrorY(iPnt +1));
        auto    fYerrMax    =   fTarget->GetPointY(iPnt +1);
        auto    fXerrMax    =   fTarget->GetErrorX(iPnt +1);
        auto    fXcenter    =   0.5*( fXvalMax + fXvalMin );
        auto    fXerror_    =   0.5*TMath::Sqrt( fXerrMax*fXerrMax + fXerrMin*fXerrMin );
        auto    fYslop_     =   ( fYvalMax - fYvalMin )/( fXvalMax - fXvalMin );
        auto    fYErrSlp    =   TMath::Sqrt( fYerrMax*fYerrMax + fYerrMin*fYerrMin )/( fXvalMax - fXvalMin );
        fResult ->  SetPoint        ( iPnt, fXcenter, fYslop_ );
        if ( fInverse ) fResult ->  SetPoint        ( iPnt, fXcenter, 1./fYslop_ );
        fResult ->  SetPointError   ( iPnt, fXerror_, fYErrSlp );
        if ( fInverse ) fResult ->  SetPointError        ( iPnt, fXcenter, fYErrSlp/(fYslop_*fYslop_) );
        // TODO: 1. Protection against 0 divisor
        // TODO: 2. Correct error propagation (?)
    }
    return fResult;
}      // ! TODO !
//
TGraphErrors   *fGetAverage             ( const TGraphErrors *fTarget, Int_t fNpoints = 2 )             {
    auto fResult  = new TGraphErrors();
    for ( Int_t iPnt = 0; iPnt < fTarget->GetN() - fNpoints; iPnt += fNpoints )   {
        auto    fXvalue     =   0.;
        auto    fYvalue     =   0.;
        for ( Int_t iTer = iPnt; iTer < iPnt + fNpoints; ++iTer )   {
            fXvalue         +=  fTarget->GetPointX(iTer);
            fYvalue         +=  fTarget->GetPointY(iTer);
        }
        fXvalue             /=  (Float_t)fNpoints;
        fYvalue             /=  (Float_t)fNpoints;
        fResult ->  SetPoint( fResult->GetN(), fXvalue, fYvalue );
    }
    return fResult;
}
//
TGraphErrors   *fGetMovingAvg           ( const TGraphErrors *fTarget, Int_t fNpoints = 2  )            {
    auto fResult  = new TGraphErrors();
    for ( Int_t iPnt = 0; iPnt < fTarget->GetN() - fNpoints; ++iPnt )   {
        auto    fXvalue     =   0.;
        auto    fYvalue     =   0.;
        for ( Int_t iTer = iPnt; iTer < iPnt + fNpoints; ++iTer )   {
            fXvalue         +=  fTarget->GetPointX(iTer);
            fYvalue         +=  fTarget->GetPointY(iTer);
        }
        fXvalue             /=  (Float_t)fNpoints;
        fYvalue             /=  (Float_t)fNpoints;
        fResult ->  SetPoint( fResult->GetN(), fXvalue, fYvalue );
    }
    return fResult;
}
//
Double_t        fFindBreakDownV         ( const TGraphErrors *gIV, int method, double vbd_ini )         {
    switch ( method )   {
        default:    {
            std::cout << "[INFO] Using method SensL" << std::endl;
            auto    fCurrent_Vdb    =   vbd_ini;
            auto    gSqrtIV         =   fGetYsqrtOnIV(gIV);
            //
            //>>    Fit Sqrt(IV) for V > Vbd with pol1 until we reach stable value
            auto    fPol1           =   (TF1*)gROOT->GetFunction("pol1");
            //
            //>>    Fit Loop
            while ( true )  {
                // !TODO: Check Fit options (?)
                gSqrtIV->Fit( fPol1, "Q", "", fCurrent_Vdb + 0.5, fCurrent_Vdb + 1.5 );
                auto    fFound_Vbd  =   -fPol1->GetParameter(0) / fPol1->GetParameter(1);
                std::cout << "Starting Vbd: " << fCurrent_Vdb << " - New found Vdb: " << fFound_Vbd << " - Cut-off difference: " << 0.001 << std::endl;
                if ( fabs( fFound_Vbd - fCurrent_Vdb ) > 0.001 )    {
                    fCurrent_Vdb = fFound_Vbd;
                    continue;
                }
                //
                //>>    Vdb measurement has converged, plotting the results
                gSqrtIV     ->  Draw("ap*");
                fPol1       ->  SetLineStyle(kDashed);
                fPol1       ->  SetRange(24., 27.);
                fPol1       ->  Draw("same");
                return fFound_Vbd;
            }
        }
        case 1: {
            std::cout << "[INFO] Using method 1" << std::endl;
            auto gLogDeriv          =   fGetYlogDerivOnIV(gIV);
            gLogDeriv               =   fGetMovingAvg(gLogDeriv,10);
            Double_t    fYvalMax    =   0.;
            Double_t    fCurrent_Vdb=   -1.;    // Why not initialised to vbd_ini?
            for ( Int_t iPnt = 0; iPnt < gLogDeriv->GetN(); ++iPnt )   {
                if ( gLogDeriv->GetPointY(iPnt) < fYvalMax )    continue;
                if ( gLogDeriv->GetPointX(iPnt) < 27. )         continue;
                fYvalMax    =   gLogDeriv->GetPointY(iPnt);
                fCurrent_Vdb=   gLogDeriv->GetPointX(iPnt);
            }
            return  fCurrent_Vdb;
        }
        case 3: {
            std::cout << "[INFO] Using method 3" << std::endl;
            auto    fCurrent_Vdb    =   vbd_ini;
            auto    gInvLogIV       =   fGetYlogDerivOnIV(gIV,true);
            //
            //>>    Fit Sqrt(IV) for V > Vbd with pol1 until we reach stable value
            auto    fPol1           =   (TF1*)gROOT->GetFunction("pol1");
            //
            //>>    Fit Loop
            while ( true )  {
                // !TODO: Check Fit options (?)
                gInvLogIV->Fit( fPol1, "Q", "", fCurrent_Vdb + 0.5, fCurrent_Vdb + 3.0 );
                auto    fFound_Vbd  =   -fPol1->GetParameter(0) / fPol1->GetParameter(1);
                std::cout << "Starting Vbd: " << fCurrent_Vdb << " - New found Vdb: " << fFound_Vbd << " - Cut-off difference: " << 0.001 << std::endl;
                if ( fabs( fFound_Vbd - fCurrent_Vdb ) > 0.001 )    {
                    fCurrent_Vdb = fFound_Vbd;
                    continue;
                }
                //
                //>>    Vdb measurement has converged, plotting the results
                gInvLogIV   ->  Draw("ap*");
                fPol1       ->  SetLineStyle(kDashed);
                fPol1       ->  SetRange(24., 27.);
                fPol1       ->  Draw("same");
                return fFound_Vbd;
            }
        }
    }
    // !TODO: 1. The numbering scheme is inconsistent
    // !TODO: 2. NAming and labels for methods
}      // ! TODO ! // To be re-checked
//
std::pair<double,double>    fFindQuenchingResist    ( TGraphErrors *gIV, double fitmin, double fitmax)  {
  gIV->Draw("ap*");
  auto pol1 = (TF1 *)gROOT->GetFunction("pol1");
  gIV->Fit(pol1, "", "", fitmin, fitmax);
  auto R1 = pol1->GetParameter(1);
  auto eR1 = pol1->GetParError(1);
  auto R = 1. / R1;
  auto eR = 1. / R1 / R1 * eR1;
  return std::pair<double,double>(R, eR);
} // To be re-checked
//
void    drawA1()    {
  style();

  auto xmin = 23.;
  auto xmax = 27.;
  
  auto c = new TCanvas("c", "c", 1500, 500);
  c->Divide(3, 1);
  
  c->cd(1)->DrawFrame(xmin, 1.e-10, xmax, 5.e-8, ";V_{bias} (V); I (A)");
  c->cd(1)->SetLogy();

  auto gIV_22 = fSetErrorsOnIV("SensL_light-A1_22C.csv");
  gIV_22->SetMarkerStyle(20);
  gIV_22->SetMarkerSize(0.8);
  gIV_22->SetMarkerColor(kRed+1);
  gIV_22->SetLineColor(kRed+1);
  gIV_22->Draw("pc,same");

  auto gIV_22r = fSetErrorsOnIV("SensL_light-A1_22C_repeated.csv");
  gIV_22r->SetMarkerStyle(25);
  gIV_22r->SetMarkerSize(0.8);
  gIV_22r->SetMarkerColor(kRed+1);
  gIV_22r->SetLineColor(kRed+1);
  gIV_22r->Draw("pc,same");

  auto gIV_0 = fSetErrorsOnIV("SensL_light-A1_0C.csv");
  gIV_0->SetMarkerStyle(20);
  gIV_0->SetMarkerSize(0.8);
  gIV_0->SetMarkerColor(kGreen+2);
  gIV_0->SetLineColor(kGreen+2);
  gIV_0->Draw("pc,same");

  auto gIV_0r = fSetErrorsOnIV("SensL_light-A1_0C_repeated.csv");
  gIV_0r->SetMarkerStyle(25);
  gIV_0r->SetMarkerSize(0.8);
  gIV_0r->SetMarkerColor(kGreen+2);
  gIV_0r->SetLineColor(kGreen+2);
  gIV_0r->Draw("pc,same");

  auto gIV_m10 = fSetErrorsOnIV("SensL_light-A1_-10C.csv");
  gIV_m10->SetMarkerStyle(20);
  gIV_m10->SetMarkerSize(0.8);
  gIV_m10->SetMarkerColor(kAzure-3);
  gIV_m10->SetLineColor(kAzure-3);
  //  gIV_m10->Draw("pc,same");

  c->cd(2)->DrawFrame(xmin, -1., xmax, 9., ";V_{bias} (V);LD");

  auto gLD_22 = fGetYlogDerivOnIV("SensL_light-A1_22C.csv");
  gLD_22->SetMarkerStyle(20);
  gLD_22->SetMarkerSize(0.8);
  gLD_22->SetMarkerColor(kRed+1);
  gLD_22->SetLineColor(kRed+1);
  gLD_22->Draw("pc,same");

  auto gLD_22r = fGetYlogDerivOnIV("SensL_light-A1_22C_repeated.csv");
  gLD_22r->SetMarkerStyle(25);
  gLD_22r->SetMarkerSize(0.8);
  gLD_22r->SetMarkerColor(kRed+1);
  gLD_22r->SetLineColor(kRed+1);
  gLD_22r->Draw("pc,same");

  auto gLD_0 = fGetYlogDerivOnIV("SensL_light-A1_0C.csv");
  gLD_0->SetMarkerStyle(20);
  gLD_0->SetMarkerSize(0.8);
  gLD_0->SetMarkerColor(kGreen+2);
  gLD_0->SetLineColor(kGreen+2);
  gLD_0->Draw("pc,same");
  
  auto gLD_0r = fGetYlogDerivOnIV("SensL_light-A1_0C_repeated.csv");
  gLD_0r->SetMarkerStyle(25);
  gLD_0r->SetMarkerSize(0.8);
  gLD_0r->SetMarkerColor(kGreen+2);
  gLD_0r->SetLineColor(kGreen+2);
  gLD_0r->Draw("pc,same");
  
  auto gLD_m10 = fGetYlogDerivOnIV("SensL_light-A1_-10C.csv");
  gLD_m10->SetMarkerStyle(20);
  gLD_m10->SetMarkerSize(0.8);
  gLD_m10->SetMarkerColor(kAzure-3);
  gLD_m10->SetLineColor(kAzure-3);
  gLD_m10->Draw("pc,same");
  
  c->cd(3)->DrawFrame(xmin, -1., xmax, 9., ";V_{bias} (V);ILD");

  auto gILD_22 = fGetYlogDerivOnIV("SensL_light-A1_22C.csv",TRUE);
  gILD_22->SetMarkerStyle(20);
  gILD_22->SetMarkerSize(0.8);
  gILD_22->SetMarkerColor(kRed+1);
  gILD_22->SetLineColor(kRed+1);
  gILD_22->Draw("pc,same");

  auto gILD_22r = fGetYlogDerivOnIV("SensL_light-A1_22C_repeated.csv",TRUE);
  gILD_22r->SetMarkerStyle(25);
  gILD_22r->SetMarkerSize(0.8);
  gILD_22r->SetMarkerColor(kRed+1);
  gILD_22r->SetLineColor(kRed+1);
  gILD_22r->Draw("pc,same");

  auto gILD_0 = fGetYlogDerivOnIV("SensL_light-A1_0C.csv",TRUE);
  gILD_0->SetMarkerStyle(20);
  gILD_0->SetMarkerSize(0.8);
  gILD_0->SetMarkerColor(kGreen+2);
  gILD_0->SetLineColor(kGreen+2);
  gILD_0->Draw("pc,same");
  
  auto gILD_0r = fGetYlogDerivOnIV("SensL_light-A1_0C_repeated.csv",TRUE);
  gILD_0r->SetMarkerStyle(25);
  gILD_0r->SetMarkerSize(0.8);
  gILD_0r->SetMarkerColor(kGreen+2);
  gILD_0r->SetLineColor(kGreen+2);
  gILD_0r->Draw("pc,same");
  
  auto gILD_m10 = fGetYlogDerivOnIV("SensL_light-A1_-10C.csv",TRUE);
  gILD_m10->SetMarkerStyle(20);
  gILD_m10->SetMarkerSize(0.8);
  gILD_m10->SetMarkerColor(kAzure-3);
  gILD_m10->SetLineColor(kAzure-3);
  gILD_m10->Draw("pc,same");
  
}
//
void    drawA1B1()  {

  style();
  
  auto c = new TCanvas("c", "c", 1500, 500);
  c->Divide(3, 1);
  
  c->cd(1)->DrawFrame(23, 1.e-10, 27., 5.e-8, ";V_{bias} (V); I (A)");
  c->cd(1)->SetLogy();

  auto gIV_22_A1 = fSetErrorsOnIV("SensL_light-A1_22C.csv");
  gIV_22_A1->SetMarkerStyle(20);
  gIV_22_A1->SetMarkerSize(0.8);
  gIV_22_A1->SetMarkerColor(kRed+1);
  gIV_22_A1->SetLineColor(kRed+1);
  gIV_22_A1->Draw("pc,same");

  auto gIV_22_B1 = fSetErrorsOnIV("SensL_light-B1_22C.csv");
  gIV_22_B1->SetMarkerStyle(25);
  gIV_22_B1->SetMarkerSize(0.8);
  gIV_22_B1->SetMarkerColor(kRed+1);
  gIV_22_B1->SetLineColor(kRed+1);
  gIV_22_B1->Draw("pc,same");

  auto gIV_0_A1 = fSetErrorsOnIV("SensL_light-A1_0C.csv");
  gIV_0_A1->SetMarkerStyle(20);
  gIV_0_A1->SetMarkerSize(0.8);
  gIV_0_A1->SetMarkerColor(kGreen+2);
  gIV_0_A1->SetLineColor(kGreen+2);
  gIV_0_A1->Draw("pc,same");

  auto gIV_0_B1 = fSetErrorsOnIV("SensL_light-B1_0C.csv");
  gIV_0_B1->SetMarkerStyle(25);
  gIV_0_B1->SetMarkerSize(0.8);
  gIV_0_B1->SetMarkerColor(kGreen+2);
  gIV_0_B1->SetLineColor(kGreen+2);
  gIV_0_B1->Draw("pc,same");

  auto gIV_m10_A1 = fSetErrorsOnIV("SensL_light-A1_-10C.csv");
  gIV_m10_A1->SetMarkerStyle(20);
  gIV_m10_A1->SetMarkerSize(0.8);
  gIV_m10_A1->SetMarkerColor(kAzure-3);
  gIV_m10_A1->SetLineColor(kAzure-3);
  //  gIV_m10_A1->Draw("pc,same");

  auto gIV_m10_B1 = fSetErrorsOnIV("SensL_light-B1_-10C.csv");
  gIV_m10_B1->SetMarkerStyle(25);
  gIV_m10_B1->SetMarkerSize(0.8);
  gIV_m10_B1->SetMarkerColor(kAzure-3);
  gIV_m10_B1->SetLineColor(kAzure-3);
//  gIV_m10_B1->Draw("pc,same");

  c->cd(2)->DrawFrame(23, -1., 27., 9., ";V_{bias} (V);LD");

  auto gLD_22_A1 = fGetYlogDerivOnIV("SensL_light-A1_22C.csv");
  gLD_22_A1->SetMarkerStyle(20);
  gLD_22_A1->SetMarkerSize(0.8);
  gLD_22_A1->SetMarkerColor(kRed+1);
  gLD_22_A1->SetLineColor(kRed+1);
  gLD_22_A1->Draw("pc,same");

  auto gLD_22_B1 = fGetYlogDerivOnIV("SensL_light-B1_22C.csv");
  gLD_22_B1->SetMarkerStyle(25);
  gLD_22_B1->SetMarkerSize(0.8);
  gLD_22_B1->SetMarkerColor(kRed+1);
  gLD_22_B1->SetLineColor(kRed+1);
  gLD_22_B1->Draw("pc,same");

  auto gLD_0_A1 = fGetYlogDerivOnIV("SensL_light-A1_0C.csv");
  gLD_0_A1->SetMarkerStyle(20);
  gLD_0_A1->SetMarkerSize(0.8);
  gLD_0_A1->SetMarkerColor(kGreen+2);
  gLD_0_A1->SetLineColor(kGreen+2);
  gLD_0_A1->Draw("pc,same");
  
  auto gLD_0_B1 = fGetYlogDerivOnIV("SensL_light-B1_0C.csv");
  gLD_0_B1->SetMarkerStyle(25);
  gLD_0_B1->SetMarkerSize(0.8);
  gLD_0_B1->SetMarkerColor(kGreen+2);
  gLD_0_B1->SetLineColor(kGreen+2);
  gLD_0_B1->Draw("pc,same");
  
  auto gLD_m10_A1 = fGetYlogDerivOnIV("SensL_light-A1_-10C.csv");
  gLD_m10_A1->SetMarkerStyle(20);
  gLD_m10_A1->SetMarkerSize(0.8);
  gLD_m10_A1->SetMarkerColor(kAzure-3);
  gLD_m10_A1->SetLineColor(kAzure-3);
  //  gLD_m10_A1->Draw("pc,same");
  
  auto gLD_m10_B1 = fGetYlogDerivOnIV("SensL_light-B1_-10C.csv");
  gLD_m10_B1->SetMarkerStyle(20);
  gLD_m10_B1->SetMarkerSize(0.8);
  gLD_m10_B1->SetMarkerColor(kAzure-3);
  gLD_m10_B1->SetLineColor(kAzure-3);
  //  gLD_m10_B1->Draw("pc,same");
  
  c->cd(3)->DrawFrame(23, -1., 27., 9., ";V_{bias} (V);ILD");

  auto gILD_22_A1 = fGetYlogDerivOnIV("SensL_light-A1_22C.csv",TRUE);
  gILD_22_A1->SetMarkerStyle(20);
  gILD_22_A1->SetMarkerSize(0.8);
  gILD_22_A1->SetMarkerColor(kRed+1);
  gILD_22_A1->SetLineColor(kRed+1);
  gILD_22_A1->Draw("pc,same");

  auto gILD_22_B1 = fGetYlogDerivOnIV("SensL_light-B1_22C.csv",TRUE);
  gILD_22_B1->SetMarkerStyle(25);
  gILD_22_B1->SetMarkerSize(0.8);
  gILD_22_B1->SetMarkerColor(kRed+1);
  gILD_22_B1->SetLineColor(kRed+1);
  gILD_22_B1->Draw("pc,same");

  auto gILD_0_A1 = fGetYlogDerivOnIV("SensL_light-A1_0C.csv",TRUE);
  gILD_0_A1->SetMarkerStyle(20);
  gILD_0_A1->SetMarkerSize(0.8);
  gILD_0_A1->SetMarkerColor(kGreen+2);
  gILD_0_A1->SetLineColor(kGreen+2);
  gILD_0_A1->Draw("pc,same");
  
  auto gILD_0_B1 = fGetYlogDerivOnIV("SensL_light-B1_0C.csv",TRUE);
  gILD_0_B1->SetMarkerStyle(25);
  gILD_0_B1->SetMarkerSize(0.8);
  gILD_0_B1->SetMarkerColor(kGreen+2);
  gILD_0_B1->SetLineColor(kGreen+2);
  gILD_0_B1->Draw("pc,same");
  
  auto gILD_m10_A1 = fGetYlogDerivOnIV("SensL_light-A1_-10C.csv",TRUE);
  gILD_m10_A1->SetMarkerStyle(20);
  gILD_m10_A1->SetMarkerSize(0.8);
  gILD_m10_A1->SetMarkerColor(kAzure-3);
  gILD_m10_A1->SetLineColor(kAzure-3);
  //  gILD_m10_A1->Draw("pc,same");
  
  auto gILD_m10_B1 = fGetYlogDerivOnIV("SensL_light-B1_-10C.csv",TRUE);
  gILD_m10_B1->SetMarkerStyle(25);
  gILD_m10_B1->SetMarkerSize(0.8);
  gILD_m10_B1->SetMarkerColor(kAzure-3);
  gILD_m10_B1->SetLineColor(kAzure-3);
  //  gILD_m10_B1->Draw("pc,same");
  
}
