// system includes
#include <filesystem>

// G4 includes
#include "G4Exception.hh"

// project includes
#include "OMDataManager.hh"


OMDataManager* OMDataManager::_instance = nullptr;

OMDataManager* OMDataManager::getInstance()
{
    if( _instance == nullptr )
    {
        _instance = new OMDataManager();
    }
    return _instance;
}

OMDataManager::OMDataManager()
:_File(0),
 _filename("/dev/null"),
 _file_is_open(false),
 _current_in_position(0),
 _current_in_momentum(0),
 _current_out_position(0),
 _current_out_volume_name(""),
 _current_out_process_name("")
{
    this->_DataManagerMessenger = new OMDataManagerMessenger(this);
}

void OMDataManager::setFilename(G4String val)
{
    if (this->_file_is_open)
    {
        G4Exception("OMDataManager::setFilename()",
                    "file is already open, cant set filename!",
                    RunMustBeAborted,
                    "the DataManager already has an open output file. You can not set an output file now!");
        return;
    }
   
    if (std::filesystem::exists(std::string(val)) && val !="/dev/null") //std::filesystem::exists only from C++17 onwards
    {
        G4Exception("OMDataManager::setFilename()",
                    "file already exists",
                    JustWarning,
                    "the DataManager detected that the output file already exists, file contents will be overwritten!");
    }
    this->_filename = val;
}

void OMDataManager::open()
{
    if (this->_file_is_open)
    {
        G4Exception("OMDataManager::open()",
                    "file is already open",
                    RunMustBeAborted,
                    "the DataManager already has an open output file, you cant open an output file now!");
        return;
    }

    if (std::filesystem::exists(std::string(this->_filename)) && this->_filename !="/dev/null") //std::filesystem::exists only from C++17 onwards
    {
        std::remove(this->_filename);
    }

    this->_file_is_open = true;
    this->_File = std::ofstream(this->_filename);
    this->_File << "PID, in_E, in_x, in_y, in_z, in_px, in_py, in_pz, 1_x, 1_y, 1_z, out_x, out_y, out_z, out_VolumeName, out_ProcessName" << std::endl;
}

void OMDataManager::close()
{
    this->_File.close();
    this->_file_is_open = false;
}