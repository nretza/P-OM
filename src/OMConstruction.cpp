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
#include "G4UnionSolid.hh"
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
 _submerge(false),
 _solidReflector(false),
 _world_phsical(nullptr),
 _world_logical(nullptr),
 _gelpad_solid(nullptr),
 _pmt_logical(nullptr),
 _nr_of_OUs(0),
 _gdml_filename(""),
 _ou_coord_center(0,0,0),
 _ou_coord_refX(1,0,0),
 _ou_coord_refY(0,1,0),
 _gelpad_ring_offset(0),
 _photocathode_tube_length(0)
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
        this->_world_logical = new G4LogicalVolume(new G4Box("World", 100*m,100*m,100*m), air, "World");
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
    // submerge P-OM if needed
    //------------

    if (this->_submerge) this->submerge();

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
        G4VisAttributes*   obj_vis = new G4VisAttributes();

        // PMT
        if(obj_name.find("Hamamatsu_R14374") != std::string::npos)
        {
            obj_material = glass;
            obj_vis->SetColor(1.0, 0.6, 0.5, 0.7); //slightly transparent red
        }

        // glass sphere
        else if (obj_name.find("GlasHemisphere") != std::string::npos)
        {
            obj_material = glass;
            obj_vis->SetColor(1.0, 1.0, 1.0, 0.3); // transparent white
        }

        // titanium flange
        else if (obj_name.find("Titan_flange") != std::string::npos || obj_name.find("fla-ti_revCsp_up") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.9, 0.9, 0.9, 1.0); // light grey
        }

        // HV base
        else if (obj_name.find("HV_divider") != std::string::npos)
        {
            // wrap in plastic surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, plasticSurface);

            obj_material = plastic;
            obj_vis->SetColor(0.2, 0.4, 0.2, 1.0); // dark green
        }

        // spring
        else if (obj_name.find("spring") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.9, 0.9, 0.9, 1.0); // light grey
        }

        // gel
        else if (obj_name.find("Optical_gel") != std::string::npos)
        {
            obj_material = gel;
            obj_vis->SetColor(1.0, 1.0, 1.0, 0.4); // transparent white
        }

        // cable breakout
        else if (obj_name.find("cable_breakout") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.7, 0.7, 0.7, 1.0); // grey
        }

        // frame
        else if (obj_name.find("frame") != std::string::npos)
        {
            // wrap in plastic surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, plasticSurface);

            obj_material = plastic;
            obj_vis->SetColor(0.2, 0.2, 0.2, 1.0); // almost black
        }

        // glass sphere (v13)
        else if (obj_name.find("HS-BOR-17-09") != std::string::npos)
        {
            obj_material = glass;
            obj_vis->SetColor(1.0, 1.0, 1.0, 0.3); // transparent white
        }

        // titanium flange (v13)
        else if (obj_name.find("FLA-TI-17-14") != std::string::npos || obj_name.find("fla-ti_revCsp_up") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.9, 0.9, 0.9, 1.0); // light grey
        }

        // titanium backplate (v13)
        else if (obj_name.find("10136520") != std::string::npos || obj_name.find("fla-ti_revCsp_up") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.9, 0.9, 0.9, 1.0); // light grey
        }

        // titanium ring (v13)
        else if (obj_name.find("052-10132172") != std::string::npos || obj_name.find("fla-ti_revCsp_up") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.9, 0.9, 0.9, 1.0); // light grey
        }

        // cable breakout 1 (v13)
        else if (obj_name.find("014-10132184") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.7, 0.7, 0.7, 1.0); // grey
        }

        // cable breakout 2 (v13)
        else if (obj_name.find("051-10132214") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = titanium;
            obj_vis->SetColor(0.7, 0.7, 0.7, 1.0); // grey
        }

        // cable breakout 3 (v13)
        else if (obj_name.find("BR101") != std::string::npos)
        {
            // wrap in titanium surface for reflection properties
            new G4LogicalSkinSurface(obj_name, obj_logical, titaniumSurface);

            obj_material = plastic;
            obj_vis->SetColor(0.2, 0.2, 0.2, 1.0); // almost black
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
        }

        obj_logical->SetMaterial(obj_material);
        obj_logical->SetVisAttributes(obj_vis);
    }
}

