{
  gROOT->Reset();
  gROOT->GetList()->Delete();
  gROOT->GetListOfCanvases()->Delete();
  gROOT.LoadMacro("~/root/FigUtils.C");
  
//--
  TFile file("analysis.root");
  file.ls();


  MyDefaultStyle->cd();
  cDphi = new TCanvas("cDphi","cDphi",-2);
//  hDphi.SetStats(0);
  gStyle->SetOptStat(101110);
  hDphi.SetYTitle("arbitrary");
  hDphi.SetXTitle("Delta phi (GEN-REC) [rad]");
  hDphi.GetXaxis()->SetNdivisions(508);

  hDphi.SetLineWidth(2);
  hDphi.DrawCopy();
  cDphi.Print(0,".eps");


  cDeta = new TCanvas("cDeta","cDeta",-2);
  MyDefaultStyle->cd();
  gStyle->SetOptStat(101110);
  hDeta.SetYTitle("arbitrary");
  hDeta.SetXTitle("Delta eta (GEN-REC)");
  hDeta.SetLineWidth(2);
  hDeta.GetXaxis()->SetNdivisions(508);
  hDeta.DrawCopy();
  cDeta.Print(0,".eps");


  MyDefaultStyle->cd();
  cz = new TCanvas("cz","cz",-2);
  gStyle->SetOptStat(101110);
  hz.SetYTitle("arbitrary");
  hz.SetXTitle("z of track IP [cm]");
  hz.SetLineWidth(2);
  hz.GetXaxis()->SetNdivisions(508);
  hz.DrawCopy();
  cz.Print(0,".eps");



  MyDefaultStyle->cd();
  cPt = new TCanvas("cPt","cPt",-2);
  gStyle->SetOptStat(101110);
  hPt.SetYTitle("arbitrary");
  hPt.SetXTitle("reconstructed p_{T} [GeV/c]");
  hPt.SetLineWidth(2);
  hPt->DrawCopy();
  cPt.Print(0,".eps");

  MyDefaultStyle->cd();
  cChi2 = new TCanvas("chi2","chi2",-2);
  gStyle->SetOptStat(101110);
  hChi2.SetYTitle("arbitrary");
  hChi2.SetXTitle("chi2");
  hChi2.SetLineWidth(2);
  hChi2->DrawCopy();
  cChi2.Print(0,".eps");

goto end;

  cCounters = new TCanvas("cCounters","cCounters",-1);
  MyDefaultStyle->cd();
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



  MyDefaultStyle->cd();
  cEfficPt = new TCanvas("cEfficPt","cEfficPt",-2);
  TH1D * heff = DivideErr(hEffPt_N,hEffPt_D,"hEffic","B");
  heff->SetMinimum(0.5);
  heff->SetMaximum(1.02);
  heff->SetXTitle("p_{T} [GeV/c]");
  heff->SetYTitle("efficiency");
  heff->SetMarkerStyle(25);
  heff->SetMarkerColor(4);
  heff->DrawCopy("E");
  cEfficPt.Print(0,".eps");

  MyDefaultStyle->cd();
  cEfficEta = new TCanvas("cEfficEta","cEfficEta",-2);
  hEffEta_N->Rebin(4);
  hEffEta_D->Rebin(4);
  TH1D * heffEta = DivideErr(hEffEta_N,hEffEta_D,"hEffic","B");
  heffEta->SetMinimum(0.5);
  heffEta->SetMaximum(1.02);
  heffEta->SetXTitle("eta ");
  heffEta->SetYTitle("efficiency");
  heffEta->DrawCopy("E");
  cEfficEta.Print(0,".eps");
  
  MyDefaultStyle->cd();
  cPtRecVsGen = new TCanvas("cPtRecVsGen","cPtRecVsGen",-2);
  hPtRecVsGen.SetTitleOffset(1.5,"y");
  hPtRecVsGen.SetLabelOffset(0.015,"y");
  hPtRecVsGen.SetYTitle(" p_{T} reconstructed");
  hPtRecVsGen.SetXTitle(" p_{T} generated");
  hPtRecVsGen.SetMaximum(15);
  hPtRecVsGen.SetMinimum(0.);
  hPtRecVsGen.GetYaxis()->SetNdivisions(115);
  hPtRecVsGen.GetXaxis()->SetNdivisions(118);
  hPtRecVsGen.SetStats(0);
  hPtRecVsGen.SetMarkerStyle(25);
  hPtRecVsGen.SetMarkerColor(4);
  hPtRecVsGen.DrawCopy("P");
  cPtRecVsGen.Print(0,".eps");

end:

    
}
