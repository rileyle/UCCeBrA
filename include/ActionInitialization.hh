#ifndef ActionInitialization_h
#define ActionInitialization_h 1

// ActionInitialization — wires per-thread user actions for both serial and
// MT Geant4 builds. Replaces the manual SetUserAction() calls in main().
// In serial mode G4RunManager calls Build() once on the main thread.
// In MT mode G4MTRunManager calls BuildForMaster() on the master thread and
// Build() once per worker thread.

#include "G4VUserActionInitialization.hh"
#include "Stopwatch.hh"

class DetectorConstruction;
class PrimaryGeneratorAction;

class ActionInitialization : public G4VUserActionInitialization
{
public:
  explicit ActionInitialization(DetectorConstruction* detector);
  ~ActionInitialization() override;

  // Called once on the master thread (MT only). Creates minimal master actions.
  void BuildForMaster() const override;

  // Called once per worker thread (MT) or once on the main thread (serial).
  // Creates the full set of user actions for event processing.
  void Build() const override;

private:
  DetectorConstruction*       fDetector;
  mutable Stopwatch           fStopwatch;       // shared; EventAction holds a pointer to it
  // Master PrimaryGeneratorAction: kept alive so that /Source/ messengers
  // registered in BuildForMaster() remain valid until the run ends.
  // Not used for event generation (workers have their own instances).
  mutable PrimaryGeneratorAction* fMasterGenerator = nullptr;
};

#endif
