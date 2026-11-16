#include "EventAction.hh"
#include "RunAction.hh"
#include "G4AutoLock.hh"

// Mutex protecting console progress output so that worker threads do not
// interleave their progress lines. Only used in MT builds.
namespace {
  G4Mutex outputMutex = G4MUTEX_INITIALIZER;
}

EventAction::EventAction()
{
  gammaCollectionID = -1;
  outDetsOnly       = false;
  NTotalEvents      = 0;
  everyNevents      = 1;
  stopwatch         = nullptr;
}

EventAction::~EventAction()
{
}

// ---------------------------------------------------------------------------
// threadSuffix — insert "_tN" before the last '.' in baseName.
// Returns baseName unchanged when not running under G4MTRunManager
// (G4GetThreadId() returns -1 in serial builds).
// ---------------------------------------------------------------------------
G4String EventAction::threadSuffix(const G4String& baseName) const
{
  if (baseName.empty()) return baseName;

  const auto tid = G4Threading::G4GetThreadId();
  if (tid < 0)          // serial build: no suffix
    return baseName;

  const G4String suffix = "_t" + std::to_string(tid);

  // Insert suffix before the last '.' (after the last path separator).
  const std::string s    = baseName;
  const auto slash = s.find_last_of("/\\");
  const auto dot   = s.find_last_of('.');
  const bool hasExt = (dot != std::string::npos) &&
                      (slash == std::string::npos || dot > slash);

  if (!hasExt) return baseName + suffix;
  return s.substr(0, dot) + suffix + s.substr(dot);
}

// ---------------------------------------------------------------------------
void EventAction::BeginOfEventAction(const G4Event* ev)
{
  evt = ev;

  // Add a G4UserEventInformation object to store event info
  EventInformation* eventInfo = new EventInformation;
  G4EventManager::GetEventManager()->SetUserInformation(eventInfo);

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  if (gammaCollectionID < 0)
    gammaCollectionID = SDman->GetCollectionID("gammaCollection");
}


