#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/print.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"

#include <iostream>

using namespace std;
using namespace edm;

#include "TFile.h"
#include "TH1D.h"
#include "TProfile.h"

   



class PixelTrackAnalysis : public edm::EDAnalyzer {
public:
  explicit PixelTrackAnalysis(const edm::ParameterSet& conf);
  ~PixelTrackAnalysis();
  virtual void beginJob(const edm::EventSetup& es) { }
  virtual void analyze(const edm::Event& ev, const edm::EventSetup& es);
  virtual void endJob() { }
private:
  void myprint(const reco::Track & track) const;
  string collectionLabel;
  TH1D * hDphi, *hDeta, *hz, *hNrec, *hNgen, *hNghost, *hNfake, *hChi2;
  TH1D *hPt, *hEffPt_N, *hEffPt_D, *hEffEta_N, *hEffEta_D;
  TProfile * hPtRecVsGen;
  TFile * rootFile;
};

PixelTrackAnalysis::PixelTrackAnalysis(const edm::ParameterSet& conf)
{
  collectionLabel = conf.getParameter<std::string>("TrackCollection");
  edm::LogInfo("PixelTrackAnalysis")<<" CTOR";

  rootFile = new TFile("analysis.root","RECREATE");

  hDphi = new TH1D("hDphi","hDphi",100,-0.05, 0.05);
  hDeta = new TH1D("hDeta","hDeta",100,-0.01,0.01);
  hz   = new TH1D("hz","hz",50,-0.15,0.15);
  hPt    = new TH1D("hPt","hPt",35,0.,35.);
  hPtRecVsGen = new TProfile("hPtRecVsGen","hPtRecVsGen",10,0.5,10.5);
  hEffEta_N = new TH1D("hEffEta_N","hEffEta_N",50,-2.45,2.55);
  hEffEta_D = new TH1D("hEffEta_D","hEffEta_D",50,-2.45,2.55);
  hEffPt_N = new TH1D("hEffPt_N","hEffPt_N",10,0.5,10.5);
  hEffPt_D = new TH1D("hEffPt_D","hEffPt_D",10,0.5,10.5);
  hNrec = new TH1D("hNrec","hNrec",12,0.,12.);
  hNgen = new TH1D("hNgen","hNgen",12,0.,12.);
  hNghost = new TH1D("hNghost","hNghost",12,0.,12.);
  hNfake = new TH1D("hNfake","hNfake",12,0.,12.);
  hChi2 = new TH1D("hChi2","hChi2",50,0.,10.);
}

PixelTrackAnalysis::~PixelTrackAnalysis()
{
  std::cout <<"WRITING ROOT FILE"<< std::endl;
  edm::LogInfo("PixelTrackAnalysis")<<" DTOR";
  rootFile->Write();
  std::cout << "rootFile WRITTEN" << std::endl;
}

void PixelTrackAnalysis::analyze(
    const edm::Event& ev, const edm::EventSetup& es)
{
  cout <<"*** PixelTrackAnalysis, analyze event: " << ev.id() << endl;
  typedef reco::TrackCollection::const_iterator IT;

  edm::Handle<reco::TrackCollection> trackCollection;
  ev.getByLabel(collectionLabel,trackCollection);

  edm::Handle<edm::HepMCProduct> mcsource;
  ev.getByType(mcsource);

  HepMC::GenEvent * myGenEvent = new  HepMC::GenEvent(*(mcsource->GetEvent()));

  const reco::TrackCollection tracks = *(trackCollection.product());
  cout << "Number of tracks: "<< tracks.size() << " tracks" << std::endl;

  int Ngen = 0;
  int Nrec = 0;
  int Nghost = 0;
  for ( HepMC::GenEvent::particle_iterator p = myGenEvent->particles_begin();
       p != myGenEvent->particles_end(); ++p ) {

    if ( abs((*p)->pdg_id()) != 13) continue; 
    Ngen++;
    bool isReconstructed = false;
    float phi_mc = (*p)->momentum().phi();
    float pt_gen = (*p)->momentum().perp();
    float eta_gen = (*p)->momentum().eta(); 
    if (fabs(eta_gen) < 2.1) hEffPt_D->Fill(pt_gen);
    if (pt_gen >2.5) hEffEta_D->Fill(eta_gen);
    for (IT it=tracks.begin(); it!=tracks.end(); it++) {
      float phi_rec = (*it).momentum().phi();
      float eta_rec = (*it).momentum().eta();
      float pt_rec = (*it).pt();
      float chi2   = (*it).chi2();
      float dphi = phi_mc - phi_rec;
      float deta = eta_rec - eta_gen;
      if (fabs(deta) < 0.3)  hDphi->Fill(dphi);
      if (fabs(dphi) < 0.3)  hDeta->Fill(deta);
      if (fabs(deta) < 0.03 && fabs(dphi) < 0.06)  {
        hPtRecVsGen->Fill(pt_gen,pt_rec); 
        if (isReconstructed) Nghost++;
        hChi2->Fill(chi2);
        if (fabs(eta_gen) < 2.1) hEffPt_N->Fill(pt_gen);
        if (pt_gen > 2.5) hEffEta_N->Fill(eta_gen);
        hPt->Fill(pt_rec);
        isReconstructed = true;
      }
    }
    if (isReconstructed) Nrec++;
  }
  hNgen->Fill(Ngen);
  hNrec->Fill(Nrec);
  hNghost->Fill(Nghost);
   
  int Nfake = 0;
  for (IT it=tracks.begin(); it!=tracks.end(); it++) {
    hz->Fill((*it).vertex().z());
    bool isFake = true;
    float phi_rec = (*it).momentum().phi();
    float eta_rec = (*it).momentum().eta();
    for ( HepMC::GenEvent::particle_iterator p = myGenEvent->particles_begin();
       p != myGenEvent->particles_end(); ++p ) {

      float phi_mc = (*p)->momentum().phi();
      float eta_gen = (*p)->momentum().eta(); 
      float dphi = phi_mc - phi_rec;
      float deta = eta_rec - eta_gen;

      if (fabs(deta) < 0.03 && fabs(dphi) < 0.06)  isFake = false;
    }
    if (isFake) Nfake++;
  }
  hNfake->Fill(Nfake);

  delete myGenEvent;


  cout <<"------------------------------------------------"<<endl;
}

DEFINE_FWK_MODULE(PixelTrackAnalysis)
