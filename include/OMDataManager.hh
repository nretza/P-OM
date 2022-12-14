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

        // inline
        void setFilename(G4String val);

        G4String getFilename(){return this->_filename;};
        G4bool getFileIsOpen(){return this->_file_is_open;};

        void firstStepHandover(G4ThreeVector pos)
        {this->_current_first_step_pos = pos;}

        void preTrackHandover(G4int pid, G4double E, G4ThreeVector position, G4ThreeVector momentum)
        {this->_current_pid = pid;
         this->_current_in_energy = E;
         this->_current_in_position = position;
         this->_current_in_momentum = momentum;};

        void postTrackHandover(G4ThreeVector position, G4String volume_name, G4String process_name)
        {this->_current_out_position = position;
         this->_current_out_volume_name = volume_name;
         this->_current_out_process_name = process_name;};

        void write(){this->_File << this->_current_pid               << ", "
                                 << this->_current_in_energy         << ", "
                                 << this->_current_in_position[0]    << ", "
                                 << this->_current_in_position[1]    << ", "
                                 << this->_current_in_position[2]    << ", "
                                 << this->_current_in_momentum[0]    << ", "
                                 << this->_current_in_momentum[1]    << ", "
                                 << this->_current_in_momentum[2]    << ", "
                                 << this->_current_first_step_pos[0] << ", "
                                 << this->_current_first_step_pos[1] << ", "
                                 << this->_current_first_step_pos[2] << ", "
                                 << this->_current_out_position[0]   << ", "
                                 << this->_current_out_position[1]   << ", "
                                 << this->_current_out_position[2]   << ", "
                                 << this->_current_out_volume_name   << ", "
                                 << this->_current_out_process_name  << std::endl;};



    private:

        OMDataManager();
        static OMDataManager* _instance;
        OMDataManagerMessenger* _DataManagerMessenger;

        std::ofstream _File;

        G4String      _filename;
        G4bool        _file_is_open;

        G4int         _current_pid;
        G4double      _current_in_energy;
        G4ThreeVector _current_in_position;
        G4ThreeVector _current_in_momentum;

        G4ThreeVector _current_first_step_pos;

        G4ThreeVector _current_out_position;
        G4String      _current_out_volume_name;
        G4String      _current_out_process_name;


};
#endif
