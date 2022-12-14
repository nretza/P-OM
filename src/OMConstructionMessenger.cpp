// system includes

// G4 includes
#include "G4Tokenizer.hh"
#include "G4SystemOfUnits.hh"

// project includes
#include "OMConstructionMessenger.hh"


OMConstructionMessenger::OMConstructionMessenger(OMConstruction* Constr)
: G4UImessenger(),
 _Construction(Constr)
{
    this->GDMLDir = new G4UIdirectory("/geometry/gdml/");
    this->GDMLDir->SetGuidance("options for the imported GDML file");

    this->OpticalUnitDir = new G4UIdirectory("/geometry/optical_unit/");
    this->OpticalUnitDir->SetGuidance("options for the placement of optical units (pmt + gelpad)");

    this->gdmlfileCmd = new G4UIcmdWithAString("/geometry/gdml/file",this);
    this->gdmlfileCmd->SetGuidance("set the GDML-File to be imported.");
    this->gdmlfileCmd->SetParameterName("filename",false);
    this->gdmlfileCmd->AvailableForStates(G4State_PreInit);
    this->gdmlfileCmd->SetToBeBroadcasted(false);

    this->OUOrgCmd = new G4UIcmdWith3Vector("/geometry/optical_unit/setOrigin", this);
    this->OUOrgCmd->SetGuidance("Sets the optical unit coord origin to selected value");
    this->OUOrgCmd->AvailableForStates(G4State_PreInit);
    this->OUOrgCmd->SetToBeBroadcasted(false);

    this->OURefXCmd = new G4UIcmdWith3Vector("/geometry/optical_unit/setRefX", this);
    this->OURefXCmd->SetGuidance("sets the optical unit coord refX to the selected value");
    this->OURefXCmd->AvailableForStates(G4State_PreInit);
    this->OURefXCmd->SetToBeBroadcasted(false);

    this->OURefYCmd = new G4UIcmdWith3Vector("/geometry/optical_unit/setRefY", this);
    this->OURefYCmd->SetGuidance("sets the optical unit coord refY to the selected value");
    this->OURefYCmd->AvailableForStates(G4State_PreInit);
    this->OURefYCmd->SetToBeBroadcasted(false);

    this->OUPlaceCmd = new G4UIcommand("/geometry/optical_unit/place", this);
    this->OUPlaceCmd->SetGuidance("places a optical unit on to the specified coordinates");
    this->OUPlaceCmd->AvailableForStates(G4State_PreInit);
    this->OUPlaceCmd->SetToBeBroadcasted(false);

    G4UIparameter* radius_parameter = new G4UIparameter("radius [mm]", 'd', false);
    G4UIparameter* theta_parameter  = new G4UIparameter("theta [degree]", 'd', false);
    G4UIparameter* phi_parameter    = new G4UIparameter("phi [degree]", 'd', false);
    // radius_parameter->SetParameterRange("radius > 0");
    // theta_parameter->SetParameterRange("theta >= - 90 && theta =< 90");
    // phi_parameter->SetParameterRange("phi >= 0 && phi <= 360");
    this->OUPlaceCmd->SetParameter(radius_parameter);
    this->OUPlaceCmd->SetParameter(theta_parameter);
    this->OUPlaceCmd->SetParameter(phi_parameter);
}

OMConstructionMessenger::~OMConstructionMessenger()
{
    delete this->GDMLDir;
    delete this->OpticalUnitDir;
    delete this->gdmlfileCmd;
    delete this->OUOrgCmd;
    delete this->OURefXCmd;
    delete this->OURefYCmd;
    delete this->OUPlaceCmd;
}

void OMConstructionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    // Set gdml filename
    if( command == this->gdmlfileCmd )
    {
        this->_Construction->setGDMLFilename(newValue);
    }

    // Set OU origin
    if( command == this->OUOrgCmd )
    {
        this->_Construction->setOUCoordCenter(this->OUOrgCmd->GetNew3VectorValue(newValue) * mm);
    }

    // Set OU coord refX
    if( command == this->OURefXCmd )
    {
        this->_Construction->setOUCoordRefX(this->OURefXCmd->GetNew3VectorValue(newValue) * mm);
    }

    // Set OU coord refY
    if( command == this->OURefYCmd )
    {
        this->_Construction->setOUCoordRefY(this->OURefYCmd->GetNew3VectorValue(newValue) * mm);
    }

    // place OU
    if( command == this->OUPlaceCmd )
    {
        G4Tokenizer next(newValue);
        G4double radius = StoD(next()) * mm;
        G4double theta = StoD(next()) * degree;
        G4double phi = StoD(next()) * degree;
        this->_Construction->addOpticalUnit(radius, theta, phi);
    }
}