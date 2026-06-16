/*#ifndef New_Target_H
#define New_Target_H 1

#include "G4Material.hh"
#include "Materials.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "CADMesh.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "globals.hh"

using namespace std;

class New_Target
{
public:

  G4LogicalVolume *expHall_log;
  Materials* materials;

  New_Target(G4LogicalVolume*, Materials*);
  ~New_Target();

  void Construct();
  
  G4double Hemi_Radius;      // Outer
  G4double Hemi_Thickness;

  G4double Mid_Radius;     //Outer
  G4double Mid_Thickness;
  G4double Mid_Length;

  G4double Ring_Radius;     //Outer
  G4double Ring_Thickness;
  G4double Ring_Length;
  
  G4double End_Radius;     //Outer
  G4double End_Thickness;
  G4double End_Length;

  G4Material* Al;

  G4double startAngle; 
  G4double spanningAngle; 

  G4ThreeVector Hemi_Pos;
  //G4RotationMatrix Hemi_Rot;
  
  G4ThreeVector Mid_Shift;
  //G4RotationMatrix Mid_Rot;
  
  G4ThreeVector Ring_Shift;
 // G4RotationMatrix Ring_Rot;

  G4ThreeVector End_Shift;
  G4RotationMatrix Rot;

  G4LogicalVolume*  Hemi_log;
  G4VPhysicalVolume* Hemi_phys;
  
  G4LogicalVolume*  Mid_log;
  G4VPhysicalVolume* Mid_phys;
  
  G4LogicalVolume*  Ring_log;
  G4VPhysicalVolume* Ring_phys;

  G4LogicalVolume*  End_log;
  G4VPhysicalVolume* End_phys;

};

#endif*/
