#include "B4PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4Threading.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double pos_z = -19. * cm;

B4PrimaryGeneratorAction::B4PrimaryGeneratorAction()
        : G4VUserPrimaryGeneratorAction(),
          fParticleGun(nullptr)
{

    G4int nofParticles = 1;
    fParticleGun = new G4ParticleGun(nofParticles);

    // default particle kinematic
    //
    auto particleDefinition
            = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
    fParticleGun->SetParticleDefinition(particleDefinition);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -16.*mm));
    fParticleGun->SetParticleEnergy(0.025*eV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4PrimaryGeneratorAction::~B4PrimaryGeneratorAction()
{
    delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

    G4double halfX = 2.5 * cm;
    G4double halfY = 2.5 * cm;
    G4double px = G4UniformRand() * (2.* halfX) - halfX;
    G4double py = G4UniformRand() * (2.* halfY) - halfY;

    // Set gun position
    fParticleGun
            ->SetParticlePosition(G4ThreeVector(px * mm, py * mm, pos_z));

    //G4double energy = G4RandGauss(2.5e-3, 1e-4);
    //fParticleGun->SetParticleEnergy(energy * eV);

    fParticleGun->GeneratePrimaryVertex(anEvent);
}
