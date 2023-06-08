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
    // sanity check to avoid segmentation error
    if (step->GetPostStepPoint()->GetPhysicalVolume() == nullptr) return;

    // if post step point is glass, hand over position and momentum to datamanager
    // post step point - position on glass
    // pre  step point - momentum direction before refraction
    if (step->GetPostStepPoint()->GetPhysicalVolume()->GetName().find("GlasHemisphere") != std::string::npos)
    {
        OMDataManager::getInstance()->glassContactHandover(step->GetPostStepPoint()->GetPosition() / mm,
                                                           step->GetPreStepPoint()->GetMomentumDirection());
    }
}

