// system includes
#include <fstream>
#include <cmath> //only NAN

// boost includes
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

// G4 includes 
#include "G4ios.hh"

// project includes
#include "OMDataReader.hh"

OMDataReader::OMDataReader(const char* PATH):
_path(PATH),
_error_message("Error(OMDataReader):"),
_warning_message("Warning(OMDataReader):"),
_returnOnError(NAN)
{
    std::ifstream in(_path);
    G4String line;
    int linenumber = 0;

    // read line by line
    while(getline( in, line ))
    {
        linenumber++;

        // remove \r
        std::string str(line);
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.cend());
        
        //Ignore comments
        size_t p = line.find("#");
        if (p!=G4String::npos) line=line.substr(0,p);

        //Split line into vector
        std::vector<G4String> strs;
        boost::split(strs, line, boost::is_any_of("\t "));
        
        //Remove empty elements from vector
        std::vector<G4String>::iterator start = remove_if(strs.begin(),strs.end(),std::bind1st(std::equal_to<G4String>(),G4String("")));
        strs.erase( start, strs.end() );
        
        // ignore empty lines
        if (strs.size()==0) continue;
    
        //ARRAY
        if (strs[0]=="ARRAY")
        {
            // array size check
            if (strs.size()<=2)
            {
                G4cerr<<_error_message<<_path<<":"<<linenumber<<": "<<line<<" array is empty"<<G4endl;
                continue;
            }

            G4String name=strs[1];

            // double definition check
            if (this->HasArray(name)) G4cerr<<_warning_message<<_path<<": "<<linenumber<<":"<<name<<" is defined twice."<<G4endl;

            // conert value from str to G4double
            try
            {
                std::vector<G4double> values;
                for (unsigned i=2; i<strs.size(); i++) values.push_back(boost::lexical_cast<double>(strs[i]));
                this->_vectordata[name]=values;
            }
            catch(const boost::bad_lexical_cast &)
            {
                G4cerr<<_error_message<<_path<<":"<<linenumber<<": "<<line<<" element can not be converted to double"<<G4endl;
                continue;
            }
        }

        // STR
        else if (strs[0]=="STR")
        {
            // size check
            if (strs.size()!=3)
            {
                G4cerr<<_error_message<<_path<<":"<<linenumber<<": "<<line<<" string is empty or too many"<<G4endl;
                continue;
            }
            G4String name=strs[1];

            // double definition check
            if (this->HasString(name)) G4cerr<<_warning_message<<_path<<": "<<linenumber<<":"<<name<<" is defined twice."<<G4endl;

            try
            {
                this->_stringdata[name]=strs[2];
            }
            catch(const boost::bad_lexical_cast &)
            {
                G4cerr<<_error_message<<_path<<":"<<linenumber<<": "<<strs[2]<<" can not be converted to string"<<G4endl;
                continue;
            }
        }

        //Scalar
        else
        {
            // size check
            if (strs.size()!=2)
            {
                G4cerr<<_error_message<<_path<<":"<<linenumber<<": "<<line<<" has no value"<<G4endl;
                continue;
            }

            G4String name=strs[0];

            // double definition check
            if (this->HasScalar(name)) G4cerr<<_warning_message<<_path<<": "<<linenumber<<":"<<name<<" is defined twice."<<G4endl;

            //Convert value from string to G4double
            try
            {
                G4double value=boost::lexical_cast<double>(strs[1]);
                this->_scalardata[name]=value;
            }
            catch(const boost::bad_lexical_cast &)
            {
                G4cerr<<_error_message<<_path<<": "<<linenumber<<":"<<strs[1]<<" can not be converted to double"<<G4endl;
            }

        }
    }
    in.close();
}

OMDataReader::~OMDataReader()
{
    // TODO
}

