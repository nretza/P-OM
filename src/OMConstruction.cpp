// system includes
#include <cmath>

// G4 includes
#include "G4NistManager.hh"
#include "G4GDMLParser.hh"
#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Sphere.hh"
#include "G4Ellipsoid.hh"
#include "G4EllipticalTube.hh"
#include "G4IntersectionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

// project includes
#include "OMConstruction.hh"


OMConstruction::OMConstruction()
: G4VUserDetectorConstruction(),
 _world_phsical(nullptr),
 _world_logical(nullptr),
 _gdml_subtraction_solid(nullptr),
 _gelpad_solid(nullptr),
 _pmt_logical(nullptr),
 _nr_of_OUs(0),
 _gdml_filename(""),
 _ou_coord_center(0,0,0),
 _ou_coord_refX(1,0,0),
 _ou_coord_refY(0,1,0)
{
    this->_MaterialManager       = OMMaterialManager::getInstance();
    this->_ConstructionMessenger = new OMConstructionMessenger(this);
}

OMConstruction::~OMConstruction()
{
    delete this->_MaterialManager;
    delete this->_ConstructionMessenger;
    delete this->_world_logical;
    delete this->_world_phsical;
    delete this->_gdml_subtraction_solid;
    delete this->_gelpad_solid;
    delete this->_pmt_logical;
}

G4VPhysicalVolume* OMConstruction::Construct()
{
    //------------
    // read world from GDML or give empty world
    //------------

    if(this->_gdml_filename == "")
    {
        G4Material* air      = this->_MaterialManager->BuildAir();
        this->_world_logical = new G4LogicalVolume(new G4Box("World", 50*m,50*m,50*m), air, "World");
        this->_world_phsical = new G4PVPlacement(nullptr, G4ThreeVector(), "World", this->_world_logical, nullptr, false, 0);
    }
    else
    {
        G4GDMLParser parser;
        parser.SetOverlapCheck(false);
        parser.Read(_gdml_filename);
        this->_world_phsical = parser.GetWorldVolume();
        this->_world_logical = this->_world_phsical->GetLogicalVolume();
        this->configureGDMLObjects();
    }

    //------------
    // place optical units (gelpad and pmt)
    //------------

    this->placeOpticalUnits();

    //------------
    // setup and return world
    //------------

    G4VisAttributes* world_vis = new G4VisAttributes(false);  // visibility = false
    this->_world_logical->SetVisAttributes(world_vis);

    return this->_world_phsical;
}

