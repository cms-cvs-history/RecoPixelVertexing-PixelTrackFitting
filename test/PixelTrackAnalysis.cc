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
  bool selectMu;
  TH1D * hDphi, *hDeta, *hz, *hNrec, *hNgen, *hNghost, *hNfake, *hNmatch, 
    *hChi2, *hDphi2, *hDeta2, *hNghost2;
  TH1D *hPt, *hEffPt_N, *hEffPt_D, *hEffEta_N, *hEffEta_D;
  TH1D *hPtPix, *hEtaPix, *hPhiPix, *hNumPixTracks; 
  TH1D *hEtaPix2, *hNmcGood, *hNmcGoodReco, *hNmcGood2, *hNmcGoodReco2;

  TProfile * hPtRecVsGen;
  TFile * rootFile;
};

PixelTrackAnalysis::PixelTrackAnalysis(const edm::ParameterSet& conf) {
  collectionLabel = conf.getParameter<std::string>("TrackCollection");
  selectMu = conf.getUntrackedParameter<bool>("SelectMu",true);
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

  hNrec = new TH1D("hNrec","hNrec",100,0.,100.);
  hNgen = new TH1D("hNgen","hNgen",100,0.,100.);
  hNghost = new TH1D("hNghost","hNghost",100,0.,100.);
  hNfake = new TH1D("hNfake","hNfake",100,0.,100.);
  hNmatch = new TH1D("hNmatch","hNmatch",100,0.,100.);
  hNghost2 = new TH1D("hNghost2","hNghost2",100,0.,100.);

  hChi2 = new TH1D("hChi2","hChi2",50,0.,10.);

  hPtPix    = new TH1D("hPtPix","PixTrack Pt",35,0.,35.);
  hEtaPix = new TH1D("hEtaPix","PixTrack Eta",60,3.0,3.0);
  hPhiPix = new TH1D("hPhiPix","PixTrack Phi",70,-3.5,3.5);
  hNumPixTracks = new TH1D("hNumPixTracks","Number of PixTracks",100,0.,100.);
  hNmcGood = new TH1D("hNmcGood","Num of MC good tracks",100,0.,100.);
  hNmcGoodReco = new TH1D("hNmcGoodReco","Num of recon. MC good tracks",
			  100,0.,100.);
  hNmcGood2 = new TH1D("hNmcGood2","Num of MC good tracks",100,0.,100.);
  hNmcGoodReco2 = new TH1D("hNmcGoodReco2","Num of recon. MC good tracks",
			  100,0.,100.);

  hDphi2 = new TH1D("hDphi2","hDphi2",100,-0.02, 0.02);
  hDeta2 = new TH1D("hDeta2","hDeta2",100,-0.004,0.004);
  hEtaPix2 = new TH1D("hEtaPix2","PixTrack Eta",60,3.0,3.0);
}

