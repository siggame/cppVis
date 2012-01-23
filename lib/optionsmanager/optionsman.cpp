#include "optionsman.h"
#include "common.h"
#include <fstream>
#include <QDomDocument>
#include <QFile>

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

    QDomDocument doc( "OptionsML" );
    QFile file( filename.c_str() );

    if( !file.open( QIODevice::ReadOnly ) )
      THROW( Exception, "%s options file could not be found.", filename.c_str() );

    if( !doc.setContent( &file ) )
    {
      file.close();
      THROW( Exception, "%s was unable to parse the XML", filename.c_str() );
    }

    file.close();

    QDomElement root = doc.documentElement();

    if( root.tagName() != "options" )
      THROW( Exception, "%s did not have an options root", filename.c_str() );

    QDomNode n = root.firstChild();
    while( !n.isNull() )
    {
      QDomElement e = n.toElement();
      if( !e.isNull() )
      {
        cout << qPrintable( e.tagName() ) << endl;
        
      }

      n = n.nextSibling();

    }
    

#if 0
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
#endif

  }

  void _OptionsMan::saveOptions()
  {
    map< string, ofstream* > files;
    for( vector< pair< string, string > >::iterator i = m_domains.begin(); i != m_domains.end(); i++ )
    {
      ofstream *out = new ofstream( i->first.c_str(), ofstream::binary | ofstream::trunc );
      files[ i->second ] = out;
    }

    for( map< string, Option >::iterator i = m_options.begin(); i != m_options.end(); i++ )
    {
      ofstream& out = *files[ i->second.domain ];
      out << i->second;

    }

    for( map< string, ofstream* >::iterator i = files.begin(); i != files.end(); i++ )
    {
      delete i->second;
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
      THROW( Exception, "Could not find key %s", key.c_str() );
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
      THROW( Exception, "Could not find key %s", key.c_str() );
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
      THROW( Exception, "Could not find key %s", key.c_str() );
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

    os.write( (const char*)&rhs.type, sizeof( unsigned char ) );

    unsigned int sz = rhs.key.size();
    os.write( (const char*)&sz, sizeof( unsigned int ) );
    os << rhs.key;
    
    switch( rhs.type )
    {
      case OP_INT:
      case OP_FLOAT:
        os.write( (const char*)&rhs.fMinRange, sizeof( float ) );
        os.write( (const char*)&rhs.fMaxRange, sizeof( float ) );
        os.write( (const char*)&rhs.fValue, sizeof( float ) );
        //os << (float)rhs.fMinRange << (float)rhs.fMaxRange << (float)rhs.fValue;
      break;
      case OP_STRING:
        sz = rhs.sValue.size();
        os.write( (const char*)&sz, sizeof( unsigned int ) );
        os << rhs.sValue;
      case OP_COMBO:
        sz = (unsigned int)rhs.sOptions.size();
        os.write( (const char*)&sz, sizeof( unsigned int ) );
        for( vector<string>::const_iterator i = rhs.sOptions.begin(); i != rhs.sOptions.end(); i++ )
        {
          sz = i->size();
          os.write( (const char*)&sz, sizeof( unsigned int ) );
          os << *i;
        }
      break;
      default:
        THROW( Exception, "Error, Option contains strange type, %d", rhs.type );
    }

    return os;
  } // operator <<


} // visualizer
