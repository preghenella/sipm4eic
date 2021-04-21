#include "IV_Utility.h"

TGraphErrors *IV(const char *fname, bool invert = false)
{
  auto gIV = new TGraphErrors(fname);

  // add errors
  for (int i = 0; i < gIV->GetN() ; ++i) {

    double errorI = 0., errorV = 0.;
    
    // error on I
    for (int j = 0; j < 6; ++j) {
      if (gIV->GetY()[i] < belowI[j]) {
	errorI = percI[j] * gIV->GetY()[i] + fixedI[j];
	break;
      }
    }
    // error on V
    errorV = percV[0] * gIV->GetX()[i] + fixedV[0];
    
    gIV->SetPointError(i, errorV, errorI);
    
  }

  if (invert) {
    for (int i = 0; i < gIV->GetN() ; ++i) {
      gIV->GetX()[i] = -gIV->GetX()[i];
      gIV->GetY()[i] = -gIV->GetY()[i];
    }
  }
  return gIV;
}

TGraphErrors *sqrtIV(const TGraphErrors *gIV);
TGraphErrors *sqrtIV(const char *fname) { return sqrtIV(IV(fname)); }

TGraphErrors *D(const TGraphErrors *gIV);
TGraphErrors *D(const char *fname)  { return D(IV(fname)); }

TGraphErrors *LD(const TGraphErrors *gIV);
TGraphErrors *LD(const char *fname)  { return LD(IV(fname)); }

TGraphErrors *ILD(const TGraphErrors *gIV);
TGraphErrors *ILD(const char *fname) { return ILD(IV(fname)); }

double Vbd(const char *fname, int method = 3, double vbd_ini = 24.5) { return Vbd(IV(fname), method, vbd_ini); }

std::pair<double,double> Rq(const char *fname, double fitmin = 2, double fitmax = 3) { return Rq(IV(fname, true), fitmin, fitmax); }

TGraphErrors *sqrtIV(const TGraphErrors *gIV) {
  auto gsqrtIV = new TGraphErrors;
  for (int i = 0; i < gIV->GetN() ; ++i) {
    auto x = gIV->GetX()[i];
    auto y = std::sqrt(gIV->GetY()[i]);
    gsqrtIV->SetPoint(i, x, y);
  }
  return gsqrtIV;  
}

std::pair<double,double> Rq(TGraphErrors *gIV, double fitmin, double fitmax)
{
  gIV->Draw("ap*");
  auto pol1 = (TF1 *)gROOT->GetFunction("pol1");
  gIV->Fit(pol1, "", "", fitmin, fitmax);
  auto R1 = pol1->GetParameter(1);
  auto eR1 = pol1->GetParError(1);
  auto R = 1. / R1;
  auto eR = 1. / R1 / R1 * eR1;
  return std::pair<double,double>(R, eR);
}

TGraphErrors *D(const TGraphErrors *gIV) {
  auto gD = new TGraphErrors;
  for (int i = 0; i < gIV->GetN() - 1; ++i) {
    auto xmin = gIV->GetX()[i];
    auto xmax = gIV->GetX()[i + 1];
    auto ymin = gIV->GetY()[i];
    auto ymax = gIV->GetY()[i + 1];
    auto x = 0.5 * (xmax + xmin);
    auto y = (ymax - ymin) / (xmax - xmin);
    gD->SetPoint(i, x, y);
  }
  return gD;
}

TGraphErrors *LD(const TGraphErrors *gIV) {
  auto gLD = new TGraphErrors;
  for (int i = 0; i < gIV->GetN() - 1; ++i) {
    auto xmin = gIV->GetX()[i];
    auto xmax = gIV->GetX()[i + 1];
    auto exmin = gIV->GetEX()[i];
    auto exmax = gIV->GetEX()[i + 1];
    auto ymin = std::log(gIV->GetY()[i]);
    auto ymax = std::log(gIV->GetY()[i + 1]);
    auto eymin = gIV->GetEY()[i] / gIV->GetY()[i]; 
    auto eymax = gIV->GetEY()[i + 1] / gIV->GetY()[i + 1];
    auto x = 0.5 * (xmax + xmin);
    auto ex = 0.5 * std::sqrt(exmin * exmin + exmax * exmax);
    auto y = (ymax - ymin) / (xmax - xmin);
    auto ey = std::sqrt(eymax * eymax + eymin * eymin) / (xmax - xmin);
    gLD->SetPoint(i, x, y);
    gLD->SetPointError(i, ex, ey);
  }
  return gLD;
}

