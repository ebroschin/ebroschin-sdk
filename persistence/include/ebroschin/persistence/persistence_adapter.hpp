#pragma once

#include "persistence_store.hpp"

#include <concepts>

namespace ebroschin::persistence {

class PersistenceStore;

class PersistenceAdapterBase {
public:
  virtual ~PersistenceAdapterBase() = default;

  virtual void Initialize() = 0;
  virtual void Deinitialize() {}
};

template<typename TPersistenceStore, typename TAdapterInterface>
requires std::derived_from<TPersistenceStore, PersistenceStore>
class PersistenceAdapter : public PersistenceAdapterBase, public TAdapterInterface {
public:
  explicit PersistenceAdapter(TPersistenceStore& store):
    store_{store}
  {}

  PersistenceAdapter(const PersistenceAdapter&) = delete;
  PersistenceAdapter& operator=(const PersistenceAdapter&) = delete;
  PersistenceAdapter(PersistenceAdapter&&) = delete;
  PersistenceAdapter& operator=(PersistenceAdapter&&) = delete;

protected:
  TPersistenceStore& store_;
};

}