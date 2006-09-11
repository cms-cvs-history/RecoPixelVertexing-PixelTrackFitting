#include "RecoPixelVertexing/PixelTrackFitting/interface/PixelTrackCleaner.h"

#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

using namespace std;
using namespace reco;


PixelTrackCleaner::PixelTrackCleaner()
{
}


vector<PixelTrackCleaner::TrackWithRecHits> PixelTrackCleaner::cleanTracks(vector<TrackWithRecHits> trackHitPairs)
{
  typedef std::vector<const TrackingRecHit *> RecHits;

  LogDebug("PixelTrackCleaner") << "Cleanering tracks" << "\n";
  int size = trackHitPairs.size();
  for (int i = 0; i < size; i++) trackOk.push_back(true);

  for (iTrack1 = 0; iTrack1 < size; iTrack1++)
  {
    track1 = trackHitPairs.at(iTrack1).first;
    RecHits recHits1 = trackHitPairs.at(iTrack1).second;

    if (!trackOk.at(iTrack1)) continue;

    for (iTrack2 = iTrack1 + 1; iTrack2 < size; iTrack2++)
    {
      if (!trackOk.at(iTrack1) || !trackOk.at(iTrack2)) continue;

      track2 = trackHitPairs.at(iTrack2).first;
      RecHits recHits2 = trackHitPairs.at(iTrack2).second;

      int commonRecHits = 0;
      for (int iRecHit1 = 0; iRecHit1 < (int)recHits1.size(); iRecHit1++)
      {
        for (int iRecHit2 = 0; iRecHit2 < (int)recHits2.size(); iRecHit2++)
        {
          if (recHitsAreEqual(recHits1.at(iRecHit1), recHits2.at(iRecHit2))) commonRecHits++;
        }
      }
      if (commonRecHits > 1) cleanTrack();
    }
  }

  vector<TrackWithRecHits> cleanedTracks;

  for (int i = 0; i < size; i++)
  {
    if (trackOk.at(i)) cleanedTracks.push_back(trackHitPairs.at(i));
  }
  return cleanedTracks;
}


void PixelTrackCleaner::cleanTrack()
{
  if (track1->pt() > track2->pt()) trackOk.at(iTrack2) = false;
  else trackOk.at(iTrack1) = false;
}


bool PixelTrackCleaner::recHitsAreEqual(const TrackingRecHit *recHit1, const TrackingRecHit *recHit2)
{
  if (recHit1->geographicalId() != recHit2->geographicalId()) return false;
  LocalPoint pos1 = recHit1->localPosition();
  LocalPoint pos2 = recHit2->localPosition();
  return ((pos1.x() == pos2.x()) && (pos1.y() == pos2.y()));
}