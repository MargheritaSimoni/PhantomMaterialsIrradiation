#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::B4RunAction()
: G4UserRunAction()
{ 
    // set printing event number per each event
    G4RunManager::GetRunManager()->SetPrintProgress(1);
    
    // Create analysis manager
    // The choice of analysis technology is done via selectin of a namespace
    // in B4Analysis.hh
    auto analysisManager = G4AnalysisManager::Instance();
    G4cout << "Using " << analysisManager->GetType() << G4endl;
    
    // Create directories
    //analysisManager->SetHistoDirectoryName("histograms");
    //analysisManager->SetNtupleDirectoryName("ntuple");
    analysisManager->SetVerboseLevel(2);
    analysisManager->SetNtupleMerging(false);
    // Note: merging ntuples is available only with Root output
    
    // Book histograms, ntuple
    //
    
    // Creating histograms // name and description of istograms
    analysisManager->CreateH1("ENeutron","Neutron Energy in Detector",200, 1.e-04*eV, 100*eV,"eV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(0, "Energy [eV]");

    analysisManager->CreateH2("DetPos","Position in detector", 200, -5.*cm, 5.*cm, 200, -5.*cm, 5.*cm, "cm", "cm");
    analysisManager->SetH2XAxisTitle(0, "x [cm]");
    analysisManager->SetH2YAxisTitle(0, "y [cm]");

    analysisManager->CreateH2("GenPos","Generator Position", 200, -5.*cm, 5.*cm, 200, -5.*cm, 5.*cm, "cm", "cm");
    analysisManager->SetH2XAxisTitle(1, "x [cm]");
    analysisManager->SetH2YAxisTitle(1, "y [cm]");

    // Creating ntuple
    //
    analysisManager->CreateNtuple("B4", "Data Tree");
    analysisManager->CreateNtupleDColumn("Neutron_ene");
    analysisManager->CreateNtupleDColumn("X");
    analysisManager->CreateNtupleDColumn("Y");
    //analysisManager->CreateNtupleDColumn("X0");
    //analysisManager->CreateNtupleDColumn("Y0");
    analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4RunAction::~B4RunAction()
{
    delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
    //inform the runManager to save random number seed
    //G4RunManager::GetRunManager()->SetRandomNumberStore(true);
    
    // Get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();
    
    // Open an output file
    //
    G4String fileName = "B4";
    analysisManager->OpenFile(fileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4RunAction::EndOfRunAction(const G4Run* /*run*/)
{
    // print histogram statistics
    //
    auto analysisManager = G4AnalysisManager::Instance();
    if ( analysisManager->GetH1(0) ) {
        G4cout << G4endl << " ----> print histograms statistic ";
        if(isMaster) {
            G4cout << "for the entire run " << G4endl << G4endl;
        }
        else {
            G4cout << "for the local thread " << G4endl << G4endl;
        }

        
        G4cout << " ENeutrons : mean = "
        << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy")
        << " rms = "
        << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;

        G4cout << " Xpos : mean = "
        << G4BestUnit(analysisManager->GetH2(0)->mean_x(), "Length")
        << " rms = "
        << G4BestUnit(analysisManager->GetH2(0)->rms_x(),  "Length") << G4endl;
        
        G4cout << " Ypos : mean = "
        << G4BestUnit(analysisManager->GetH2(1)->mean_y(), "Length")
        << " rms = "
        << G4BestUnit(analysisManager->GetH2(1)->rms_y(),  "Length") << G4endl;
    }
    
    // save histograms & ntuple
    //
    analysisManager->Write();
    analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
