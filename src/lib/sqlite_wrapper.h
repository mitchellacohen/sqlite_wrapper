#ifndef __SRC__LIB__SQLLITE__SQLITE_WRAPPER_H
#define __SRC__LIB__SQLLITE__SQLITE_WRAPPER_H

#include <sqlite3.h>

#include <regex>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"

namespace sqlite {

// Simple interface for modifying data in an SQLite database.
class SqliteWrapper {
 public:
  // Scrolls through a previously prepared and executed statement.
  class Scroll {
   public:
    Scroll(sqlite3_stmt* stmt) : stmt_(stmt) {}

    // Advances to the next row. Returns NOT_FOUND if the end has been reached.
    absl::Status NextRow() {
      int result_code = sqlite3_step(stmt_);  // execute
      switch (result_code) {
        case SQLITE_ROW:
          return absl::OkStatus();
        case SQLITE_DONE:
          sqlite3_finalize(stmt_);
          return absl::NotFoundError("No more rows.");
        default:
          return absl::AbortedError(
              absl::StrCat("Step result code: ", result_code));
      }
    }

    std::string GetColumnText(int col) {
      if (stmt_ == nullptr) {
        return "";
      }
      auto* value = sqlite3_column_text(stmt_, col);
      if (value == nullptr) {
        return "NULL";
      }
      return std::string(reinterpret_cast<const char*>(value));
    }

    int GetColumnInt(int col) {
      return stmt_ == nullptr ? 0 : sqlite3_column_int(stmt_, 0);
    }

   private:
    sqlite3_stmt* stmt_;  // Does not own.
  };

  // Open a database for read/write.
  absl::Status OpenDatabase(const std::string& database_name);

  // Execute a given query from which no returned data is needed.
  absl::Status Execute(const std::string& query);

  // Execute a given query and return a scroller that can be used to loop
  // through the query results.
  absl::Status Execute(const std::string& query,
                       std::unique_ptr<Scroll>* scroll);

  // Close the database.
  absl::Status CloseDatabase();

  // Wrap a string with single quotes. If the input is the nullptr, the
  // unquotes string NULL will be returned. Single quotes in the input become
  // 2 single quotes.
  static std::string Quote(const std::string* input) {
    if (input == nullptr) {
      return "NULL";
    }
    return absl::StrCat(
        "'", std::regex_replace(*input, std::regex("\\'"), "''"), "'");
  }

 private:
  sqlite3* db_ = nullptr;
  std::string database_name_;

  inline absl::Status CreateErrorStatus(const std::string& message,
                                        const std::string& query,
                                        int result_code) {
    return CreateErrorStatus(
        message, query, db_ == nullptr ? "" : sqlite3_errmsg(db_), result_code);
  }

  // Create an error status based on the current SQLite error message and the
  // given |message|, |query|, |error_msg| and |result_code|.
  inline absl::Status CreateErrorStatus(const std::string& message,
                                        const std::string& query,
                                        const std::string& error_msg,
                                        int result_code) {
    return absl::AbortedError(absl::StrCat(
        "Error: ", message, "\nResult Code: ", result_code,
        "\nSQL Error Message: ", error_msg, "\nSQL Query: ", query, "\n"));
  }
};

}  // namespace sqlite

#endif  // __SRC__LIB__SQLLITE__SQLITE_WRAPPER_H
