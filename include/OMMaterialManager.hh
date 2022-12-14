#ifndef OM_MATERIAL_MANAGER_H
#define OM_MATERIAL_MANAGER_H 1

// system includes

// G4 Includes
#include "G4Material.hh"
#include "G4OpticalSurface.hh"

// project includes
#include "OMDataReader.hh"

/*  OMMaterialmanager implemented as singleton to not accidentally increase file i/o on reading optical physics data */

class OMMaterialManager{

    public:

        static OMMaterialManager* getInstance();
        ~OMMaterialManager();

        G4Material* BuildVacuum();
        G4Material* BuildAir();
        G4Material* BuildWater();
        G4Material* BuildGlass();
        G4Material* BuildGel();
        G4Material* BuildTitanium();
        G4Material* BuildPlastic();
        G4Material* BuildPhotocathode();

        G4OpticalSurface* BuildAirSurface();
        G4OpticalSurface* BuildWaterSurface();
        G4OpticalSurface* BuildGlassSurface();
        G4OpticalSurface* BuildGelSurface();
        G4OpticalSurface* BuildTitaniumSurface();
        G4OpticalSurface* BuildPlasticSurface();
        G4OpticalSurface* BuildPhotocathodeSurface();
        G4OpticalSurface* BuildReflectorSurface();


    private:

        static OMMaterialManager* _instance;
        OMMaterialManager();

        OMDataReader* _datareader;

        G4Material* _vacuum;
        G4Material* _air;
        G4Material* _water;
        G4Material* _glass;
        G4Material* _gel;
        G4Material* _titanium;
        G4Material* _plastic;
        G4Material* _photocathode;
        
        G4OpticalSurface* _waterSurface;
        G4OpticalSurface* _airSurface;
        G4OpticalSurface* _glassSurface;
        G4OpticalSurface* _gelSurface;
        G4OpticalSurface* _titaniumSurface;
        G4OpticalSurface* _plasticSurface;
        G4OpticalSurface* _photocathodeSurface;
        G4OpticalSurface* _reflectorSurface;

};

#endif