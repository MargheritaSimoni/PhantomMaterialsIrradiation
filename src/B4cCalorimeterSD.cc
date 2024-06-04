#include "B4cCalorimeterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorimeterSD::B4cCalorimeterSD(
                                   const G4String& name,
                                   const G4String& hitsCollectionName,
                                   G4int nofCells)
: G4VSensitiveDetector(name),
fHitsCollection(nullptr),
fNofCells(nofCells)
{
    collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorimeterSD::~B4cCalorimeterSD() 
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection
    fHitsCollection
    = new B4cCalorHitsCollection(SensitiveDetectorName, collectionName[0]);
    
    // Add this collection in hce
    auto hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection( hcID, fHitsCollection );
    
    // Create hits
    // fNofCells for cells + one more for total sums
    for (G4int i=0; i<fNofCells+1; i++ ) {
        fHitsCollection->insert(new B4cCalorHit());
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B4cCalorimeterSD::ProcessHits(G4Step* step, 
                                     G4TouchableHistory*)
{  
    // energy deposit
    auto edep = step->GetTotalEnergyDeposit();
    
    // step length
    G4double stepLength = 0.;
    stepLength = step->GetStepLength();

    if ( edep==0. && stepLength == 0. ) return false;
    auto touchable = (step->GetPreStepPoint()->GetTouchable());
    
    // Get calorimeter cell id
    auto layerNumber = touchable->GetReplicaNumber(1);
    
    // Get hit accounting data for this cell
    auto hit = (*fHitsCollection)[layerNumber];
    if ( ! hit ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hit " << layerNumber;
        G4Exception("B4cCalorimeterSD::ProcessHits()",
                    "MyCode0004", FatalException, msg);
    }
    
    // Get hit for total accounting
    auto hitTotal
    = (*fHitsCollection)[fHitsCollection->entries()-1];
    
    // Add values
    hit->Add(edep, stepLength);
    hitTotal->Add(edep, stepLength);
    
    // Qui prende la definizione della particle ID (pid)
    auto pid = step->GetTrack()->GetDefinition()->GetParticleName();
    
    // Questa funzione controlla se la traccia passa dal volume xtal al volume pmt
    G4bool neutronCross = Mcross(step, "AirLayer", "Detector");
    
    //se un fotone ottico passa dal cristallo al pmt entra in questo blocco
    if (pid == "neutron" && neutronCross) { // replaced opticalphoton with NEUTRON

        // prende l'energia cinetica del fotone e chiama la funzione AddPhoton() che è definita nel CalorHit.cc, è semplicemente un contatore che aggiunge +1 al numero di fotoni totali
        G4double neutronE = step->GetPostStepPoint()->GetKineticEnergy();
        G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
        hit->AddNeutron(neutronE);
        hit->AddPosition(pos.x(), pos.y());
        hitTotal->AddNeutron(neutronE);
        hitTotal->AddPosition(pos.x(), pos.y());

        //G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
        //hitTotal->AddPosition(pos.x, pos.y);

        //G4cout<<pid<<" - pre: "<<step->GetPreStepPoint()->GetPhysicalVolume()->GetName()<< " - post: "<<step->GetPostStepPoint()->GetPhysicalVolume()->GetName()<< "- photon Cross: "<< neutronCross<< " Neutron Energy: "<<neutronE<<G4endl;
        // questo termina la traccia in modo da risparmiare tempo nella simulazione
        Kill(step);
    }
    return true;
}


void B4cCalorimeterSD::Kill(G4Step *step){
    step->GetTrack()->SetTrackStatus(fStopAndKill);
}

//returns true if the particle crosses the boundary between vol1 and vol2
//Note: use the Physical Volumes names!
bool B4cCalorimeterSD::Mcross(const G4Step* aStep, const G4String& vol1_name, const G4String& vol2_name){
    G4bool did_cross=false;
    if (aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName()==vol1_name && aStep->GetPostStepPoint()->GetPhysicalVolume()->GetName()==vol2_name){
        did_cross = true;
    }
    return did_cross;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cCalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
    if ( verboseLevel>1 ) {
        G4int nofHits = fHitsCollection->entries();
        G4cout
        << G4endl
        << "-------->Hits Collection: in this event they are " << nofHits
        << " hits in the tracker chambers: " << G4endl;
        for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
    }
}
