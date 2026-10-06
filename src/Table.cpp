#include "MiniDB.hpp"
#include <algorithm>
#include <charconv>

using namespace std;

namespace minidb {
	Table::Table(string name, vector<Column> columns)
		: name_(move(name)), columns_(move(columns)) {}

	const string& Table::name() const {
		return name_;
	}

	const vector<Column>& Table::columns() const {
		return columns_;
	}

	const vector<vector<string>>& Table::rows() const {
		return rows_;
	}

	optional<size_t> Table::columnIndex(const string& name) const {
		for (size_t i = 0; i < columns_.size(); ++i)
			if (columns_[i].name == name)
				return i;
			
		return nullopt;
	}

	bool Table::validValue(size_t column, const string& value) const {
		if (columns_[column].type == DataType::Text) {
			return true;
		}

		int n;
		auto [p, e] = from_chars(value.data(), value.data() + value.size(), n);
		return e == errc{} && p == value.data() + value.size();
	}

	bool Table::matches(const vector<string>& row, const optional<Condition>& where, string& error) const {
		if (!where) {
			return true;
		}

		auto i = columnIndex(where->column);
		if (!i) {
			error = "Unknown column: " + where->column;
			return false;
		}

		return row[*i] == where->value;
	}

	bool Table::insert(const vector<string>& values, string& error) {
		if (values.size() != columns_.size()) {
			error = "Column count does not match values";
			return false;
		}

		for (size_t i = 0; i < values.size(); ++i) {
			if (!validValue(i, values[i])) {
				error = "Invalid value for column: " + columns_[i].name;
				return false;
			}
		}

		rows_.push_back(values);
		return true;
	}

	vector<vector<string>> Table::select(const optional<Condition>& where, string& error) const {
		vector<vector<string>> result;
		for (const auto& row : rows_) {
			auto prior = error;
			bool ok = matches(row, where, error);
			if (!error.empty() && error != prior) {
				return {};
			}
			if (ok) {
				result.push_back(row);
			}
		}
		return result;
	}

	size_t Table::update(const string& column, const string& value, const optional<Condition>& where, string& error) {
		auto target = columnIndex(column);
		if (!target) {
			error = "Unknown column: " + column;
			return 0;
		}
		if (!validValue(*target, value)) {
			error = "Invalid value for column: " + column;
			return 0;
		}

		size_t n = 0;
		for (auto& row : rows_) {
			if (matches(row, where, error)) {
				row[*target] = value;
				++n;
			}
		}
		return n;
	}

	size_t Table::erase(const optional<Condition>& where, string& error) {
		auto old = rows_.size();
		auto end = remove_if(rows_.begin(), rows_.end(), [&](const auto& row) {
			return matches(row, where, error);
		});
		rows_.erase(end, rows_.end());
		return old - rows_.size();
	}

	void Table::setRows(vector<vector<string>> rows) {
		rows_ = move(rows);
	}
}