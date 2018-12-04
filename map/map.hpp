/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {

public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;

private:
	// private members
	Compare cmp_ins;
	size_t len;
	enum color_type {red, black};

	// private member class
	class node {
	public:
		value_type value;
		color_type color;
		node *parent, *left, *right;
		node *prev, *next;
		node(const value_type &v, color_type c, node *p=nullptr, node *l=nullptr, node *r=nullptr):
			value(v), color(c), parent(p), left(l), right(r) {}
		// copy from other node: only copy value and color, and initialize others with nullptr
		node(const node &other): value(other.value), color(other.color) {
			parent = left = right = nullptr;
			prev = next = nullptr;
		}
	} *root, *first, *last;

	// utility functions
	bool _lt(const Key &a, const Key &b) {
		return cmp_ins(a, b);
	}

	bool _le(const Key &a, const Key &b) {
		return !cmp_ins(b, a);
	}

	bool _gt(const Key &a, const Key &b) {
		return cmp_ins(b, a);
	}

	bool _ge(const Key &a, const Key &b) {
		return !cmp_ins(a, b);
	}

	bool _eq(const Key &a, const Key &b) {
		return !(cmp_ins(a, b) || cmp_ins(b, a));
	}

	bool _ne(const Key &a, const Key &b) {
		return cmp_ins(a, b) || cmp_ins(b, a);
	}

	void _make_empty(node *t) {
		if(t == nullptr) return;
		_make_empty(t->left);
		_make_empty(t->right);
		delete t;
	}

	node *_find(const Key &key) {
		node *t = root;
		while(t && _ne(t->value.first, key))
		{
			if(_gt(t->value.first, key)) t = t->left;
			else t = t->right;
		}
		if(t == nullptr) t = last;  // not found it? Replace it with last;
		return t;
	}

	node *&_index(const Key *key) {  // returns a ptr for insertion
		node *t = root;
		while(t && _ne(t->value.first, key))
		{
			if(_gt(t->value.first, key)) t = t->left;
			else t = t->right;
		}
		return t;
	}

	node *&_insert(const node *&n, const value_type &value) {
		++len;
		// TODO
	}

	void _erase(const node *&n) {
		--len;
		// TODO
	}

	// build tree from another tree in inorder recursively
	node *_inorder(const map &other, node *cur, node *prev) {
		// visit nodes
		if(cur == nullptr) return nullptr;
		node *left = _inorder(other, cur->left, prev);
		node *n = new node(cur);
		node *right = _inorder(other, cur->right, n);
		// create a new node, and link these pointers
		n->left = left;
		if(left) left->parent = n;
		n->right = right;
		if(right) right->parent = n;
		n->prev = left;
		if(left) left->next = n;
		// mark for special nodes: root, first
		if(cur == other.root) root = n;
		else if(cur == other.first) first = n;
		// last does not exist in the tree
		if(cur->next == other.last)
		{
			last = n->next = new node(other.last);
			last->prev = n;
		}
		// return this node for connection
		return n;
	}

public:
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		node *cur;
		map *container;
	public:
		iterator() {
			// TODO
			cur = nullptr;
			container = nullptr;
		}
		iterator(const iterator &other) {
			// TODO
			cur = other.cur;
			container = other.container;
		}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator iter = *this;
			cur = cur->next;
			return iter;
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
			cur = cur->next;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator iter = *this;
			cur = cur->prev;
			return iter;
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
			cur = cur->prev;
			return *this;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {
			return cur->value;
		}
		bool operator==(const iterator &rhs) const {
			return cur == rhs.cur;
		}
		bool operator==(const const_iterator &rhs) const {
			return cur == rhs.cur;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return cur != rhs.cur;
		}
		bool operator!=(const const_iterator &rhs) const {
			return cur != rhs.cur;
		}

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			return &(cur->value);
		}

		friend map;
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
			node *cur;
			map *container;
		public:
			const_iterator() {
				// TODO
				cur = nullptr;
				container = nullptr;
			}
			const_iterator(const const_iterator &other) {
				// TODO
				cur = other.cur;
				container = other.container;
			}
			const_iterator(const iterator &other) {
				// TODO
				cur = other.cur;
				container = other.container;
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
			const_iterator operator++(int) {
				const_iterator iter = *this;
				cur = cur->next;
				return iter;
			}
			const_iterator & operator++() {
				cur = cur->ret;
				return *this;
			}
			const_iterator operator--(int) {
				iterator iter = *this;
				cur = cur->prev;
				return iter;
			}
			const_iterator & operator--() {
				cur = cur->prev;
				return *this;
			}
			const value_type & operator*() const {
				return cur->value;
			}
			bool operator==(const iterator &rhs) const {
				return cur == rhs.cur;
			}
			bool operator==(const const_iterator &rhs) const {
				return cur == rhs.cur;
			}
			bool operator!=(const iterator &rhs) const {
				return cur != rhs.cur;
			}
			bool operator!=(const const_iterator &rhs) const {
				return cur != rhs.cur;
			}
			const value_type* operator->() const noexcept {
				return &(cur->value);
			}

			friend map;
	};
	/**
	 * TODO two constructors
	 */
	map() {
		len = 0;
		root = nullptr;
		last = new node(value_type(-1, T()), black);  // members in last does not matter
		last.next = nullptr;  // except for next: it must be nullptr
		first = last;  // first is the same as last when map is empty
	}
	map(const map &other) {
		len = other.len;
		if(other.empty())
			return;  // other.root doesn't even exist!
		_inorder(other, other.root, nullptr);
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {  // clear() and just do what map*(const map &other) do
		if(head == other.head)  // avoid copying itself
			return *this;
		clear();
		if(other.empty())
			return *this;
		_inorder(other, other.root, nullptr);
	}
	/**
	 * TODO Destructors
	 */
	~map() {
		_make_empty(root);
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		node *n = _find(key);
		if(n == last) throw index_out_of_bound();
		return n->value.second;
	}
	const T & at(const Key &key) const {
		node *n = _find(key);
		if(n == last) throw index_out_of_bound();
		return n->value.second;
	}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		node *&n = _index(key);
		if(n == nullptr) n = _insert(n, value_type(key, T()));  // does T have a default constructor?
		return n->value.second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		node *&n = _index(key);
		if(n == nullptr) throw index_out_of_bound();
		return n->value.second;
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
		iterator iter;
		iter.cur = first;
		iter.container = this;
		return iter;
	}
	const_iterator cbegin() const {
		const_iterator iter;
		iter.cur = first;
		iter.container = this;
		return iter;
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
		iterator iter;
		iter.cur = last;
		iter.container = this;
		return iter;
	}
	const_iterator cend() const {
		const_iterator iter;
		iter.cur = last;
		iter.container = this;
		return iter;
	}
	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
		return len == 0;
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
		return len;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		_make_empty(root);
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		node *&n = _index(value.first);
		node *ret;
		bool success;
		if(n)  // the key has exsisted, reject this operation
		{
			ret = n;
			success = false;
		}
		else
		{
			ret = _insert(n, value);
			success = true;
		}
		iterator iter;
		iter.cur = ret;
		iter.container = *this;
		return pair(iter, success);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if(pos.container != this || pos.cur == last) throw invalid_iterator();
		node *&n = pos.cur->value;
		++pos;  // point to the next iterator
		_erase(n);
	}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
		node *n = _index(key);
		if(p) return 1;
		return 0;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		iterator iter;
		iter.cur = _find(key);
		iter.container = this;
		return iter;
	}
	const_iterator find(const Key &key) const {
		const_iterator iter;
		iter.cur = _find(key);
		iter.container = this;
		return iter;
	}
};

}

#endif
