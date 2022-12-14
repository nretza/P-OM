// system includes

// G4 includes
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalPhoton.hh"
#include "G4RandomDirection.hh"

// project includes
#include "OMPrimaryGenerator.hh"


OMPrimaryGenerator::OMPrimaryGenerator()
: G4VUserPrimaryGeneratorAction(),
 _generalParticleSource(nullptr)
{
    /*
        NOTE:
        The Settings in this code are just defaults, the actual configuration is set in init_primary.mac 
    */

    G4double light_radius    = 5 * m;   // radius of the sphere the light emerges from
    G4double detector_radius = 0.30 * m;  // radius of the sphere the light is targeted at
    G4double opening_angle   = atan(detector_radius / light_radius);

    // create source
    this->_generalParticleSource = new G4GeneralParticleSource();
    G4SingleParticleSource* currentSource = this->_generalParticleSource->GetCurrentSource();

    // particle definition
    currentSource->SetParticleDefinition(G4OpticalPhoton::Definition());

    // light initial position
    currentSource->GetPosDist()->SetPosDisType("Surface");
    currentSource->GetPosDist()->SetPosDisShape("Sphere");
    currentSource->GetPosDist()->SetRadius(light_radius);
    currentSource->GetPosDist()->SetCentreCoords(G4ThreeVector(0,0,0));

    // light momentum vector
    currentSource->GetAngDist()->SetAngDistType("iso");
    currentSource->GetAngDist()->SetMinPhi(0 * degree);
    currentSource->GetAngDist()->SetMaxPhi(360 * degree);
    currentSource->GetAngDist()->SetMinTheta(0 * degree);
    currentSource->GetAngDist()->SetMaxTheta(opening_angle);

    // light energy
    currentSource->GetEneDist()->SetEnergyDisType("Mono");
    currentSource->GetEneDist()->SetMonoEnergy(3.0 *eV);

}

OMPrimaryGenerator::~OMPrimaryGenerator()
{
    delete this->_generalParticleSource;
}

void OMPrimaryGenerator::GeneratePrimaries(G4Event* event)
{
    this->_generalParticleSource->SetParticlePolarization(G4RandomDirection());
    this->_generalParticleSource->GeneratePrimaryVertex(event);
}