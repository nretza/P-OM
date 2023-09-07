// system includes
#include <time.h>

// G4 includes
#include "G4ios.hh"

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

void OMRunAction::BeginOfRunAction(const G4Run* run)
{
    G4cout << "==========================" << G4endl;
    G4cout << ">> starting run " << run->GetRunID() << G4endl;
    OMDataManager::getInstance()->open();

    this->startClock = clock();
}

void OMRunAction::EndOfRunAction(const G4Run* run)
{
    this->endClock = clock();
    double elapsed = double(endClock - startClock)/ (double) CLOCKS_PER_SEC;

    OMDataManager::getInstance()->close();
    G4cout << ">> run " << run->GetRunID() << " finished in " << elapsed << " seconds." << G4endl;
    G4cout << "==========================" << G4endl;
}