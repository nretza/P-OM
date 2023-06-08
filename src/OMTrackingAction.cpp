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
                                                   track->GetGlobalTime(),
                                                   track->GetPosition() / mm,
                                                   track->GetTotalEnergy() / eV,
                                                   track->GetMomentumDirection());
}

void OMTrackingAction::PostUserTrackingAction(const G4Track* track)
{

    G4int copy_nr_depth = 0;
    G4String volume_name = track->GetTouchableHandle()->GetVolume()->GetName();

    // handle copy nr in PMT correctly -> will always get copy nr of pmt, not of parts inside
    if (volume_name.find("photocathode") != std::string::npos) copy_nr_depth = 2;
    else if (volume_name.find("Absorber") != std::string::npos) copy_nr_depth = 1;
    else if (volume_name.find("VacZylinder") != std::string::npos) copy_nr_depth = 1;
    else if (volume_name.find("upperVacSphere") != std::string::npos) copy_nr_depth = 1;
    else if (volume_name.find("lowerVacSphere") != std::string::npos) copy_nr_depth = 1;

    
    OMDataManager::getInstance()->postTrackHandover(track->GetGlobalTime(),
                                                    track->GetPosition() / mm,
                                                    track->GetTotalEnergy() / eV,
                                                    track->GetMomentumDirection(),
                                                    volume_name,
                                                    track->GetTouchableHandle()->GetCopyNumber(copy_nr_depth),
                                                    track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName());
    OMDataManager::getInstance()->write();
    OMDataManager::getInstance()->reset();
}