TGraphErrors *ILD(const TGraphErrors *gIV) {
  auto gLD = LD(gIV);
  auto gILD = new TGraphErrors;
  for (int i = 0; i < gLD->GetN(); ++i) {
    auto x = gLD->GetX()[i];
    auto y = 1. / gLD->GetY()[i];
    gILD->SetPoint(i, x, y);
  }
  return gILD;
}

void
drawA1()
{

  style();

  auto xmin = 23.;
  auto xmax = 27.;
  
  auto c = new TCanvas("c", "c", 1500, 500);
  c->Divide(3, 1);
  
  c->cd(1)->DrawFrame(xmin, 1.e-10, xmax, 5.e-8, ";V_{bias} (V); I (A)");
  c->cd(1)->SetLogy();

  auto gIV_22 = IV("SensL_light-A1_22C.csv");
  gIV_22->SetMarkerStyle(20);
  gIV_22->SetMarkerSize(0.8);
  gIV_22->SetMarkerColor(kRed+1);
  gIV_22->SetLineColor(kRed+1);
  gIV_22->Draw("pc,same");

  auto gIV_22r = IV("SensL_light-A1_22C_repeated.csv");
  gIV_22r->SetMarkerStyle(25);
  gIV_22r->SetMarkerSize(0.8);
  gIV_22r->SetMarkerColor(kRed+1);
  gIV_22r->SetLineColor(kRed+1);
  gIV_22r->Draw("pc,same");

  auto gIV_0 = IV("SensL_light-A1_0C.csv");
  gIV_0->SetMarkerStyle(20);
  gIV_0->SetMarkerSize(0.8);
  gIV_0->SetMarkerColor(kGreen+2);
  gIV_0->SetLineColor(kGreen+2);
  gIV_0->Draw("pc,same");

  auto gIV_0r = IV("SensL_light-A1_0C_repeated.csv");
  gIV_0r->SetMarkerStyle(25);
  gIV_0r->SetMarkerSize(0.8);
  gIV_0r->SetMarkerColor(kGreen+2);
  gIV_0r->SetLineColor(kGreen+2);
  gIV_0r->Draw("pc,same");

  auto gIV_m10 = IV("SensL_light-A1_-10C.csv");
  gIV_m10->SetMarkerStyle(20);
  gIV_m10->SetMarkerSize(0.8);
  gIV_m10->SetMarkerColor(kAzure-3);
  gIV_m10->SetLineColor(kAzure-3);
  //  gIV_m10->Draw("pc,same");

  c->cd(2)->DrawFrame(xmin, -1., xmax, 9., ";V_{bias} (V);LD");

  auto gLD_22 = LD("SensL_light-A1_22C.csv");
  gLD_22->SetMarkerStyle(20);
  gLD_22->SetMarkerSize(0.8);
  gLD_22->SetMarkerColor(kRed+1);
  gLD_22->SetLineColor(kRed+1);
  gLD_22->Draw("pc,same");

  auto gLD_22r = LD("SensL_light-A1_22C_repeated.csv");
  gLD_22r->SetMarkerStyle(25);
  gLD_22r->SetMarkerSize(0.8);
  gLD_22r->SetMarkerColor(kRed+1);
  gLD_22r->SetLineColor(kRed+1);
  gLD_22r->Draw("pc,same");

  auto gLD_0 = LD("SensL_light-A1_0C.csv");
  gLD_0->SetMarkerStyle(20);
  gLD_0->SetMarkerSize(0.8);
  gLD_0->SetMarkerColor(kGreen+2);
  gLD_0->SetLineColor(kGreen+2);
  gLD_0->Draw("pc,same");
  
  auto gLD_0r = LD("SensL_light-A1_0C_repeated.csv");
  gLD_0r->SetMarkerStyle(25);
  gLD_0r->SetMarkerSize(0.8);
  gLD_0r->SetMarkerColor(kGreen+2);
  gLD_0r->SetLineColor(kGreen+2);
  gLD_0r->Draw("pc,same");
  
  auto gLD_m10 = LD("SensL_light-A1_-10C.csv");
  gLD_m10->SetMarkerStyle(20);
  gLD_m10->SetMarkerSize(0.8);
  gLD_m10->SetMarkerColor(kAzure-3);
  gLD_m10->SetLineColor(kAzure-3);
  gLD_m10->Draw("pc,same");
  
  c->cd(3)->DrawFrame(xmin, -1., xmax, 9., ";V_{bias} (V);ILD");

  auto gILD_22 = ILD("SensL_light-A1_22C.csv");
  gILD_22->SetMarkerStyle(20);
  gILD_22->SetMarkerSize(0.8);
  gILD_22->SetMarkerColor(kRed+1);
  gILD_22->SetLineColor(kRed+1);
  gILD_22->Draw("pc,same");

  auto gILD_22r = ILD("SensL_light-A1_22C_repeated.csv");
  gILD_22r->SetMarkerStyle(25);
  gILD_22r->SetMarkerSize(0.8);
  gILD_22r->SetMarkerColor(kRed+1);
  gILD_22r->SetLineColor(kRed+1);
  gILD_22r->Draw("pc,same");

  auto gILD_0 = ILD("SensL_light-A1_0C.csv");
  gILD_0->SetMarkerStyle(20);
  gILD_0->SetMarkerSize(0.8);
  gILD_0->SetMarkerColor(kGreen+2);
  gILD_0->SetLineColor(kGreen+2);
  gILD_0->Draw("pc,same");
  
  auto gILD_0r = ILD("SensL_light-A1_0C_repeated.csv");
  gILD_0r->SetMarkerStyle(25);
  gILD_0r->SetMarkerSize(0.8);
  gILD_0r->SetMarkerColor(kGreen+2);
  gILD_0r->SetLineColor(kGreen+2);
  gILD_0r->Draw("pc,same");
  
  auto gILD_m10 = ILD("SensL_light-A1_-10C.csv");
  gILD_m10->SetMarkerStyle(20);
  gILD_m10->SetMarkerSize(0.8);
  gILD_m10->SetMarkerColor(kAzure-3);
  gILD_m10->SetLineColor(kAzure-3);
  gILD_m10->Draw("pc,same");
  
}

