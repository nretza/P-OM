// system includes

// G4 includes

// project includes
#include "OMDataManagerMessenger.hh"


OMDataManagerMessenger::OMDataManagerMessenger(OMDataManager* Constr)
: G4UImessenger(),
 _DataManager(Constr)
{
    this->_dataDir = new G4UIdirectory("/daq/");
    this->_dataDir->SetGuidance("options for data acquisition");


    this->_outputFileCmd = new G4UIcmdWithAString("/daq/output_file",this);
    this->_outputFileCmd->SetGuidance("Set the output file to write into.");
    this->_outputFileCmd->SetParameterName("filename",false);
    this->_outputFileCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    this->_outputFileCmd->SetToBeBroadcasted(false);

    this->_dataFilterOutProcessCmd = new G4UIcmdWithAString("/daq/outProcess_filter",this);
    this->_dataFilterOutProcessCmd->SetGuidance("Filters data if the outProcess is part of the given string.");
    this->_dataFilterOutProcessCmd->SetParameterName("processes",false);
    this->_dataFilterOutProcessCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    this->_dataFilterOutProcessCmd->SetToBeBroadcasted(false);

    this->_dataFilterOutVolumeCmd = new G4UIcmdWithAString("/daq/outVolume_filter",this);
    this->_dataFilterOutVolumeCmd->SetGuidance("Filters data if the outVolume is part of the given string.");
    this->_dataFilterOutVolumeCmd->SetParameterName("volumes",false);
    this->_dataFilterOutVolumeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    this->_dataFilterOutVolumeCmd->SetToBeBroadcasted(false);
        
    this->_dataFilterGlassCmd = new G4UIcmdWithABool("/daq/glass_filter",this);
    this->_dataFilterGlassCmd->SetGuidance("Determines if tracks that did not touch the glass should be filtered out.");
    this->_dataFilterGlassCmd->SetParameterName("yes/no",false);
    this->_dataFilterGlassCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    this->_dataFilterGlassCmd->SetToBeBroadcasted(false);

    this->_dataFilterPhotonsCmd = new G4UIcmdWithABool("/daq/photons_filter",this);
    this->_dataFilterPhotonsCmd->SetGuidance("Determines if tracks that are not photons should be filtered out.");
    this->_dataFilterPhotonsCmd->SetParameterName("yes/no",false);
    this->_dataFilterPhotonsCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    this->_dataFilterPhotonsCmd->SetToBeBroadcasted(false);
}

OMDataManagerMessenger::~OMDataManagerMessenger()
{
    delete this->_outputFileCmd;
    delete this->_dataFilterOutProcessCmd;
    delete this->_dataFilterOutVolumeCmd;
    delete this->_dataFilterGlassCmd;
    delete this->_dataFilterPhotonsCmd;
}

void OMDataManagerMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    // Set output filename
    if( command == this->_outputFileCmd)
    {
        this->_DataManager->setFilename(newValue);
    }

    // Set outProcess filter
    if( command == this->_dataFilterOutProcessCmd)
    {
        this->_DataManager->setDataOutProcessFilter(newValue);
    }

    // Set outVolume filter
    if( command == this->_dataFilterOutVolumeCmd)
    {
        this->_DataManager->setDataOutVolumeFilter(newValue);
    }

    // Set glass filter
    if ( command == this->_dataFilterGlassCmd )
    {
        this->_DataManager->setDataGlassFilter(this->_dataFilterGlassCmd->GetNewBoolValue(newValue));
    }

    // Set photon filter
    if ( command == this->_dataFilterPhotonsCmd )
    {
        this->_DataManager->setPhotonsFilter(this->_dataFilterPhotonsCmd->GetNewBoolValue(newValue));
    }
}