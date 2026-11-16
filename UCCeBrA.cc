//PROGRAM UCCeBrA
// Conditional MT/serial run manager: G4MTRunManager when Geant4 was built
// with multithreading support (G4MULTITHREADED defined by the build system),
// G4RunManager otherwise.
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include "G4Threading.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#ifdef G4UI_USE_XM
#include "G4UIXm.hh"
#endif

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"

#ifdef G4VIS_USE
#include "VisManager.hh"
#endif

#include "Git_Hash.hh"

int main(int argc, char** argv)
{
  // Create the run manager: multi-threaded when G4MULTITHREADED is defined
  // (i.e. Geant4 was built with MT support), single-threaded otherwise.
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
  // Use all available hardware cores.
  runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
  G4cout << "Multithreaded mode: " << G4Threading::G4GetNumberOfCores()
         << " threads" << G4endl;
#else
  G4RunManager* runManager = new G4RunManager;
#endif

  G4cout << "Executable: " << argv[0] << G4endl;
  G4cout << "Git commit: " << GIT_HASH  << G4endl;
  G4cout << "Git branch: " << GIT_BRANCH << G4endl;

  // Mandatory initialisation: geometry and physics.
  DetectorConstruction* detector = new DetectorConstruction();
  PhysicsList* physicsList = new PhysicsList(detector);
  runManager->SetUserInitialization(detector);
  runManager->SetUserInitialization(physicsList);

  // ActionInitialization replaces the old manual SetUserAction() calls.
  // It creates per-thread copies of EventAction, PrimaryGeneratorAction,
  // RunAction, and TrackingAction as required by G4MTRunManager.
  // In serial builds G4RunManager calls Build() once on the main thread.
  runManager->SetUserInitialization(new ActionInitialization(detector));

  G4UIsession* session = nullptr;

#ifdef G4VIS_USE
  G4VisManager* visManager = nullptr;
#endif

  if (argc == 1)  // interactive mode
    {
#ifdef G4VIS_USE
      visManager = new VisManager;
      visManager->Initialize();
#endif

#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);
#else
      session = new G4UIterminal();
#endif
    }

  G4UImanager* UI = G4UImanager::GetUIpointer();

  if (session)  // interactive
    {
      session->SessionStart();
      delete session;
    }
  else           // batch
    {
      G4String command  = "/control/execute ";
      G4String fileName = argv[1];
      UI->ApplyCommand(command + fileName);
    }

  if (argc == 1) {
#ifdef G4VIS_USE
    delete visManager;
#endif
  }

  delete runManager;
  return 0;
}
