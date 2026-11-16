#pragma once
// Stopwatch.hh — shared timing and event-counting state for MT runs.
// Owned by ActionInitialization; each worker EventAction holds a pointer.
// Only completedEvents is accessed cross-thread (std::atomic).
// All other members are accessed only on the master thread or under G4AutoLock.

#include "G4Timer.hh"
#include <atomic>

struct Stopwatch {
  G4int    timerCount = 0;
  G4Timer* Timer      = nullptr;   // owned by ActionInitialization
  G4double rate       = 0.0;
  std::atomic<G4int> completedEvents{0};
};
