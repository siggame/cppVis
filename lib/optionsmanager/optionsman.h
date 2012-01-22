#ifndef OPTIONSMAN_H
#define OPTIONSMAN_H

#include <iostream>
#include <string>
#include <map>
#include "interfaces/ioptionsman.h"

using namespace std;

namespace visualizer
{
  enum OpType
  {
    OP_INT,
    OP_FLOAT,
    OP_STRING, 
    OP_COMBO
  };

  struct Option
  {
    Option()
    {
    }

    Option( const string& rKey, const string& rDomain, const string& rValue )
    {
      key = rKey;
      type = OP_STRING;
      sValue = rValue;
      domain = rDomain;

    }

    Option( const string& rKey, const string& rDomain, const float& rValue )
    {
      key = rKey;
      type = OP_FLOAT;
      fValue = rValue;
      domain = rDomain;

    }

    float fMinRange;
    float fMaxRange;
    float fValue;

    string key;
    string sValue;
    vector<string> sOptions;

    OpType type;

    string domain;

  };


  class _OptionsMan : public IOptionsMan
  {
    Q_INTERFACES( IOptionsMan );
    private:
      map< string, Option > m_options;
      vector< pair<string, string> > m_domains;
      friend class OptionsDialog;
    public: 
      void addOption( const Option& op );

      void setup();
      void destroy();

      void loadOptionFile( const string& filename, const string& domain );
      void saveOptions();

      string& getString( const string& key );
      const string& getString( const string& key ) const;

      float& getNumber( const string& key );
      const float& getNumber( const string& key ) const;

  }; // _OptionsMan

  ostream& operator << ( ostream& os, const Option& rhs );
  istream& operator >> ( istream& os, Option& rhs );

  extern _OptionsMan *OptionsMan;

} // visualizer


#endif // OPTIONSMAN_H
