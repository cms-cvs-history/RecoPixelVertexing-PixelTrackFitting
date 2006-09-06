/*

Peter Trueb, 04.09.2006
Class to test the track cleaning
              the track reconstruction efficiency
              the track hit pattern

*/



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
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackReco/interface/HitPattern.h"
#include "SimDataFormats/HepMCProduct/interface/HepMCProduct.h"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimGeneral/HepPDT/interface/HepPDTable.h"
#include "SimTracker/TrackerHitAssociation/interface/TrackerHitAssociator.h"
#include "RecoPixelVertexing/PixelTriplets/interface/OrderedHitTriplets.h"
#include "RecoTracker/TkTrackingRegions/interface/GlobalTrackingRegion.h"
#include "RecoPixelVertexing/PixelTriplets/interface/PixelHitTripletGenerator.h"
#include "RecoPixelVertexing/PixelTrackFitting/interface/PixelFitter.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"

#include <iostream>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TF1.h>
#include <TH2F.h>
#include <TH1F.h>
#include <TParameter.h>

#define MC 0
#define SIM 1
#define REC 2
#define MATCHED 3
#define RECFILT 4
#define MATCHEDFILT 5
#define NOTREC 6
#define MULTIREC 7
#define MULTIRECFILT 8
#define NOTRECFILT 9

using namespace std;
using namespace edm;


class AdvancedPixelTrackTest : public EDAnalyzer {
public:
  explicit AdvancedPixelTrackTest(const ParameterSet& conf);
  ~AdvancedPixelTrackTest();
  virtual void beginJob(const EventSetup& eventSetup);
  virtual void analyze(const Event& event, const EventSetup& eventSetup);
  virtual void endJob();

  typedef reco::TrackCollection::const_iterator trackIT;
  typedef TrackingRecHitRefVector::iterator recHitIT;
  typedef std::vector<const TrackingRecHit *> RecHits;

private:
  string collectionLabel;
  const ParameterSet params;

  void analyzeRecTrack(const reco::Track & track, RecHits, int type);
  int matchRecTrack(const reco::Track & track, RecHits);
  void analyzeSimTrack(SimTrack track, int type);
  void analyzeMC();
  void analyzeHitPattern(const reco::Track & track);
  bool trackIsValid(SimTrack track);
  double minDr(SimTrack track);

  const HepMC::GenEvent *mcEvent;
  Handle<vector<SimVertex> > simVertices;
  Handle<vector<SimTrack> > simTracks;
  TrackerHitAssociator* associator;
  reco::TrackCollection filteredTracks;
  OrderedHitTriplets triplets;

  static const int maxSimTracks = 5000;
  int simTrackMatches[maxSimTracks];
  HepPDTable *particleTable;

  int nFilteredRecTracks, nRecTracks;
  int totRecTracks, totSimTracks, totMatchedTracks, totFilteredRecTracks, totFilteredMatchedTracks;

  static const int nHistos = 10;
  TFile *histoFile;
  TH1F *hPt[nHistos], *hEta[nHistos], *hPhi[nHistos], *hVtxZ[nHistos], *hVtxIP[nHistos], *hMinDr[nHistos];
  TH1F *hNRecTracks, *hNFilteredRecTracks, *hNMCTracks, *hNFinalMCParticles, *hNFinalChargedMCParticles, *hNPixelHits;
  TH1F *hSimTrackMatches, *hSimTrackMatchesFilt;
  TH1F *hPurity, *hPurityFilt, *hEfficiency, *hEfficiencyFilt;
};


AdvancedPixelTrackTest::AdvancedPixelTrackTest(const ParameterSet& conf) : params(conf)
{
  collectionLabel = conf.getParameter<std::string>("TrackCollection");
  LogInfo("AdvancedPixelTrackTest") << "Constructor";

  totRecTracks = 0;
  totSimTracks = 0;
  totMatchedTracks = 0;
  totFilteredRecTracks = 0;
  totFilteredMatchedTracks = 0;
}


