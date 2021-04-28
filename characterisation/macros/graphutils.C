namespace graphutils {

  TGraph *
  diff(TGraph *gin, TGraph *gref)
  {
    auto gout = new TGraph;
    for (int i = 0; i < gin->GetN(); ++i) {
      auto x = gin->GetX()[i];
      auto y = gin->GetY()[i];
      if (x < gref->GetX()[0]) continue;
      if (x > gref->GetX()[gref->GetN() - 1]) continue;
      auto ref = gref->Eval(x);
      auto val = (y - ref) / ref;
      gout->SetPoint(gout->GetN(), x, val);
    }
    return gout;
  }

  TGraph *
  fromZero(TGraph *gin)
  {
    auto gout = new TGraph;
    for (int i = 0; i < gin->GetN(); ++i) {
      auto x = gin->GetX()[i] - gin->GetX()[0];
      auto y = gin->GetY()[i];
      gout->SetPoint(i, x, y);
    }
    return gout;
  }

  TGraph *
  invertY(TGraph *gin)
  {
    auto gout = new TGraph;
    for (int i = 0; i < gin->GetN(); ++i) {
      auto x = gin->GetX()[i];
      auto y = -gin->GetY()[i];
      gout->SetPoint(i, x, y);
    }
    return gout;
  }

  TGraph *
  invertX(TGraph *gin)
  {
    auto gout = new TGraph;
    for (int i = 0; i < gin->GetN(); ++i) {
      auto x = -gin->GetX()[i];
      auto y = gin->GetY()[i];
      gout->SetPoint(i, x, y);
    }
    return gout;
  }

  TGraph *
  average(TGraph *gin, int n = 2)
  {
    auto gout = new TGraph;
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
  
  TGraph *
  rms(TGraph *gin, int n = 2)
  {
    auto gout = new TGraph;
    for (int i = 0; i < gin->GetN() - n; i += n) {
      double x = 0.;
      double y = 0.;
      for (int j = i; j < i + n; ++j) {
	x += gin->GetX()[j];
	y += gin->GetY()[j];
      }
      x /= (double)n;
      y /= (double)n;
      auto mean = y;
      y = 0.;
      for (int j = i; j < i + n; ++j) {
	y += (gin->GetY()[j] - mean) * (gin->GetY()[j] - mean);
      }
      y /= (double)n;
      gout->SetPoint(gout->GetN(), x, std::sqrt(y));
    }
    return gout;
  }
  
  TGraph *
  moving_average(TGraph *gin, int n = 2)
  {
    auto gout = new TGraph;
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
  
}
