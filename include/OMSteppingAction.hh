#ifndef OM_STEPPINGACTION_H
#define OM_STEPPINGACTION_H 1

// system includes

// G4 Includes
#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

//ROOT includes

// Project includes

class OMSteppingAction : public G4UserSteppingAction
{
    public:

        OMSteppingAction();
        ~OMSteppingAction();

        void UserSteppingAction(const G4Step*);

};
#endif