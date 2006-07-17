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
#include "TH1.h"
#include "TH1D.h"
#include "TROOT.h"

   



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
  TH1D * myHisto;
  TFile * rootFile;
};

PixelTrackAnalysis::PixelTrackAnalysis(const edm::ParameterSet& conf)
{
  collectionLabel = conf.getParameter<std::string>("TrackCollection");
  edm::LogInfo("PixelTrackAnalysis")<<" CTOR";
  rootFile = new TFile("analysis.root","RECREATE");
  myHisto = new TH1D("myHisto","myHisto",100,-6.3, 6.3);
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

  float phi_mc = 0;
  for ( HepMC::GenEvent::particle_iterator p = myGenEvent->particles_begin();
       p != myGenEvent->particles_end(); ++p ) {

      std::cout << " HERE !!!!" << std::endl;

      if ( abs((*p)->pdg_id()) == 13
           && (*p)->momentum().perp() > 5.0 ) { 
        phi_mc = (*p)->momentum().phi();
      }

    }

  delete myGenEvent;

  const reco::TrackCollection tracks = *(trackCollection.product());
  cout << "Number of tracks: "<< tracks.size() << " tracks" << std::endl;
  for (IT it=tracks.begin(); it!=tracks.end(); it++) {
    float phi_rec = (*it).momentum().phi();
    float dphi = phi_mc - phi_rec;
    myHisto->Fill(dphi); 
  }

  cout <<"------------------------------------------------"<<endl;
}

DEFINE_FWK_MODULE(PixelTrackAnalysis)