void OMConstruction::configureGDMLObjects()
{
    //-----------
    // construct materials
    //-----------

    G4Material* water    = this->_MaterialManager->BuildWater();
    G4Material* air      = this->_MaterialManager->BuildAir();
    G4Material* gel      = this->_MaterialManager->BuildGel();
    G4Material* glass    = this->_MaterialManager->BuildGlass();
    G4Material* plastic  = this->_MaterialManager->BuildPlastic();
    G4Material* titanium = this->_MaterialManager->BuildTitanium();

    G4OpticalSurface* plasticSurface  = this->_MaterialManager->BuildPlasticSurface();
    G4OpticalSurface* titaniumSurface = this->_MaterialManager->BuildTitaniumSurface();

    //-----------
    // set world material
    //-----------

    this->_world_logical->SetMaterial(air);

    //-----------
    // define gdml subtraction solid, from which gelpads will be subtracted at placement
    //-----------
    
    this->_gdml_subtraction_solid = new G4MultiUnion("GelpadSubtractionSolid");
     
    //-----------
    // loop through objects and set properties
    //-----------

    const int nr_of_objects = this->_world_logical->GetNoDaughters();
    for(int i=0; i<nr_of_objects; i++)
    {
        G4VPhysicalVolume* obj_phsical  = this->_world_logical->GetDaughter(i);
        G4LogicalVolume*   obj_logical  = obj_phsical->GetLogicalVolume();
        G4VSolid*          obj_solid    = obj_logical->GetSolid();
        G4String           obj_name     = obj_phsical->GetName();
        G4Material*        obj_material = nullptr;
        G4VisAttributes* obj_vis = new G4VisAttributes();

        // PMT
        if(obj_name.find("Hamamatsu_R14374") != std::string::npos)
        {
            obj_material = glass;
            obj_vis->SetColor(1.0, 0.6, 0.5, 0.7); //slightly transparent red
            this->_gdml_subtraction_solid->AddNode(obj_solid, G4Transform3D());
        }

        // glass sphere
        else if (obj_name.find("GlasHemisphere") != std::string::npos)
        {
            obj_material = glass;
            obj_vis->SetColor(1.0, 1.0, 1.0, 0.3); // transparent white
            this->_gdml_subtraction_solid->AddNode(obj_solid, G4Transform3D());
        }

        // titanium ring
        else if (obj_name.find("Titan_flange") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.9, 0.9, 0.9, 1.0); // light grey
            this->_gdml_subtraction_solid->AddNode(obj_solid, G4Transform3D());
        }

        // HV base
        else if (obj_name.find("HV_divider") != std::string::npos)
        {
            // wrap in plastic surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, plasticSurface);

            obj_material = plastic;
            obj_vis->SetColor(0.2, 0.4, 0.2, 1.0); // dark green
            this->_gdml_subtraction_solid->AddNode(obj_solid, G4Transform3D());
        }

        // spring
        else if (obj_name.find("spring") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.9, 0.9, 0.9, 1.0); // light grey
            this->_gdml_subtraction_solid->AddNode(obj_solid, G4Transform3D());
        }

        // gel
        else if (obj_name.find("Optical_gel") != std::string::npos)
        {
            obj_material = gel;
            obj_vis->SetColor(1.0, 1.0, 1.0, 0.4); // transparent white
            this->_gdml_subtraction_solid->AddNode(obj_solid, G4Transform3D());
        }

        // water
        else if (obj_name.find("water") != std::string::npos)
        {
            obj_material = water;
            obj_vis->SetColor(0.1, 0.1, 0.5, 0.3); // transparent dark blue
        }

        // air
        else if (obj_name.find("air") != std::string::npos)
        {
            obj_material = air;
            obj_vis->SetVisibility(false); // transparent
        }

        // plastic is assumed (PMT mounting etc..)
        else
        {
            // wrap in plastic surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, plasticSurface);

            obj_material = plastic;
            obj_vis->SetColor(0.2, 0.2, 0.2, 1.0); // almost black
            // this->_gelpad_subtraction_solid->AddNode(obj_solid, G4Transform3D());
        }

        obj_logical->SetMaterial(obj_material);
        obj_logical->SetVisAttributes(obj_vis);
    }

    this->_gdml_subtraction_solid->Voxelize();
}

