#include "optionsman.h"
#include "common.h"
#include <fstream>

using namespace std;

namespace visualizer
{

  _OptionsMan *OptionsMan = 0;

  void _OptionsMan::setup()
  {
    if( !OptionsMan )
    {
      OptionsMan = new _OptionsMan;
    }
    else
    {
      THROW( Exception, "OptionsManager Already Initialized" );
    }

  }

  void _OptionsMan::destroy()
  {
    delete OptionsMan;
    OptionsMan = 0;
  }

  void _OptionsMan::loadOptionFile( const std::string& filename, const string& domain )
  {
    m_domains.push_back( pair<string, string>( filename, domain ) );
    ifstream in( filename.c_str(), ifstream::binary );

    if( !in.is_open() )
    {
      return;
    }

    while( !in.eof() )
    {
      Option o;
      in >> o;
      o.domain = domain;
      m_options[ o.key ] = o;

    }
    

    in.close();

  }

  void _OptionsMan::saveOptions()
  {
    map< string, ofstream* > files;
    for( vector< pair< string, string > >::iterator i = m_domains.begin(); i != m_domains.end(); i++ )
    {
      ofstream out( i->first.c_str(), ofstream::binary | ofstream::trunc );
      files[ i->second ] = &out;
    }

    for( map< string, Option >::iterator i = m_options.begin(); i != m_options.end(); i++ )
    {
      ofstream& out = *files[ i->second.domain ];
      out << i->second;

    }

  }

  istream& operator >> ( istream& os, Option& rhs )
  {

    os.read( (char*)&rhs.type, sizeof( unsigned char ) );
    unsigned int len;
    unsigned int ops;
    char *str;

    os.read( (char*)&len, sizeof( unsigned int ) );
    str = new char[len+1];
    os.read( (char*)str, len );
    str[len] = 0;
    rhs.key = str;
    delete str;

    switch( rhs.type )
    {
      case OP_INT:
      case OP_FLOAT:
        os.read( (char*)&rhs.fMinRange, sizeof( float ) );
        os.read( (char*)&rhs.fMaxRange, sizeof( float ) );
        os.read( (char*)&rhs.fValue, sizeof( float ) );
      break;
      case OP_STRING:
        os.read( (char*)&len, sizeof( unsigned int ) );
        str = new char[len+1];
        os.read( (char*)str, len );
        str[len] = 0;
        rhs.sValue = str;
        delete str;
      case OP_COMBO:
        os.read( (char*)&ops, sizeof( unsigned int ) );
        for( size_t i = 0; i < ops; i++ )
        {
          os.read( (char*)&len, sizeof( unsigned int ) );
          str = new char[len+1];
          os.read( (char*)str, len );
          str[len] = 0;
          rhs.sOptions.push_back( str );
          delete str;
        }
      break;
      default:
        THROW( Exception, "Error, Unable to read option type, %d.", rhs.type );

    }
   
    return os;
  } // _OptionsMan::loadOptionFile()

  void _OptionsMan::addOption( const Option& op )
  {
    m_options[ op.key ] = op;

  }

  string& _OptionsMan::getString( const string& key )
  {
    map< string, Option >::iterator f = m_options.find( key );
    if( f == m_options.end() )
    {
      THROW( Exception, "Could not find key" );
    }
    else
    {
      if( f->second.type != OP_STRING && f->second.type != OP_COMBO )
      {
        THROW( Exception, "Option not of type string" );
      }
      else
      {
        return f->second.sValue;
      }
    }

  }

  const string& _OptionsMan::getString( const string& key ) const
  {
    map< string, Option >::const_iterator f = m_options.find( key );
    if( f == m_options.end() )
    {
      THROW( Exception, "Could not find key" );
    }
    else
    {
      if( f->second.type != OP_STRING && f->second.type != OP_COMBO )
      {
        THROW( Exception, "Option not of type string" );
      }
      else
      {
        return f->second.sValue;
      }
    }

  }

  float& _OptionsMan::getNumber( const string& key ) 
  {
    map< string, Option >::iterator f = m_options.find( key );
    if( f == m_options.end() )
    {
      THROW( Exception, "Could not find key %s.", key.c_str() );
    }
    else
    {
      if( f->second.type != OP_INT && f->second.type != OP_FLOAT )
      {
        THROW( Exception, "Option not of type number." );
      }
      else
      {
        return f->second.fValue;
      }
    }

  }

  const float& _OptionsMan::getNumber( const string& key ) const
  {
    map< string, Option >::const_iterator f = m_options.find( key );
    if( f == m_options.end() )
    {
      THROW( Exception, "Could not find key" );
    }
    else
    {
      if( f->second.type != OP_INT && f->second.type != OP_FLOAT )
      {
        THROW( Exception, "Option not of type number." );
      }
      else
      {
        return f->second.fValue;
      }
    }

  }

  ostream& operator << ( ostream& os, const Option& rhs )
  {

    os << (unsigned char)rhs.type;

    os << (unsigned int)rhs.key.size();
    os << rhs.key;
    
    switch( rhs.type )
    {
      case OP_INT:
      case OP_FLOAT:
        os << (float)rhs.fMinRange << (float)rhs.fMaxRange << (float)rhs.fValue;
      break;
      case OP_STRING:
        os << (unsigned int)rhs.sValue.size() << rhs.sValue;
      case OP_COMBO:
        os << (unsigned int)rhs.sOptions.size();
        for( vector<string>::const_iterator i = rhs.sOptions.begin(); i != rhs.sOptions.end(); i++ )
        {
          os << (unsigned int)i->size() << *i;
        }
      break;
      default:
        THROW( Exception, "Error, Option contains strange type, %d", rhs.type );
    }

    return os;
  } // operator <<


} // visualizer
