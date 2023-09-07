#ifndef OM_RUNACTION_H
#define OM_RUNACTION_H 1

// system includes

// G4 Includes
#include "G4UserRunAction.hh"
#include "G4Run.hh"

//ROOT includes

// Project includes

class OMRunAction : public G4UserRunAction
{
    public:

        OMRunAction();
        ~OMRunAction();

        void BeginOfRunAction(const G4Run* run);
        void EndOfRunAction(const G4Run* run);

        clock_t startClock;
        clock_t endClock;

};
#endif