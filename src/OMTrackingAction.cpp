// system includes

// G4 includes
#include "G4SystemOfUnits.hh"
// project includes
#include "OMTrackingAction.hh"
#include "OMDataManager.hh"
#include "G4VProcess.hh"

OMTrackingAction::OMTrackingAction()
{
    // TODO
}

OMTrackingAction::~OMTrackingAction()
{
    // TODO
}

void OMTrackingAction::PreUserTrackingAction(const G4Track* track)
{
    OMDataManager::getInstance()->preTrackHandover(track->GetParticleDefinition()->GetPDGEncoding(),
                                                   track->GetTotalEnergy() / eV,
                                                   track->GetPosition() / mm,
                                                   track->GetMomentumDirection());
}

void OMTrackingAction::PostUserTrackingAction(const G4Track* track)
{
    OMDataManager::getInstance()->postTrackHandover(track->GetPosition() / mm,
                                                    track->GetVolume()->GetName(),
                                                    track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());
    OMDataManager::getInstance()->write();
}

