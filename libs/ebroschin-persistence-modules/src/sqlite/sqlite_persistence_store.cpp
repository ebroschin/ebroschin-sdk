#include "ebroschin/persistence-modules/sqlite/sqlite_persistence_store.hpp"

namespace ebroschin::persistence::modules::sqlite {

SqlitePersistenceStore::SqlitePersistenceStore(const std::string& file_name)
  : db_{file_name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE}
{}

void SqlitePersistenceStore::Initialize() {
  db_.exec("PRAGMA foreign_keys=ON;");
  db_.exec("PRAGMA synchronous=NORMAL;");
  db_.exec("PRAGMA journal_mode=WAL;");
}

}