void EventAction::EndOfEventAction(const G4Event* ev)
{
  evt = ev;

  ios::fmtflags f(G4cout.flags());

  // --- Progress reporting ---------------------------------------------------
  // In MT mode use the shared atomic counter so progress reflects all threads.
  // The stopwatch is null only if SetStopwatch() was never called (should not
  // happen in normal use, but guard defensively).
  if (stopwatch) {
    G4int completed =
      stopwatch->completedEvents.fetch_add(1, std::memory_order_relaxed) + 1;

    if (completed % everyNevents == 0 && completed > 0) {
      stopwatch->Timer->Stop();
      stopwatch->timerCount++;
      stopwatch->rate +=
        ((double)everyNevents / stopwatch->Timer->GetRealElapsed() - stopwatch->rate)
        / stopwatch->timerCount;

      // Wrap console output in a mutex so worker threads don't interleave.
      G4AutoLock lock(&outputMutex);
      G4cout << std::fixed << std::setprecision(0) << std::setw(3)
             << std::setfill(' ')
             << (float)completed / NTotalEvents * 100 << " %   "
             << stopwatch->rate << " events/s ";

      G4double hours, minutes, seconds;
      G4double time = (float)(NTotalEvents - completed) / stopwatch->rate;
      hours   = floor(time / 3600.0);
      if (hours > 0) {
        G4cout << std::setprecision(0) << std::setw(2) << hours << ":";
        G4cout << std::setfill('0');
      } else {
        G4cout << std::setfill(' ');
      }
      minutes = floor(fmod(time, 3600.0) / 60.0);
      if (minutes > 0) {
        G4cout << std::setprecision(0) << std::setw(2) << minutes << ":";
        G4cout << std::setfill('0');
      } else {
        G4cout << std::setfill(' ');
      }
      seconds = fmod(time, 60.0);
      if (seconds > 0)
        G4cout << std::setprecision(0) << std::setw(2) << seconds;
      G4cout << std::setfill(' ');
      G4cout << " remaining       \r" << std::flush;

      stopwatch->Timer->Start();
    }
  }

  // --- Hit collection -------------------------------------------------------
  EventInformation* eventInfo = (EventInformation*)ev->GetUserInformation();

  G4HCofThisEvent* HCE = ev->GetHCofThisEvent();
  if (HCE) {
    TrackerGammaHitsCollection* gammaCollection =
      (TrackerGammaHitsCollection*)(HCE->GetHC(gammaCollectionID));

    G4int Nhits = gammaCollection->entries();

    if (Nhits > 0) {

      G4double Edep[100]     = {0};
      G4int    firstHit[100] = {-1};
      G4int    detMax        = 0;

      for (G4int i = 0; i < Nhits; i++) {
        G4int    det = (*gammaCollection)[i]->GetDetID() - 1;
        G4double E   = (*gammaCollection)[i]->GetEdep() / keV;

        Edep[det] += E;

        if (det > detMax) detMax = det;

        // Sometimes the first hit is not a gamma — use the first hit in the
        // collection as fallback; TrackerGammaSD marks the true first gamma hit.
        if (firstHit[det] < 0)
          firstHit[det] = i;
        if ((*gammaCollection)[i]->IsFirst())
          firstHit[det] = i;
      }

      G4int event_id = ev->GetEventID();

      // Count detectors with non-zero energy deposit.
      G4int NDetsHit = 0;
      for (int det = 0; det < detMax + 1; det++)
        if (Edep[det] > 0) NDetsHit++;

      // Detected event header line.
      evfile << "D" << std::setw(4) << NDetsHit
             << std::setw(15) << std::right << event_id << G4endl;

      // Per-crystal detection lines.
      for (int det = 0; det < detMax + 1; det++) {
        if (Edep[det] > 0) {
          // Full-energy peak check (including two-gamma pileup).
          G4int FEP = 0;
          for (G4int i = 0; i < eventInfo->GetNEmittedGammas(); i++) {
            if (abs(eventInfo->GetEmittedGammaEnergy(i) - Edep[det]) < 0.001)
              FEP = 1;
            for (G4int j = i + 1; j < eventInfo->GetNEmittedGammas(); j++) {
              if (abs(eventInfo->GetEmittedGammaEnergy(i)
                      + eventInfo->GetEmittedGammaEnergy(j)
                      - Edep[det]) < 0.001)
                FEP = 1;
            }
          }
          eventInfo->SetFullEnergy(FEP);

          evfile
            << "C" << std::setw(4) << std::right
            << (*gammaCollection)[firstHit[det]]->GetDetID()
            << std::fixed << std::setprecision(2) << std::setw(10) << std::right
            << Edep[det]
            << std::setw(12) << std::right
            << (*gammaCollection)[firstHit[det]]->GetPos().getX() / mm
            << std::setw(12) << std::right
            << (*gammaCollection)[firstHit[det]]->GetPos().getY() / mm
            << std::setw(12) << std::right
            << (*gammaCollection)[firstHit[det]]->GetPos().getZ() / mm
            << std::setw(4) << std::right
            << FEP
            << std::setprecision(2) << std::setw(12) << std::right
            << std::scientific
            << (*gammaCollection)[firstHit[det]]->GetGlobalTime()
            << G4endl;
        }
      }
    }
  }

  // Emitted gamma kinematics lines (unless suppressed by /output/detsOnly).
  G4int event_id = ev->GetEventID();
  if (!outDetsOnly) {
    evfile << "E" << std::setw(4) << eventInfo->GetNEmittedGammas()
           << std::setw(15) << std::right << event_id << G4endl;
    for (G4int i = 0; i < eventInfo->GetNEmittedGammas(); i++) {
      evfile << "     "
             << std::fixed << std::setprecision(2)
             << std::setw(10) << std::right
             << eventInfo->GetEmittedGammaEnergy(i)
             << std::setw(12) << std::right
             << eventInfo->GetEmittedGammaPosX(i)
             << std::setw(12) << std::right
             << eventInfo->GetEmittedGammaPosY(i)
             << std::setw(12) << std::right
             << eventInfo->GetEmittedGammaPosZ(i)
             << std::setw(12) << std::right
             << std::setprecision(4)
             << eventInfo->GetEmittedGammaPhi(i)
             << std::setw(12) << std::right
             << eventInfo->GetEmittedGammaTheta(i)
             << G4endl;
    }
  }

  G4cout.flags(f);
}
// --------------------------------------------------
void EventAction::openEvfile()
{
  // Apply per-thread suffix so each worker writes its own file in MT mode.
  // In serial builds threadSuffix() returns the name unchanged.
  G4String threadedName = threadSuffix(outFileName);
  if (!evfile.is_open()) evfile.open(threadedName.c_str());
  if (!evfile.is_open())
    G4cout << "ERROR opening evfile." << G4endl;
  else
    G4cout << "\nOpened output file: " << threadedName << G4endl;
}
//--------------------------------------------------- 
void EventAction::closeEvfile()
{
  evfile.close();
}
//----------------------------------------------------
void EventAction::SetOutFile(G4String name)
{
  outFileName = name;
  closeEvfile();
  openEvfile();
}
