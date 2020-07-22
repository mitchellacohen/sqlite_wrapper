#include <cstdlib>
#include <iostream>

#include "absl/status/status.h"
#include "src/lib/sqlite/sqlite_wrapper.h"

using namespace ::sqlite;
using namespace ::std;

using ::absl::Status;

const char DATABASE_PATH[] =
    "/Users/mitchmac2016/Documents/bazel/cpp-template/sample_main.db";

void Check(const Status& status) {
  if (status != absl::OkStatus()) {
    cerr << status.message();
    exit(static_cast<int>(status.code()));
  }
}

void Check(const SqliteWrapper::Scroll* scroll) {
  if (scroll == nullptr) {
    cerr << "Null scroll unexpected.";
    exit(1);
  }
}

int main() {
  sqlite::SqliteWrapper sqlite;
  Check(sqlite.OpenDatabase(DATABASE_PATH));

  Check(sqlite.Execute("DROP TABLE IF EXISTS person;"));
  Check(sqlite.Execute("DROP TABLE IF EXISTS relation;"));

  Check(sqlite.Execute(R"(
      CREATE TABLE IF NOT EXISTS person (
	        id	INTEGER NOT NULL UNIQUE,
	        name	TEXT,
	        PRIMARY KEY(id));)"));
  Check(sqlite.Execute(R"(
      CREATE TABLE IF NOT EXISTS relation (
	        parent_id	INTEGER NOT NULL,
	        child_id	INTEGER NOT NULL,
	        PRIMARY KEY(parent_id, child_id));)"));

  Check(sqlite.Execute("BEGIN"));

  Check(sqlite.Execute(R"(
      INSERT INTO person (id, name) VALUES (101, 'Anne');
      INSERT INTO person (id, name) VALUES (102, 'Bill');
      INSERT INTO person (id, name) VALUES (103, NULL);)"));
  Check(sqlite.Execute(R"(
      INSERT INTO relation (parent_id, child_id) VALUES (101, 102);
      INSERT INTO relation (parent_id, child_id) VALUES (102, 103);)"));

  Check(sqlite.Execute("COMMIT"));

  std::unique_ptr<SqliteWrapper::Scroll> scroll;
  Check(sqlite.Execute(R"(
      SELECT parent.id, parent.name, child.id, child.name
          FROM person parent, person child, relation
          WHERE parent.id = parent_id AND child.id = child_id;)",
                       &scroll));
  Check(scroll.get());
  absl::Status status = scroll->NextRow();
  while (status == absl::OkStatus()) {
    int col = 0;
    int parent_id = scroll->GetColumnInt(col++);
    auto parent_name = scroll->GetColumnText(col++);
    int child_id = scroll->GetColumnInt(col++);
    auto child_name = scroll->GetColumnText(col++);
    cout << parent_id << " (" << parent_name << ") => " << child_id << " ("
         << child_name << ")" << endl;
    status = scroll->NextRow();
  }
  if (!absl::IsNotFound(status)) {
    Check(status);
  }
  Check(sqlite.CloseDatabase());

  return 0;
}
