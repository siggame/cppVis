#ifndef IOPTIONSMAN_H
#define IOPTIONSMAN_H

#include <QtPlugin>
#include <vector>
#include <string>

using std::string;

namespace visualizer
{
  class IOptionsMan
  {
    public:
      virtual void loadOptionFile( const string& filename, const string& domain ) = 0;

      virtual string& getString( const string& key ) = 0;
      virtual const string& getString( const string& key ) const = 0;

      virtual float& getNumber( const string& key ) = 0;
      virtual const float& getNumber( const string& key ) const = 0;

  }; // IOptionsMan

} // visualizer

Q_DECLARE_INTERFACE( visualizer::IOptionsMan, "siggame.vis2.optionsman/0.2" );

#endif