void
drawA1B1()
{

  style();
  
  auto c = new TCanvas("c", "c", 1500, 500);
  c->Divide(3, 1);
  
  c->cd(1)->DrawFrame(23, 1.e-10, 27., 5.e-8, ";V_{bias} (V); I (A)");
  c->cd(1)->SetLogy();

  auto gIV_22_A1 = IV("SensL_light-A1_22C.csv");
  gIV_22_A1->SetMarkerStyle(20);
  gIV_22_A1->SetMarkerSize(0.8);
  gIV_22_A1->SetMarkerColor(kRed+1);
  gIV_22_A1->SetLineColor(kRed+1);
  gIV_22_A1->Draw("pc,same");

  auto gIV_22_B1 = IV("SensL_light-B1_22C.csv");
  gIV_22_B1->SetMarkerStyle(25);
  gIV_22_B1->SetMarkerSize(0.8);
  gIV_22_B1->SetMarkerColor(kRed+1);
  gIV_22_B1->SetLineColor(kRed+1);
  gIV_22_B1->Draw("pc,same");

  auto gIV_0_A1 = IV("SensL_light-A1_0C.csv");
  gIV_0_A1->SetMarkerStyle(20);
  gIV_0_A1->SetMarkerSize(0.8);
  gIV_0_A1->SetMarkerColor(kGreen+2);
  gIV_0_A1->SetLineColor(kGreen+2);
  gIV_0_A1->Draw("pc,same");

  auto gIV_0_B1 = IV("SensL_light-B1_0C.csv");
  gIV_0_B1->SetMarkerStyle(25);
  gIV_0_B1->SetMarkerSize(0.8);
  gIV_0_B1->SetMarkerColor(kGreen+2);
  gIV_0_B1->SetLineColor(kGreen+2);
  gIV_0_B1->Draw("pc,same");

  auto gIV_m10_A1 = IV("SensL_light-A1_-10C.csv");
  gIV_m10_A1->SetMarkerStyle(20);
  gIV_m10_A1->SetMarkerSize(0.8);
  gIV_m10_A1->SetMarkerColor(kAzure-3);
  gIV_m10_A1->SetLineColor(kAzure-3);
  //  gIV_m10_A1->Draw("pc,same");

  auto gIV_m10_B1 = IV("SensL_light-B1_-10C.csv");
  gIV_m10_B1->SetMarkerStyle(25);
  gIV_m10_B1->SetMarkerSize(0.8);
  gIV_m10_B1->SetMarkerColor(kAzure-3);
  gIV_m10_B1->SetLineColor(kAzure-3);
//  gIV_m10_B1->Draw("pc,same");

  c->cd(2)->DrawFrame(23, -1., 27., 9., ";V_{bias} (V);LD");

  auto gLD_22_A1 = LD("SensL_light-A1_22C.csv");
  gLD_22_A1->SetMarkerStyle(20);
  gLD_22_A1->SetMarkerSize(0.8);
  gLD_22_A1->SetMarkerColor(kRed+1);
  gLD_22_A1->SetLineColor(kRed+1);
  gLD_22_A1->Draw("pc,same");

  auto gLD_22_B1 = LD("SensL_light-B1_22C.csv");
  gLD_22_B1->SetMarkerStyle(25);
  gLD_22_B1->SetMarkerSize(0.8);
  gLD_22_B1->SetMarkerColor(kRed+1);
  gLD_22_B1->SetLineColor(kRed+1);
  gLD_22_B1->Draw("pc,same");

  auto gLD_0_A1 = LD("SensL_light-A1_0C.csv");
  gLD_0_A1->SetMarkerStyle(20);
  gLD_0_A1->SetMarkerSize(0.8);
  gLD_0_A1->SetMarkerColor(kGreen+2);
  gLD_0_A1->SetLineColor(kGreen+2);
  gLD_0_A1->Draw("pc,same");
  
  auto gLD_0_B1 = LD("SensL_light-B1_0C.csv");
  gLD_0_B1->SetMarkerStyle(25);
  gLD_0_B1->SetMarkerSize(0.8);
  gLD_0_B1->SetMarkerColor(kGreen+2);
  gLD_0_B1->SetLineColor(kGreen+2);
  gLD_0_B1->Draw("pc,same");
  
  auto gLD_m10_A1 = LD("SensL_light-A1_-10C.csv");
  gLD_m10_A1->SetMarkerStyle(20);
  gLD_m10_A1->SetMarkerSize(0.8);
  gLD_m10_A1->SetMarkerColor(kAzure-3);
  gLD_m10_A1->SetLineColor(kAzure-3);
  //  gLD_m10_A1->Draw("pc,same");
  
  auto gLD_m10_B1 = LD("SensL_light-B1_-10C.csv");
  gLD_m10_B1->SetMarkerStyle(20);
  gLD_m10_B1->SetMarkerSize(0.8);
  gLD_m10_B1->SetMarkerColor(kAzure-3);
  gLD_m10_B1->SetLineColor(kAzure-3);
  //  gLD_m10_B1->Draw("pc,same");
  
  c->cd(3)->DrawFrame(23, -1., 27., 9., ";V_{bias} (V);ILD");

  auto gILD_22_A1 = ILD("SensL_light-A1_22C.csv");
  gILD_22_A1->SetMarkerStyle(20);
  gILD_22_A1->SetMarkerSize(0.8);
  gILD_22_A1->SetMarkerColor(kRed+1);
  gILD_22_A1->SetLineColor(kRed+1);
  gILD_22_A1->Draw("pc,same");

  auto gILD_22_B1 = ILD("SensL_light-B1_22C.csv");
  gILD_22_B1->SetMarkerStyle(25);
  gILD_22_B1->SetMarkerSize(0.8);
  gILD_22_B1->SetMarkerColor(kRed+1);
  gILD_22_B1->SetLineColor(kRed+1);
  gILD_22_B1->Draw("pc,same");

  auto gILD_0_A1 = ILD("SensL_light-A1_0C.csv");
  gILD_0_A1->SetMarkerStyle(20);
  gILD_0_A1->SetMarkerSize(0.8);
  gILD_0_A1->SetMarkerColor(kGreen+2);
  gILD_0_A1->SetLineColor(kGreen+2);
  gILD_0_A1->Draw("pc,same");
  
  auto gILD_0_B1 = ILD("SensL_light-B1_0C.csv");
  gILD_0_B1->SetMarkerStyle(25);
  gILD_0_B1->SetMarkerSize(0.8);
  gILD_0_B1->SetMarkerColor(kGreen+2);
  gILD_0_B1->SetLineColor(kGreen+2);
  gILD_0_B1->Draw("pc,same");
  
  auto gILD_m10_A1 = ILD("SensL_light-A1_-10C.csv");
  gILD_m10_A1->SetMarkerStyle(20);
  gILD_m10_A1->SetMarkerSize(0.8);
  gILD_m10_A1->SetMarkerColor(kAzure-3);
  gILD_m10_A1->SetLineColor(kAzure-3);
  //  gILD_m10_A1->Draw("pc,same");
  
  auto gILD_m10_B1 = ILD("SensL_light-B1_-10C.csv");
  gILD_m10_B1->SetMarkerStyle(25);
  gILD_m10_B1->SetMarkerSize(0.8);
  gILD_m10_B1->SetMarkerColor(kAzure-3);
  gILD_m10_B1->SetLineColor(kAzure-3);
  //  gILD_m10_B1->Draw("pc,same");
  
}

