#include "RunAction.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#endif

#include <iomanip>
#include <cmath>

RunAction::RunAction(DetectorConstruction* detector, EventAction* ev)
  : myDetector(detector), evaction(ev)
{
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  if (G4Threading::IsMasterThread()) {
    // Master thread: print the run header and start the global timer.
    G4cout << " Beginning of run" << G4endl;
    G4cout << " Simulating " << run->GetNumberOfEventToBeProcessed()
           << " events." << G4endl;

    // Set progress-reporting interval on the master EventAction based on
    // total event count.
    evaction->SetNTotalevents(run->GetNumberOfEventToBeProcessed());
    if      (run->GetNumberOfEventToBeProcessed() > 1000000) evaction->SetEveryNEvents(10000);
    else if (run->GetNumberOfEventToBeProcessed() > 1000)    evaction->SetEveryNEvents(1000);
    else if (run->GetNumberOfEventToBeProcessed() > 100)     evaction->SetEveryNEvents(100);
    else                                                      evaction->SetEveryNEvents(1);

    // Start the per-event progress timer (shared Stopwatch).
    if (evaction->GetStopwatch() && evaction->GetStopwatch()->Timer)
      evaction->GetStopwatch()->Timer->Start();

    // Start the dedicated run-level wall-clock timer for the end-of-run summary.
    // This timer is never stopped and restarted mid-run, so GetRealElapsed()
    // always reflects the full run duration.
    fRunTimer.Start();

  } else {
    // Worker thread: set event count and reporting interval.
    // The output file is opened by EventAction::SetOutFile() when the
    // /Output/Filename macro command is replayed on this worker thread.
    // We do not call openEvfile() here to avoid double-opening the file.
    evaction->SetNTotalevents(run->GetNumberOfEventToBeProcessed());
    if      (run->GetNumberOfEventToBeProcessed() > 1000000) evaction->SetEveryNEvents(10000);
    else if (run->GetNumberOfEventToBeProcessed() > 1000)    evaction->SetEveryNEvents(1000);
    else if (run->GetNumberOfEventToBeProcessed() > 100)     evaction->SetEveryNEvents(100);
    else                                                      evaction->SetEveryNEvents(1);
  }
}

void RunAction::EndOfRunAction(const G4Run*)
{
  if (G4Threading::IsMasterThread()) {
    // Master thread: stop the run-level timer and print the end-of-run summary.
    fRunTimer.Stop();
    {
      G4Timer* t = &fRunTimer;

      G4cout << "                                                     " << G4endl;

      // Helper lambda: print elapsed time in hh:mm:ss format.
      auto printTime = [](const char* label, G4double time) {
        G4cout << label;
        G4double hours   = floor(time / 3600.0);
        G4double minutes = floor(fmod(time, 3600.0) / 60.0);
        G4double seconds = fmod(time, 60.0);
        if (hours > 0) {
          G4cout << std::setprecision(0) << std::setw(2) << hours << ":";
          G4cout << std::setfill('0');
        } else {
          G4cout << std::setfill(' ');
        }
        if (minutes > 0) {
          G4cout << std::setprecision(0) << std::setw(2) << minutes << ":";
          G4cout << std::setfill('0');
        } else {
          G4cout << std::setfill(' ');
        }
        if (seconds > 0)
          G4cout << std::setprecision(2) << std::setw(5) << seconds;
        G4cout << std::setfill(' ');
      };

      printTime("Real time: ",      t->GetRealElapsed());
      printTime("   System time: ", t->GetSystemElapsed());
      printTime("   User time: ",   t->GetUserElapsed());

      G4int total = evaction->GetNTotalevents();
      if (t->GetRealElapsed() > 0)
        G4cout << "   " << total / t->GetRealElapsed() << " events/s" << G4endl;
    }

  } else {  // Worker thread
    // Worker thread: close this worker's per-thread output file.
    evaction->closeEvfile();
  }
}
