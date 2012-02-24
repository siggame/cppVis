#ifndef IRESOURCEMAN_H
#define IRESOURCEMAN_H

#include <set>
#include <vector>
#include <string>
#include "typedefs.h"
//#include "resource.h"
#include <QtPlugin>

namespace visualizer
{

  class Resource
  {
    public:
      /** Default constructor */
      Resource(ResourceType type)
      {
        m_type = type;
      }

      /** Default destructor */
      ~Resource(){}

      bool unload()
      {
        return true;
      }

      ResourceType type()
      {
        return m_type;
      }

      inline bool isReferencedBy(const std::string & referencer)
      {
        if (m_references.find(referencer) != m_references.end())
        {
          return true;
        }
        return false;
      }

      inline std::set<std::string> referenceList()
      {
        return m_references;
      }

      inline unsigned int numReferences()
      {
        return m_references.size();
      }

#ifdef DEBUG
      void printReferences()
      {
        MESSAGE( "References:" );
          for (std::set<std::string>::iterator it = m_references.begin();
              it != m_references.end(); it++)
          {
            MESSAGE( "%s", it->c_str() );
          }
      }
#endif

      inline bool reference(const std::string & referencer)
      {
        if (!isReferencedBy(referencer))
        {
          m_references.insert(referencer);
          return true;
        }

#ifdef DEBUG
        MESSAGE( "Referencer: \"%s\" already exists", reference.c_str() );
#endif
        return false;
      }

      inline bool deReference(const std::string & referencer)
      {
        if (isReferencedBy(referencer))
        {
          m_references.erase(referencer);
          return true;
        }

#ifdef DEBUG
        MESSAGE( "Referencer: \"%s\" doesn't exist.", reference.c_str() );
#endif
        return false;
      }

    protected:
      ResourceType m_type;
      std::set<std::string> m_references;
      std::string filename;
    private:
  };

  class IResourceMan
  {
    public:
      virtual Resource* reference( const std::string& rName, const std::string& referencer ) = 0;
      virtual bool release( const std::string& rName, const std::string& references ) = 0;

      virtual bool regFile( const ResID_t& rName, const std::string& filename ) = 0;
      virtual bool del( const ResID_t& rName ) = 0;

      virtual bool loadResourceFile( const std::string& filename ) = 0;
      virtual bool saveResourceFile( const std::string& filename ) = 0;

      virtual bool exists( const ResID_t& rName ) = 0;
      virtual std::vector<std::string> listResourceNames() = 0;

      virtual void loadTexture
        ( 
         const std::string& filename, 
         const std::string& name 
        )= 0;

      virtual void loadTexture
        ( 
         QImage& image, 
         const std::string& name 
        ) = 0;

      virtual void loadFont
        ( 
         const std::string& fontWidths, 
         const std::string& fontTexture, 
         const std::string& name
        ) = 0;

  };

} // visualizer

Q_DECLARE_INTERFACE( visualizer::IResourceMan, "siggame.vis2.resourceman/0.2" );

#endif
