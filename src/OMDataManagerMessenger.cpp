// system includes

// G4 includes

// project includes
#include "OMDataManagerMessenger.hh"


OMDataManagerMessenger::OMDataManagerMessenger(OMDataManager* Constr)
: G4UImessenger(),
 _DataManager(Constr)
{
    this->_dataDir = new G4UIdirectory("/data_acquisition/");
    this->_dataDir->SetGuidance("options for data acquisition");


    this->_outputFileCmd = new G4UIcmdWithAString("/data_acquisition/output_file",this);
    this->_outputFileCmd->SetGuidance("set the output file to write into.");
    this->_outputFileCmd->SetParameterName("filename",false);
    this->_outputFileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    this->_outputFileCmd->SetToBeBroadcasted(false);

}

OMDataManagerMessenger::~OMDataManagerMessenger()
{
}

void OMDataManagerMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    // Set output filename
    if( command == this->_outputFileCmd)
    {
        this->_DataManager->setFilename(newValue);
    }
}