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
          fAbsHCID(-1),
          fGapHCID(-1)
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
    if ( fAbsHCID == -1 ) {
        fAbsHCID
                = G4SDManager::GetSDMpointer()->GetCollectionID("airLayerHitsCollection");
        fGapHCID
                = G4SDManager::GetSDMpointer()->GetCollectionID("detectorHitsCollection");
    }

    // Get hits collections
    auto gapHC = GetHitsCollection(fAbsHCID, event);
    auto absoHC = GetHitsCollection(fGapHCID, event);

    // Get hit with total values
    auto absoHit = (*absoHC)[absoHC->entries()-1];
    auto gapHit = (*gapHC)[gapHC->entries()-1];


    //Raccoglie le informazioni sull'evento primario
    //GetPrimaryVertex(i) prende le info sull'i-esimo vertice, 0 è il primo in assoluto, quindi quello dove viene generata la particella incidente

    G4PrimaryVertex* pv = event->GetPrimaryVertex(0);
    // pv->GetT0()/CLHEP::ns  //questo è per avere il tempo del vertice, lo tengo come reminder
    G4ThreeVector generatorPosition = G4ThreeVector(pv->GetX0(),pv->GetY0(),pv->GetZ0());

    //Qui ci sono ulteriori info sulla particella primaria tipo energia ed impulso che in questo momento non stai usando, ma se per esempio vuoi usare un fascio non parallelo puoi decommentare qui
    //G4PrimaryParticle* pp = pv->GetPrimary();
    //G4double generatorEnergy = pp->GetKineticEnergy() + pp->GetMass();
    //G4ThreeVector generatorMomentum =  pp->GetMomentumDirection();

    // Print per event (modulo n)
    //
    auto eventID = event->GetEventID();
    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
        G4cout << "---> End of event: " << eventID << G4endl;

        PrintEventStatistics(
                gapHit->GetNNeutrons(), gapHit->GetENeutrons(),
                gapHit->GetEdep(),
                gapHit->GetXpos(), gapHit->GetYpos(), generatorPosition);
    }

    // Fill histograms, ntuple
    //

    // get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();

    //Qui salva le informazioni nel tree e negli istogrammi, la funzione GetNPhotons() è definita nel CalorHit.hh e semplicemente legge il numero di fotoni salvato nella variabile fPhotons della collezione di hit.

    // fill histograms
    if(gapHit->GetENeutrons()>=0 ){
        //if(gapHit->GetXpos()>=-25 && gapHit->GetYpos()>=-25 && gapHit->GetXpos()<=25 && gapHit->GetYpos()<=25){
        analysisManager->FillH1(0, gapHit->GetENeutrons());}//}

    if(gapHit->GetXpos()>-888 && gapHit->GetYpos()>-888){
        analysisManager->FillH2(0, gapHit->GetXpos(), gapHit->GetYpos());}

    analysisManager->FillH2(1, generatorPosition[0], generatorPosition[1]);

    // fill ntuple
    analysisManager->FillNtupleDColumn(0, gapHit->GetENeutrons());
    analysisManager->FillNtupleDColumn(1, gapHit->GetXpos());
    analysisManager->FillNtupleDColumn(2, gapHit->GetYpos());
    //analysisManager->FillNtupleDColumn(3, generatorPosition[0]);
    //analysisManager->FillNtupleDColumn(4, generatorPosition[1]);

    analysisManager->AddNtupleRow();
}  
