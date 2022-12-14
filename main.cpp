// defines

// system includes
#include <iostream>

// G4 includes
#include "G4RunManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"

// project includes
#include "OMConstruction.hh"
#include "OMPrimaryGenerator.hh"
#include "OMSteppingAction.hh"
#include "OMTrackingAction.hh"
#include "OMRunAction.hh"
#include "OMDataManager.hh"


int main(int argc,char** argv)
{
    G4RunManager* runManager = new G4RunManager;

    // set up optical physics
    G4VModularPhysicsList* OMphysicsList = new FTFP_BERT;
    G4OpticalPhysics* OMopticalPhysics   = new G4OpticalPhysics();
    OMphysicsList->RegisterPhysics(OMopticalPhysics);

    // run manager initialisation
    runManager->SetUserInitialization(OMphysicsList);
    runManager->SetUserInitialization(new OMConstruction());
    runManager->SetUserAction        (new OMPrimaryGenerator());
    runManager->SetUserAction        (new OMRunAction());
    runManager->SetUserAction        (new OMTrackingAction());
    runManager->SetUserAction        (new OMSteppingAction());
    //runManager->Initialize(); // done in macro

    // call singletons where necessary
    OMDataManager::getInstance();

    // set random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::MTwistEngine);
    CLHEP::HepRandom::setTheSeed((unsigned)clock());

    // Initialize visualization
    if ( argc == 1 || argv[1] == std::string("--vis") || argv[1] == std::string("--interactive") )
    {

        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        G4VisExecutive* visManager = new G4VisExecutive();
        visManager->SetVerboseLevel(0);
        visManager->Initialize();

        G4UImanager* UImanager = G4UImanager::GetUIpointer();
        UImanager->ApplyCommand("/control/execute macros/init.mac");
        ui->SessionStart();
        
        delete ui;
        delete visManager;
    }
    else if ( argv[1] == std::string("--batch") )
    {
        G4UImanager* UImanager = G4UImanager::GetUIpointer();
        UImanager->ApplyCommand("/control/execute macros/init.mac");
    }
    else
    {
        G4Exception("int main()",
                    "invalid command arguments",
                    FatalErrorInArgument,
                    "Can't make sense of command line arguments.\nTry '--batch' for batch mode or '--vis' for interactive mode!");
    }

    delete runManager;
    return 0;
}