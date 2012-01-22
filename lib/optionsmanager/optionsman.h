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
    float fMinRange;
    float fMaxRange;
    float fValue;

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
      void addOption( const Option& op );
    public: 
      void setup();
      void destroy();

      void loadOptionFile( const string& filename, const string& domain );

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
