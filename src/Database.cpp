#include "MiniDB.hpp"
#include <charconv>
#include <sstream>

namespace minidb {

  Database::Database(std::string dataDirectory)
    : storage_(std::move(dataDirectory)) {
    loadTables();
  }

  void Database::loadTables() {
    std::string error;
    for (const auto& name : storage_.listTables(error)) {
      auto table = storage_.load(name, error);
      if (table) {
        tables_.emplace(name, std::move(*table));
        rebuildIndex(name);
      }
    }
  }

  void Database::rebuildIndex(const std::string& name) {
    auto& index = indexes_[name];
    index.clear();

    const auto& table = tables_.at(name);
    if (table.columns().empty() || table.columns()[0].type != DataType::Int) {
      return;
    }

    for (std::size_t i = 0; i < table.rows().size(); ++i) {
      int key = 0;
      if (
        std::from_chars(table.rows()[i][0].data(),
        table.rows()[i][0].data() + table.rows()[i][0].size(),
        key
      ).ec == std::errc{}) {
        index.insert(key, i);
      }
    }
  }

  std::string Database::execute(const std::string& sql) {
    Statement s = parser_.parse(sql);
    if (s.kind == Statement::Kind::Invalid) {
      return "Error: " + s.error;
    }

    std::string error;

    switch (s.kind) {
      case Statement::Kind::Create: {
        if (tables_.count(s.table)) {
          return "Error: Table already exists";
        }

        Table table(s.table, s.columns);
        if (!storage_.save(table, error)) {
          return "Error: " + error;
        }

        tables_.emplace(s.table, std::move(table));
        rebuildIndex(s.table);
        return "Table created.";
      }
      case Statement::Kind::Insert: {
        auto it = tables_.find(s.table);
        if (it == tables_.end()) {
          return "Error: Table not found";
        }

        if (!it->second.insert(s.values, error)) {
          return "Error: " + error;
        }
        if (!storage_.save(it->second, error)) {
          return "Error: " + error;
        }

        rebuildIndex(s.table);
        return "1 row inserted.";
      }
      case Statement::Kind::Select: {
        auto it = tables_.find(s.table);
        if (it == tables_.end()) {
          return "Error: Table not found";
        }

        std::vector<std::vector<std::string>> rows;
        const auto& table = it->second;

        if (s.where && !table.columns().empty() && s.where->column == table.columns()[0].name &&
            table.columns()[0].type == DataType::Int) {
          int key = 0;
          auto [p, ec] = std::from_chars(
            s.where->value.data(),
            s.where->value.data() + s.where->value.size(),
            key
          );
          if (ec == std::errc{} && p == s.where->value.data() + s.where->value.size()) {
            for (auto rowId : indexes_[s.table].find(key)) {
              rows.push_back(table.rows()[rowId]);
            }
          }
        } else {
          rows = table.select(s.where, error);
        }

        if (!error.empty()) {
          return "Error: " + error;
        }
        if (rows.empty()) {
          return "No rows.";
        }

        std::ostringstream out;
        for (const auto& row : rows) {
          for (std::size_t i = 0; i < row.size(); ++i) {
            if (i) {
              out << " | ";
            }
            out << row[i];
          }
          out << '\n';
        }
        return out.str();
      }
      case Statement::Kind::Update: {
        auto it = tables_.find(s.table);
        if (it == tables_.end()) {
          return "Error: Table not found";
        }

        auto n = it->second.update(s.setColumn, s.setValue, s.where, error);
        if (!error.empty()) {
          return "Error: " + error;
        }
        if (!storage_.save(it->second, error)) {
          return "Error: " + error;
        }

        rebuildIndex(s.table);
        return std::to_string(n) + " row(s) updated.";
      }
      case Statement::Kind::Delete: {
        auto it = tables_.find(s.table);
        if (it == tables_.end()) {
          return "Error: Table not found";
        }

        auto n = it->second.erase(s.where, error);
        if (!error.empty()) {
          return "Error: " + error;
        }
        if (!storage_.save(it->second, error)) {
          return "Error: " + error;
        }

        rebuildIndex(s.table);
        return std::to_string(n) + " row(s) deleted.";
      }
      default:
        return "Error: Unsupported command";
    }
  }
}