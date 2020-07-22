#include "src/lib/sqlite/sqlite_wrapper.h"

#include <iostream>

using namespace std;
using ::absl::Status;
using ::absl::StrCat;

namespace sqlite {

Status SqliteWrapper::OpenDatabase(const string& database_name) {
  if (db_ != nullptr) {
    return absl::UnavailableError(
        StrCat("Databae already in use: ", database_name));
  }
  database_name_ = database_name;
  int result_code = sqlite3_open(database_name.c_str(), &db_);
  if (result_code == SQLITE_OK) {
    return absl::OkStatus();
  }
  sqlite3_close_v2(db_);
  return CreateErrorStatus("open", database_name, result_code);
}

Status SqliteWrapper::Execute(const string& query) {
  if (db_ == nullptr) {
    return absl::UnavailableError("Error: Database not connected.");
  }

  char* err_msg = nullptr;
  int result_code = sqlite3_exec(db_, query.c_str(), 0, 0, &err_msg);
  if (result_code != SQLITE_OK) {
    Status status = CreateErrorStatus("exec", query, err_msg, result_code);
    sqlite3_free(err_msg);
    return status;
  }

  return absl::OkStatus();
}

Status SqliteWrapper::Execute(const string& query, unique_ptr<Scroll>* scroll) {
  if (db_ == nullptr) {
    return absl::UnavailableError("Error: Database not connected.");
  }
  sqlite3_stmt* stmt;

  int result_code = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr);
  if (result_code != SQLITE_OK) {
    sqlite3_reset(stmt);
    return CreateErrorStatus("prepare", query, result_code);
  }

  *scroll = absl::make_unique<Scroll>(stmt);
  return absl::OkStatus();
}

absl::Status SqliteWrapper::CloseDatabase() {
  int result_code = sqlite3_close_v2(db_);
  db_ = nullptr;
  if (result_code != SQLITE_OK) {
    return CreateErrorStatus("close", database_name_, result_code);
  }
  return absl::OkStatus();
}

}  // namespace sqlite
