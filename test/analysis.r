{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
//--
  TFile file("analysis.root");
  file.ls();

  c = new TCanvas("c","c",-2);
  myHisto.SetTitle("10GeV muons, charge -1");
  myHisto.SetYTitle("arbitrary");
  myHisto.SetXTitle("Delta phi (MC-REC) [rad]");
  myHisto.DrawCopy();
  c.Print(0,"eps");

}