void OMConstruction::placeOpticalUnits()
{
    if (this->_nr_of_OUs <= 0) return;

    //-----------
    // construct parts if needed
    //-----------

    if(this->_gelpad_solid == nullptr) this->constructGelpad();
    if(this->_pmt_logical  == nullptr) this->constructPMT();

    //-----------
    // materials and surfaces
    //-----------

    G4Material* gel = this->_MaterialManager->BuildGel();
    G4OpticalSurface* reflector    = this->_MaterialManager->BuildReflectorSurface();

    //-----------
    // offset of component solids to placement point
    //-----------

    G4Transform3D gelpad_offset(G4RotationMatrix(), G4ThreeVector(0, 0, -5.11 * mm));
    G4Transform3D PMTOffset(G4RotationMatrix(G4ThreeVector(1,0,0), 90 * degree), G4ThreeVector(0, 0, - 53 * mm));

    for(int i=0; i < this->_nr_of_OUs; i++)
    {
        //-----------
        // read positional data and calculate 3D transform
        //-----------

        G4ThreeVector ou_pos = this->_ou_positions[i];
        G4ThreeVector ou_ctr = this->_ou_centers[i];

        G4ThreeVector id_rot = G4ThreeVector(0,0,1);
        G4ThreeVector rotation_vec = ou_pos - ou_ctr;
        rotation_vec.setMag(1);

        G4double delta;
        G4ThreeVector cross;
        if (rotation_vec == id_rot)
        {
            cross = G4ThreeVector(1,0,0);
            delta = 0;
        }
        else if (rotation_vec == - id_rot)
        {
            cross = G4ThreeVector(1,0,0);
            delta = 180*degree;
        }
        else
        {
            cross = id_rot.cross(rotation_vec);
            delta = acos(rotation_vec.dot(id_rot));
        }
        G4Transform3D transform3D(G4RotationMatrix(cross, delta), ou_pos);

        //-----------
        // subtract subtraction solid consisting of PMT and all other parts from gelpad
        //-----------

        G4VSolid* gelpad_solid_to_place = this->_gelpad_solid;

        // from everything else
        if (this->_gdml_subtraction_solid != nullptr) gelpad_solid_to_place = new G4SubtractionSolid("gelpad",
                                                                            this->_gelpad_solid,
                                                                            this->_gdml_subtraction_solid,
                                                                            gelpad_offset.inverse() * transform3D.inverse());
        // from PMT
        gelpad_solid_to_place = new G4SubtractionSolid("gelpad",
                                                        gelpad_solid_to_place,
                                                        this->_pmt_logical->GetSolid(),
                                                        gelpad_offset.inverse() * PMTOffset);

        //-----------
        // create and configure logical volume for gelpad
        //-----------

        G4LogicalVolume* gelpad_logical = new G4LogicalVolume(gelpad_solid_to_place, gel, "gelpad");
        G4VisAttributes* gelpad_vis = new G4VisAttributes();
        gelpad_vis->SetColor(1,1,1,0.4);
        gelpad_logical->SetVisAttributes(gelpad_vis);

        //-----------
        // place gelpad and pmt
        //-----------

        G4VPhysicalVolume* gelpad_placement = new G4PVPlacement(transform3D * gelpad_offset, // 3D transform
                                                                gelpad_logical,              // logical volume
                                                                "gelpad",                    // name
                                                                this->_world_logical,        // mother (logical) volume
                                                                false,                       // no boolean operations
                                                                i);                          // its copy number

        G4VPhysicalVolume* pmt_placement = new G4PVPlacement(transform3D * PMTOffset,  // 3D transform
                                                             this->_pmt_logical,       // logical volume
                                                             "PMT",                    // name
                                                             this->_world_logical,     // mother (logical) volume
                                                             false,                    // no boolean operations
                                                             i);                       // its copy number
       
        //-----------
        // set reflector surface to pmt
        //-----------

        G4VPhysicalVolume* vac_lower = pmt_placement->GetLogicalVolume()->GetDaughter(1);

        new G4LogicalBorderSurface("reflector", pmt_placement, vac_lower, reflector);
        new G4LogicalBorderSurface("reflector", vac_lower, pmt_placement, reflector);

        //-----------
        // store pointer (just in case)
        //-----------

        this->_placed_gelpads.push_back(gelpad_placement);
        this->_placed_pmts.push_back(pmt_placement);
    }
}

void OMConstruction::constructGelpad()
{
    // gelpad cone
    // slightly thicker than real gelpad, will be subtracted by PMT/Glass later.
    const G4double gelpad_opening_angle = 50 * degree;
    const G4double gelpad_small_radius  = 40 * mm;
    const G4double gelpad_thickness     = 24 * mm;
    const G4double gelpad_large_radius  = gelpad_small_radius + tan ( gelpad_opening_angle ) * gelpad_thickness;

    G4VSolid* gelpad_cone = new G4Cons("gelpad_cone",
                                        0  * mm,               //inside radius at -pDz
                                        gelpad_small_radius,   //outside radius at -pDz
                                        0  * mm,               //inside radius at +pDz
                                        gelpad_large_radius,   //outside radius at +pDz
                                        gelpad_thickness / 2,  //half length in Z
                                        0,                     //starting angle of the segment in radians
                                        2*pi);                 //the angle of the segment in radians


    // gelpad sphere models inside of glass dome (to cut edges of cone)
    // slightly larger than real counterpart
    G4double sphere_radius = 203 * mm;
    G4VSolid* gelpad_sphere_cut = new G4Sphere("gelpad_sphere_cut",
                                               0,               //r min
                                               sphere_radius,   //r max
                                               0,               //start phi
                                               2*pi,            // end phi
                                               0,               //start theta
                                               pi);             //end theta


    // Intersection between gelpad cone and sphere
    // creates spherical top surface on gelpad
    G4double z_translation = - sphere_radius + (gelpad_thickness / 2);
    G4VSolid* gelpad_solid = new G4IntersectionSolid("gelpad",
                                                gelpad_cone,                         //solid 1
                                                gelpad_sphere_cut,                   //solid 2
                                                new G4RotationMatrix(0,0,0),         //rotation (identity)
                                                G4ThreeVector(0, 0, z_translation)); //translation

    this->_gelpad_solid = gelpad_solid;
}

