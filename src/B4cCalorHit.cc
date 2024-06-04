#include "B4cCalorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include <iomanip>

// nb: scambiato tutti gli fPhotons con fNeutrons

G4ThreadLocal G4Allocator<B4cCalorHit>* B4cCalorHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHit::B4cCalorHit()
 : G4VHit(),
   fEdep(0.),
   fTrackLength(0.),
   fNeutrons(0),
   eNeutron(-999),
   xPos(-999),
   yPos(-999)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHit::~B4cCalorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHit::B4cCalorHit(const B4cCalorHit& right)
  : G4VHit()
{
  fEdep        = right.fEdep;
  fTrackLength = right.fTrackLength;
    fNeutrons = right.fNeutrons;
    eNeutron = right.eNeutron;
    xPos = right.xPos;
    yPos = right.yPos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const B4cCalorHit& B4cCalorHit::operator=(const B4cCalorHit& right)
{
  fEdep        = right.fEdep;
  fTrackLength = right.fTrackLength;
    fNeutrons = right.fNeutrons;
    eNeutron = right.eNeutron;
    xPos = right.xPos;
    yPos = right.yPos;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B4cCalorHit::operator==(const B4cCalorHit& right) const
{
  return ( this == &right ) ? true : false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::Print()
{
  G4cout
     << "Edep: " 
     << std::setw(7) << G4BestUnit(fEdep,"Energy")
     << " track length: " 
     << std::setw(7) << G4BestUnit( fTrackLength,"Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorHit::AddNeutron(G4double neutronE) {
        fNeutrons += 1;
        eNeutron = neutronE;
   };

void B4cCalorHit::AddPosition(G4double posx, G4double posy) {
    xPos = posx;
    yPos = posy;
};
