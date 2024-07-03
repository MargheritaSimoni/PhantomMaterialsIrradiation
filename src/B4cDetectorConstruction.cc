#include "B4cDetectorConstruction.hh"
#include "B4cCalorimeterSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"
#include "G4RotationMatrix.hh"
#include "G4SubtractionSolid.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4NCrystal/G4NCrystal.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
G4GlobalMagFieldMessenger* B4cDetectorConstruction::fMagFieldMessenger = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::B4cDetectorConstruction()
: G4VUserDetectorConstruction(),
fCheckOverlaps(true),
fNofLayers(-1)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cDetectorConstruction::~B4cDetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::Construct()
{
    // Define materials
    DefineMaterials();

    // Define volumes
    return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::DefineMaterials()
{
    //VACUUM
    G4double z, a, density;
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                   kStateGas, 2.73*kelvin, 3.e-18*pascal);


    //STANDARD GEANT4 MATERIALS
    auto nistManager = G4NistManager::Instance();
    // air
    nistManager->FindOrBuildMaterial("G4_AIR");
    // water
    nistManager->FindOrBuildMaterial("G4_WATER"); //nb.: water is one of the only three materials that are converted to work with thermal neutron libraries

    // PMMA
    nistManager->FindOrBuildMaterial("G4_PLEXIGLASS");
    nistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE");
    nistManager->FindOrBuildMaterial("G4_POLYPROPYLENE");

    // Cd
    nistManager->FindOrBuildMaterial("G4_Cd");

    // MATERIALS DEFINED USING THERMAL LIBRARIES CROSS SECTIONS
//PMMA uses thermal XS of H inside polyethylene
    G4Element* elC = nistManager->FindOrBuildElement("C");
    G4Element* elO = nistManager->FindOrBuildElement("O");
    //G4Element* elOO = new G4Element("Oxygen" ,"O", z= 8., 16.00*g/mole);
    G4Element* elTSHPE = new G4Element("TS_H_of_Polyethylene" , "h_polyethylene", 1.0, 1.0079*g/mole);
    G4Material* matPMMA_TS = new G4Material("G4_PLEXIGLASS_TS", density=1.18*g/cm3, 3, kStateSolid,  293.15*kelvin);
    matPMMA_TS->AddElement(elTSHPE,8);
    matPMMA_TS->AddElement(elO,2);
    matPMMA_TS->AddElement(elC, 5);


    //MATERIALS DEFINED USING NCRYSTAL LIBRARIES nb: NC has a standard temperature of 293..15 instead of 273.15 og G4
    //nb.: NC takes the density from the file
    G4Material * matPMMA_NC = G4NCrystal::createMaterial("PMMA.ncmat");
    matPMMA_NC->SetName("NC_PLEXIGLASS");

    G4Material * matPVC_NC = G4NCrystal::createMaterial("PVC.ncmat");
    matPVC_NC->SetName("NC_POLYVINYL_CHLORIDE");

    G4Material * matPP_NC = G4NCrystal::createMaterial("PP.ncmat");
    matPP_NC->SetName("NC_POLYPROPYLENE");

    G4Material * matH2O_NC = G4NCrystal::createMaterial("LiquidWaterH2O_T293.6K.ncmat");
    matH2O_NC->SetName("NC_WATER");

    // Print materials
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B4cDetectorConstruction::DefineVolumes()
{
    // Geometry parameters
    // world
    G4double worldSizeXY = 25. *cm;
    G4double worldSizeZ  = 25. *cm;

    //detector
    G4double detectorXY =  20.*cm;
    G4double detectorZ =  0.2*mm;

    //Polymer solid
    G4double dimPolymerXY=10.*cm;//20.*cm;
G4double dimPolymerZ=7.714852819736141*cm;

    //Cd foil
    G4double foilCdX =  1*mm;
    G4double foilCdY =  dimPolymerXY;
    G4double foilCdZ =  dimPolymerZ;

    // Positions
    G4double samplePositionZ= 0.0; //
    G4double detectorPositionZ= dimPolymerZ/2.+detectorZ/2.+0.5*cm;//0.5*mm;
    G4double foilPositionX= foilCdX/2 + dimPolymerXY/2 + 1*mm; //
    
    fNofLayers = 1;

    // Get materials
auto polymer = G4Material::GetMaterial("G4_WATER");
    auto worldMaterial = G4Material::GetMaterial("Galactic");
    auto cadmium = G4Material::GetMaterial("G4_Cd");

    if (  !polymer ||  !worldMaterial || !cadmium ) {
        G4ExceptionDescription msg;
        msg << "Cannot retrieve materials already defined.";
        G4Exception("B4DetectorConstruction::DefineVolumes()",
                    "MyCode0001", FatalException, msg);
    }


//
    // World
    //
    auto worldS
            = new G4Box("World",           // its name
                        worldSizeXY/2, worldSizeXY/2, worldSizeZ/2); // its size

    auto worldLV
            = new G4LogicalVolume(
                    worldS,           // its solid
                    worldMaterial,  // its material
                    "World");         // its name

    auto worldPV
            = new G4PVPlacement(
                    0,                // no rotation
                    G4ThreeVector(),  // at (0,0,0)
                    worldLV,          // its logical volume
                    "World",          // its name
                    0,                // its mother  volume
                    false,            // no boolean operation
                    0,                // copy number
                    fCheckOverlaps);  // checking overlaps

    //
    // Detector
    //

    auto detectorS
            = new G4Box("Detector",     // its name
                        detectorXY/2, detectorXY/2, detectorZ/2); // its size

    auto detectorLV
            = new G4LogicalVolume(
                    detectorS,     // its solid
                    worldMaterial,  // its material
                    "detectorLV");   // its name

    new G4PVPlacement(
            0,                // no rotation
            G4ThreeVector(0,0,detectorPositionZ+detectorZ/2),  // at (0,0,0)
            detectorLV,          // its logical volume
            "Detector",    // its name
            worldLV,          // its mother  volume
            false,            // no boolean operation
            0,                // copy number
            fCheckOverlaps);  // checking overlaps

    //
    // air layer for detector
    //

    auto foilCdS
            = new G4Box("foilCd",             // its name
                        foilCdX/2, foilCdY/2, foilCdZ/2); // its size

    auto foilCdLV
            = new G4LogicalVolume(
                    foilCdS,             // its solid
                    cadmium,      // its material
                    "foilCdLV");         // its name
    new G4PVPlacement(
            0,                // no rotation
            G4ThreeVector(foilPositionX,0,0), // its position
            foilCdLV,            // its logical volume
            "foilCd",            // its name
            worldLV,          // its mother  volume
            false,            // no boolean operation
            0,                // copy number
            fCheckOverlaps);  // checking overlaps

    ////////////////////////////////////////////////////////////////////////////


    auto VolumePolymerS
            = new G4Box("VolumePolymer",             // its name
                        dimPolymerXY/2, dimPolymerXY/2, dimPolymerZ/2); // its size

    auto VolumePolymerLV
            = new G4LogicalVolume(
                    VolumePolymerS,             // its solid
                    polymer,      // its material
                    "VolumePolymerLV");         // its name
    new G4PVPlacement(
            0,                // no rotation
            G4ThreeVector(0,0,-samplePositionZ), // its position
            VolumePolymerLV,            // its logical volume
            "VolumePolymer",            // its name
            worldLV,          // its mother  volume
            false,            // no boolean operation
            0,                // copy number
            fCheckOverlaps);  // checking overlaps



    //
    // Visualization attributes
    //
    worldLV->SetVisAttributes (G4VisAttributes::GetInvisible());

    G4VisAttributes* polymerAtt = new G4VisAttributes(G4Colour(1., 0., 1.));
    polymerAtt->SetVisibility(true);
    polymerAtt->SetForceSolid(true);
    VolumePolymerLV->SetVisAttributes(polymerAtt);

    // Set transparency attributes
    G4VisAttributes* visAttributesDetector = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0, 0.5)); // Red color with 50% transparency
    visAttributesDetector->SetForceWireframe(true); // Display wireframe
    visAttributesDetector->SetForceSolid(true);
    detectorLV->SetVisAttributes(visAttributesDetector);

    G4VisAttributes* visAttributesFoilCd = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.5)); // Green color with 50% transparency
    visAttributesFoilCd->SetForceSolid(true);
    foilCdLV->SetVisAttributes(visAttributesFoilCd);


    //
    // Always return the physical World
    //
    return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cDetectorConstruction::ConstructSDandField()
{
    // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

    //
    // Sensitive detectors
    //
    auto foilSD
    = new B4cCalorimeterSD("foilSD", "foilHitsCollection", fNofLayers);
    G4SDManager::GetSDMpointer()->AddNewDetector(foilSD);
    SetSensitiveDetector("foilCdLV",foilSD);

    auto detectorSD
            = new B4cCalorimeterSD("detectorSD", "detectorHitsCollection", fNofLayers);
    G4SDManager::GetSDMpointer()->AddNewDetector(detectorSD);
    SetSensitiveDetector("detectorLV",detectorSD);

    auto sampleSD
    = new B4cCalorimeterSD("sampleSD", "sampleHitsCollection", fNofLayers);
    G4SDManager::GetSDMpointer()->AddNewDetector(sampleSD);
    SetSensitiveDetector("VolumePolymerLV",sampleSD);

    //
    // Magnetic field
    //
    // Create global magnetic field messenger.
    // Uniform magnetic field is then created automatically if
    // the field value is not zero.
    G4ThreeVector fieldValue;
    fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
    fMagFieldMessenger->SetVerboseLevel(1);

    // Register the field messenger for deleting
    G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
