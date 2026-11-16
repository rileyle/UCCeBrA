#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4EventManager.hh"
#include "TrackerGammaSD.hh"
#include "G4Event.hh"
#include "EventInformation.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4Threading.hh"
#include "Stopwatch.hh"
#include <fstream>
#include <string>

class EventAction : public G4UserEventAction
{
public:
  EventAction();
  ~EventAction();

  void BeginOfEventAction(const G4Event*);
  void EndOfEventAction(const G4Event*);
  void SetOutFile(G4String);
  void SetOutDetsOnly() { outDetsOnly = true; }
  void closeEvfile();
  void openEvfile();
  const G4Event* GetEvent() { return evt; }

  void SetNTotalevents(G4int n) { NTotalEvents = n; }
  G4int GetNTotalevents()       { return NTotalEvents; }
  void SetEveryNEvents(G4int n) { everyNevents = n; }
  G4int GetEveryNEvents()       { return everyNevents; }

  // MT support: called by ActionInitialization::Build() to provide the shared
  // Stopwatch (atomic event counter + master timer pointer).
  void SetStopwatch(Stopwatch* sw) { stopwatch = sw; }
  Stopwatch* GetStopwatch() const  { return stopwatch; }

private:
  // Returns baseName with "_tN" inserted before the last '.' in MT mode.
  // Returns baseName unchanged in serial mode (thread id < 0).
  G4String threadSuffix(const G4String& baseName) const;

  G4int     gammaCollectionID;
  G4String  outFileName;   // base filename (without thread suffix)
  G4bool    outDetsOnly;
  std::ofstream evfile;
  G4int     NTotalEvents;
  G4int     everyNevents;
  const G4Event* evt;

  // Pointer to the shared Stopwatch owned by ActionInitialization.
  // In serial builds this is set to nullptr and the legacy timer path is used.
  Stopwatch* stopwatch = nullptr;
};

#endif //EVENTACTION_H