TGraphErrors *
moving_average(TGraphErrors *gin, int n = 2)
{
  auto gout = new TGraphErrors;
  for (int i = 0; i < gin->GetN() - n; ++i) {
    double x = 0.;
    double y = 0.;
    for (int j = i; j < i + n; ++j) {
      x += gin->GetX()[j];
      y += gin->GetY()[j];
    }
    x /= (double)n;
    y /= (double)n;
    gout->SetPoint(i, x, y);
  }
  return gout;
}


TGraphErrors *
average(TGraphErrors *gin, int n = 2)
{
  auto gout = new TGraphErrors;
  for (int i = 0; i < gin->GetN() - n; i += n) {
    double x = 0.;
    double y = 0.;
    for (int j = i; j < i + n; ++j) {
      x += gin->GetX()[j];
      y += gin->GetY()[j];
    }
    x /= (double)n;
    y /= (double)n;
    gout->SetPoint(gout->GetN(), x, y);
  }
  return gout;
}


double
Vbd(const TGraphErrors *gIV, int method, double vbd_ini)
{

  std::cout << "using method" << method << std::endl;

  switch (method) {

  case 0: { // the SensL method
    auto vbd = vbd_ini;
    auto gsqrtIV = sqrtIV(gIV);
    // fit sqrt(IV) for V > Vbd with pol1 until we reach stable value
    auto pol1 = (TF1 *)gROOT->GetFunction("pol1");
    while (true) {
      gsqrtIV->Fit(pol1, "q", "", vbd + 0.5, vbd + 1.5);
      //      auto vbd_new = pol1->GetX(0.);   // -pol1->GetParameter(0) / pol1->GetParameter(1);
      auto vbd_new = -pol1->GetParameter(0) / pol1->GetParameter(1);
      std::cout << "vbd: " << vbd << "--> vdb_new: " << vbd_new << std::endl;
      if (fabs(vbd_new - vbd) > 0.001) {
	vbd = vbd_new;
	continue;
      }
      gsqrtIV->Draw("ap*");
      pol1->SetLineStyle(kDashed);
      pol1->SetRange(24., 27.);
      pol1->Draw("same");
      return vbd_new;
    }
  }
    
  case 1: {
    // LD has its maximum
    auto gLD = LD(gIV);
    gLD = moving_average(gLD, 10);
    double ymax = 0.;
    double vbd = -1.;
    for (int i = 0; i < gLD->GetN(); ++i) {
      if (gLD->GetY()[i] < ymax) continue;
      if (gLD->GetX()[i] > 27) continue;
      ymax = gLD->GetY()[i];
      vbd = gLD->GetX()[i];
    }
    return vbd;
  }
    
  case 3: {
    // estimate Vbd with method 1
    auto vbd = vbd_ini; // Vbd(gIV, 1) - 0.5;
    auto gILD = ILD(gIV);
    // fit ILD for V > Vbd with pol1 until we reach stable value
    auto pol1 = (TF1 *)gROOT->GetFunction("pol1");
    while (true) {
      gILD->Fit(pol1, "q", "", vbd + 0.5, vbd + 3.0);
      gILD->Draw("ap*");
      auto vbd_new = -pol1->GetParameter(0) / pol1->GetParameter(1);
      std::cout << "vbd: " << vbd << "--> vdb_new: " << vbd_new << std::endl;
      if (fabs(vbd_new - vbd) > 0.001) {
	vbd = vbd_new;
	continue;
      }
      return vbd_new;
    } 
  }
    
  }

  return 0.;
}

