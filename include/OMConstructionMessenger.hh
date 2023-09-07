#ifndef OM_CONSTRUCTION_MESSENGER_H
#define OM_CONSTRUCTION_MESSENGER_H 1

// system includes

// G4 includes
#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

// project includes
#include "OMConstruction.hh"

// forward declarations
class OMConstruction;

class OMConstructionMessenger: public G4UImessenger
{
    public:

        // constructors
        OMConstructionMessenger(OMConstruction*);
        ~OMConstructionMessenger();

        // member functions
        virtual void SetNewValue(G4UIcommand*, G4String);

    private:

        // construction instance
        OMConstruction* _Construction;

        // menu dirs
        G4UIdirectory* GDMLDir;
        G4UIdirectory* OpticalUnitDir;

        // commands
        G4UIcmdWithABool*     submergeCmd;
        G4UIcmdWithABool*     solidReflectorCmd;
        G4UIcmdWithAString*   gdmlfileCmd;

        G4UIcmdWith3Vector*   OUOrgCmd;
        G4UIcmdWith3Vector*   OURefXCmd;
        G4UIcmdWith3Vector*   OURefYCmd;
        G4UIcommand*          OUPlaceCmd;

        G4UIcmdWithADoubleAndUnit*   GelRingCmd;
        G4UIcmdWithADoubleAndUnit*   PhotocathodeTubeCmd;


};

#endif