void OMDataReader::PrintAll()
{
    G4cout<<"Loaded from file "<<_path<<G4endl;
    G4cout<<" SCALARS"<<G4endl;
    for (std::map<G4String,G4double>::iterator it=this->_scalardata.begin(); it!=this->_scalardata.end(); it++)
    {
        G4cout<<"  "<<it->first<<" :\t"<< it->second<<G4endl;
    }

    G4cout<<" ARRAYS"<<G4endl;
    for (std::map<G4String,std::vector<G4double> >::iterator it=this->_vectordata.begin(); it!=this->_vectordata.end(); it++)
    {
        std::vector<G4double> v=it->second;
        G4cout<<"  "<<it->first<<"["<<v.size()<<"] :\t";
        for (unsigned i=0; i<v.size(); i++)
        {
            G4cout<<v[i]<<"\t";
        }
        G4cout<<G4endl;
    }
}

bool OMDataReader::HasScalar(G4String name)
{
    return this->_scalardata.count(name)!=0;
}

bool OMDataReader::HasString(G4String name)
{
  return this->_stringdata.count(name)!=0;
}

bool OMDataReader::HasArray(G4String name)
{
    return this->_vectordata.count(name)!=0;
}

std::vector<G4String> OMDataReader::GetScalarNames()
{
    std::vector<G4String> names;
    for (std::map<G4String,G4double>::iterator it=this->_scalardata.begin(); it!=this->_scalardata.end(); it++)
    {
        names.push_back(it->first);
    }
    return names;
}

std::vector<G4String> OMDataReader::GetArrayNames()
{
    std::vector<G4String> names;
    for (std::map<G4String,std::vector<G4double> >::iterator it=this->_vectordata.begin(); it!=this->_vectordata.end(); it++)
    {
        names.push_back(it->first);
    }
    return names;
}

G4double OMDataReader::GetScalar(G4String name, G4double multiplicate)
{
    if (this->HasScalar(name))
    {
        return this->_scalardata[name]*multiplicate;
    }
    else
    {
        G4cerr<<_error_message<<"Scalar "<<name<<" was not found in "<<_path<<G4endl;
        return this->_returnOnError;
    }
}

G4String OMDataReader::GetString(G4String name)
{
  if (this->HasString(name))
    {
        return this->_stringdata[name];
    }
    else
    {
    G4cerr<<_error_message<<"String "<<name<<" was not found in "<<_path<<G4endl;
    return "errpor";//returnOnError;
    }
}

std::vector<G4double> OMDataReader::GetArrayVector(G4String name)
{
    if (HasArray(name))
    {
        return this->_vectordata[name];
    }
    else
    {
        G4cerr<<_error_message<<"Array "<<name<<" was not found in "<<_path<<G4endl;
        return std::vector<G4double>{};
    }
}

G4double* OMDataReader::ArrayFromScalar(G4String name, G4int length, G4double multiplicate)
{
    G4double s=GetScalar(name);
    G4double* a=new G4double[length];
    for (G4int i=0; i<length; i++)
    {
        a[i]=s*multiplicate;
    }
    return a;
}

G4double* OMDataReader::GetArray(G4String name, unsigned supposedLength, G4double multiplicate)
{
    std::vector<G4double> v = GetArrayVector(name);
    if (v.size()!=supposedLength)
    {
        G4cerr<<_error_message<<name<<"["<<v.size()<<"] is not of length "<<supposedLength<<G4endl;
    }
    G4double* a=new G4double[v.size()];
    for (unsigned i=0; i<v.size(); i++)
    {
        a[i]=v[i]*multiplicate;
    }
    return a;
}

G4long* OMDataReader::GetArrayLong(G4String name, unsigned supposedLength)
{
  std::vector<G4double> v = GetArrayVector(name);
  if (v.size()!=supposedLength)
  {
    G4cerr<<_error_message<<name<<"["<<v.size()<<"] is not of length "<<supposedLength<<G4endl;
  }
  G4long* a=new G4long[v.size()];
  for (unsigned i=0; i<v.size(); i++)
  {
    a[i]=(long) v[i];
  }
  return a;
}

G4int OMDataReader::GetArrayLength(G4String name)
{
    return GetArrayVector(name).size();
}

void OMDataReader::SetScalar(G4String name, G4double value)
{
    this->_scalardata[name]=value;
}
void OMDataReader::SetArray(G4String name, std::vector<G4double> value)
{
    this->_vectordata[name]=value;
}