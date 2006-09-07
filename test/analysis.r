{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  
//--
  TFile file("analysis.root");
  file.ls();
  cDphi = new TCanvas("cDphi","cDphi",-1);
  hDphi.SetYTitle("arbitrary");
  hDphi.SetXTitle("Delta phi (MC-REC) [rad]");
  hDphi.DrawCopy();
  cDphi.Print(0,".eps");

  cDeta = new TCanvas("cDeta","cDeta",-1);
  hDeta.SetYTitle("arbitrary");
  hDeta.SetXTitle("Delta eta (MC-REC) [1]");
  hDeta.DrawCopy();
  cDeta.Print(0,".eps");

  cz = new TCanvas("cz","cz",-1);
  hz.SetYTitle("arbitrary");
  hz.SetXTitle("z of track IP [cm]");
  hz.DrawCopy();
  cz.Print(0,".eps");

  cPtRecVsGen = new TCanvas("cPtRecVsGen","cPtRecVsGen",-2);
  hPtRecVsGen.SetYTitle(" pT reconstructed");
  hPtRecVsGen.SetXTitle(" pT reconstructed");
  hPtRecVsGen.SetMaximum(15);
  hPtRecVsGen.SetMinimum(0.);
  hPtRecVsGen.SetStats(0);
  hPtRecVsGen.DrawCopy();
  cPtRecVsGen.Print(0,".eps");

  cCounters = new TCanvas("cCounters","cCounters",-1);
  hNgen->SetStats(0);
  hNgen->SetXTitle("number of reconstructed objects");
  hNgen->SetYTitle("events");
  hNgen->SetLineColor(1); hNgen->SetLineWidth(2);
  hNgen->DrawCopy();
  hNrec->SetLineColor(2); hNrec->SetLineWidth(2);
  hNrec->DrawCopy("same");
  hNghost->SetLineColor(6); hNghost->SetLineWidth(2);
  hNghost->DrawCopy("same");
  hNfake->SetLineColor(4); hNfake->SetLineWidth(2);
  hNfake->DrawCopy("same");
  TLegend legend(0.2,0.4,0.6.,0.8);
  legend.AddEntry(hNgen,"generated");
  legend.AddEntry(hNrec,"reconstructed  av=0.9/muon");
  legend.AddEntry(hNghost,"ghosts (mirrors) av=0.35/muon");
  legend.AddEntry(hNfake,"fake");
  legend.Draw();
  cCounters.Print(0,".eps");
    
  cChi2 = new TCanvas("chi2","chi2",-1);
  hChi2->DrawCopy();
  cChi2.Print(0,".eps");
}
