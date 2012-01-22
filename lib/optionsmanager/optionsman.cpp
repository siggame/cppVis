#include "optionsman.h"
#include "common.h"

namespace visualizer
{

  _OptionsMan *OptionsMan = 0;

  void _OptionsMan::setup()
  {
  }

  void _OptionsMan::destroy()
  {
  }

  void _OptionsMan::loadOptionFile( const std::string& filename, const string& domain )
  {
    THROW( Exception, "No Options File Found!" );
  }

  istream& operator >> ( istream& os, Option& rhs )
  {

    os.read( (char*)&rhs.type, sizeof( unsigned char ) );
    unsigned int len;
    unsigned int ops;
    char *str;
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
