// system includes

// G4 includes
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4NistManager.hh"
#include "G4OpticalSurface.hh"
#include "G4SystemOfUnits.hh"
#include "G4Types.hh"

// project includes
#include "OMMaterialManager.hh"

OMMaterialManager* OMMaterialManager::_instance = nullptr;

OMMaterialManager* OMMaterialManager::getInstance()
{
    if( _instance == nullptr )
    {
        _instance = new OMMaterialManager();
    }
    return _instance;
}

OMMaterialManager::OMMaterialManager():
_vacuum(nullptr),
_air(nullptr),
_water(nullptr),
_glass(nullptr),
_gel(nullptr),
_titanium(nullptr),
_plastic(nullptr),
_photocathode(nullptr),
_waterSurface(nullptr),
_airSurface(nullptr),
_glassSurface(nullptr),
_gelSurface(nullptr),
_titaniumSurface(nullptr),
_plasticSurface(nullptr),
_reflectorSurface(nullptr)
{
    this->_datareader = new OMDataReader("macros/optical_properties.cfg");
}

OMMaterialManager::~OMMaterialManager()
{
    delete this->_datareader;
}

G4Material* OMMaterialManager::BuildVacuum()
{
    if (this->_vacuum != nullptr) return this->_vacuum;

    G4NistManager* nist = G4NistManager::Instance();
    G4Material* vacuum = nist->FindOrBuildMaterial("G4_Galactic");
    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();
    
    unsigned length      = this->_datareader->GetArrayLength("vacuumEnergy");
    G4double* energy     = this->_datareader->GetArray("vacuumEnergy",length,eV);
    G4double* refraction = this->_datareader->GetArray("vacuumRefraction",length);
    G4double* absorption = this->_datareader->GetArray("vacuumAbsorption",length, km);

    properties->AddProperty("RINDEX", energy, refraction, length);
    properties->AddProperty("ABSLENGTH", energy, absorption, length);

    vacuum->SetMaterialPropertiesTable(properties);

    this->_vacuum = vacuum;
    return vacuum;
}

G4Material* OMMaterialManager::BuildAir()
{
    if (this->_air != nullptr) return this->_air;

    G4NistManager* nist = G4NistManager::Instance();
    G4Material* air = nist->FindOrBuildMaterial("G4_AIR");
    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();
    
    unsigned length      = this->_datareader->GetArrayLength("airEnergy");
    G4double* energy     = this->_datareader->GetArray("airEnergy",length,eV);
    G4double* refraction = this->_datareader->GetArray("airRefraction",length);
    G4double* absorption = this->_datareader->GetArray("airAbsorption",length, km);

    properties->AddProperty("RINDEX", energy, refraction, length);
    properties->AddProperty("ABSLENGTH", energy, absorption, length);

    air->SetMaterialPropertiesTable(properties);

    this->_air = air;
    return air;
}

G4Material* OMMaterialManager::BuildWater()
{
    if (this->_water != nullptr) return this->_water;

    G4NistManager* nist = G4NistManager::Instance();
    G4Material* water = nist->FindOrBuildMaterial("G4_WATER");
    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned length      = this->_datareader->GetArrayLength("waterEnergy");
    G4double* energy     = this->_datareader->GetArray("waterEnergy",length,eV);
    G4double* refraction = this->_datareader->GetArray("waterRefraction",length);
    G4double* absorption = this->_datareader->GetArray("waterAbsorption",length, m);

    properties->AddProperty("RINDEX", energy, refraction, length);
    properties->AddProperty("ABSLENGTH", energy, absorption, length);

    water->SetMaterialPropertiesTable(properties);

    this->_water = water;
    return water;
}

G4Material* OMMaterialManager::BuildGel()
{
    if (this->_gel != nullptr) return this->_gel;

    G4Material* OpticalGel = new G4Material("OpticalGel", 12, 12*g, 2.51*g/cm3, kStateSolid);

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned length      = this->_datareader->GetArrayLength("opticalGelEnergy");
    G4double* energy     = this->_datareader->GetArray("opticalGelEnergy",length,eV);
    G4double* refraction = this->_datareader->GetArray("opticalGelRefraction",length);
    G4double* absorption = this->_datareader->GetArray("opticalGelAbsorption",length, mm);

    properties->AddProperty("RINDEX", energy, refraction, length);
    properties->AddProperty("ABSLENGTH", energy, absorption, length);

    OpticalGel->SetMaterialPropertiesTable(properties);

    this->_gel = OpticalGel;
    return OpticalGel;
}

