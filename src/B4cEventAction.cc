#include "B4cEventAction.hh"
#include "B4cCalorimeterSD.hh"
#include "B4cCalorHit.hh"
#include "B4Analysis.hh"
#include "B4PrimaryGeneratorAction.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//NOTA IMPORTANTE: abso qui è la collezione di hit nel PMT, xtal nel cristallo come puoi vedere a riga 79

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::B4cEventAction()
        : G4UserEventAction(),
          fFoilHCID(-1),
          fdetectorHCID(-1),
          fSampleHCID(-1)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cEventAction::~B4cEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4cCalorHitsCollection*
B4cEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
    auto hitsCollection
            = static_cast<B4cCalorHitsCollection*>(
                    event->GetHCofThisEvent()->GetHC(hcID));

    if ( ! hitsCollection ) {
        G4ExceptionDescription msg;
        msg << "Cannot access hitsCollection ID " << hcID;
        G4Exception("B4cEventAction::GetHitsCollection()",
                    "MyCode0003", FatalException, msg);
    }

    return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::PrintEventStatistics(
        G4double nNeutrons, G4double eNeutrons,
        G4double Edep, G4double xPos, G4double yPos, G4ThreeVector gp) const
{
    // print event statistics
    G4cout
            << "   Neutrons in the detector: "
            << std::setw(7) << nNeutrons
            << "       neutron energy: "
            << std::setw(7) << G4BestUnit(eNeutrons, "Energy")
            << G4endl
            << "        total energy: "
            << std::setw(7) << G4BestUnit(Edep, "Energy")
            << G4endl
            << "        X position: "
            << std::setw(7) << G4BestUnit(xPos, "Length")
            << G4endl
            << "        Y position: "
            << std::setw(7) << G4BestUnit(yPos, "Length")
            << G4endl
            <<"Generator Position: "
            <<gp[0]<<" "<<gp[1]<<" "<<gp[2]
            <<G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::BeginOfEventAction(const G4Event* /*event*/)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4cEventAction::EndOfEventAction(const G4Event* event)
{
    // Get hits collections IDs (only once)
    if ( fFoilHCID == -1 ) {
        fFoilHCID
                = G4SDManager::GetSDMpointer()->GetCollectionID("foilHitsCollection");
    }

    if ( fdetectorHCID == -1 ) {
        fdetectorHCID
                = G4SDManager::GetSDMpointer()->GetCollectionID("detectorHitsCollection");
    }

    if ( fSampleHCID == -1 ) {
        fSampleHCID
                = G4SDManager::GetSDMpointer()->GetCollectionID("sampleHitsCollection");
    }
    // Get hits collections
    auto detectorHC = GetHitsCollection(fdetectorHCID, event);
    auto sampleHC = GetHitsCollection(fSampleHCID, event);
    auto foilHC = GetHitsCollection(fFoilHCID, event);

    //auto absoHC = GetHitsCollection(fAbsHCID, event);

    // Get hit with total values
    //auto absoHit = (*absoHC)[absoHC->entries()-1];
    auto detectorHit = (*detectorHC)[detectorHC->entries()-1];
    auto sampleHit = (*sampleHC)[sampleHC->entries()-1];
    auto foilHit = (*foilHC)[foilHC->entries()-1];


    //Raccoglie le informazioni sull'evento primario
    //GetPrimaryVertex(i) prende le info sull'i-esimo vertice, 0 è il primo in assoluto, quindi quello dove viene generata la particella incidente

    G4PrimaryVertex* pv = event->GetPrimaryVertex(0);
    // pv->GetT0()/CLHEP::ns  //questo è per avere il tempo del vertice, lo tengo come reminder
    G4ThreeVector generatorPosition = G4ThreeVector(pv->GetX0(),pv->GetY0(),pv->GetZ0());

    //Qui ci sono ulteriori info sulla particella primaria tipo energia ed impulso che in questo momento non stai usando, ma se per esempio vuoi usare un fascio non parallelo puoi decommentare qui
    G4PrimaryParticle* pp = pv->GetPrimary();
    G4double generatorEnergy = pp->GetKineticEnergy(); // + pv->GetMass();
    //G4ThreeVector generatorMomentum =  pp->GetMomentumDirection();

    // Print per event (modulo n)
    //
    auto eventID = event->GetEventID();
    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
        G4cout << "---> End of event: " << eventID << G4endl;

        PrintEventStatistics(
                detectorHit->GetNNeutronsInDetector(), detectorHit->GetENeutronsInDetector(),
                detectorHit->GetEdep(),
                detectorHit->GetXposInDetector(), detectorHit->GetYposInDetector(), generatorPosition);
    }

    // Fill histograms, ntuple
    //

    // get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();

    //Qui salva le informazioni nel tree e negli istogrammi, la funzione GetNPhotons() è definita nel CalorHit.hh e semplicemente legge il numero di fotoni salvato nella variabile fPhotons della collezione di hit.

    ///////////////////////// fill histograms //////////////////////////
    //Neutron generator
    analysisManager->FillH1(6,generatorEnergy);
    analysisManager->FillH2(2, generatorPosition[0], generatorPosition[1]);

    //transmission detector
    if(detectorHit->GetENeutronsInDetector()>=0 ){
        analysisManager->FillH1(0, detectorHit->GetENeutronsInDetector());
        analysisManager->FillH2(0, detectorHit->GetXposInDetector(), detectorHit->GetYposInDetector());
        if(detectorHit->GetXposInDetector()>=-50 && detectorHit->GetYposInDetector()>=-50 && detectorHit->GetXposInDetector()<=50 && detectorHit->GetYposInDetector()<=50){
            analysisManager->FillH1(1, detectorHit->GetENeutronsInDetector());
            analysisManager->FillH2(1, detectorHit->GetXposInDetector(), detectorHit->GetYposInDetector());
        }

    }

    //Sample
    if(sampleHit->GetBoundaryEnergy()>=0 ) {
        analysisManager->FillH1(2, sampleHit->GetBoundaryEnergy());
        analysisManager->FillH1(3, sampleHit->GetBoundaryPosition().z());
    }

    std::vector<G4double> gammaEnergies = foilHit->GetGammaFoilEnergies();
// Check if the vector is not empty
    if (!gammaEnergies.empty()) {
        for (G4double energy : gammaEnergies) {
                analysisManager->FillH1(4, energy);
        }
    }

    if(sampleHit->GetBoundaryPosition().x()>=10./2*CLHEP::cm ) {
        analysisManager->FillH1(5, sampleHit->GetBoundaryEnergy());
        analysisManager->FillH2(3, sampleHit->GetBoundaryPosition().z(), sampleHit->GetBoundaryPosition().y());

    }

    // fill ntuple
    analysisManager->FillNtupleDColumn(0, sampleHit->GetBoundaryEnergy());
    analysisManager->FillNtupleDColumn(1, sampleHit->GetBoundaryPosition().x());
    analysisManager->FillNtupleDColumn(2, sampleHit->GetBoundaryPosition().y());
    analysisManager->FillNtupleDColumn(3, sampleHit->GetBoundaryPosition().z());

    //analysisManager->FillNtupleDColumn(3, generatorPosition[0]);
    //analysisManager->FillNtupleDColumn(4, generatorPosition[1]);

    analysisManager->AddNtupleRow();
}  
