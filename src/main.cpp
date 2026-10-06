#include "MiniDB.hpp"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  minidb::Database database;

  if (argc == 3 && string(argv[1]) == "--query") {
    cout << database.execute(argv[2]) << "\n";
    return 0;
  }

  cout << "MiniDB - type EXIT to quit.\n";

  string sql;
  while (true) {
    cout << "MiniDB> ";
    if (!getline(cin, sql)) {
      break;
    }
    if (sql == "EXIT" || sql == "exit" || sql == "QUIT" || sql == "quit") {
      break;
    }
    if (sql.empty()) {
      continue;
    }
    cout << database.execute(sql) << "\n";
  }
  return 0;
}