PixelTrackAnalysis::~PixelTrackAnalysis() {
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
  ev.getByLabel("source",mcsource);
  //ev.getByType(mcsource);

  HepMC::GenEvent * myGenEvent = new  HepMC::GenEvent(*(mcsource->GetEvent()));

  const reco::TrackCollection tracks = *(trackCollection.product());
  cout << "Number of tracks: "<< tracks.size() << " tracks" << std::endl;
  hNumPixTracks->Fill(float(tracks.size()));

  // matching cuts from Marcin 
  float detaMax=0.03;
  float dphiMax=0.06;
  if ( !selectMu ) { // Matching cuts from d.k. 
    detaMax=0.008; // 0.03;
    dphiMax=0.03; // 0.06; 
  }

  int Ngen = 0; // MC tracks
  int Nrec = 0; // MC tracks whitch match a RECO track 
                // several MC tracks can match 1 RECO track.
  int Nghost = 0; // more then 1 RECO track for 1 MC track
  int NmcGood=0, NmcGood2=0, NmcGoodReco2=0, NmcGoodReco=0;

  for ( HepMC::GenEvent::particle_iterator p = myGenEvent->particles_begin();
       p != myGenEvent->particles_end(); ++p ) {

    int pid = abs( (*p)->ParticleID()) ;
    int status = (*p)->StatusCode();
    float phi_mc = (*p)->momentum().phi();
    float pt_gen = (*p)->momentum().perp();
    float eta_gen = (*p)->momentum().eta(); 

    if ( selectMu) {  // select only muons 
      if( pid != 13 ) continue;
    } else {  // select all srable charged particles
      if( status != 1) continue;
      if(pid!=13 && pid!=11 && pid!=211 && pid!=321 && pid!=2212) 
	continue; 
      if(pt_gen<0.8 || abs(eta_gen)>2.5 ) continue;
    }
    Ngen++;
    bool isReconstructed = false;
    bool isGoodReco = false;
    bool isGoodReco2= false;
    bool isGoodRecoPt = false;
    bool isGoodRecoEta = false;

    if (fabs(eta_gen) < 2.1) hEffPt_D->Fill(pt_gen);
    if (pt_gen >2.5) hEffEta_D->Fill(eta_gen);
    if ( (fabs(eta_gen) < 2.1) && (pt_gen >2.5) ) NmcGood++;
    if ( (fabs(eta_gen) < 2.1) && (pt_gen >1.0) ) NmcGood2++;

    for (IT it=tracks.begin(); it!=tracks.end(); it++) {
      float phi_rec = (*it).momentum().phi();
      float eta_rec = (*it).momentum().eta();
      float pt_rec = (*it).pt();
      float chi2   = (*it).chi2();
      float dphi = phi_mc - phi_rec;
      float deta = eta_rec - eta_gen;
      if (fabs(deta) < 0.3)  hDphi->Fill(dphi);
      if (fabs(dphi) < 0.3)  hDeta->Fill(deta);
      if (fabs(deta) < detaMax && fabs(dphi) < dphiMax)  {
        hPtRecVsGen->Fill(pt_gen,pt_rec); 
        if (isReconstructed) Nghost++;
        hChi2->Fill(chi2);
        if (fabs(eta_gen) < 2.1) isGoodRecoEta=true;
        if (pt_gen > 2.5) isGoodRecoPt=true;
	if ( (fabs(eta_gen) < 2.1) && (pt_gen >2.5) ) isGoodReco=true;
	if ( (fabs(eta_gen) < 2.1) && (pt_gen >1.0) ) isGoodReco2=true;
        hPt->Fill(pt_rec);
        isReconstructed = true;
      }
    }
    if (isReconstructed) Nrec++;
    if (isGoodReco) NmcGoodReco++;
    if (isGoodReco2) NmcGoodReco2++;
    if (isGoodRecoEta) hEffPt_N->Fill(pt_gen);
    if (isGoodRecoPt) hEffEta_N->Fill(eta_gen);
  }
  hNgen->Fill(Ngen);
  hNrec->Fill(Nrec);
  hNghost->Fill(Nghost);

  hNmcGood->Fill(NmcGood);
  hNmcGood2->Fill(NmcGood2);
  hNmcGoodReco->Fill(NmcGoodReco);
  hNmcGoodReco2->Fill(NmcGoodReco2);
   
  int Nfake = 0; // Reco tracks which do not match a MC track
  int Nmatch=0; // Reco tracks which match a MC track
  Nghost = 0; // 2 pixetracks overlap
  for (IT it=tracks.begin(); it!=tracks.end(); it++) {
    float phi_rec = (*it).momentum().phi();
    float eta_rec = (*it).momentum().eta();
    float pt_rec = (*it).pt();
    float chi2   = (*it).chi2();
    hPtPix->Fill(pt_rec);
    hEtaPix->Fill(eta_rec);
    hPhiPix->Fill(phi_rec);

    hz->Fill((*it).vertex().z());

    bool isFake = true;
    for ( HepMC::GenEvent::particle_iterator p = myGenEvent->particles_begin();
       p != myGenEvent->particles_end(); ++p ) {

      float phi_mc = (*p)->momentum().phi();
      float eta_gen = (*p)->momentum().eta(); 
      float dphi = phi_mc - phi_rec;
      float deta = eta_rec - eta_gen;

      if (fabs(deta) < detaMax && fabs(dphi) < dphiMax)  isFake = false;
    }
    if (isFake) Nfake++;
    else Nmatch++;

    // check overlaps. 2 pix tracks from 1 MC track 
    bool isGhost=false;
    for (IT it2=(it+1); it2!=tracks.end(); it2++) {
      float phi_rec2 = (*it2).momentum().phi();
      float eta_rec2 = (*it2).momentum().eta();
      float pt_rec2 = (*it2).pt();
      float dphi2 = phi_rec2 - phi_rec;
      float deta2 = eta_rec - eta_rec2;
      hDphi2->Fill(dphi2);
      hDeta2->Fill(deta2);

      if (fabs(deta2) < 0.0003 && fabs(dphi2) < 0.01)  {
	hEtaPix2->Fill(eta_rec);
	isGhost=true;
      }

    }
    if(isGhost) Nghost++;
  }
  hNfake->Fill(Nfake);
  hNmatch->Fill(Nmatch);
  hNghost2->Fill(Nghost);

  delete myGenEvent;


  cout <<"------------------------------------------------"<<endl;
}

DEFINE_FWK_MODULE(PixelTrackAnalysis);
