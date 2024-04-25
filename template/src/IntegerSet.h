#pragma once

#include <iostream>
#include <set>
#include <queue>
#include <stack>
class Node {
public:
	int value;
	Node* left;
	Node* right;
	Node(int value, Node* left = nullptr, Node* right = nullptr) : value(value), left(left), right(right) {}
};

class IntegerSet {
public:
	class Iterator {
	public:
		Iterator& operator++() {
			if (!stack.empty()) {
				current = stack.top();
				stack.pop();
			}
			if (current != nullptr) {
				pushLeft(current->right);
			}
			return *this;
		}
		Iterator operator++(int) {
			Iterator res(*this);
			++(*this);
			return res;
		}
		int operator*() const {
			return current->value;
		}
		int* operator->() const {
			return &current->value;
		}
		bool operator==(const Iterator& other) const {
			return current == other.current;
		}
		bool operator!=(const Iterator& other) const {
			return !((*this) == other);
		}

	private:
		friend class IntegerSet;
		Iterator(Node* current) : current(nullptr) {
			if (current != nullptr) {
				stack.push(nullptr);
				pushLeft(current);
				++(*this);
			}
		}
		void pushLeft(Node* root) {
			while (root != nullptr) {
				stack.push(root);
				root = root->left;
			}
		}
		std::stack<Node*> stack;
		Node* current;
	};
	Iterator begin() const {
		return Iterator(root);
	}
	Iterator end() const {
		return Iterator(nullptr);
	}
private:
	Node* root;
	size_t count;

private:
	void insert(Node*& root, int element);
	void copy(Node*& root, const Node* oldRoot);
	void free(Node* root);
	bool contains(const Node* root, int element) const;
	bool isSorted(const std::vector<int>& elements) const;
	void createBST(Node*& root, const std::vector<int>& elements, int start, int end);
	void serialize(const Node* root, std::ostream& out) const;
	void insertElement(Node*& copyNode, IntegerSet& res, int value) const;
public:
	IntegerSet();
	IntegerSet(const IntegerSet&);
	IntegerSet& operator=(const IntegerSet&);
	~IntegerSet();

	
public:
	// Връща броя на елементите в множеството
	size_t size() const;

	/// Проверява дали две множества се състоят от едни и същи елементи
	bool equals(const IntegerSet&) const;

	/// Проверява дали елемент се съдържа в множеството
	bool contains(int) const;

	/// Проверява дали текущия обект е подмножество на друг
	bool subsetOf(const IntegerSet&) const;

	/// Връща обединението на две множества
	IntegerSet getUnion(const IntegerSet&) const;

	/// Връща сечението на две множества
	IntegerSet getIntersection(const IntegerSet&) const;

	/// Десериализира съдържанието на едно множество.
	/// Новата информация напълно заменя старата.
	void deserialize(std::istream&);

	/// Сериализира съдържанието на едно множество
	void serialize(std::ostream&) const;
};

inline std::istream& operator>>(std::istream& in, IntegerSet& set)
{
	set.deserialize(in);
	return in;
}

inline std::ostream& operator<<(std::ostream& out, const IntegerSet& set)
{
	set.serialize(out);
	return out;
}
