#ifndef OM_DATAMANAGER_MESSENGER_H
#define OM_DATAMANAGER_MESSENGER_H 1

// system includes

// G4 includes
#include "G4UImessenger.hh"
#include "G4UIcmdWithAString.hh"

// project includes
#include "OMDataManager.hh"

// forward declarations
class OMDataManager;

class OMDataManagerMessenger: public G4UImessenger
{
    public:

        // constructors
        OMDataManagerMessenger(OMDataManager*);
        ~OMDataManagerMessenger();

        // member functions
        virtual void SetNewValue(G4UIcommand*, G4String);

    private:

        // DataManager instance
        OMDataManager* _DataManager;

        // menu dirs
        G4UIdirectory* _dataDir;

        // commands
        G4UIcmdWithAString*   _outputFileCmd;
};

#endif