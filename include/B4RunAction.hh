#ifndef B4RunAction_h
#define B4RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <G4Accumulable.hh>

class G4Run;

/// Run action class
///
/// It accumulates statistic and computes dispersion of the energy deposit 
/// and track lengths of charged particles with use of analysis tools:
/// H1D histograms are created in BeginOfRunAction() for the following 
/// physics quantities:
/// - Edep in absorber
/// - Edep in gap
/// - Track length in absorber
/// - Track length in gap
/// The same values are also saved in the ntuple.
/// The histograms and ntuple are saved in the output file in a format
/// accoring to a selected technology in B4Analysis.hh.
///
/// In EndOfRunAction(), the accumulated statistic and computed 
/// dispersion is printed.
///

class B4RunAction : public G4UserRunAction
{
  public:
    B4RunAction();
    virtual ~B4RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

