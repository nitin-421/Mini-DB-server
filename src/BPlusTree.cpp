#include "MiniDB.hpp"
#include <algorithm>

namespace minidb {
	BPlusTree::BPlusTree(std::size_t leafSize)
		: leafSize_(leafSize) {
		clear();
	}

	void BPlusTree::clear() {
		leaves_.clear();
		leaves_.push_back({});
	}

	void BPlusTree::insert(int key, std::size_t rowId) {
		std::size_t leaf = 0;
		while (
			leaves_[leaf].next != static_cast<std::size_t>(-1)
			&&
			!leaves_[leaf].entries.empty() 
			&&
			key > leaves_[leaf].entries.back().first
		) {
			leaf = leaves_[leaf].next;
		}

		auto& entries = leaves_[leaf].entries;
		entries.insert(std::upper_bound(entries.begin(), entries.end(), key, [](int k, const auto& p) {
			return k < p.first;
		}),
		{key, rowId});

		if (entries.size() <= leafSize_) {
			return;
		}

		Leaf sibling;
		
		auto mid = entries.begin() + entries.size() / 2;
		sibling.entries.assign(mid, entries.end());
		entries.erase(mid, entries.end());
		sibling.next = leaves_[leaf].next;
		leaves_[leaf].next = leaves_.size();
		leaves_.push_back(std::move(sibling));
	}

	std::vector<std::size_t> BPlusTree::find(int key) const {
		std::vector<std::size_t> result;
		for (std::size_t leaf = 0; leaf != static_cast<std::size_t>(-1); leaf = leaves_[leaf].next) {
			for (const auto& [k, row] : leaves_[leaf].entries) {
				if (k == key) {
					result.push_back(row);
				}
			}
		}
		return result;
	}
}
