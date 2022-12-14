// system includes

// G4 includes

// project includes
#include "OMRunAction.hh"
#include "OMDataManager.hh"

OMRunAction::OMRunAction()
{
    // TODO
}

OMRunAction::~OMRunAction()
{
    // TODO
}

void OMRunAction::BeginOfRunAction(const G4Run*)
{
    OMDataManager::getInstance()->open();
}

void OMRunAction::EndOfRunAction(const G4Run*)
{
    OMDataManager::getInstance()->close();
}