G4Material* OMMaterialManager::BuildGlass()
{
    if (this->_glass != nullptr) return this->_glass;

    G4NistManager* nist = G4NistManager::Instance();
    G4Material* glass = nist->FindOrBuildMaterial("G4_Pyrex_Glass");

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned length      = this->_datareader->GetArrayLength("glassEnergy");
    G4double* energy     = this->_datareader->GetArray("glassEnergy",length,eV);
    G4double* refraction = this->_datareader->GetArray("glassRefraction",length);
    G4double* absorption = this->_datareader->GetArray("glassAbsorption",length, mm);

    properties->AddProperty("RINDEX", energy, refraction, length);
    properties->AddProperty("ABSLENGTH", energy, absorption, length);

    glass->SetMaterialPropertiesTable(properties);

    this->_glass = glass;
    return glass;
}

G4Material* OMMaterialManager::BuildTitanium()
{
    if (this->_titanium != nullptr) return this->_titanium;

    G4NistManager* nist = G4NistManager::Instance();
    G4Material* titanium = nist->FindOrBuildMaterial("G4_Ti");

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned length      = this->_datareader->GetArrayLength("titaniumEnergy");
    G4double* energy     = this->_datareader->GetArray("titaniumEnergy",length,eV);
    G4double* refraction = this->_datareader->GetArray("titaniumRefraction",length);
    G4double* absorption = this->_datareader->GetArray("titaniumAbsorption",length, mm);

    properties->AddProperty("RINDEX", energy, refraction, length);
    properties->AddProperty("ABSLENGTH", energy, absorption, length);

    titanium->SetMaterialPropertiesTable(properties);

    this->_titanium = titanium;
    return titanium;
}

G4Material* OMMaterialManager::BuildPlastic()
{
    if (this->_plastic != nullptr) return this->_plastic;

    G4NistManager* nist = G4NistManager::Instance();
    G4Material* plastic = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned length      = this->_datareader->GetArrayLength("plasticEnergy");
    G4double* energy     = this->_datareader->GetArray("plasticEnergy",length,eV);
    G4double* refraction = this->_datareader->GetArray("plasticRefraction",length);
    G4double* absorption = this->_datareader->GetArray("plasticAbsorption",length, mm);

    properties->AddProperty("RINDEX", energy, refraction, length);
    properties->AddProperty("ABSLENGTH", energy, absorption, length);

    plastic->SetMaterialPropertiesTable(properties);

    this->_plastic = plastic;
    return plastic;
}

G4Material* OMMaterialManager::BuildPhotocathode()
{
    if (this->_photocathode != nullptr) return this->_photocathode;

    G4Material* photocathode = new G4Material("Photocathode", 12, 12*g, 2.51*g/cm3, kStateSolid);

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned length      = this->_datareader->GetArrayLength("photoEnergy");
    G4double* energy     = this->_datareader->GetArray("photoEnergy",length,eV);
    G4double* refraction = this->_datareader->GetArray("photoRefraction",length);
    G4double* absorption = this->_datareader->GetArray("photoAbsorption",length, mm);

    properties->AddProperty("RINDEX", energy, refraction, length);
    properties->AddProperty("ABSLENGTH", energy, absorption, length);

    photocathode->SetMaterialPropertiesTable(properties);

    this->_photocathode = photocathode;
    return photocathode;
}

///-----------------------------------------------------------------------

G4OpticalSurface* OMMaterialManager::BuildPlasticSurface()
{
    if (this->_plasticSurface != nullptr) return this->_plasticSurface;

    G4OpticalSurface* plasticSurface = new G4OpticalSurface("plasticSurface");

    plasticSurface->SetType(dielectric_dielectric);
    plasticSurface->SetModel(unified);
    plasticSurface->SetFinish(ground);
    plasticSurface->SetSigmaAlpha(this->_datareader->GetScalar("plasticSigmaAlpha"));

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned length        = this->_datareader->GetArrayLength("plasticEnergy");
    G4double* energy       = this->_datareader->GetArray("plasticEnergy",length,eV);
    G4double* reflectivity = this->_datareader->GetArray("plasticReflection", length);

    properties->AddProperty("REFLECTIVITY", energy, reflectivity, length);

    this->_plasticSurface = plasticSurface;
    return plasticSurface;
}

