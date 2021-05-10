void
makedcr(const char *fnin, const char *fnout)
{
  TFile fout(fnout, "RECREATE");
  TTree tree("dcr", "sipm4eic DCR data");
  tree.ReadFile(fnin);
  tree.Write();
  fout.Close(); 
}