AdvancedPixelTrackTest::~AdvancedPixelTrackTest()
{
  LogInfo("AdvancedPixelTrackTest") << "Destructor";
}


void AdvancedPixelTrackTest::beginJob(const EventSetup& eventSetup)
{
  histoFile = new TFile("histos.root", "RECREATE");

  char suffix[100];
  for (int i = 0; i < nHistos; i++)
  {
    if (i == MC) sprintf(suffix, "MC");
    else if (i == SIM) sprintf(suffix, "Sim");
    else if (i == REC) sprintf(suffix, "Rec");
    else if (i == MATCHED) sprintf(suffix, "Matched");
    else if (i == RECFILT) sprintf(suffix, "RecFilt");
    else if (i == MATCHEDFILT) sprintf(suffix, "MatchedFilt");
    else if (i == NOTREC) sprintf(suffix, "NotRec");
    else if (i == MULTIREC) sprintf(suffix, "MultiRec");
    else if (i == MULTIRECFILT) sprintf(suffix, "MultiRecFilt");
    else if (i == NOTRECFILT) sprintf(suffix, "NotRecFilt");

    hPt[i] = new TH1F( Form("pt%s", suffix), Form("pt%s", suffix), 50, 0., 20.);
    hEta[i] = new TH1F( Form("eta%s", suffix), Form("eta%s", suffix), 50, -5., 5.);
    hPhi[i] = new TH1F( Form("phi%s", suffix), Form("phi%s", suffix), 32, -3.2, 3.2);
    hVtxZ[i] = new TH1F( Form("vtxZ%s", suffix), Form("vtxZ%s", suffix), 60, -150., 150.);
    hVtxIP[i] = new TH1F( Form("vtxIP%s", suffix), Form("vtxIP%s", suffix), 50, 0., 50.);
    hMinDr[i] = new TH1F( Form("minDr%s", suffix), Form("minDr%s", suffix), 50, 0., .2);
  }


  hNRecTracks = new TH1F( "nRecTracks", "nRecTracks", 50, 0., 50.);
  hNFilteredRecTracks = new TH1F( "nFilteredRecTracks", "nFilteredRecTracks", 50, 0., 50.);
  hNMCTracks = new TH1F( "nMCTracks", "nMCTracks", 50, 0., 50.);
  hNFinalMCParticles = new TH1F( "nFinalMCParticles", "nFinalMCParticles", 50, 0., 50.);
  hNFinalChargedMCParticles = new TH1F( "nFinalChargedMCParticles", "nFinalChargedMCParticles", 50, 0., 50.);
  hNPixelHits = new TH1F("nPixelHits", "nPixelHits", 10, 0., 10.);

  hSimTrackMatches = new TH1F ("simTrackMatches", "# recHits matched to #mu-SimTrack", 30, 0., 30.);
  hSimTrackMatchesFilt = new TH1F ("simTrackMatchesFilt", "# recHits matched to #mu-SimTrack", 30, 0., 30.);

  hPurity = new TH1F("purity", "purity", 51, 0., 1.02);
  hPurityFilt = new TH1F("purityFilt", "purityFilt", 51, 0., 1.02);
  hEfficiency = new TH1F("efficiency", "efficiency", 51, 0., 1.02);
  hEfficiencyFilt = new TH1F("efficiencyFilt", "efficiencyFilt", 51, 0., 1.02);
}


