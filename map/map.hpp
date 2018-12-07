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
#include <iostream>
#include <cassert>
using namespace std;

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
	bool DEBUG = false;

	// private member class
	class node {
	public:
		value_type *value;
		color_type color;
		node *left, *right;
		node *prev, *next;
		node(value_type *v, color_type c, node *l=nullptr, node *r=nullptr):
			value(v), color(c), left(l), right(r) {}
		// copy from other node: only copy value and color,
		// and initialize others with nullptr
		node(const node &other):
			color(other.color), left(other.left), right(other.right) {
			value = new value_type(*other.value);
		}
		~node() {
			delete value;
		}
		void set_value(value_type *other) {
			delete value;
			value = new value_type(*other);
		}
	} *root, *first, *last, *np, *nn;
	// first: prev of the first actual node; last: next of the last actual node
	// we don't need to update first or last, but we only need to create and destroy them

	// utility functions
	void swap_nodes(node *a, node *b)
	{
		if(a == np)
			np = b;// cout << "np from " << a->value->first.val << " to " << b->value->first.val << endl;
		else if(b == np)
			np = a;// cout << "np from " << b->value->first.val << " to " << a->value->first.val << endl;
		if(a == nn)
			nn = b;// cout << "nn from " << a->value->first.val << " to " << b->value->first.val << endl;
		else if(b == nn)
			nn = a;// cout << "nn from " << b->value->first.val << " to " << a->value->first.val << endl;
		assert(_lt(a->value->first, b->value->first));
		//assert(a->prev && a->next && b->prev && b->next);
		if(a->next == b)
		{  // p<->b<->a<->n
			node *p = a->prev, *n = b->next;
			if(p) p->next = b;
			b->next = a;
			a->next = n;
			if(n) n->prev = a;
			a->prev = b;
			b->prev = p;
		}
		else if(a->next == nullptr)
		{  // p<->a<->n  b
			node *p = b->prev, *n = b->next;
			if(p) p->next = a;
			a->next = n;
			b->next = nullptr;
			b->prev = nullptr;
			if(n) n->prev = a;
			a->prev = p;
		}
		else if(a->next->next == b)
		{  // p<->b<->m<->a<->n
			node *p = a->prev, *m = a->next, *n = b->next;
			if(p) p->next = b;
			b->next = m;
			m->next = a;
			a->next = n;
			if(n) n->prev = a;
			a->prev = m;
			m->prev = b;
			b->prev = p;
		}
		else
		{  // ap<->b<->an<->...<->bp<->a<->bn
			node *ap = a->prev, *an = a->next;
			node *bp = b->prev, *bn = b->next;
			if(ap) ap->next = b;
			b->next = an;
			if(bp) bp->next = a;
			a->next = bn;
			if(bn) bn->prev = a;
			a->prev = bp;
			if(an) an->prev = b;
			b->prev = ap;
		}
	}

	bool _lt(const Key &a, const Key &b) const {
		return cmp_ins(a, b);
	}

	bool _le(const Key &a, const Key &b) const {
		return !cmp_ins(b, a);
	}

	bool _gt(const Key &a, const Key &b) const {
		return cmp_ins(b, a);
	}

	bool _ge(const Key &a, const Key &b) const {
		return !cmp_ins(a, b);
	}

	bool _eq(const Key &a, const Key &b) const {
		return !(cmp_ins(a, b) || cmp_ins(b, a));
	}

	bool _ne(const Key &a, const Key &b) const {
		return cmp_ins(a, b) || cmp_ins(b, a);
	}

	// linked list
	void _init() {
		// initialize first and last, members of whom do not matter,
		// except for first.prev and last.next
		first = new node(new value_type(-1, T()), black);
		last = new node(new value_type(-1, T()), black);
		first->prev = nullptr;
		first->next = last;
		last->prev = first;
		last->next = nullptr;
	}

	// RED BLACK TREE
	void _make_empty(node *t) {
		if(t == nullptr) return;
		_make_empty(t->left);
		_make_empty(t->right);
		delete t;
	}

	node *_find(const Key &key) const {
		node *t = root;
		while(t != nullptr && _ne(t->value->first, key))
		{
			if(_gt(t->value->first, key)) t = t->left;
			else t = t->right;
		}
		if(t == nullptr) t = last;  // not found it? Replace it with last;
		return t;
	}

	node *_index(const Key &key) const {  // returns a ptr for insertion
		node *t = root;
		while(t != nullptr && _ne(t->value->first, key))
		{
			if(_gt(t->value->first, key)) t = t->left;
			else t = t->right;
		}
		return t;
	}

	// rotations
	void LL(node *g) {
		node *p = g->left, *t = p->left;
		swap_nodes(p, g);
		node tmp = *g;
		g->set_value(p->value);
		g->left = t;
		g->right = p;
		p->set_value(tmp.value);
		p->left = p->right;
		p->right = tmp.right;
		//cout << "LL" << endl;
	}

	void LR(node *g) {
		node *p = g->left, *t = p->right;
		swap_nodes(t, g);
		node tmp = *g;
		g->set_value(t->value);
		g->right = t;
		p->right = t->left;
		t->set_value(tmp.value);
		t->left = t->right;
		t->right = tmp.right;
		//cout << "LR" << endl;
	}

	void RR(node *g) {
		node *p = g->right, *t = p->right;
		swap_nodes(g, p);
		node tmp = *g;
		g->set_value(p->value);
		g->right = t;
		g->left = p;
		p->set_value(tmp.value);
		p->right = p->left;
		p->left = tmp.left;
		//cout << "RR" << endl;
	}

	void RL(node *g) {
		node *p = g->right, *t = p->left;
		swap_nodes(g, t);
		node tmp = *g;
		g->set_value(t->value);
		g->left = t;
		p->left = t->right;
		t->set_value(tmp.value);
		t->right = t->left;
		t->left = tmp.left;
		//cout << "RL" << endl;
	}

	node *_insert(node *n, value_type *value) {
		++len;
		node *t, *p, *g;
		
		if(root == nullptr)
		{
			root = new node(value, black);
			// insert root between first and last
			root->prev = first;
			root->next = last;
			first->next = root;
			last->prev = root;
			return root;
		}

		p = g = t = root;
		while(true)
		{
			if(t)
			{
				if(t->left && t->left->color == red && t->right && t->right->color == red)
				{
					t->left->color = t->right->color = black;
					t->color = red;
					_insert_adjust(g, p, t);
				}
				g = p;
				p = t;
				t = (_gt(t->value->first, value->first) ? t->left : t->right);
			}
			else
			{
				t = new node(value, red);
				if(_lt(value->first, p->value->first))
				{
					p->left = t;
					// insert t between p and p->prev
					t->prev = p->prev;
					t->next = p;
					p->prev->next = t;
					p->prev = t;
				}
				else
				{
					p->right = t;
					// insert t between p and p->next
					t->prev = p;
					t->next = p->next;
					p->next->prev = t;
					p->next = t;
				}
				_insert_adjust(g, p, t);
				root->color = black;
				return t;
			}
		}
	}

	void _insert_adjust(node *g, node *p, node *t) {
		if(p->color) return;
		if(p == root)
		{
			p->color = black;
			return;
		}
		if(g->left == p)
			if(p->left == t) LL(g);
			else LR(g);
		else
			if(p->right == t) RR(g);
			else RL(g);
	}

	void _erase(node *n) {
		--len;

		DEBUG = (n->value->first.val == -1);
		const Key &x = n->value->first;
		const Key *del = new Key(n->value->first);
		node *t, *p, *c;
		bool flag = false;
		// a<->b<->c<->d
		np = n->prev;
		nn = n->next;
		//np->next = nullptr;
		//nn->prev = nullptr;
		//n->prev = nullptr;
		//n->next = nullptr;
		// a-> b <-c<->d
		/*np = n->prev;
		nn = n->next;
		np->next = nn;
		nn->prev = np;
		n->prev = nullptr;
		n->next = nullptr;*/
		//cout << "delete " << n->value->first.val << ", prev: " << np->value->first.val
		//     << ", next: " << nn->value->first.val << endl;
		// a<->c<->d  b
		// np  nn     n

		if(root == nullptr) return;
		if(_eq(root->value->first, x) && root->left == nullptr && root->right == nullptr)
		{
			// cut the prev and next of root
			root->prev->next = root->next;
			root->next->prev = root->prev;
			delete root;
			root = nullptr;
			delete del;
			return;
		}

		p = c = t = root;
		while(true)
		{
			_erase_adjust(p, c, t, *del);
			if(_eq(c->value->first, *del) && c->left && c->right)
			{
				node *tmp = c->right;
				while(tmp->left) tmp = tmp->left;  // find the stand
				assert(c->value->first.val+1 == tmp->value->first.val);
				// a-> b <-c<->d
				// np  c tmp/nn
				//c->prev = np;
				//c->next = nn;
				np = tmp->prev;
				nn = tmp->next;
				//tmp->prev = nullptr;
				//tmp->next = nullptr;
				// a<->b-> c <-d
				//   c/np tmp  nn
				// a<->c<->d  b
				// np nn/tmp  c
				/*assert(nn != c);
				c->prev = tmp->prev;
				c->next = tmp->next;
				c->prev->next = c;
				c->next->prev = c;
				tmp->prev = nullptr;
				tmp->next = nullptr;
				np = c->prev;
				nn = c->next;*/
				//cout << "change to " << tmp->value->first.val << ", prev: " << np->value->first.val
		     	//	 << ", next: " << nn->value->first.val << endl;
				// a<->b<->d  c
				c->set_value(tmp->value);
				delete del;
				del = new Key(tmp->value->first);  // now delete the stand...
				p = c;
				c = c->right;  // on the right tree
				t = p->left;
				continue;
			}

			if(_eq(c->value->first, *del))
			{
				//assert(_lt(c->prev->value->first, c->next->value->first));
				//cout << c->prev->value->first.val << ' ' << c->value->first.val << ' '
				//	 << c->next->value->first.val << endl;
				// cut the prev and next of c
				if(flag)
				{
					c->prev->next = c->next;
					c->next->prev = c->prev;
					flag = false;
				}
				//assert(_lt(c->prev->value->first, c->next->value->first));
				//cout << np->value->first.val << ' ' << c->value->first.val << ' '
				//     << nn->value->first.val << endl;
				np->next = nn;
				nn->prev = np;
				//cout << "delete " << c->value->first.val
				//	 << ", link " << np->value->first.val
				//	 << " and " << nn->value->first.val << endl;
				assert(nn != c);
				delete c;
				if(p->left == c) p->left = nullptr;
				else p->right = nullptr;
				root->color = black;
				np = nn = nullptr;
				delete del;
				return;
			}
			p = c;
			c = (_lt(*del, p->value->first) ? p->left : p->right);
			t = (c == p->left ? p->right : p->left);
			assert(c != nullptr);
		}
		np = nn = nullptr;
		delete del;
	}

	void _erase_adjust(node *&p, node *&c, node *&t, const Key &del) {
		if(c->color == red) return;
		if(c == root)
		{
			if(c->left && c->right && c->right->color == c->left->color)
			{
				c->color = red;
				c->left->color = c->right->color = black;
				return;
			}
		}
		
		if((c->left && c->left->color || c->left == nullptr)
		&& (c->right && c->right->color || c->right == nullptr))
		{
			if((t->left && t->left->color || t->left == nullptr)
			&& (t->right && t->right->color || t->right == nullptr))
			{
				p->color = black;
				t->color = c->color = red;
			}
			else
			{
				if(p->left == t)
				{
					if(t->left && t->left->color == red)
					{
						t->left->color = black;
						LL(p);
						p = t;
					}
					else
					{
						LR(p);
						p = p->right;
						p->color = black;
					}
				}
				else if(t->right && t->right->color == red)
				{
					t->right->color = black;
					RR(p);
					p = t;
				}
				else
				{
					RL(p);
					p = p->left;
					p->color = black;
				}
				c->color = red;
			}
		}
		else
		{
			if(_eq(c->value->first, del))
			{
				if(c->left && c->right)
				{
					if(c->right->color == black)
					{
						LL(c);
						c = c->right;
					}
					return;
				}
				if(c->left)
				{
					LL(c);
					p = c;
					c = c->right;
				}
				else
				{
					RR(c);
					p = c;
					c = c->left;
				}
			}
			else
			{
				p = c;
				c = (_lt(del, p->value->first) ? p->left : p->right);
				t = (c == p->left ? p->right : p->left);
				if(c->color == black)
				{
					if(t == p->right) RR(p);
					else LL(p);
					p = t;
					t = (c == p->left ? p->right : p->left);
					_erase_adjust(p, c, t, del);
				}
			}
		}
	}

	// build tree from another tree in inorder recursively
	node *_inorder(const map &other, node *cur, node *prev) {
		// visit nodes
		if(cur == nullptr) return nullptr;
		node *left = _inorder(other, cur->left, prev);
		node *n = new node(*cur);
		n->prev = np;
		np->next = n;
		np = n;
		node *right = _inorder(other, cur->right, n);
		// create a new node, and link these pointers
		n->left = left;
		n->right = right;
		// mark for special nodes: root
		if(cur == other.root) root = n;
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
			return *cur->value;
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
			return cur->value;
		}

		friend map;
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
			const node *cur;
			const map *container;
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
				return *cur->value;
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
				return cur->value;
			}

			friend map;
	};
	/**
	 * TODO two constructors
	 */
	map() {
		len = 0;
		root = nullptr;
		np = nn = nullptr;
		_init();
	}
	map(const map &other) {
		np = nn = nullptr;
		_init(); // but first, init first and last
		len = other.len;
		if(other.empty())
			return;  // other.root doesn't even exist!
		np = first;
		_inorder(other, other.root, nullptr);
		np->next = last;
		last->prev = np;
		np = nullptr;
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {  // clear() and just do what map*(const map &other) do
		if(first == other.first)  // avoid copying itself
			return *this;
		clear();
		len = other.len;
		if(other.empty())
			return *this;
		np = first;
		_inorder(other, other.root, nullptr);
		np->next = last;
		last->prev = np;
		np = nullptr;
		return *this;
	}
	/**
	 * TODO Destructors
	 */
	~map() {
		_make_empty(root);
		// also, delete first and last
		delete first;
		delete last;
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
		return n->value->second;
	}
	const T & at(const Key &key) const {
		node *n = _find(key);
		if(n == last) throw index_out_of_bound();
		return n->value->second;
	}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		node *n = _index(key);
		if(n == nullptr) n = _insert(n, new value_type(key, T()));
		return n->value->second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		node *n = _index(key);
		if(n == nullptr) throw index_out_of_bound();
		return n->value->second;
	}
	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
		iterator iter;
		iter.cur = first->next;
		iter.container = this;
		return iter;
	}
	const_iterator cbegin() const {
		const_iterator iter;
		iter.cur = first->next;
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
		len = 0;
		_make_empty(root);
		first->next = last;
		last->prev = first;
		root = nullptr;
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		node *n = _index(value.first);
		node *ret;
		bool success;
		if(n)  // the key has exsisted, reject this operation
		{
			ret = n;
			success = false;
		}
		else
		{
			ret = _insert(n, new value_type(value));
			success = true;
		}
		iterator iter;
		iter.cur = ret;
		iter.container = this;
		return pair<iterator, bool>(iter, success);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if(pos.container != this || pos.cur == last) throw invalid_iterator();
		node *n = pos.cur;
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
		if(n) return 1;
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
