#ifndef OM_PRIMARY_GENERATOR_H
#define OM_PRIMARY_GENERATOR_H 1

// system includes

// G4 includes
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

// project includes

class G4Event;
class OMPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:

        OMPrimaryGenerator();
        ~OMPrimaryGenerator();
        
        virtual void GeneratePrimaries( G4Event* event );

    private:

        G4GeneralParticleSource *_generalParticleSource;
};

#endif