void AdvancedPixelTrackTest::analyze(const Event& event, const EventSetup& eventSetup)
{
  cout <<"**************** AdvancedPixelTrackTest, analyze event: " << event.id() << " ********************" << endl;

  // == get the event data =====================================================================================

  // MC
  particleTable = & HepPDT::theTable();
  Handle<HepMCProduct> mcProduct;
//  event.getByLabel("VtxSmeared", mcProduct);
  event.getByType(mcProduct);
  mcEvent = mcProduct->GetEvent();

  // PixelHits
  edm::Handle<SiPixelRecHitCollection> pixelHits;
  event.getByType(pixelHits);

  // Triplets
  PixelHitTripletGenerator tripGen;
  tripGen.init(*pixelHits, eventSetup);
  GlobalTrackingRegion region;
  triplets.clear();
  tripGen.hitTriplets(region, triplets, eventSetup);

  // Tracks
  std::string fitterName = params.getParameter<std::string>("Fitter");
  edm::ESHandle<PixelFitter> fitter;
  eventSetup.get<TrackingComponentsRecord>().get(fitterName,fitter);
  typedef OrderedHitTriplets::const_iterator IT;

  reco::TrackCollection tracks;
  tracks.clear();
  vector<RecHits> hits;

  for (IT it = triplets.begin(); it != triplets.end(); it++) {
    RecHits recHits;
    recHits.push_back( (*it).inner() );
    recHits.push_back( (*it).middle() );
    recHits.push_back( (*it).outer() );
    const reco::Track* track = fitter->run(eventSetup, recHits, region);
    if (track)
    {
      tracks.push_back(*track);
      hits.push_back(recHits);
      delete track;
    }
  }

  // recTracks
  edm::Handle<reco::TrackCollection> trackCollection;
  event.getByLabel(collectionLabel,trackCollection);
  const reco::TrackCollection filteredTracks = *(trackCollection.product());

  vector<RecHits> filteredHits;
  for (trackIT it=filteredTracks.begin(); it!=filteredTracks.end(); it++)
  {
    RecHits recHits;
    for (recHitIT recHit = (*it).recHitsBegin(); recHit != (*it).recHitsEnd(); recHit++) recHits.push_back(&**recHit);
    filteredHits.push_back(recHits);
  }

  // simTracks
  event.getByLabel("g4SimHits", simTracks);

  // vertices
  event.getByLabel("g4SimHits", simVertices);

  // associator
  associator = new TrackerHitAssociator(event);

  // == analze now ========================================================================================

  nRecTracks = 0;
  nFilteredRecTracks = 0;

  // MC
  analyzeMC();

  // analyze recTracks
  cout << "Number of Triplets: "<< triplets.size() << endl;
  cout << "Number of FilteredTracks: "<< filteredTracks.size() << endl;
  hNRecTracks->Fill(tracks.size());
  for (int i = 0; i < maxSimTracks; i++) simTrackMatches[i] = 0;
  for (int i = 0; i < (int)tracks.size(); i++) analyzeRecTrack(tracks.at(i), hits.at(i), REC);

  // analyze simTracks
  int nAllSimTracks = simTracks.product()->size(), nSimTracks = 0, nMatchedTracks = 0;
  for (int i = 0; i < nAllSimTracks; i++)
  {
    SimTrack simTrack = (*simTracks.product())[i];
    if (trackIsValid(simTrack))
    {
      nSimTracks++;
      analyzeSimTrack(simTrack, SIM);
      hSimTrackMatches->Fill(simTrackMatches[i]);
      if (simTrackMatches[i] == 0) analyzeSimTrack(simTrack, NOTREC);
      if (simTrackMatches[i] > 0)
      {
        analyzeSimTrack(simTrack, MATCHED);
        nMatchedTracks++;
      }
      if (simTrackMatches[i] > 1) analyzeSimTrack(simTrack, MULTIREC);
    }
  }
  if (tracks.size() > 0.) hPurity->Fill((double)nMatchedTracks/nRecTracks);
  hEfficiency->Fill((double)nMatchedTracks/nSimTracks);
  printf("nMC tracks %i\n", nSimTracks);
  printf("nMatched tracks %i\n", nMatchedTracks);
  hNMCTracks->Fill(nSimTracks);

  // analyze filteredTracks
  for (int i = 0; i < maxSimTracks; i++) simTrackMatches[i] = 0;
  int nFilteredMatchedTracks = 0;
  for (int i = 0; i < (int)filteredTracks.size(); i++) analyzeRecTrack(filteredTracks.at(i), filteredHits.at(i), RECFILT);
  for (int i = 0; i < nAllSimTracks; i++)
  {
    SimTrack simTrack = (*simTracks.product())[i];
    if (trackIsValid(simTrack))
    {
      hSimTrackMatchesFilt->Fill(simTrackMatches[i]);
      if (simTrackMatches[i] == 0) analyzeSimTrack(simTrack, NOTRECFILT);
      if (simTrackMatches[i] > 0)
      {
        analyzeSimTrack(simTrack, MATCHEDFILT);
        nFilteredMatchedTracks++;
      }
      if (simTrackMatches[i] > 1) analyzeSimTrack(simTrack, MULTIRECFILT);
    }
  }
  if (filteredTracks.size() > 0.) hPurityFilt->Fill((double)nFilteredMatchedTracks/nFilteredRecTracks);
  hEfficiencyFilt->Fill((double)nFilteredMatchedTracks/nSimTracks);
  hNFilteredRecTracks->Fill(nFilteredRecTracks);
  printf("nMatched filtered tracks %i\n", nFilteredMatchedTracks);

  totSimTracks+=nSimTracks;
  totRecTracks+=nRecTracks;
  totMatchedTracks+=nMatchedTracks;
  totFilteredRecTracks+=nFilteredRecTracks;
  totFilteredMatchedTracks+=nFilteredMatchedTracks;
  cout <<"-----------------------------------------------------------------"<<endl;
}


