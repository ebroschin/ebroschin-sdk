#pragma once

namespace ebroschin::persistence {

class PersistenceStore {
public:
  PersistenceStore() = default;
  virtual ~PersistenceStore() = default;

  PersistenceStore(const PersistenceStore&) = delete;
  PersistenceStore& operator=(const PersistenceStore&) = delete;
  PersistenceStore(PersistenceStore&&) = delete;
  PersistenceStore& operator=(PersistenceStore&&) = delete;

  virtual void Initialize() = 0;
  virtual void Deinitialize() {}
};

}