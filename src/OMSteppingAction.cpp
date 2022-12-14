// system includes

// G4 includes
#include "G4SystemOfUnits.hh"
// project includes
#include "OMSteppingAction.hh"
#include "OMDataManager.hh"
#include "G4VProcess.hh"

OMSteppingAction::OMSteppingAction()
{
    // TODO
}

OMSteppingAction::~OMSteppingAction()
{
    // TODO
}

void OMSteppingAction::UserSteppingAction(const G4Step* step)
{
    // get position of first step point, usually where the sphere is first hit
    if (step->GetTrack()->GetCurrentStepNumber() == 1)
    {
        OMDataManager::getInstance()->firstStepHandover(step->GetPostStepPoint()->GetPosition());
    }
}

