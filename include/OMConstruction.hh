#ifndef OM_CONSTRUCTION_H
#define OM_CONSTRUCTION_H 1

// system includes
#include <vector>

// G4 includes
#include "G4Material.hh"
#include "G4RotationMatrix.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

// project includes
#include "OMConstructionMessenger.hh"
#include "OMMaterialManager.hh"

// forward declarations
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4VSolid;
class G4GDMLParser;
class OMConstructionMessenger;
class G4MultiUnion;

class OMConstruction : public G4VUserDetectorConstruction
{
    public:

        // constructors
        OMConstruction();
        ~OMConstruction();

        // member functions
        G4VPhysicalVolume* Construct();
        void submerge();
        void constructGelpad();
        void constructPMT();
        void placeOpticalUnits();
        void configureGDMLObjects();
        void addOpticalUnit(G4double, G4double, G4double);

        // inline stuff
        void setGDMLFilename(G4String filename){this->_gdml_filename = filename;};
        void setOUCoordCenter(G4ThreeVector center){this->_ou_coord_center = center;};
        void setOUCoordRefX(G4ThreeVector refX){refX.setMag(1); this->_ou_coord_refX = refX;};
        void setOUCoordRefY(G4ThreeVector refY){refY.setMag(1); this->_ou_coord_refY = refY;};

        void setGelpadRingOffset(G4double size){this->_gelpad_ring_offset = size;};
        void setPhotocathodeTubeLength(G4double size){this->_photocathode_tube_length = size;};

        G4String getGDMLFilename(){return this->_gdml_filename;};
        G4ThreeVector getOUCoordCenter(){return this->_ou_coord_center;};
        G4ThreeVector getOUCoordRefX(){return this->_ou_coord_refX;};
        G4ThreeVector getOUCoordRefY(){return this->_ou_coord_refY;};

        G4bool getSubmerge(){return this->_submerge;};
        void   setSubmerge(G4bool val){this->_submerge = val;};

    private:

        OMMaterialManager*                _MaterialManager;
        OMConstructionMessenger*          _ConstructionMessenger;

        G4bool                            _submerge;

        G4VPhysicalVolume*                _world_phsical;
        G4LogicalVolume*                  _world_logical;
        G4VSolid*                         _gelpad_solid;
        G4LogicalVolume*                  _pmt_logical;

        G4int                             _nr_of_OUs;
        std::vector<G4VPhysicalVolume*>   _placed_gelpads;
        std::vector<G4VPhysicalVolume*>   _placed_pmts;

        G4String                          _gdml_filename;

        G4ThreeVector                     _ou_coord_center;
        G4ThreeVector                     _ou_coord_refX;
        G4ThreeVector                     _ou_coord_refY;

        G4double                          _gelpad_ring_offset;
        G4double                          _photocathode_tube_length;

        std::vector<G4ThreeVector>        _ou_centers;
        std::vector<G4ThreeVector>        _ou_positions;
};

#endif