void OMConstruction::constructPMT()
{
    //-----------
    // get material pointers
    //-----------

    G4Material* vacuum       = this->_MaterialManager->BuildVacuum();
    G4Material* glass        = this->_MaterialManager->BuildGlass();
    G4Material* plastic      = this->_MaterialManager->BuildPlastic();
    G4Material* photocathode = this->_MaterialManager->BuildPhotocathode();

    G4OpticalSurface* plastic_surface = this->_MaterialManager->BuildPlasticSurface();

    //-----------
    // create vacuum solids and logical volumes
    //-----------

    G4double VacSphereRadius   = 50   * mm;
    G4double VacZylinderHeight = 10   * mm;
    G4double VacZylinderRadius = 38   * mm;
    G4double VacSphereCutoff   = sqrt( pow(VacSphereRadius,2) - pow(VacZylinderRadius,2));

    /*
                ..------..
             -°°          °°-    <---- SphereRadius
           /                  \ 
          |<------------------>|   -
          |   ZylinderRadius   |   | ZylinderHeight
          |                    |   -
           \                  /
             -..          ..-
                °°------°°
    */

    G4Ellipsoid* upperVacSphere = new G4Ellipsoid("upperVacSphere",
                                                    VacSphereRadius,   // x semiaxis
                                                    VacSphereRadius,   // y semiaxis
                                                    VacSphereRadius,   // z semiaxis
                                                    VacSphereCutoff,   // bottom cutoff
                                                    VacSphereRadius);  // top cutoff

    G4Ellipsoid* lowerVacSphere = new G4Ellipsoid("lowerVacSphere",
                                                    VacSphereRadius,   // x semiaxis
                                                    VacSphereRadius,   // y semiaxis
                                                    VacSphereRadius,   // z semiaxis
                                                    -VacSphereRadius,  // bottom cutoff
                                                    -VacSphereCutoff); // top cutoff

    G4EllipticalTube* VacZylinder = new G4EllipticalTube("VacZylinder",
                                                        VacZylinderRadius,        // x semiaxis
                                                        VacZylinderRadius,        // y semiaxis
                                                        VacZylinderHeight / 2);   // height

    G4LogicalVolume* upperVacSphereLog = new G4LogicalVolume(upperVacSphere, vacuum, "upperVacSphere");
    G4LogicalVolume* lowerVacSphereLog = new G4LogicalVolume(lowerVacSphere, vacuum, "lowerVacSphere");
    G4LogicalVolume* VacZylinderLog    = new G4LogicalVolume(VacZylinder, vacuum, "VacZylinder");

    G4VisAttributes* vacuum_vis = new G4VisAttributes();
    vacuum_vis->SetColor(0.5, 0.5, 0.5, 0.4);    //transparent grey
    upperVacSphereLog->SetVisAttributes(vacuum_vis);
    lowerVacSphereLog->SetVisAttributes(vacuum_vis);
    VacZylinderLog->SetVisAttributes(vacuum_vis);

    //-----------
    // create photocathode solid and logical volume (absorbs the photon to detect it)
    //-----------

    G4double photocathode_thickness = 1 * mm;
    G4double photocathode_padding   = 0 * mm;

    G4Sphere* photocathode_subsolid = new G4Sphere("photocathode_subsolid",
                                                    VacSphereRadius - photocathode_thickness - photocathode_padding,
                                                    VacSphereRadius - photocathode_padding,
                                                    0,360*degree, 0, 180* degree);


    G4IntersectionSolid* photocathode_solid = new G4IntersectionSolid("photocathode",
                                                                    upperVacSphere,
                                                                    photocathode_subsolid);

    G4LogicalVolume* photocathodeLog = new G4LogicalVolume(photocathode_solid, photocathode, "photocathode");

    G4VisAttributes* photocathode_vis = new G4VisAttributes();
    photocathode_vis->SetColor(0.9, 0.9, 0.1, 0.4); //golden-ish yellow
    photocathodeLog->SetVisAttributes(photocathode_vis);

    //-----------
    // create absorber solid and logical volume (simulate PMT-cascade structures, used to absorb photons)
    //-----------

    G4double tubRadius =  20   * mm;
    G4double tubHeight =  50   * mm;
    G4double tubOffset = -16.5 * mm;

    G4Tubs* AbsorberTubs =new G4Tubs("Absorber",
                                       0,
                                       tubRadius,
                                       tubHeight / 2,
                                       0,
                                       360 * degree);

    G4LogicalVolume* AbsorberLog = new G4LogicalVolume(AbsorberTubs, plastic, "Absorber");
    G4VisAttributes* absorber_vis = new G4VisAttributes();
    absorber_vis->SetColor(0.3, 0.3, 0.3, 1.0);
    AbsorberLog->SetVisAttributes(absorber_vis);
    new G4LogicalSkinSurface("AbsorberSurface", AbsorberLog, plastic_surface);

    //-----------
    // read in pmt geometry and configure
    //-----------

    G4GDMLParser parser;
    parser.SetOverlapCheck(false);
    parser.Read("geometry/Hamamatsu_R14374_tube_with_tail.gdml");
    this->_pmt_logical = parser.GetVolume("Hamamatsu_R14374_tube_with_tail_LV");

    G4VisAttributes* pmt_vis = new G4VisAttributes();
    pmt_vis->SetColor(1.0, 0.6, 0.5, 0.7); //slightly transparent red
    this->_pmt_logical->SetVisAttributes(pmt_vis);
    this->_pmt_logical->SetMaterial(glass);

    //-----------
    // place vacuum parts, photocathode and absorber in pmt
    //-----------

    G4double vacOffsetInPMT = 29 * mm;

    G4double VacZylinderOffset    = vacOffsetInPMT;
    G4double upperVacSphereOffset = VacZylinderOffset + VacZylinderHeight / 2 - VacSphereCutoff;
    G4double lowerVacSphereOffset = VacZylinderOffset - VacZylinderHeight / 2 + VacSphereCutoff;


    new G4PVPlacement(new G4RotationMatrix(),
                      G4ThreeVector(),
                      photocathodeLog,
                      "photocathode",
                      upperVacSphereLog,
                      false,
                      0);

    new G4PVPlacement(new G4RotationMatrix(G4ThreeVector(1,0,0), 90 * degree),
                      G4ThreeVector(0,upperVacSphereOffset,0),
                      upperVacSphereLog,
                      "upperVacSphere",
                      this->_pmt_logical,
                      false,
                      0);

    new G4PVPlacement(new G4RotationMatrix(G4ThreeVector(1,0,0), 90 * degree),
                      G4ThreeVector(0,lowerVacSphereOffset,0),
                      lowerVacSphereLog,
                      "lowerVacSphere",
                      this->_pmt_logical,
                      false,
                      0);

    new G4PVPlacement(new G4RotationMatrix(G4ThreeVector(1,0,0), 90 * degree),
                      G4ThreeVector(0,VacZylinderOffset,0),
                      VacZylinderLog,
                      "VacZylinder",
                      this->_pmt_logical,
                      false,
                      0);
        
    new G4PVPlacement(new G4RotationMatrix(G4ThreeVector(1,0,0), 90 * degree),
                      G4ThreeVector(0,tubOffset,0),
                      AbsorberLog,
                      "Absorber",
                      this->_pmt_logical,
                      false,
                      0);
}

void OMConstruction::addOpticalUnit(G4double radius, G4double theta, G4double phi)
{
    // transform Gelpad coord to global
    G4Point3D from1(this->_ou_coord_refX);
    G4Point3D from2(this->_ou_coord_refY);
    G4Point3D from3(this->_ou_coord_refX.cross(this->_ou_coord_refY));

    G4Point3D to1(1,0,0);
    G4Point3D to2(0,1,0);
    G4Point3D to3(0,0,1);

    G4Transform3D ou2global(from1, from2, from3, to1, to2, to3);

    // get gelpad position and transform
    G4Point3D ou_pos_in_ou_coord(radius * sin(theta) * cos(phi),
                                 radius * sin(theta) * sin(phi),
                                 radius * cos(theta));

    G4ThreeVector ou_pos_in_global_coord = ou_pos_in_ou_coord.transform(ou2global) + this->_ou_coord_center;

    // store values
    this->_ou_centers.push_back(this->_ou_coord_center);
    this->_ou_positions.push_back(ou_pos_in_ou_coord);
    this->_nr_of_OUs++;
    
}