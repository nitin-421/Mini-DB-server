#include "MiniDB.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

namespace minidb {
	Storage::Storage(string directory)
		: directory_(move(directory)) {
		error_code ec;
		filesystem::create_directories(directory_, ec);
	}

	bool Storage::save(const Table& table, string& error) const {
		ofstream out(filesystem::path(directory_) / (table.name() + ".tbl"), ios::trunc);
		if (!out) {
			error = "Cannot write table file";
			return false;
		}

		out << table.name() << '\n' << table.columns().size() << '\n';
		for (const auto& c : table.columns()) {
			out << quoted(c.name) << ' ' << (c.type == DataType::Int ? "INT" : "TEXT") << '\n';
		}
		out << table.rows().size() << '\n';
		for (const auto& row : table.rows()) {
			for (const auto& value : row) {
				out << quoted(value) << ' ';
			}
			out << '\n';
		}
		return static_cast<bool>(out);
	}

	optional<Table> Storage::load(const string& name, string& error) const {
		ifstream in(filesystem::path(directory_) / (name + ".tbl"));
		if (!in) {
			error = "Cannot open table file";
			return nullopt;
		}

		string storedName, type;
		size_t ncols, nrows;
		
		if (!getline(in, storedName) || !(in >> ncols)) {
			error = "Invalid table file";
			return nullopt;
		}

		vector<Column> cols;
		for (size_t i = 0; i < ncols; ++i) {
			string col;
			if (!(in >> quoted(col) >> type) || (type != "INT" && type != "TEXT")) {
				error = "Invalid table schema";
				return nullopt;
			}
			cols.push_back({col, type == "INT" ? DataType::Int : DataType::Text});
		}

		if (!(in >> nrows)) {
			error = "Invalid table rows";
			return nullopt;
		}

		vector<vector<string>> rows(nrows, vector<string>(ncols));
		for (auto& row : rows) {
			for (auto& value : row) {
				if (!(in >> quoted(value))) {
					error = "Invalid row data";
					return nullopt;
				}
			}
		}

		Table table(storedName, cols);
		table.setRows(move(rows));
		return table;
	}

	vector<string> Storage::listTables(string& error) const {
		vector<string> names;
		error_code ec;
		for (const auto& item : filesystem::directory_iterator(directory_, ec)) {
			if (item.path().extension() == ".tbl") {
				names.push_back(item.path().stem().string());
			}
		}
		if (ec) {
			error = "Cannot read data directory";
		}
		return names;
	}
}