bool AdvancedPixelTrackTest::trackIsValid(SimTrack track)
{
    const double ptMin = 1.;
    const double etaMax = 2.5;
    int partId = track.genpartIndex();
    if (partId == -1) return false;
    if (mcEvent->particle(partId)->status() != 1) return false;
    if (particleTable->getParticleData(mcEvent->particle(partId)->pdg_id())->charge() == 0.) return false;
    if (track.momentum().perp() < ptMin) return false;
    if (TMath::Abs(track.momentum().pseudoRapidity()) > etaMax) return false;
    return true;
}


void AdvancedPixelTrackTest::analyzeMC()
{
//   cout << "NMCParticles " << mcEvent->particles_size() << endl;
//   mcEvent->print();

  HepMC::GenEvent::particle_const_iterator it;
  int nFinalMCParticles = 0, nFinalChargedMCParticles = 0;
  for(it = mcEvent->particles_begin(); it != mcEvent->particles_end(); ++it)
  {
//     if (TMath::Abs((*it)->pdg_id() == 15)) printf("tau found\n");
    hPt[MC]->Fill((*it)->Momentum().perp());
    hEta[MC]->Fill((*it)->Momentum().pseudoRapidity());
    hPhi[MC]->Fill((*it)->Momentum().phi());
    hVtxZ[MC]->Fill((*it)->CreationVertex().z());
    hVtxIP[MC]->Fill((*it)->CreationVertex().perp());
    if ((*it)->status() == 1)
    {
      nFinalMCParticles++;
      if (particleTable->getParticleData((*it)->pdg_id())->charge() != 0.) nFinalChargedMCParticles++;
    }
  }
  hNFinalMCParticles->Fill(nFinalMCParticles);
  hNFinalChargedMCParticles->Fill(nFinalChargedMCParticles);
}


void AdvancedPixelTrackTest::analyzeSimTrack(SimTrack track, int type)
{
  int genpart = track.genpartIndex(); //-1 if none
  HepMC::GenParticle* particle = mcEvent->particle(genpart);
  hVtxZ[type]->Fill(particle->CreationVertex().z());
  hVtxIP[type]->Fill(particle->CreationVertex().perp());
  hPt[type]->Fill(track.momentum().perp());
  hEta[type]->Fill(track.momentum().pseudoRapidity());
  hPhi[type]->Fill(track.momentum().phi());
  hMinDr[type]->Fill(minDr(track));
}


double AdvancedPixelTrackTest::minDr(SimTrack track)
{
  HepMC::GenParticle* particle = mcEvent->particle(track.genpartIndex());
  double minDr = 9999.;
  for (int i = 0; i < (int)simTracks.product()->size(); i++)
  {
    SimTrack simTrack = (*simTracks.product())[i];
    if (trackIsValid(simTrack))
    {
      HepMC::GenParticle* particle2 = mcEvent->particle(simTrack.genpartIndex());
      double dr = particle2->Momentum().deltaR(particle->Momentum());
      if ((dr != 0.) && (dr < minDr)) minDr = dr;
    }
  }
  return minDr;
}


