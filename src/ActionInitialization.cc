#include "ActionInitialization.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"
#include "EventAction_Messenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorAction_Messenger.hh"
#include "RunAction.hh"
#include "TrackingAction.hh"

ActionInitialization::ActionInitialization(DetectorConstruction* detector)
  : fDetector(detector)
{
  // Allocate the shared master-level timer.
  fStopwatch.Timer = new G4Timer();
}

ActionInitialization::~ActionInitialization()
{
  delete fStopwatch.Timer;
  // fMasterGenerator is deleted by Geant4 if registered via SetUserAction,
  // but since we do not register it, we own it and must delete it.
  delete fMasterGenerator;
}

// ---------------------------------------------------------------------------
// BuildForMaster — master thread only (MT builds).
// The master does not process events, but must register all messengers whose
// macro commands may appear in .mac files BEFORE /run/initialize. In MT mode
// Build() is called per-worker during Initialize(), so any pre-initialize
// commands must be registered here on the master thread first.
// ---------------------------------------------------------------------------
void ActionInitialization::BuildForMaster() const
{
  G4cout << "ActionInitialization: building master thread actions" << G4endl;

  auto* eventAction = new EventAction();
  // Give the master EventAction a pointer to the shared Stopwatch so that
  // RunAction::EndOfRunAction can access the timer and event count.
  eventAction->SetStopwatch(&fStopwatch);
  // Register /output/ commands on master so they are available pre-initialize.
  (void) new EventAction_Messenger(eventAction);

  SetUserAction(new RunAction(fDetector, eventAction));

  // Register /Source/ commands on master so .mac files can configure the
  // source before /run/initialize. The master PrimaryGeneratorAction is never
  // used for event generation — it exists only to keep the messenger alive.
  // Worker instances created in Build() receive the same macro commands
  // (the G4UImanager command tree is shared across all threads).
  fMasterGenerator = new PrimaryGeneratorAction(fDetector);
  (void) new PrimaryGeneratorAction_Messenger(fMasterGenerator);
  // Do NOT call SetUserAction(fMasterGenerator) here — the master thread
  // must not have a PrimaryGeneratorAction in MT mode.
}

// ---------------------------------------------------------------------------
// Build — called once per worker thread (MT) or once on main thread (serial).
// Creates the full set of user actions required to process events.
// ---------------------------------------------------------------------------
void ActionInitialization::Build() const
{
  // EventAction: owns the per-thread output file and hit aggregation.
  auto* eventAction = new EventAction();
  // Share the Stopwatch so the atomic event counter and master timer are visible.
  eventAction->SetStopwatch(&fStopwatch);
  SetUserAction(eventAction);
  // Messenger registers /output/ macro commands; ownership stays with caller.
  (void) new EventAction_Messenger(eventAction);

  // Primary generator: emits particles for each event.
  auto* generatorAction = new PrimaryGeneratorAction(fDetector);
  SetUserAction(generatorAction);
  (void) new PrimaryGeneratorAction_Messenger(generatorAction);

  // RunAction: opens/closes per-thread output file; master handles timing.
  SetUserAction(new RunAction(fDetector, eventAction));

  // TrackingAction: records emitted gamma kinematics into EventInformation.
  SetUserAction(new TrackingAction(eventAction));
}
