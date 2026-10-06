#pragma once

#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <vector>

using namespace std;

namespace minidb {

	enum class DataType { Int, Text };
	struct Column {
		string name;
		DataType type;
	};
	struct Condition {
		string column;
		string value;
	};
	struct Statement {
		enum class Kind { Create, Insert, Select, Update, Delete, Invalid } kind = Kind::Invalid;
		string table;
		vector<Column> columns;
		vector<string> values;
		string setColumn;
		string setValue;
		optional<Condition> where;
		string error;
	};

	class Parser {
	public:
		Statement parse(const string& sql) const;
	};

	class BPlusTree {
	public:
		explicit BPlusTree(size_t leafSize = 8);
		void insert(int key, size_t rowId);
		vector<size_t> find(int key) const;
		void clear();

	private:
		struct Leaf {
			vector<pair<int, size_t>> entries;
			size_t next = static_cast<size_t>(-1);
		};
		size_t leafSize_;
		vector<Leaf> leaves_;
	};

	class Table {
	public:
		Table() = default;
		Table(string name, vector<Column> columns);
		const string& name() const;
		const vector<Column>& columns() const;
		const vector<vector<string>>& rows() const;
		bool insert(const vector<string>& values, string& error);
		vector<vector<string>> select(const optional<Condition>& where, string& error) const;
		size_t update(const string& column, const string& value, const optional<Condition>& where, string& error);
		size_t erase(const optional<Condition>& where, string& error);
		void setRows(vector<vector<string>> rows);

	private:
		string name_;
		vector<Column> columns_;
		vector<vector<string>> rows_;
		optional<size_t> columnIndex(const string& name) const;
		bool matches(const vector<string>& row, const optional<Condition>& where, string& error) const;
		bool validValue(size_t column, const string& value) const;
	};

	class Storage {
	public:
		explicit Storage(string directory);
		bool save(const Table& table, string& error) const;
		optional<Table> load(const string& name, string& error) const;
		vector<string> listTables(string& error) const;

	private:
		string directory_;
	};

	class Database {
	public:
		explicit Database(string dataDirectory = "data");
		string execute(const string& sql);

	private:
		Parser parser_;
		Storage storage_;
		map<string, Table> tables_;
		map<string, BPlusTree> indexes_;
		void loadTables();
		void rebuildIndex(const string& tableName);
	};
}
