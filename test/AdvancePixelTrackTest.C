/*

Peter Trueb, 04.09.2006

Macros to plot the histograms produced by the AdvancePixelTrackTest

*/

TCanvas *canvas;
TFile *f = new TFile("histos.root");

Init()
{
	gROOT->SetStyle("Plain");
	gStyle->SetTitleBorderSize(0);
	gStyle->SetPalette(1,0);
	gStyle->SetTitleW(0.5);
	gStyle->SetTitleH(0.08);
}


InitCanvas(TCanvas *aCanvas, char* option = 0)
{
    if (!option) option = "";
    aCanvas->SetTickx();
    aCanvas->SetTicky();
    aCanvas->SetBottomMargin(.14);
    aCanvas->SetLeftMargin(.13);
    if (strcmp(option, "log") == 0) aCanvas->SetLogy();
    if (strcmp(option, "logz") == 0) aCanvas->SetLogz();
}


InitPad(char* option = 0)
{
    if (!option) option = "";
    gPad->SetTickx();
    gPad->SetTicky();
    gPad->SetBottomMargin(.14);
    gPad->SetLeftMargin(.13);
    if (strcmp(option, "log") == 0) gPad->SetLogy();
    if (strcmp(option, "logz") == 0) gPad->SetLogz();
}


TLegend* NewLegend(double x1, double x2, double y1, double y2)
{
	TLegend *l = new TLegend(x1, x2, y1, y2);
	l->SetFillColor(0);
	l->SetBorderSize(0);
	return l;
}


TLatex* NewLatex()
{
  TLatex *latex = new TLatex;
  latex->SetNDC(kTRUE);
  latex->SetTextSize(0.05);
  return latex;
}


void DrawHisto(TH1 *histo, int color = kBlack, int lineStyle = 1, bool first = false, int width = 2, bool title = false)
{
  histo->GetXaxis()->SetTitleSize(0.055);
  histo->GetYaxis()->SetTitleSize(0.055);
  histo->GetXaxis()->SetLabelSize(0.05);
  histo->GetYaxis()->SetLabelSize(0.05);
  histo->GetXaxis()->SetTitleOffset(1.15);
  histo->GetYaxis()->SetTitleOffset(1.05);

	if (!title) histo->SetTitle("");
	histo->SetLineColor(color);
	histo->SetLineStyle(lineStyle);
	histo->SetLineWidth(width);
	if (first) histo->Draw("");
	else histo->Draw("same");
}


void DrawFirstHisto(TH1 *histo)
{
	DrawHisto(histo, kBlack, 1, true);
}


void DrawSecondHisto(TH1 *histo)
{
	DrawHisto(histo, kRed, 2, false);
}


void DrawThirdHisto(TH1 *histo)
{
	DrawHisto(histo, kBlue, 3, false);
}


void DrawFourthHisto(TH1 *histo)
{
        DrawHisto(histo, kGreen+100, 5, false);
}


void DrawFifthHisto(TH1 *histo)
{
        DrawHisto(histo, kMagenta, 8, false);
}


void DrawHistos(TH1 *histo1, TH1 *histo2, TH1 *histo3 = 0)
{
  double max = histo1->GetMaximum();
  if (histo2->GetMaximum() > max) max = histo2->GetMaximum();
  if (histo3 && (histo3->GetMaximum() > max)) max = histo3->GetMaximum();

  histo1->SetMaximum(max*1.1);
  histo1->SetMinimum(0.);

  DrawFirstHisto(histo1);
  DrawSecondHisto(histo2);
  if (histo3) DrawThirdHisto(histo3);
}


// reconstruction

DrawRecComparison()
{
  Init();
  canvas = new TCanvas();
  InitCanvas(canvas);   // somehow a second init is necessary for a nice plot
  canvas->Divide(3,2);
  canvas->cd(1);
  RecComparison("ptSim", "ptMatchedFilt", "pt_{sim}");
  canvas->cd(2);
  RecComparison("etaSim", "etaMatchedFilt", "#eta_{sim}");
  canvas->cd(3);
  RecComparison("phiSim", "phiMatchedFilt", "#phi_{sim}");
  canvas->cd(4);
  RecComparison("vtxZSim", "vtxZMatchedFilt", "vertex(z)_{sim}");
  canvas->cd(5);
  RecComparison("vtxIPSim", "vtxIPMatchedFilt", "vertex(IP)_{sim}");
  canvas->cd(6);
  RecComparison("minDrSim", "minDrMatchedFilt", "minDr_{sim}");
  canvas->SaveAs("reconstruction.ps");
}


