#pragma once

#include <ebroschin/persistence/persistence_store.hpp>
#include <SQLiteCpp/SQLiteCpp.h>

#include <string>

namespace ebroschin::persistence::modules::sqlite {

class SqlitePersistenceStore final : public PersistenceStore {
public:
  explicit SqlitePersistenceStore(const std::string& file_name);

  void Initialize() override;

  [[nodiscard]] SQLite::Database& GetDatabase() noexcept
  { return db_; }

private:
  SQLite::Database db_;
};

}