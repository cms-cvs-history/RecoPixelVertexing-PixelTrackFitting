{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
//--
  TFile file("analysis.root");
  file.ls();

  cDphi = new TCanvas("cDphi","cDphi",-2);
  hDphi.SetYTitle("arbitrary");
  hDphi.SetXTitle("Delta phi (MC-REC) [rad]");
  hDphi.DrawCopy();
  cDphi.Print(0,".jpg");

  cDeta = new TCanvas("cDeta","cDeta",-2);
  hDeta.SetYTitle("arbitrary");
  hDeta.SetXTitle("Delta eta (MC-REC) [1]");
  hDeta.DrawCopy();
  cDeta.Print(0,".jpg");

  cz = new TCanvas("cz","cz",-2);
  hz.SetYTitle("arbitrary");
  hz.SetXTitle("z of track IP [cm]");
  hz.DrawCopy();
  cz.Print(0,".jpg");

  cPtRecVsGen = new TCanvas("cPtRecVsGen","cPtRecVsGen",-2);
  hPtRecVsGen.SetYTitle(" pT reconstructed");
  hPtRecVsGen.SetXTitle(" pT reconstructed");
  hPtRecVsGen.DrawCopy();
  cPtRecVsGen.Print(0,".jpg");

}