DrawRecComparisonRatio()
{
  Init();
  canvas = new TCanvas();
  InitCanvas(canvas);   // somehow a second init is necessary for a nice plot
  canvas->Divide(3,2);
  canvas->cd(1);
  RecRatio("ptSim", "ptMatchedFilt", "pt_{sim}");
  canvas->cd(2);
  RecRatio("etaSim", "etaMatchedFilt", "#eta_{sim}");
  canvas->cd(3);
  RecRatio("phiSim", "phiMatchedFilt", "#phi_{sim}");
  canvas->cd(4);
  RecRatio("vtxZSim", "vtxZMatchedFilt", "vertex(z)_{sim}");
  canvas->cd(5);
  RecRatio("vtxIPSim", "vtxIPMatchedFilt", "vertex(IP)_{sim}");
  canvas->cd(6);
  RecRatio("minDrSim", "minDrMatchedFilt", "minDr_{sim}");
  canvas->SaveAs("recEfficiency.ps");
}


RecComparison(char *name1, char *name2, char *xTitle)
{
  InitPad();
  TH1 *histo1 = f->Get(name1);
  histo1->SetStats(kFALSE);
  histo1->GetXaxis()->SetTitle(xTitle);
  histo1->GetYaxis()->SetTitle("# SimTracks");
  TH1 *histo2 = f->Get(name2);

  DrawHistos(histo1, histo2);

  TLegendEntry *le;
  TLegend *l = NewLegend(0.74,0.70,0.88,0.88);
  le = l->AddEntry(histo1, "sim", "l");
  le = l->AddEntry(histo2, "matched", "l");
  l->Draw();
}


RecRatio(char *name1, char *name2, char *xTitle)
{
  InitPad();
  TH1 *histo1 = f->Get(name1)->Clone();
  TH1 *histo2 = f->Get(name2)->Clone();

  histo2->SetStats(kFALSE);
  histo2->GetXaxis()->SetTitle(xTitle);
  histo2->GetYaxis()->SetTitle("rec. efficiency");

  histo2->Divide(histo1);
  DrawFirstHisto(histo2);

  TLegend *l = NewLegend(0.74,0.70,0.88,0.88);
  l->AddEntry(histo2, "rec", "l");
  l->Draw();
}


DrawNTracks()
{
  Init();
  InitCanvas(canvas = new TCanvas());

  TH1 *histo1 = f->Get("nRecTracks");
  histo1->SetStats(kFALSE);
  histo1->GetXaxis()->SetTitle("# Tracks");
  histo1->GetYaxis()->SetTitle("# Events");
  TH1 *histo2 = f->Get("nFilteredRecTracks");
  TH1 *histo3 = f->Get("nMCTracks");
  TH1 *histo4 = f->Get("nFinalMCParticles");
  TH1 *histo5 = f->Get("nFinalChargedMCParticles");
  double max = TMath::Max(histo1->GetMaximum(), histo2->GetMaximum());
  if (histo3->GetMaximum() > max) max = histo3->GetMaximum();
  if (histo4->GetMaximum() > max) max = histo4->GetMaximum();
  if (histo5->GetMaximum() > max) max = histo5->GetMaximum();
  histo1->SetMaximum(max*1.1);

  DrawFirstHisto(histo1);
  DrawSecondHisto(histo2);
  DrawThirdHisto(histo3);
  DrawFourthHisto(histo4);
  DrawFifthHisto(histo5);

  TLegendEntry *le;
  TLegend *l = NewLegend(0.60,0.65,0.88,0.88);
  le = l->AddEntry(histo1, "all recTracks", "l");
  le = l->AddEntry(histo2, "filtered recTracks", "l");
  le = l->AddEntry(histo3, "MCTracks", "l");
  le = l->AddEntry(histo4, "FinalMCParticles", "l");
  le = l->AddEntry(histo5, "FinalChargedMCParticles", "l");
  l->Draw();

  canvas->SaveAs("nTracks.ps");
}


DrawMatching()
{
  Init();
  InitCanvas(canvas = new TCanvas());

  TH1 *histo1 = f->Get("simTrackMatches");
  histo1->SetStats(kFALSE);
  histo1->GetXaxis()->SetTitle("# matched RecHits");
  histo1->GetYaxis()->SetTitle("# SimTracks");
  TH1 *histo2 = f->Get("simTrackMatchesFilt");
  double max = TMath::Max(histo1->GetMaximum(), histo2->GetMaximum());
  histo1->SetMaximum(max*1.1);

  DrawFirstHisto(histo1);
  DrawSecondHisto(histo2);

  TLegendEntry *le;
  TLegend *l = NewLegend(0.74,0.70,0.88,0.88);
  le = l->AddEntry(histo1, "all", "l");
  le = l->AddEntry(histo2, "filtered", "l");
  l->Draw();

  canvas->SaveAs("recHitMatching.ps");
}


// == Cleaning =========================================================================================

