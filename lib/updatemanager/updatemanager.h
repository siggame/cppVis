#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QCryptographicHash>

namespace visualizer
{
  class _UpdateManager: public IUpdateManager
  {
  };

  extern _UpdateManager *UpdateManager;

} // visualizer

#endif // UPDATEMANAGER_H
