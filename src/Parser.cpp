	#include "MiniDB.hpp"
	#include <algorithm>
	#include <cctype>
	#include <regex>
	#include <sstream>

	using namespace std;

namespace minidb {
	namespace {
		string trim(string value) {
			auto b = value.find_first_not_of(" \t\r\n");
			auto e = value.find_last_not_of(" \t\r\n");
			return b == string::npos ? "" : value.substr(b, e - b + 1);
		}

		string upper(string value) {
			transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
				return static_cast<char>(toupper(c));
			});
			return value;
		}

		vector<string> splitValues(const string& text) {
			vector<string> out;
			string current;
			bool quoted = false;
			for (char c : text) {
				if (c == '\'') {
					quoted = !quoted;
					continue;
				}
				if (c == ',' && !quoted) {
					out.push_back(trim(current));
					current.clear();
				} else {
					current += c;
				}
			}
			out.push_back(trim(current));
			return out;
		}

		optional<Condition> condition(const string& text) {
			smatch m;
			static const regex r(R"(^\s*([A-Za-z_]\w*)\s*=\s*(?:'([^']*)'|([^\s;]+))\s*$)", regex::icase);
			if (!regex_match(text, m, r)) {
				return nullopt;
			}
			return Condition{m[1], m[2].matched ? m[2].str() : m[3].str()};
		}
	}
	Statement Parser::parse(const string& input) const {
		string sql = trim(input);
		if (!sql.empty() && sql.back() == ';') {
			sql.pop_back();
		}

		smatch m;
		Statement s;

		static const regex create(R"(^CREATE\s+TABLE\s+([A-Za-z_]\w*)\s*\((.*)\)$)", regex::icase);
		if (regex_match(sql, m, create)) {
			s.kind = Statement::Kind::Create;
			s.table = m[1];
			for (const auto& part : splitValues(m[2])) {
				istringstream in(part);
				string n, t, extra;
				in >> n >> t >> extra;
				if (n.empty() || t.empty() || !extra.empty() || (upper(t) != "INT" && upper(t) != "TEXT")) {
					s.kind = Statement::Kind::Invalid;
					s.error = "Invalid column definition";
					return s;
				}
				s.columns.push_back({n, upper(t) == "INT" ? DataType::Int : DataType::Text});
			}
			if (s.columns.empty()) {
				s.kind = Statement::Kind::Invalid;
				s.error = "Table needs columns";
			}
			return s;
		}

		static const regex insert(R"(^INSERT\s+INTO\s+([A-Za-z_]\w*)\s+VALUES\s*\((.*)\)$)", regex::icase);
		if (regex_match(sql, m, insert)) {
			s.kind = Statement::Kind::Insert;
			s.table = m[1];
			s.values = splitValues(m[2]);
			return s;
		}

		static const regex select(R"(^SELECT\s+\*\s+FROM\s+([A-Za-z_]\w*)(?:\s+WHERE\s+(.+))?$)", regex::icase);
		if (regex_match(sql, m, select)) {
			s.kind = Statement::Kind::Select;
			s.table = m[1];
			if (m[2].matched) {
				s.where = condition(m[2]);
				if (!s.where) {
					s.kind = Statement::Kind::Invalid;
					s.error = "Invalid WHERE condition";
				}
			}
			return s;
		}

		static const regex update(R"(^UPDATE\s+([A-Za-z_]\w*)\s+SET\s+([A-Za-z_]\w*)\s*=\s*(?:'([^']*)'|([^\s]+))\s+WHERE\s+(.+)$)", regex::icase);
		if (regex_match(sql, m, update)) {
			s.kind = Statement::Kind::Update;
			s.table = m[1];
			s.setColumn = m[2];
			s.setValue = m[3].matched ? m[3].str() : m[4].str();
			s.where = condition(m[5]);
			if (!s.where) {
				s.kind = Statement::Kind::Invalid;
				s.error = "Invalid WHERE condition";
			}
			return s;
		}

		static const regex del(R"(^DELETE\s+FROM\s+([A-Za-z_]\w*)(?:\s+WHERE\s+(.+))?$)", regex::icase);
		if (regex_match(sql, m, del)) {
			s.kind = Statement::Kind::Delete;
			s.table = m[1];
			if (m[2].matched) {
				s.where = condition(m[2]);
				if (!s.where) {
					s.kind = Statement::Kind::Invalid;
					s.error = "Invalid WHERE condition";
				}
			}
			return s;
		}

		s.error = "Unsupported or invalid SQL command";
		return s;
	}
}