G4OpticalSurface* OMMaterialManager::BuildTitaniumSurface()
{
    if (this->_titaniumSurface != nullptr) return this->_titaniumSurface;

    G4OpticalSurface* titaniumSurface = new G4OpticalSurface("titaniumSurface");

    titaniumSurface->SetType(dielectric_metal);
    titaniumSurface->SetModel(unified);
    titaniumSurface->SetFinish(ground);
    titaniumSurface->SetSigmaAlpha(this->_datareader->GetScalar("titaniumSigmaAlpha"));

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned length        = this->_datareader->GetArrayLength("titaniumEnergy");
    G4double* energy       = this->_datareader->GetArray("titaniumEnergy",length,eV);
    G4double* reflectivity = this->_datareader->GetArray("titaniumReflection", length);

    properties->AddProperty("REFLECTIVITY", energy, reflectivity, length);


    this->_titaniumSurface = titaniumSurface;
    return titaniumSurface;
}

G4OpticalSurface* OMMaterialManager::BuildReflectorSurface()
{
    if (this->_reflectorSurface != nullptr) return this->_reflectorSurface;

    G4OpticalSurface* reflectorSurface = new G4OpticalSurface("reflectorSurface");

    reflectorSurface->SetType(dielectric_metal);
    reflectorSurface->SetModel(unified);
    reflectorSurface->SetFinish(polished);
    reflectorSurface->SetSigmaAlpha(this->_datareader->GetScalar("reflectorSigmaAlpha"));

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned  length       = this->_datareader->GetArrayLength("reflectorEnergy");
    G4double* energy       = this->_datareader->GetArray("reflectorEnergy",length,eV);
    G4double* reflectivity = this->_datareader->GetArray("reflectorReflection", length);
    G4double* transmission = this->_datareader->GetArray("reflectorTransmission", length);

    properties->AddProperty("REFLECTIVITY", energy, reflectivity, length);
    properties->AddProperty("TRANSMITTANCE", energy, transmission, length);
    
    this->_reflectorSurface = reflectorSurface;
    return reflectorSurface;
}

///-----------------------------------------------------------------------

// not in use
G4OpticalSurface* OMMaterialManager::BuildAirSurface()
{
    if (this->_airSurface != nullptr) return this->_airSurface;

    G4OpticalSurface* airSurface = new G4OpticalSurface("airSurface");

    this->_airSurface = airSurface;
    return airSurface;
}

// not in use
G4OpticalSurface* OMMaterialManager::BuildWaterSurface()
{
    if (this->_waterSurface != nullptr) return this->_waterSurface;

    G4OpticalSurface* waterSurface = new G4OpticalSurface("waterSurface");

    this->_waterSurface = waterSurface;
    return waterSurface;
}

// not in use
G4OpticalSurface* OMMaterialManager::BuildGelSurface()
{
    if (this->_gelSurface != nullptr) return this->_gelSurface;

    G4OpticalSurface* gelSurface = new G4OpticalSurface("gelSurface");

    this->_gelSurface = gelSurface;
    return gelSurface;
}

// not in use
G4OpticalSurface* OMMaterialManager::BuildGlassSurface()
{
    if (this->_glassSurface != nullptr) return this->_glassSurface;

    G4OpticalSurface* glassSurface = new G4OpticalSurface("glassSurface");

    this->_glassSurface = glassSurface;
    return glassSurface;
}

// not in use
G4OpticalSurface* OMMaterialManager::BuildPhotocathodeSurface()
{
    if (this->_photocathodeSurface != nullptr) return this->_photocathodeSurface;

    G4OpticalSurface* photocathodeSurface = new G4OpticalSurface("photocathodeSurface");

    photocathodeSurface->SetType(dielectric_dielectric);
    photocathodeSurface->SetModel(glisur);
    photocathodeSurface->SetFinish(polished);
    photocathodeSurface->SetSigmaAlpha(this->_datareader->GetScalar("photoSigmaAlpha"));

    G4MaterialPropertiesTable* properties = new G4MaterialPropertiesTable();

    unsigned  length       = this->_datareader->GetArrayLength("PhotoEnergy");
    G4double* energy       = this->_datareader->GetArray("photoEnergy",length,eV);
    G4double* reflectivity = this->_datareader->GetArray("photoReflection", length);
    G4double* transmission = this->_datareader->GetArray("photoTransmission", length);

    properties->AddProperty("REFLECTIVITY", energy, reflectivity, length);
    properties->AddProperty("TRANSMITTANCE", energy, transmission, length);

    this->_photocathodeSurface = photocathodeSurface;
    return photocathodeSurface;
}