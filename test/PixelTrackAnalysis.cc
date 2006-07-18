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
  TH1D * hDphi, *hDeta, *hz, *hNrec, *hNgen;
  TProfile * hPtRecVsGen;
  TFile * rootFile;
};

PixelTrackAnalysis::PixelTrackAnalysis(const edm::ParameterSet& conf)
{
  collectionLabel = conf.getParameter<std::string>("TrackCollection");
  edm::LogInfo("PixelTrackAnalysis")<<" CTOR";

  rootFile = new TFile("analysis.root","RECREATE");

  hDphi = new TH1D("hDphi","hDphi",100,-0.3, 0.3);
  hDeta = new TH1D("hDeta","hDeta",100,-0.3,0.3);
  hz   = new TH1D("hz","hz",100,-0.5,0.5);
  hPtRecVsGen = new TProfile("hPtRecVsGen","hPtRecVsGen",15,0.,15.,"s");
  hNrec = new TH1D("hNrec","hNrec",15,0.,15.);
  hNgen = new TH1D("hNgen","hNgen",15,0.,15.);
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
  for ( HepMC::GenEvent::particle_iterator p = myGenEvent->particles_begin();
       p != myGenEvent->particles_end(); ++p ) {

    if ( abs((*p)->pdg_id()) == 13 && (*p)->momentum().perp() > 1.0 ) { 
      Ngen++;
      bool isReconstructed = false;
      float phi_mc = (*p)->momentum().phi();
      float pt_gen = (*p)->momentum().perp();
      float eta_gen = (*p)->momentum().eta(); 
      for (IT it=tracks.begin(); it!=tracks.end(); it++) {
        float phi_rec = (*it).momentum().phi();
        float eta_rec = (*it).momentum().eta();
        float pt_rec = (*it).pt();
        float dphi = phi_mc - phi_rec;
        float deta = eta_rec - eta_gen;
        if (fabs(deta) < 0.3)  hDphi->Fill(dphi);
        if (fabs(dphi) < 0.3)  hDeta->Fill(deta);
        if (fabs(deta) < 0.1 && fabs(dphi) < 0.1)  {
          hPtRecVsGen->Fill(pt_gen,pt_rec); 
          isReconstructed = true;
        }
      }
      if (isReconstructed) Nrec++;
    }
  }
  hNgen->Fill(Ngen);
  hNrec->Fill(Nrec);

  for (IT it=tracks.begin(); it!=tracks.end(); it++) {
     hz->Fill((*it).vertex().z());
  }
  delete myGenEvent;


  cout <<"------------------------------------------------"<<endl;
}

DEFINE_FWK_MODULE(PixelTrackAnalysis)