void OMConstruction::submerge()
{
    // i did not find a suitable solution to automate this given different geometries

    // as of now, just place some custom code depending on geometry here.

    if (_gdml_filename == "")
    {
        this->_world_logical->SetMaterial(this->_MaterialManager->BuildWater());
    }

    else if ( _gdml_filename == "geometry/P-OM_hemisphere_v13/mother.gdml" ||
              _gdml_filename == "geometry/P-OM_hemisphere_v11/mother.gdml")
    {
        // transformations
        G4Point3D from1(this->_ou_coord_refX);
        G4Point3D from2(this->_ou_coord_refY);
        G4Point3D from3(this->_ou_coord_refX.cross(this->_ou_coord_refY));

        G4Point3D to1(1,0,0);
        G4Point3D to2(0,1,0);
        G4Point3D to3(0,0,1);

        G4Transform3D ou2global(from1, from2, from3, to1, to2, to3);

        // solids
        G4double air_indside_radius = 207 * mm;
        G4Ellipsoid* air_inside = new G4Ellipsoid("air_inside",
                                                air_indside_radius,
                                                air_indside_radius,
                                                air_indside_radius,
                                                0,
                                                air_indside_radius);

        // subtract from air inside
        G4SubtractionSolid* water = new G4SubtractionSolid("water", this->_world_logical->GetSolid(), air_inside);

        // subtract overlaps with P-OM
        const int nr_of_objects = this->_world_logical->GetNoDaughters();
        for(int i=0; i<nr_of_objects; i++)
        {
            G4VPhysicalVolume* obj_phsical  = this->_world_logical->GetDaughter(i);
            G4LogicalVolume*   obj_logical  = obj_phsical->GetLogicalVolume();
            G4VSolid*          obj_solid    = obj_logical->GetSolid();
            G4String           obj_name     = obj_phsical->GetName();

            // from relevant parts
            if (obj_name.find("GlasHemisphere") != std::string::npos)
            {
                water = new G4SubtractionSolid("water",
                                                water,
                                                obj_solid,
                                                ou2global.inverse());
            }
        }

        // logical and placement
        G4LogicalVolume* waterLog = new G4LogicalVolume(water, this->_MaterialManager->BuildWater(), "water"); 
        waterLog->SetVisAttributes(new G4VisAttributes(false));
        new G4PVPlacement(ou2global, "water", waterLog, this->_world_phsical, false, 0);
    }

    else if ( _gdml_filename == "geometry/P-OM_module_v11/mother.gdml"  ||
              _gdml_filename == "geometry/P-OM_module_v11/mother_closed_frame.gdml")
    {   
        G4double air_indside_radius = 207 * mm;
        G4double air_middle_offset  = 45 * mm;

        // create air volumes
        G4Tubs*  air_inbetween = new G4Tubs("air_inbetween",
                                            0,
                                            air_indside_radius,
                                            air_middle_offset/2,
                                            0,
                                            360 * degree);
    
        G4Ellipsoid*  air_inside_left = new G4Ellipsoid("air_inside_left",
                                                air_indside_radius,
                                                air_indside_radius,
                                                air_indside_radius,
                                                0,
                                                air_indside_radius);
        
        G4Ellipsoid* air_inside_right = new G4Ellipsoid("air_inside_right",
                                                air_indside_radius,
                                                air_indside_radius,
                                                air_indside_radius,
                                                - air_indside_radius,
                                                0);

        // subtract from air inside
        G4VSolid* water = this->_world_logical->GetSolid();
        water = new G4SubtractionSolid("water", water, air_inbetween, new G4RotationMatrix(), G4ThreeVector(0,0,0));
        water = new G4SubtractionSolid("water", water, air_inside_left, new G4RotationMatrix(), G4ThreeVector(0,0,air_middle_offset));
        water = new G4SubtractionSolid("water", water, air_inside_right, new G4RotationMatrix(), G4ThreeVector(0,0,- air_middle_offset));

        // subtract overlaps with P-OM
        const int nr_of_objects = this->_world_logical->GetNoDaughters();
        for(int i=0; i<nr_of_objects; i++)
        {
            G4VPhysicalVolume* obj_phsical  = this->_world_logical->GetDaughter(i);
            G4LogicalVolume*   obj_logical  = obj_phsical->GetLogicalVolume();
            G4VSolid*          obj_solid    = obj_logical->GetSolid();
            G4String           obj_name     = obj_phsical->GetName();

            // from relevant parts
            if (obj_name.find("GlasHemisphere") != std::string::npos)
            {
                water = new G4SubtractionSolid("water",
                                                water,
                                                obj_solid);
            }
        }

        // logical and placement
        G4LogicalVolume* waterLog = new G4LogicalVolume(water, this->_MaterialManager->BuildWater(), "water");
        waterLog->SetVisAttributes(new G4VisAttributes(false));
        new G4PVPlacement(G4Transform3D(), "water", waterLog, this->_world_phsical, false, 0);
    }

    else if ( _gdml_filename == "geometry/P-OM_module_v13/mother.gdml")
    {   
        G4double air_indside_radius = 207 * mm;
        G4double air_middle_offset  =  80 * mm;

        // create air volumes
        G4Tubs*  air_inbetween = new G4Tubs("air_inbetween",
                                            0,
                                            air_indside_radius,
                                            air_middle_offset/2,
                                            0,
                                            360 * degree);
    
        G4Ellipsoid*  air_inside_left = new G4Ellipsoid("air_inside_left",
                                                air_indside_radius,
                                                air_indside_radius,
                                                air_indside_radius,
                                                0,
                                                air_indside_radius);
        
        G4Ellipsoid* air_inside_right = new G4Ellipsoid("air_inside_right",
                                                air_indside_radius,
                                                air_indside_radius,
                                                air_indside_radius,
                                                - air_indside_radius,
                                                0);

        // subtract from air inside
        G4VSolid* water = this->_world_logical->GetSolid();
        water = new G4SubtractionSolid("water", water, air_inbetween, new G4RotationMatrix(), G4ThreeVector(0,0,0));
        water = new G4SubtractionSolid("water", water, air_inside_left, new G4RotationMatrix(), G4ThreeVector(0,0,air_middle_offset));
        water = new G4SubtractionSolid("water", water, air_inside_right, new G4RotationMatrix(), G4ThreeVector(0,0,- air_middle_offset));

        // subtract overlaps with P-OM
        const int nr_of_objects = this->_world_logical->GetNoDaughters();
        for(int i=0; i<nr_of_objects; i++)
        {
            G4VPhysicalVolume* obj_phsical  = this->_world_logical->GetDaughter(i);
            G4LogicalVolume*   obj_logical  = obj_phsical->GetLogicalVolume();
            G4VSolid*          obj_solid    = obj_logical->GetSolid();
            G4String           obj_name     = obj_phsical->GetName();

            // from relevant parts
            if (obj_name.find("GlasHemisphere") != std::string::npos)
            {
                water = new G4SubtractionSolid("water",
                                                water,
                                                obj_solid);
            }
        }

        // logical and placement
        G4LogicalVolume* waterLog = new G4LogicalVolume(water, this->_MaterialManager->BuildWater(), "water");
        waterLog->SetVisAttributes(new G4VisAttributes(false));
        new G4PVPlacement(G4Transform3D(), "water", waterLog, this->_world_phsical, false, 0);
    }

    else
    {
        G4Exception("void OMConstruction::submerge()",
                    "no suitable implementation for submerging exist!",
                    JustWarning,
                    "No suitable implementation exists for submerging the existing geometry in water. Will continue run with air world.\nPlease implement a method in OMConstruction::submerge() to make this warning disappear.");
    }
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

    G4Material*       gel       = this->_MaterialManager->BuildGel();
    G4OpticalSurface* reflector = this->_MaterialManager->BuildReflectorSurface();

    //-----------
    // offset of component solids to placement point (should be at tip pf PMT)
    //-----------

    G4Transform3D gelpad_offset(G4RotationMatrix(), G4ThreeVector(0, 0, -5.11 * mm));
    //G4Transform3D gelpad_offset(G4RotationMatrix(), G4ThreeVector(0, 0, -7.11 * mm));
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
        G4Transform3D ouPlacementTransform(G4RotationMatrix(cross, delta), ou_pos);

        //-----------
        // subtract relevant solids from gelpad
        //-----------

        G4VSolid* gelpad_solid_to_place = this->_gelpad_solid;
        
        // from gdml glass
        G4bool glass_subtracted = false;
        const int nr_of_objects = this->_world_logical->GetNoDaughters();
        for(int i=0; i<nr_of_objects; i++)
        {
            G4VPhysicalVolume* obj_phsical  = this->_world_logical->GetDaughter(i);
            G4LogicalVolume*   obj_logical  = obj_phsical->GetLogicalVolume();
            G4VSolid*          obj_solid    = obj_logical->GetSolid();
            G4String           obj_name     = obj_phsical->GetName();
            
            if (obj_name.find("GlasHemisphere") != std::string::npos)
            {
                gelpad_solid_to_place = new G4SubtractionSolid("gelpad",
                                                                gelpad_solid_to_place,
                                                                obj_solid,
                                                                gelpad_offset.inverse() * ouPlacementTransform.inverse());
                glass_subtracted = true;
            }
        }

        // if gelpad is not subracted from gdml glass, subreact from custom glass sphere (to get the curvature right)
        if (!glass_subtracted)
        {
            G4double sphere_radius         = 201.90 * mm;
            G4double pmt_to_glass_distance =      2 * mm;
            G4Transform3D sphere_transform(G4RotationMatrix(), G4ThreeVector(0, 0, sphere_radius - pmt_to_glass_distance));
            gelpad_solid_to_place = new G4IntersectionSolid("gelpad",
                                                            gelpad_solid_to_place,
                                                            new G4Orb("gelpad_glass_subtr_solid", sphere_radius),
                                                            gelpad_offset.inverse() * sphere_transform.inverse());
        }

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

        G4VPhysicalVolume* gelpad_placement = new G4PVPlacement(ouPlacementTransform * gelpad_offset, // 3D transform
                                                                gelpad_logical,              // logical volume
                                                                "gelpad",                    // name
                                                                this->_world_logical,        // mother (logical) volume
                                                                false,                       // no boolean operations
                                                                i);                          // its copy number

        G4VPhysicalVolume* pmt_placement = new G4PVPlacement(ouPlacementTransform * PMTOffset,  // 3D transform
                                                             this->_pmt_logical,       // logical volume
                                                             "PMT",                    // name
                                                             this->_world_logical,     // mother (logical) volume
                                                             false,                    // no boolean operations
                                                             i);                       // its copy number
       
        //-----------
        // set reflector around gelpad
        //-----------

        if ( this->getSolidReflector() )
        {
            new G4LogicalBorderSurface("gelpadSolidReflectorInside", gelpad_placement, this->_world_phsical, this->_MaterialManager->BuildReflectorSurface());
            new G4LogicalBorderSurface("gelpadSolidReflectorOutside", this->_world_phsical, gelpad_placement, this->_MaterialManager->BuildReflectorSurface());
        }

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
    //const G4double gelpad_thickness     = 30 * mm;
    const G4double gelpad_large_radius  = gelpad_small_radius + tan ( gelpad_opening_angle ) * gelpad_thickness;

    const G4double gelpad_overflow_max_radius = 25 * cm;
    const G4double gelpad_overflow_height     =  5 * cm;
    const G4double gelpad_overflow_offset     =  5.11 * mm - this->_gelpad_ring_offset + gelpad_overflow_height / 2;


    G4VSolid* gelpad_cone = new G4Cons("gelpad_cone",
                                        0  * mm,               //inside radius at -pDz
                                        gelpad_small_radius,   //outside radius at -pDz
                                        0  * mm,               //inside radius at +pDz
                                        gelpad_large_radius,   //outside radius at +pDz
                                        gelpad_thickness / 2,  //half length in Z
                                        0,                     //starting angle of the segment in radians
                                        2*pi);                 //the angle of the segment in radians

    // Tube to model overflow of interface gel between gelpad and glass
    G4Tubs* gelpad_overflow_tubs = new G4Tubs("gelpad_overflow_tub",
                                             0,
                                             gelpad_overflow_max_radius,
                                             gelpad_overflow_height / 2,
                                             0,
                                             360 * degree);

    // gelpad cone and tubs
    G4VSolid* gelpad_cone_and_tubs = new G4UnionSolid("gelpad_cone_and_tubs",
                                                      gelpad_cone,
                                                      gelpad_overflow_tubs,
                                                      nullptr,
                                                      G4ThreeVector(0,0,gelpad_overflow_offset));

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
                                                gelpad_cone_and_tubs,                //solid 1
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
    G4double VacCylinderHeight = 10   * mm;
    G4double VacCylinderRadius = 38   * mm;
    G4double VacSphereCutoff   = sqrt( pow(VacSphereRadius,2) - pow(VacCylinderRadius,2));

    /*
                ..------..
             -°°          °°-    <---- SphereRadius
           /                  \ 
          |<------------------>|   -
          |   CylinderRadius   |   | CylinderHeight
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

    G4EllipticalTube* VacCylinder = new G4EllipticalTube("VacCylinder",
                                                        VacCylinderRadius,        // x semiaxis
                                                        VacCylinderRadius,        // y semiaxis
                                                        VacCylinderHeight / 2);   // height

    G4LogicalVolume* upperVacSphereLog = new G4LogicalVolume(upperVacSphere, vacuum, "upperVacSphere");
    G4LogicalVolume* lowerVacSphereLog = new G4LogicalVolume(lowerVacSphere, vacuum, "lowerVacSphere");
    G4LogicalVolume* VacCylinderLog    = new G4LogicalVolume(VacCylinder, vacuum, "VacCylinder");

    G4VisAttributes* vacuum_vis = new G4VisAttributes();
    vacuum_vis->SetColor(0.5, 0.5, 0.5, 0.4);    //transparent grey
    upperVacSphereLog->SetVisAttributes(vacuum_vis);
    lowerVacSphereLog->SetVisAttributes(vacuum_vis);
    VacCylinderLog->SetVisAttributes(vacuum_vis);

    //-----------
    // create photocathode solid and logical volume (absorbs the photon to detect it)
    //-----------

     /*
                ..------..
             -°°          °°-    <---- photocathode
           /                  \ 
          |--------------------|   -
          |                    |   | photocathode tube (length adjustable by macro)
          |                    |   -
     */

    G4double photocathode_thickness = 1 * mm;
    G4double photocathode_padding   = 0 * mm;
    G4double photocathode_tube_length = std::max(0.001 * mm, this->_photocathode_tube_length);

    G4Sphere* photocathode_subsolid = new G4Sphere("photocathode_subsolid",
                                                    VacSphereRadius - photocathode_thickness - photocathode_padding,
                                                    VacSphereRadius - photocathode_padding,
                                                    0,360*degree, 0, 180* degree);


    G4IntersectionSolid* photocathode_solid = new G4IntersectionSolid("photocathode",
                                                                      upperVacSphere,
                                                                      photocathode_subsolid);

    G4LogicalVolume* photocathodeLog = new G4LogicalVolume(photocathode_solid, photocathode, "photocathode");

    G4Tubs* photocathode_cylinder_subsolid = new G4Tubs("photocathode_cylinder",
                                                    VacCylinderRadius - photocathode_thickness - photocathode_padding,
                                                    VacCylinderRadius - photocathode_padding,
                                                    photocathode_tube_length/2,
                                                    0,360*degree);

    G4IntersectionSolid* photocathode_cylinder_solid = new G4IntersectionSolid("photocathode",
                                                                                photocathode_cylinder_subsolid,
                                                                                VacCylinder,
                                                                                nullptr,
                                                                                G4ThreeVector(0,0, (photocathode_tube_length - VacCylinderHeight) / 2));

    G4LogicalVolume* photocathodeTubeLog = new G4LogicalVolume(photocathode_cylinder_solid, photocathode, "photocathode_tube");

    G4VisAttributes* photocathode_vis = new G4VisAttributes();
    photocathode_vis->SetColor(0.9, 0.9, 0.1, 0.4); //golden-ish yellow
    photocathodeLog->SetVisAttributes(photocathode_vis);
    photocathodeTubeLog->SetVisAttributes(photocathode_vis);


    //-----------
    // create absorber solid and logical volume (simulate PMT-cascade structures, used to absorb photons)
    //-----------

    G4double tubRadius =  20 * mm;
    G4double tubHeight =  50 * mm;
    G4double tubOffset = -19 * mm;

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

    G4double VacCylinderOffset    = vacOffsetInPMT;
    G4double upperVacSphereOffset = VacCylinderOffset + VacCylinderHeight / 2 - VacSphereCutoff;
    G4double lowerVacSphereOffset = VacCylinderOffset - VacCylinderHeight / 2 + VacSphereCutoff;

    G4double photocathodeTubeOffset = (VacCylinderHeight - photocathode_tube_length) / 2;


    new G4PVPlacement(new G4RotationMatrix(),
                      G4ThreeVector(),
                      photocathodeLog,
                      "photocathode",
                      upperVacSphereLog,
                      false,
                      0);

    new G4PVPlacement(new G4RotationMatrix(),
                      G4ThreeVector(0,0,photocathodeTubeOffset),
                      photocathodeTubeLog,
                      "photocathodeTube",
                      VacCylinderLog,
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
                      G4ThreeVector(0,VacCylinderOffset,0),
                      VacCylinderLog,
                      "VacCylinder",
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
    this->_ou_positions.push_back(ou_pos_in_global_coord);
    this->_nr_of_OUs++;
    
}