#ifndef OM_TRACKINGACTION_H
#define OM_TRACKINGACTION_H 1

// system includes

// G4 Includes
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"

//ROOT includes

// Project includes

class OMTrackingAction : public G4UserTrackingAction
{
    public:

        OMTrackingAction();
        ~OMTrackingAction();

        void PreUserTrackingAction(const G4Track*);
        void PostUserTrackingAction(const G4Track*);

};
#endif