void AdvancedPixelTrackTest::analyzeRecTrack(const reco::Track & track, RecHits recHits, int type)
{
  int matchedTrack = matchRecTrack(track, recHits);
  if (matchedTrack >= 0)
  {
    SimTrack simTrack = (*simTracks.product())[matchedTrack];
    analyzeSimTrack(simTrack, type);
    if (type == RECFILT) nFilteredRecTracks++;  //only those which were matched to a valid track
    else if (type == REC) nRecTracks++; //only those which were matched to a valid track
  }
  if (type == RECFILT) analyzeHitPattern(track);
}


void AdvancedPixelTrackTest::analyzeHitPattern(const reco::Track & track)
{
  reco::HitPattern pattern = track.hitPattern();
  hNPixelHits->Fill(pattern.numberOfValidPixelHits());
}


int AdvancedPixelTrackTest::matchRecTrack(const reco::Track & track, RecHits recHits)
//returns if the recTrack could have been matched to a valid simTrack
{
  int simRecHitsMatches[maxSimTracks];
  for (int i = 0; i < maxSimTracks; i++) simRecHitsMatches[i] = 0;
  for (int i = 0; i < (int)recHits.size(); i++)
  {
    std::vector<PSimHit> matched;
    matched.clear();
    matched = associator->associateHit(*recHits.at(i));

    PSimHit closestSimHit;
    if (!matched.empty())
    {
      float closest=9999.;
      for (std::vector<PSimHit>::const_iterator m = matched.begin(); m<matched.end(); m++)
      {
        float x_res = ((*m).entryPoint().x()+(*m).exitPoint().x())/2. - (*recHits.at(i)).localPosition().x();
        float y_res = ((*m).entryPoint().y()+(*m).exitPoint().y())/2. - (*recHits.at(i)).localPosition().y();
        float res = sqrt(x_res*x_res + y_res*y_res);

        if (res < closest)
        {
          closest = res;
          closestSimHit = *m;
        }
      }

      int id = closestSimHit.trackId() - 1;
      if ((id < (int)simTracks.product()->size()) && (trackIsValid((*simTracks.product())[id]))) simRecHitsMatches[id]++;  //simTracks from mc are first in the array
    }
  }
  int bestTrack = -1, maxRecHits = 1;  //require at least two matches to the same track
  for (int i = 0; i < (int)simTracks.product()->size(); i++)
  {
    if (simRecHitsMatches[i] > maxRecHits)
    {
      maxRecHits = simRecHitsMatches[i];
      bestTrack = i;
    }
  }
  if (bestTrack >= 0) simTrackMatches[bestTrack]++;
  return bestTrack;
}


void AdvancedPixelTrackTest::endJob()
{
  double efficiency = (double)totMatchedTracks/totSimTracks;
  double purity = (double)totMatchedTracks/totRecTracks;
  double efficiencyFilt = (double)totFilteredMatchedTracks/totSimTracks;
  double purityFilt = (double)totFilteredMatchedTracks/totFilteredRecTracks;

  printf("totEff %.2f %.2f\n", efficiency, efficiencyFilt);
  printf("totPur %.2f %.2f\n", purity, purityFilt);

  TParameter<double> *parameter0 = new TParameter<double>("totEfficiency", efficiency);
  TParameter<double> *parameter1 = new TParameter<double>("totPurity", purity);
  TParameter<double> *parameter2 = new TParameter<double>("totFilteredEfficiency", efficiencyFilt);
  TParameter<double> *parameter3 = new TParameter<double>("totFilteredPurity", purityFilt);

  histoFile->WriteTObject(parameter0);
  histoFile->WriteTObject(parameter1);
  histoFile->WriteTObject(parameter2);
  histoFile->WriteTObject(parameter3);

  histoFile->Write();
  histoFile->Close();
}


DEFINE_FWK_MODULE(AdvancedPixelTrackTest)
