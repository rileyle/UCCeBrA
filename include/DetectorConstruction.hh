#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

#include "G4VUserDetectorConstruction.hh"
#include "Materials.hh"
#include "Experimental_Hall.hh"
#include "Experimental_Hall_Messenger.hh"
#include "CeBrA_Array.hh"
#include "CeBrA_Array_Messenger.hh"
#include "Cradle.hh"
#include "Cradle_Messenger.hh"
#include "Lead_Brick.hh"
#include "Lead_Brick_Messenger.hh"
#include "Lab_Bench.hh"
#include "Lab_Bench_Messenger.hh"
#include "Target_Chamber.hh"
#include "Target_Chamber_Messenger.hh"
#include "Source_Capsule.hh"
#include "Source_Capsule_Messenger.hh"
#include "Source_Cradle.hh"
#include "Detector_Mount.hh"
#include "Detector_Mount_Messenger.hh"
#include "Source_Cradle_Messenger.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "TrackerGammaSD.hh"
#include "TrackerGammaSD_Messenger.hh"
#include "G4SDManager.hh"
#include "G4Tubs.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:

  DetectorConstruction();
  ~DetectorConstruction();

  G4VPhysicalVolume* Construct();

  // Called per worker thread in MT mode (and once in serial mode) to register
  // sensitive detectors. Moving SD registration here (from Construct()) is the
  // standard Geant4 MT pattern that ensures each worker thread has its own SD
  // instance properly registered in the thread-local G4SDManager.
  void ConstructSDandField() override;

  Source_Capsule* getSourceCapsule(){return capsule;};
  Cradle* getCradle(){return cradle;};
  G4LogicalVolume* HallLog(){return ExpHall_log;};

private:
  
  CeBrA_Array* the_CeBrA_Array;

  Source_Capsule* capsule;
  Source_Cradle* source_cradle;
  Lead_Brick* brick;
  Lab_Bench* bench;
  Cradle* cradle;
  Target_Chamber* chamber;

  // Logical volumes
  G4LogicalVolume* ExpHall_log;

  // Physical volumes
  G4VPhysicalVolume* ExpHall_phys;

  Experimental_Hall_Messenger* ExperimentalHallMessenger;
  TrackerGammaSD* TrackerGamma;
  TrackerGammaSD_Messenger* TrackerGammaSDMessenger;
  CeBrA_Array_Messenger* the_CeBrA_Array_Messenger;
  Source_Capsule_Messenger* capsule_Messenger;
  Source_Cradle_Messenger* source_cradle_Messenger;
  Lead_Brick_Messenger* brick_Messenger;
  Lab_Bench_Messenger* bench_Messenger;
  Cradle_Messenger* cradle_Messenger;
  Target_Chamber_Messenger* chamber_Messenger;
  Detector_Mount* mount;
  Detector_Mount_Messenger* mount_Messenger;

  Materials* materials;
};

#endif