CleaningComparison(char *name1, char *name2, char *xTitle)
{
  InitPad();
  TH1 *histo1 = f->Get(name1);
  histo1->SetStats(kFALSE);
  histo1->GetXaxis()->SetTitle(xTitle);
  histo1->GetYaxis()->SetTitle("# RecTracks");
  TH1 *histo2 = f->Get(name2);

  DrawHistos(histo1, histo2);

  TLegendEntry *le;
  TLegend *l = NewLegend(0.64,0.76,0.88,0.88);
  le = l->AddEntry(histo1, "all", "l");
  le = l->AddEntry(histo2, "cleaned", "l");
  l->Draw();
}


CleaningRatio(char *name1, char *name2, char *xTitle)
{
  InitPad();
  TH1 *histo1 = f->Get(name1)->Clone();
  TH1 *histo2 = f->Get(name2)->Clone();

  histo2->SetStats(kFALSE);
  histo2->GetXaxis()->SetTitle(xTitle);
  histo2->GetYaxis()->SetTitle("# RecTracks");

  histo2->Divide(histo1);
  DrawFirstHisto(histo2);

  TLegendEntry *le;
  TLegend *l = NewLegend(0.64,0.80,0.88,0.88);
  le = l->AddEntry(histo2, "cleaning eff.", "l");
  l->Draw();
}


DrawPurityEfficiency()
{
  Init();
  canvas = new TCanvas();
  InitCanvas(canvas);   // somehow a second init is necessary for a nice plot
  canvas->Divide(1,2);
  canvas->cd(1);
  InitPad();

  TH1 *histo1 = f->Get("purity");
  histo1->SetStats(kFALSE);
  histo1->GetXaxis()->SetTitle("purity");
  histo1->GetYaxis()->SetTitle("# Events");
  TH1 *histo2 = f->Get("purityFilt");
  double max = TMath::Max(histo1->GetMaximum(), histo2->GetMaximum());
  histo1->SetMaximum(max*1.1);

  DrawFirstHisto(histo1);
  DrawSecondHisto(histo2);

  TLegendEntry *le;
  TLegend *l = NewLegend(0.70,0.70,0.84,0.88);
  le = l->AddEntry(histo1, "before cleaning", "l");
  le = l->AddEntry(histo2, "after cleaning", "l");
  l->Draw();

  TParameter<double>* par = (TParameter<double>*)f->Get("totPurity");
  TParameter<double>* par2 = (TParameter<double>*)f->Get("totFilteredPurity");

  TLatex *latex = NewLatex();
  latex->DrawLatex(0.15, 0.8, Form("Pur: %.2f / %.2f", par->GetVal(), par2->GetVal()));

  canvas->cd(2);
  InitPad();

  histo1 = (TH1F*)f->Get("efficiency");
  histo1->SetStats(kFALSE);
  histo1->GetXaxis()->SetTitle("efficiency");
  histo1->GetYaxis()->SetTitle("# Events");
  histo2 = (TH1F*)f->Get("efficiencyFilt");
  double max = TMath::Max(histo1->GetMaximum(), histo2->GetMaximum());
  histo1->SetMaximum(max*1.1);

  DrawFirstHisto(histo1);
  DrawSecondHisto(histo2);

  l = NewLegend(0.70,0.70,0.84,0.88);
  le = l->AddEntry(histo1, "before cleaning", "l");
  le = l->AddEntry(histo2, "after cleaning", "l");
  l->Draw();

  par = (TParameter<double>*)f->Get("totEfficiency");
  par2 = (TParameter<double>*)f->Get("totFilteredEfficiency");

  TLatex *latex = NewLatex();
  latex->DrawLatex(0.15, 0.8, Form("Eff: %.2f / %.2f", par->GetVal(), par2->GetVal()));

  canvas->SaveAs("cleaningEffPur.ps");
}


DrawCleaningEffect()
{
  Init();
  canvas = new TCanvas();
  InitCanvas(canvas);   // somehow a second init is necessary for a nice plot
  canvas->Divide(3,2);
  canvas->cd(1);
  CleaningComparison("ptRec", "ptRecFilt", "#p_{T, sim}");
  canvas->cd(2);
  CleaningComparison("etaRec", "etaRecFilt", "#eta_{sim}");
  canvas->cd(3);
  CleaningComparison("phiRec", "phiRecFilt", "#phi_{sim}");
  canvas->cd(4);
  CleaningRatio("ptRec", "ptRecFilt", "#p_{T, sim}");
  canvas->cd(5);
  CleaningRatio("etaRec", "etaRecFilt", "#eta_{sim}");
  canvas->cd(6);
  CleaningRatio("phiRec", "phiRecFilt", "#phi_{sim}");
  canvas->SaveAs("cleaning.ps");
}
