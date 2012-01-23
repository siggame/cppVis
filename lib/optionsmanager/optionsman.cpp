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
        if( e.tagName() == "option" )
        {
          Option o;
          o.domain = domain;

          QDomNode s = e.firstChild();
          while( !s.isNull() )
          {
            if( s.toElement().tagName() == "key" )
            {
              o.key = qPrintable( s.toElement().text() );
            }
            else if( s.toElement().tagName() == "type" )
            {
              if( s.toElement().text() == "Integer" )
              {
                o.type = OP_INT;
              }
              else if( s.toElement().text() == "Float" )
              {
                o.type = OP_FLOAT;
              }
              else if( s.toElement().text() == "String" )
              {
                o.type = OP_STRING;
              }
              else
              {
                o.type = OP_COMBO;
              }
            } 
            else if( s.toElement().tagName() == "value" )
            {
              if( o.type == OP_INT || o.type == OP_FLOAT )
              {
                o.fValue = s.toElement().text().toFloat();
              }
              else
              {
                o.sValue = qPrintable( s.toElement().text() );
              }
            } 
            else if( s.toElement().tagName() == "minvalue" )
            {
              o.fMinRange = s.toElement().text().toFloat();
            }
            else if( s.toElement().tagName() == "maxvalue" )
            {
              o.fMaxRange = s.toElement().text().toFloat();
            } 
            else if( s.toElement().tagName() == "combos" )
            {
              QDomNode combos = s.firstChild();
              while( !combos.isNull() )
              {
                o.sOptions.push_back( qPrintable( combos.toElement().text() ) );
                combos = combos.nextSibling();
              }

            }
            
            s = s.nextSibling();
          }

          m_options[ o.key ] = o;

        }
        else
        {
          THROW( Exception, "Expecting an option.  Did not receive it." );
        }
        
      }

      n = n.nextSibling();

    }
   
  }

  void _OptionsMan::saveOptions()
  {
    map< string, ofstream* > files;
    for( vector< pair< string, string > >::iterator i = m_domains.begin(); i != m_domains.end(); i++ )
    {
      ofstream *out = new ofstream( i->first.c_str(), ofstream::binary | ofstream::trunc );
      files[ i->second ] = out;

      (*out) << "<options>" << endl;
    }

    for( map< string, Option >::iterator i = m_options.begin(); i != m_options.end(); i++ )
    {
      ofstream& out = *files[ i->second.domain ];
      out << "<option>" << endl;

      out << "<key>" << i->second.key << "</key>" << endl;
      out << "<type>"; 
      switch( i->second.type )
      {
        case OP_INT:
          out << "Integer";
          break;
        case OP_FLOAT:
          out << "Float";
          break;
        case OP_STRING:
          out << "String";
          break;
        case OP_COMBO:
          out << "Combo";
          break;
      }
      out << "</type>" << endl;

      switch( i->second.type )
      {
        case OP_INT:
        case OP_FLOAT:
          out << "<value>" << i->second.fValue << "</value>" << endl;
          if( i->second.fMinRange != -999999 )
          {
            out << "<minvalue>" << i->second.fMinRange << "</minvalue>" << endl;
          }
          if( i->second.fMaxRange != 999999 )
          {
            out << "<maxvalue>" << i->second.fMaxRange << "</maxvalue>" << endl;
          }

          break;
        case OP_STRING:
          out << "<value>" << i->second.sValue << "</value>" << endl;
        case OP_COMBO:
          out << "<combos>" << endl;
          for
            ( 
              vector<string>::iterator j = i->second.sOptions.begin();
              j != i->second.sOptions.end();
              j++
            )
          {
            out << "<op>" << *j << "</op>" << endl;
          }
          out << "</combos>" << endl;
          break;
      }

      out << "</option>" << endl;
    }

    for( map< string, ofstream* >::iterator i = files.begin(); i != files.end(); i++ )
    {
      (*i->second) << "</options>" << endl;
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
