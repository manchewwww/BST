#pragma once
#include "IntegerSet.h"

void IntegerSet::copy(Node*& root, const Node* oldRoot)
{
	if (oldRoot == nullptr) {
		return;
	}
	root = new (std::nothrow) Node(oldRoot->value);
	if (root == nullptr) {
		throw std::bad_alloc();
	}
	copy(root->left, oldRoot->left);
	copy(root->right, oldRoot->right);
}

void IntegerSet::free(Node* root) {
	if (root == nullptr) {
		return;
	}
	free(root->left);
	free(root->right);
	delete root;
	root = nullptr;
}

void IntegerSet::insert(Node*& root, int element)
{
	if (root == nullptr) {
		root = new (std::nothrow) Node(element);
		if (root == nullptr) {
			throw std::bad_alloc();
		}
	}
	else if (root->value < element) {
		insert(root->right, element);
	}
	else {
		insert(root->left, element);
	}
}

bool IntegerSet::contains(const Node* root, int element) const
{
	if (root == nullptr) {
		return false;
	}
	else if (root->value == element) {
		return true;
	}
	return contains(root->value < element ? root->right : root->left, element);
}

bool IntegerSet::isSorted(const std::vector<int>& elements) const
{
	for (size_t i = 1; i <= elements.size() - 1; i++) {
		if (elements[i - 1] >= elements[i]) {
			return false;
		}
	}
	return true;
}

void IntegerSet::createBST(Node*& root, const std::vector<int>& elements, int start, int end)
{
	if (start > end) {
		root = nullptr;
		return;
	}
	int middle = start + (end - start) / 2;
	insert(root, elements[middle]);
	createBST(root->left, elements, start, middle - 1);
	createBST(root->right, elements, middle + 1, end);
	count++;
}

void IntegerSet::serialize(const Node* root, std::ostream& out) const
{
	if (root == nullptr) {
		return;
	}
	serialize(root->left, out);
	out << ' ' << root->value;
	serialize(root->right, out);
}

void IntegerSet::insertElement(Node*& copyNode, IntegerSet& res, int value) const
{
	if (res.size() == 0) {
		res.insert(res.root, value);
	}
	else {
		res.insert(copyNode, value);
		copyNode = copyNode->right;
	}
}

IntegerSet::IntegerSet() : root(nullptr), count(0)
{
}

IntegerSet::IntegerSet(const IntegerSet& other) : IntegerSet()
{
	try {
		copy(root, other.root);
		count = other.count;
	}
	catch (const std::bad_alloc&) {
		free(root);
		throw std::bad_alloc();
	}

}

IntegerSet& IntegerSet::operator=(const IntegerSet& other)
{
	if (this != &other) {
		try {
			free(root);
			copy(root, other.root);
			count = other.count;
		}
		catch (const std::bad_alloc&) {
			free(root);
			throw std::bad_alloc();
		}

	}
	return *this;
}

IntegerSet::~IntegerSet()
{
	count = 0;
	free(root);
}

size_t IntegerSet::size() const
{
	return count;
}

bool IntegerSet::equals(const IntegerSet& other) const
{
	if (count != other.count) {
		return false;
	}
	if (root == nullptr && other.root == nullptr) {
		return true;
	}
	else if ((root == nullptr || other.root == nullptr)) {
		return false;
	}
	Iterator curIterator = begin();
	Iterator otherIterator = other.begin();
	while (curIterator != end() && otherIterator != other.end()) {
		if ((*curIterator) == (*otherIterator)) {
			++curIterator;
			++otherIterator;
		}
		else {
			return false;
		}
	}
	return true;
}

bool IntegerSet::contains(int elem) const
{
	return contains(root, elem);
}

bool IntegerSet::subsetOf(const IntegerSet& other) const
{
	if (count == 0) {
		return true;
	}
	else if ((other.count == 0 && count != 0) || count > other.count) {
		return false;
	}
	Iterator curIterator = begin();
	Iterator otherIterator = other.begin();
	while (curIterator != end()) {
		if ((*curIterator) == (*otherIterator)) {
			++curIterator;
			++otherIterator;
		}
		else if ((*curIterator) < (*otherIterator)) {
			return false;
		}
		else {
			++otherIterator;
		}
	}
	return true;
}

IntegerSet IntegerSet::getUnion(const IntegerSet& other) const
{
	if (other.count == 0) {
		return *this;
	}
	else if (count == 0) {
		return other;
	}
	IntegerSet res;
	Iterator curIterator = begin();
	Iterator otherIterator = other.begin();
	Node* copyNode = nullptr;
	try {
		while (curIterator != end() && otherIterator != other.end()) {
			if ((*curIterator) == (*otherIterator)) {
				insertElement(copyNode, res, *curIterator);
				++curIterator;
				++otherIterator;
			}
			else if ((*curIterator) < (*otherIterator)) {
				insertElement(copyNode, res, *curIterator);
				++curIterator;
			}
			else {
				insertElement(copyNode, res, *otherIterator);
				++otherIterator;
			}
			res.count++;
		}
		while (curIterator != end()) {
			res.insert(copyNode, *curIterator);
			copyNode = copyNode->right;
			++curIterator;
			res.count++;
		}
		while (otherIterator != end()) {
			res.insert(copyNode, *otherIterator);
			copyNode = copyNode->right;
			++otherIterator;
			res.count++;
		}
	}
	catch (const std::bad_alloc& e)
	{
		res.free(res.root);
		throw e;
	}
	return res;
}

IntegerSet IntegerSet::getIntersection(const IntegerSet& other) const
{
	if (other.count == 0 || count == 0) {
		return IntegerSet();
	}
	IntegerSet res;
	Iterator curIterator = begin();
	Iterator otherIterator = other.begin();
	
	Node* copyRoot = nullptr;
	while (curIterator != end() && otherIterator != other.end()) {
		if ((*curIterator) == (*otherIterator)) {
			try
			{
				if (copyRoot == nullptr) {
					res.insert(res.root, *curIterator);
					copyRoot = res.root;
				}
				else {
					copyRoot = copyRoot->right;
					res.insert(copyRoot, *curIterator);
				}
				res.count++;
				++curIterator;
				++otherIterator;
			}
			catch (const std::bad_alloc& e)
			{
				res.free(res.root);
				throw e;
			}
		}
		else if ((*curIterator) < (*otherIterator)) {
			++curIterator;
		}
		else {
			++otherIterator;
		}
	}
	return res;
}

void IntegerSet::deserialize(std::istream& in)
{
	int size;
	in >> size;
	std::vector<int> elements(size);
	if (size == 0) {
		return;
	}
	for (size_t i = 0; i < size; i++) {
		in >> elements[i];
	}
	if (!isSorted(elements)) {
		throw std::runtime_error("Elements must be ascending");
	}
	try {
		createBST(root, elements, 0, elements.size() - 1);
	}
	catch (const std::bad_alloc& e) {
		free(root);
		throw e;
	}
}

void IntegerSet::serialize(std::ostream& out) const
{
	out << count;
	serialize(root, out);
}
