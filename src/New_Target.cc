/*#include "New_Target.hh"

New_Target::New_Target(G4LogicalVolume* experimentalHall_log,
			       Materials* mat)
{
  materials = mat;
  expHall_log=experimentalHall_log;

  Al = materials->FindMaterial("Al");

  Hemi_Pos.setX(0);
  Hemi_Pos.setY(0);
  Hemi_Pos.setZ(0);

  Mid_Shift.setX(0);
  Mid_Shift.setY(5 *cm);
  Mid_Shift.setZ(0);

  Ring_Shift.setX(0);
  Ring_Shift.setY(5.5*cm);
  Ring_Shift.setZ(0);

  End_Shift.setX(0);
  End_Shift.setY(6*cm);
  End_Shift.setZ(0);

  Rot = G4RotationMatrix::IDENTITY;

  Hemi_Radius    = 4.05*2.54*cm;
  Hemi_Thickness = 1.75*mm;

  Mid_Radius = 4.05*2.54*cm;
  Mid_Thickness = 1.75 * mm;
  Mid_Length = 10 * cm;

  Ring_Radius = 4.05*2.54*cm;
  Ring_Thickness = 2 * cm;
  Ring_Length = 1 * cm;

  End_Radius = 4.05*2.54*cm;
  End_Thickness = 2 * mm;
  End_Length = 1 * cm;
}

New_Target::~New_Target()
{  
}

void New_Target::Construct()
{

  G4Sphere* sphere = new G4Sphere("HemiShell", Hemi_Radius-Hemi_Thickness, Hemi_Radius,
				  0, 180.0*deg, 0, 180.0*deg);
  Hemi_log = new G4LogicalVolume(sphere, Al, "Hemi_log");

  
  Hemi_phys = new G4PVPlacement(G4Transform3D(Rot, Hemi_Pos),
                                   Hemi_log, "Hemi",
                                   expHall_log, false, 0);

  G4Tubs* cylinderMid = new G4Tubs("Mid", Mid_Radius-Mid_Thickness, Mid_Radius, Mid_Length, 0*deg, 360*deg);

  Mid_log = new G4LogicalVolume(cylinderMid, Al, "Mid_log");

  Mid_phys = new G4PVPlacement(G4Transform3D(Rot, Mid_Shift),
                                Mid_log, "Mid", 
                                expHall_log, false, 0);

  G4Tubs* cylinderRing = new G4Tubs("Ring", Ring_Radius-Ring_Thickness, Ring_Radius, Ring_Length, 0*deg, 360*deg);

  Ring_log = new G4LogicalVolume(cylinderRing, Al, "Ring_log");

  Ring_phys = new G4PVPlacement(G4Transform3D(Rot, Ring_Shift),
                                Ring_log, "Ring", 
                                expHall_log, false, 0);

  G4Tubs* cylinderEnd = new G4Tubs("End", End_Radius, End_Radius, End_Length, 0*deg, 360*deg);

  End_log = new G4LogicalVolume(cylinderEnd, Al, "End_log");

  End_phys = new G4PVPlacement(G4Transform3D(Rot, End_Shift),
                                End_log, "End", 
                                expHall_log, false, 0);

  G4Colour dGrey (0.8, 0.8, 0.8, 1.0);
  G4VisAttributes* Vis = new G4VisAttributes(dGrey);
  Vis->SetVisibility(true);
  Vis->SetForceSolid(true);

  Hemi_log->SetVisAttributes(Vis);
  Mid_log->SetVisAttributes(Vis);
  Ring_log->SetVisAttributes(Vis);
  End_log->SetVisAttributes(Vis);

  return; 
}*/
