#ifndef OMDATAREADER_HH
#define OMDATAREADER_HH 1

// system includes
#include <string>
#include <vector>
#include <map>

//G4 includes
#include "G4Types.hh"
#include "G4String.hh"

// project includes


class OMDataReader
{
    public:

        OMDataReader(const char* PATH);
        ~OMDataReader();

        void PrintAll();

        bool HasScalar(G4String name);
        bool HasArray(G4String name);
        bool HasString(G4String name);

        std::vector<G4String> GetScalarNames();
        std::vector<G4String> GetArrayNames();

        G4String GetString(G4String name);
        G4double GetScalar(G4String name, G4double multiplicate=1.);
        G4double* GetArray(G4String name, unsigned supposedLength, G4double multiplicate=1.);
        G4long* GetArrayLong(G4String name, unsigned supposedLength);
        G4int GetArrayLength(G4String name);
        G4double* ArrayFromScalar(G4String name, G4int length, G4double multiplicate=1.);

        void SetScalar(G4String name, G4double value);
        void SetArray(G4String name, std::vector<G4double> value);

    private:

        std::vector<G4double> GetArrayVector(G4String name);

        const char* _path;

        std::map<G4String,G4double>              _scalardata;
        std::map<G4String,std::vector<G4double>> _vectordata;
        std::map<G4String,G4String>           _stringdata;
  
        const char* _error_message;
        const char* _warning_message;
        G4double    _returnOnError;
};
#endif