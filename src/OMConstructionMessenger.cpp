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

    this->OpticalUnitDir = new G4UIdirectory("/geometry/PMT/");
    this->OpticalUnitDir->SetGuidance("options for the placement of optical units (pmt + gelpad)");

    this->gdmlfileCmd = new G4UIcmdWithAString("/geometry/gdml/file",this);
    this->gdmlfileCmd->SetGuidance("set the GDML-File to be imported.");
    this->gdmlfileCmd->SetParameterName("filename",false);
    this->gdmlfileCmd->AvailableForStates(G4State_PreInit);
    this->gdmlfileCmd->SetToBeBroadcasted(false);

    this->submergeCmd = new G4UIcmdWithABool("/geometry/gdml/submerge",this);
    this->submergeCmd->SetGuidance("places water around the imported detector geometry. Air inside the detector will remain.");
    this->submergeCmd->SetParameterName("yes/no",false);
    this->submergeCmd->AvailableForStates(G4State_PreInit);
    this->submergeCmd->SetToBeBroadcasted(false);

    this->solidReflectorCmd = new G4UIcmdWithABool("/geometry/gdml/solidReflector",this);
    this->solidReflectorCmd->SetGuidance("Places a solid reflector around the Gelpad edge. Same properties as the PMT reflector.");
    this->solidReflectorCmd->SetParameterName("yes/no",false);
    this->solidReflectorCmd->AvailableForStates(G4State_PreInit);
    this->solidReflectorCmd->SetToBeBroadcasted(false);

    this->OUOrgCmd = new G4UIcmdWith3Vector("/geometry/PMT/setOrigin", this);
    this->OUOrgCmd->SetGuidance("Sets the optical unit coord origin to selected value");
    this->OUOrgCmd->AvailableForStates(G4State_PreInit);
    this->OUOrgCmd->SetToBeBroadcasted(false);

    this->OURefXCmd = new G4UIcmdWith3Vector("/geometry/PMT/setRefX", this);
    this->OURefXCmd->SetGuidance("sets the optical unit coord refX to the selected value");
    this->OURefXCmd->AvailableForStates(G4State_PreInit);
    this->OURefXCmd->SetToBeBroadcasted(false);

    this->OURefYCmd = new G4UIcmdWith3Vector("/geometry/PMT/setRefY", this);
    this->OURefYCmd->SetGuidance("sets the optical unit coord refY to the selected value");
    this->OURefYCmd->AvailableForStates(G4State_PreInit);
    this->OURefYCmd->SetToBeBroadcasted(false);

    this->OUPlaceCmd = new G4UIcommand("/geometry/PMT/place", this);
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

    this->GelRingCmd = new G4UIcmdWithADoubleAndUnit("/geometry/PMT/GelRingOffset", this);
    this->GelRingCmd->SetGuidance("sets the offset of an interface gel ring around the gelpad as it occurs during the curing in the hemisphere");
    this->GelRingCmd->AvailableForStates(G4State_PreInit);
    this->GelRingCmd->SetDefaultValue(0);
    this->GelRingCmd->SetToBeBroadcasted(false);

    this->PhotocathodeTubeCmd = new G4UIcmdWithADoubleAndUnit("/geometry/PMT/PCTubeSize", this);
    this->PhotocathodeTubeCmd->SetGuidance("sets how much of the tube part of the inner vacuum of the PMT should be covered with the photocathode");
    this->PhotocathodeTubeCmd->AvailableForStates(G4State_PreInit);
    this->PhotocathodeTubeCmd->SetDefaultValue(0);
    this->PhotocathodeTubeCmd->SetToBeBroadcasted(false);
}

OMConstructionMessenger::~OMConstructionMessenger()
{
    delete this->GDMLDir;
    delete this->OpticalUnitDir;
    delete this->submergeCmd;
    delete this->solidReflectorCmd;
    delete this->gdmlfileCmd;
    delete this->OUOrgCmd;
    delete this->OURefXCmd;
    delete this->OURefYCmd;
    delete this->OUPlaceCmd;
    delete this->GelRingCmd;
    delete this->PhotocathodeTubeCmd;
}

void OMConstructionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
    // Set gdml filename
    if( command == this->gdmlfileCmd )
    {
        this->_Construction->setGDMLFilename(newValue);
    }

    // set submerge
    if ( command == this->submergeCmd )
    {
        this->_Construction->setSubmerge(this->submergeCmd->GetNewBoolValue(newValue));
    }

    // set solid reflector
    if ( command == this->solidReflectorCmd )
    {
        this->_Construction->setSolidReflector(this->solidReflectorCmd->GetNewBoolValue(newValue));
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

    // Set Gelpad Ring
    if( command == this->GelRingCmd )
    {
        this->_Construction->setGelpadRingOffset(this->GelRingCmd->GetNewDoubleValue(newValue));
    }

    // Set Photocathode Tube
    if( command == this->PhotocathodeTubeCmd )
    {
        this->_Construction->setPhotocathodeTubeLength(this->PhotocathodeTubeCmd->GetNewDoubleValue(newValue));
    }

}