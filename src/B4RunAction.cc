#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include "G4AccumulableManager.hh"

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
    analysisManager->CreateH1("ENeutron","Neutron Energy in Detector",200, 0.5e-04*eV, 14.1*MeV,"eV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(0, "Energy [eV]");
    analysisManager->SetH1YAxisTitle(0, "Counts");

    analysisManager->CreateH1("TENeutron","Transmitted neutron Energy ",200, 0.5e-04*eV, 14.1*MeV,"eV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(1, "Energy [eV]");
    analysisManager->SetH1YAxisTitle(1, "Counts");

    analysisManager->CreateH1("EBoundary","Energy on boundary of solid ",200, 0.5e-04*eV,14.1*MeV,"eV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(2, "Energy [eV]");
    analysisManager->SetH1YAxisTitle(2, "Counts");

    analysisManager->CreateH1("ZBoundary","Z coordinate on boundary of solid ",200, -5.*cm, 5.*cm,"cm"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(3, "z [cm]");
    analysisManager->SetH1YAxisTitle(3, "Counts");

    analysisManager->CreateH1("EGamma","Energy of gamma from foil ",200, 100.*eV, 10*MeV,"MeV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(4, "Energy [MeV]");
    analysisManager->SetH1YAxisTitle(4, "Counts");

    analysisManager->CreateH1("ENside","Energy of neutrons that come out of one side ",200, 0.5e-04*eV, 14.1*MeV,"eV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(5, "Energy [eV]");
    analysisManager->SetH1YAxisTitle(5, "Counts");

    analysisManager->CreateH1("E0","Initial Neutron Energy",200, 0.5e-04*eV, 14.1*MeV,"eV","none","log"); // third entry is type of binning (log binning)//nb min can't be 0 in log scale//nb da anche il max dell'istogramma
    analysisManager->SetH1XAxisTitle(6, "Energy [eV]");
    analysisManager->SetH1YAxisTitle(6, "Counts");


    //2D istograms
    analysisManager->CreateH2("DetPos","Position in detector", 200, -10.*cm, 10.*cm, 200, -10.*cm, 10.*cm, "cm", "cm");
    analysisManager->SetH2XAxisTitle(0, "x [cm]");
    analysisManager->SetH2YAxisTitle(0, "y [cm]");

    analysisManager->CreateH2("TPos","Position in transmission detector", 200, -5*cm, 5*cm, 200, -5*cm, 5*cm, "cm", "cm");
    analysisManager->SetH2XAxisTitle(1, "x [cm]");
    analysisManager->SetH2YAxisTitle(1, "y [cm]");

    analysisManager->CreateH2("GenPos","Generator Position", 200, -5*cm, 5*cm, 200, -5*cm, 5*cm, "cm", "cm");
    analysisManager->SetH2XAxisTitle(2, "x [cm]");
    analysisManager->SetH2YAxisTitle(2, "y [cm]");

    analysisManager->CreateH2("SidePos","position on side of detector", 200, -5*cm, 5*cm, 200, -5.4*cm, 5.4*cm, "cm", "cm");
    analysisManager->SetH2XAxisTitle(3, "x [cm]");
    analysisManager->SetH2YAxisTitle(3, "y [cm]");

    // Creating ntuple
    //
    analysisManager->CreateNtuple("B4", "Data Tree");
    analysisManager->CreateNtupleDColumn("BoundaryEnergy");
    analysisManager->CreateNtupleDColumn("BoundaryX");
    analysisManager->CreateNtupleDColumn("BoundaryY");
    analysisManager->CreateNtupleDColumn("BoundaryZ");


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

void B4RunAction::EndOfRunAction(const G4Run* run)
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

        G4int nofEvents = run->GetNumberOfEvent();
        G4cout << nofEvents << " events were processed." << G4endl;

        G4cout << " Number of neutrons in the whole detector= "
        << analysisManager->GetH1(0)->entries() << G4endl;
        G4cout << " their average energy is = "
        << analysisManager->GetH1(0)->mean() << " eV "
        << ", their root mean square is= "
        << analysisManager->GetH1(0)->rms()<< " eV" << G4endl;

        G4cout << " " << G4endl;
        G4cout << " Number of neutrons in transmission detector= "
               << analysisManager->GetH1(1)->entries() << G4endl;
        G4cout << " their average energy is = "
               << analysisManager->GetH1(1)->mean() << " eV "
               << ", their root mean square is= "
               << analysisManager->GetH1(1)->rms() << " eV" << G4endl;

        G4cout << " " << G4endl;
        G4cout << " Number of neutrons that come out of solid= "
               << analysisManager->GetH1(2)->entries() << G4endl;
        G4cout << " their average energy is = "
               << analysisManager->GetH1(2)->mean() << " eV "
               << ", their root mean square is= "
               << analysisManager->GetH1(2)->rms() << " eV" << G4endl;

        G4cout << " " << G4endl;
        G4cout << " Number of neutrons that come out of side= "
               << analysisManager->GetH1(5)->entries() << G4endl;
        G4cout << " their average energy is = "
               << analysisManager->GetH1(5)->mean() << " eV "
               << ", their root mean square is= "
               << analysisManager->GetH1(5)->rms() << " eV" << G4endl;

        G4cout << " " << G4endl;
        G4cout << " Number of gamma rays produced in the foil is = "
               << analysisManager->GetH1(4)->entries() << G4endl;
        G4cout << " their average energy is = "
               << G4BestUnit(analysisManager->GetH1(4)->mean(), "Energy")
               << ", their root mean square is= "
               << G4BestUnit(analysisManager->GetH1(4)->rms(),  "Energy") << G4endl;
        G4cout << " " << G4endl;
    }
    
    // save histograms & ntuple
    //
    analysisManager->Write();
    analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
