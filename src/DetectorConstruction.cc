#include "DetectorConstruction.hh"

DetectorConstruction::DetectorConstruction()
{
  //  Materials* materials=new Materials();
  materials=new Materials();

  // Experimental Hall

  Experimental_Hall* ExperimentalHall = new Experimental_Hall(materials);
  ExpHall_phys=ExperimentalHall->Construct();
  ExpHall_log=ExperimentalHall->GetLogVolume();
  //   ExperimentalHall->Report();
  ExperimentalHallMessenger = new Experimental_Hall_Messenger(ExperimentalHall);

  capsule = new Source_Capsule(ExpHall_log, materials);

  capsule_Messenger = new Source_Capsule_Messenger(capsule);

  source_cradle = new Source_Cradle(ExpHall_log, materials);

  source_cradle_Messenger = new Source_Cradle_Messenger(source_cradle);
  
  brick = new Lead_Brick(ExpHall_log, materials);

  brick_Messenger = new Lead_Brick_Messenger(brick);

  bench = new Lab_Bench(ExpHall_log, materials);

  bench_Messenger = new Lab_Bench_Messenger(bench);

  chamber = new Target_Chamber(ExpHall_log, materials);

  chamber_Messenger = new Target_Chamber_Messenger(chamber);

  cradle = new Cradle(ExpHall_log, materials);

  cradle_Messenger = new Cradle_Messenger(cradle);
  
  the_CeBrA_Array = new CeBrA_Array(ExpHall_log, materials, cradle);

  the_CeBrA_Array_Messenger = new CeBrA_Array_Messenger(the_CeBrA_Array);

  mount = new Detector_Mount(ExpHall_log, materials);

  mount_Messenger = new Detector_Mount_Messenger(mount);


}

DetectorConstruction::~DetectorConstruction()
{
  delete ExperimentalHallMessenger;
  delete TrackerGammaSDMessenger;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  the_CeBrA_Array->Construct();
  return ExpHall_phys;
}

// ---------------------------------------------------------------------------
// ConstructSDandField — called once per worker thread in MT mode, and once
// on the main thread in serial mode. Creates and registers a per-thread
// TrackerGammaSD instance so that each thread has its own hit collection.
//
// This is the standard Geant4 MT pattern for sensitive detectors. Moving SD
// registration here (from Construct()) ensures the SD is properly associated
// with the thread-local G4SDManager on each worker thread.
//
// PHYSICS NOTE: The SD type, name, and the volumes it is attached to are
// unchanged from the serial implementation. This is a structural change only.
// Human review is required before this change is considered production-ready.
// ---------------------------------------------------------------------------
void DetectorConstruction::ConstructSDandField()
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();

  // Create a new TrackerGammaSD instance for this thread.
  // Each worker thread must have its own SD instance; they are not shared.
  TrackerGamma = new TrackerGammaSD("GammaTracker");
  TrackerGammaSDMessenger = new TrackerGammaSD_Messenger(TrackerGamma);
  SDman->AddNewDetector(TrackerGamma);

  // Attach the SD to all CeBr3 crystal logical volumes in the array.
  the_CeBrA_Array->MakeSensitive(TrackerGamma);
}
