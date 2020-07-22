#include "src/lib/sqlite/sqlite_wrapper.h"

#include <iostream>
#include <map>

#include "absl/types/optional.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

// Tests require database in the DATABASE_PATH with table
// parent("parent_id" INTEGER, "name TEXT").
namespace sqlite {

const char* DATABASE_PATH = "test.db";

TEST(SqliteTest, BasicUsage) {
  // Set up data in table.
  SqliteWrapper sqlite;
  ASSERT_EQ(sqlite.OpenDatabase(DATABASE_PATH), absl::OkStatus());
  EXPECT_EQ(sqlite.Execute("DROP TABLE IF EXISTS parent;"), absl::OkStatus());
  EXPECT_EQ(sqlite.Execute(R"(
      CREATE TABLE IF NOT EXISTS parent (
	      parent_id	INTEGER NOT NULL UNIQUE,
	      name	TEXT,
	      PRIMARY KEY(parent_id)
      );)"),
            absl::OkStatus());
  EXPECT_EQ(sqlite.Execute(R"(
      INSERT INTO parent (parent_id, name) VALUES (101, 'Anne');
      INSERT INTO parent (parent_id, name) VALUES (102, 'Bill');
      INSERT INTO parent (parent_id, name) VALUES (103, NULL);)"),
            absl::OkStatus());
  EXPECT_EQ(sqlite.CloseDatabase(), absl::OkStatus());

  ASSERT_EQ(sqlite.OpenDatabase(DATABASE_PATH), absl::OkStatus());
  std::unique_ptr<SqliteWrapper::Scroll> scroll;
  EXPECT_EQ(sqlite.Execute("SELECT * FROM parent;", &scroll), absl::OkStatus());
  ASSERT_TRUE(scroll);

  absl::Status status = scroll->NextRow();
  const std::map<int, absl::optional<std::string>> expected_results = {
      {101, "Anne"}, {102, "Bill"}, {103, "NULL"}};
  for (const auto& [expected_id, expected_name] : expected_results) {
    if (status != absl::OkStatus()) {
      break;
    }
    EXPECT_EQ(scroll->GetColumnInt(0), expected_id);
    EXPECT_EQ(scroll->GetColumnText(1), expected_name);
    status = scroll->NextRow();
  }
  EXPECT_TRUE(absl::IsNotFound(status));
  EXPECT_EQ(sqlite.CloseDatabase(), absl::OkStatus());
}

TEST(SqliteTest, BadTableNameInQuery) {
  SqliteWrapper sqlite;
  ASSERT_EQ(sqlite.OpenDatabase(DATABASE_PATH), absl::OkStatus());
  EXPECT_TRUE(absl::IsAborted(sqlite.Execute("DELETE FROM does_not_exist;")));
  EXPECT_EQ(sqlite.CloseDatabase(), absl::OkStatus());
}

}  // namespace sqlite
