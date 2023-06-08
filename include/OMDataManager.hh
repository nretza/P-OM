#ifndef OM_DATA_MANAGER_H
#define OM_DATA_MANAGER_H 1

// system includes
#include <fstream>

// G4 Includes
#include "G4ThreeVector.hh"

//ROOT includes
//#include "TTree.h" // TODO use this later

// Project includes
#include "OMDataManagerMessenger.hh"

// forward declarations
class OMDataManagerMessenger;


class OMDataManager
{
    public:

        static OMDataManager* getInstance();
        ~OMDataManager();

        void open();
        void close();
        void setFilename(G4String val);

        // inline from here on

        G4String getFilename(){return this->_filename;};
        G4bool getFileIsOpen(){return this->_file_is_open;};

        void   setDataOutProcessFilter(G4String val){this->_filter_outProcess = val;};
        G4String getDataOutProcessFilter(){return this->_filter_outProcess;};

        void   setDataOutVolumeFilter(G4String val){this->_filter_outVolume = val;};
        G4String getDataOutVolumeFilter(){return this->_filter_outVolume;};

        void   setDataGlassFilter(G4bool val){this->_filter_data_glass = val;};
        G4bool setDataGlassFilter(){return this->_filter_data_glass;};

        void   setPhotonsFilter(G4bool val){this->_filter_photons = val;};
        G4bool setPhotonsFilter(){return this->_filter_photons;};

        G4bool doFiltersApply()
        {if (this->_filter_outProcess.find(this->_current_out_process_name) != std::string::npos) return true;
         if (this->_filter_outVolume.find(this->_current_out_volume_name)   != std::string::npos) return true;
         if (this->_filter_data_glass && !this->_current_glass_contact) return true;
         if (this->_filter_photons    &&  this->_current_pid != -22   ) return true;
         else return false;};

        void preTrackHandover(G4int pid,G4double time, G4ThreeVector position,G4double energy, G4ThreeVector momentum)
        {this->_current_pid         = pid;
         this->_current_in_time     = time;
         this->_current_in_position = position;
         this->_current_in_energy   = energy; 
         this->_current_in_momentum = momentum;};

        void glassContactHandover(G4ThreeVector pos, G4ThreeVector dir)
        {if (this->_current_glass_contact) return;
         this->_current_glass_contact_pos = pos;
         this->_current_glass_contact_dir = dir;
         this->_current_glass_contact     = true;}

        void postTrackHandover(G4double time, G4ThreeVector position, G4double energy, G4ThreeVector momentum, G4String volume_name, G4int volume_copyno, G4String process_name)
        {this->_current_out_time          = time;
         this->_current_out_position      = position;
         this->_current_out_energy        = energy; 
         this->_current_out_momentum      = momentum;
         this->_current_out_volume_name   = volume_name;
         this->_current_out_volume_copyno = volume_copyno;
         this->_current_out_process_name  = process_name;};

        void write()
        {if (this->doFiltersApply()) return;
         this->_File << this->_current_pid                   << ","
                     << this->_current_in_time               << ","
                     << this->_current_in_position[0]        << ","
                     << this->_current_in_position[1]        << ","
                     << this->_current_in_position[2]        << ","
                     << this->_current_in_energy             << ","
                     << this->_current_in_momentum[0]        << ","
                     << this->_current_in_momentum[1]        << ","
                     << this->_current_in_momentum[2]        << ","
                     << this->_current_glass_contact_pos[0]  << ","
                     << this->_current_glass_contact_pos[1]  << ","
                     << this->_current_glass_contact_pos[2]  << ","
                     << this->_current_glass_contact_dir[0]  << ","
                     << this->_current_glass_contact_dir[1]  << ","
                     << this->_current_glass_contact_dir[2]  << ","
                     << this->_current_out_time              << ","
                     << this->_current_out_position[0]       << ","
                     << this->_current_out_position[1]       << ","
                     << this->_current_out_position[2]       << ","
                     << this->_current_out_energy            << ","
                     << this->_current_out_momentum[0]       << ","
                     << this->_current_out_momentum[1]       << ","
                     << this->_current_out_momentum[2]       << ","
                     << this->_current_out_volume_name       << ","
                     << this->_current_out_volume_copyno     << ","
                     << this->_current_out_process_name      << std::endl;};

        void reset(){this->_current_pid                 = 0;
                     this->_current_in_time             = 0;
                     this->_current_in_position         = G4ThreeVector(0,0,0);
                     this->_current_in_energy           = 0;
                     this->_current_in_momentum         = G4ThreeVector(0,0,0);
                     this->_current_glass_contact       = false;
                     this->_current_glass_contact_pos   = G4ThreeVector(0,0,0);
                     this->_current_glass_contact_dir   = G4ThreeVector(0,0,0);
                     this->_current_out_time             = 0;
                     this->_current_out_position         = G4ThreeVector(0,0,0);
                     this->_current_out_energy           = 0;
                     this->_current_out_momentum         = G4ThreeVector(0,0,0);
                     this->_current_out_volume_name     = "";
                     this->_current_out_volume_copyno   = 0 ;
                     this->_current_out_process_name    = "";}


    private:

        OMDataManager();
        static OMDataManager*   _instance;
        OMDataManagerMessenger* _DataManagerMessenger;

        std::ofstream _File;

        G4String      _filename;
        G4bool        _file_is_open;

        G4String      _filter_outProcess;
        G4String      _filter_outVolume;
        G4bool        _filter_data_glass;
        G4bool        _filter_photons;

        G4int         _current_pid;

        G4double      _current_in_time;
        G4ThreeVector _current_in_position;
        G4double      _current_in_energy;
        G4ThreeVector _current_in_momentum;

        G4bool        _current_glass_contact;
        G4ThreeVector _current_glass_contact_pos;
        G4ThreeVector _current_glass_contact_dir;

        G4double      _current_out_time;
        G4ThreeVector _current_out_position;
        G4double      _current_out_energy;
        G4ThreeVector _current_out_momentum;

        G4String      _current_out_volume_name;
        G4int         _current_out_volume_copyno;
        G4String      _current_out_process_name;


};
#endif
