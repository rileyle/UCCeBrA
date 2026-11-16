#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "DetectorConstruction.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "G4UnitsTable.hh"
#include "G4Threading.hh"
#include "G4Timer.hh"
#include "EventAction.hh"

// RunAction — per-run user action.
// In MT mode the master RunAction handles overall timing and summary output.
// Worker RunActions open/close each worker's own output file.
class RunAction : public G4UserRunAction
{
public:
  RunAction(DetectorConstruction*, EventAction*);
  ~RunAction();

  void BeginOfRunAction(const G4Run*);
  void EndOfRunAction(const G4Run*);

private:
  DetectorConstruction* myDetector;
  EventAction*          evaction;
  // Dedicated wall-clock timer for the master end-of-run summary.
  // Separate from the Stopwatch timer so progress Stop()/Start() cycles
  // do not reset the total elapsed time measurement.
  G4Timer               fRunTimer;
};

#endif
