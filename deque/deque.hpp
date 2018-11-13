#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

const size_t block_size = 512;

template<class T>
class deque {
	// private members

	// utility class

	// node of the linked list, each represent a block of memory
	// elements in this block is stored just like a vector (expect for double_space)
	class node {
	public:
		T *data;  // elements stored in the block
		size_t len;  // number of the elements
		const size_t size;  // number of the maximum elements that can be stored
		node *next;  // next node
		node *prev;  // prev node

		node(node *p=nullptr, node *n=nullptr):
			next(n), prev(p), len(0), size(block_size) {
			data = new T[size];
		}
	} *head, *tail;
	// head: the head of the block link (head doesn't store elements)
	// tail: the tail of the block link (tail doesn't store elements)
	// nullptr <- head <-> node <-> ... <-> node <-> tail -> nullptr

	size_t len;  // to make size() O(1)

	// utility functions

	// initialize head and tail
	void node_init()
	{
		// head = new node; tail = new node(head); head->next = tail;
		tail = (head = new node)->next = new node(head);
	}

	// add a new node as pos->prev and return it
	node *node_before(node *pos)
	{
		pos->prev->prev->next = pos->prev = new node(pos->prev, pos);
		return pos->prev;
	}

	// add a new node as pos->next and return it
	node *node_after(node *pos)
	{
		pos->next->next->prev = pos->next = new node(pos, pos->next);
		return pos->next;
	}

	// remove a node
	void node_remove(node *pos)
	{
		pos->prev->next = pos->next;
		pos->next->prev = pos->prev;
		delete[] pos;
	}

public:
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
		T* first;
		T* last;
		T* cur;
		node *block;
		deque *container;

		// utility functions
		void set_block(node *b)
		{
			block = b;
			first = b->data;
			last = first+b->size;
		}
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const {
			//TODO
			iterator ret;
			ret.block = block;

			int idx = n+(cur-first);
			while(idx >= ret.block->len)
			{
				idx -= ret.block->len;
				ret.block = ret.block->next;
			}
			ret.first = ret.block->data;
			ret.last = ret.first+ret.block->len;
			ret.cur = ret.first+idx;

			return ret;
		}
		iterator operator-(const int &n) const {
			//TODO
			iterator ret;
			ret.block = block;

			int idx = n+(cur-first);
			while(idx > ret.block->len)
			{
				idx -= ret.block->len;
				ret.block = ret.block->prev;
			}
			ret.first = ret.block->data;
			ret.last = ret.first+ret.block->len;
			ret.cur = ret.last-idx;

			return ret;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
			if(container != rhs.container)
				throw invalid_iterator();

			int ret;
			if(block == rhs.block)  // in the same block
			{
				ret = cur-rhs.cur;
			}
			else
			{
				node *cur;
				// find rhs to the tail
				ret = 0;
				for(cur = block->next; cur != rhs.block && cur != tail; cur = cur->next)
					ret += cur->len;
				if(cur == tail)  // not found
				{
					// find rhs to the head
					ret = 0;
					for(cur = block->tail; cur != rhs.block && cur != head; cur = cur->prev)
						ret += cur->len;
					// rhs before this
					ret += (cur-first+1) + (rhs.last-rhs.cur);
				}
				else  // rhs after this
				{
					ret += (last-cur) + (rhs.cur-rhs.first+1);
				}
			}
			return ret;
		}
		iterator operator+=(const int &n) {
			//TODO
			int idx = n+(cur-first);
			while(idx >= block->len)
			{
				idx -= block->len;
				block = block->next;
			}
			first = block->data;
			last = first+block->len;
			cur = first+idx;

			return *this;
		}
		iterator operator-=(const int &n) {
			//TODO
			int idx = n+(last-cur);
			while(idx > block->len)
			{
				idx -= block->len;
				block = block->prev;
			}
			first = block->data;
			last = first+block->len;
			cur = last-idx;

			return *this;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator ret = *this;
			if(++cur == last)
			{
				block = block->next;
				first = block->data;
				last = first+block->len;
				cur = first;
			}
			return ret;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			if(++cur == last)
			{
				block = block->next;
				first = block->data;
				last = first+block->len;
				cur = first;
			}
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator ret = *this;
			if(cur-- == first)
			{
				block = block->prev;
				first = block->data;
				last = first+block->len;
				cur = last-1;
			}
			return ret;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			if(cur-- == first)
			{
				block = block->prev;
				first = block->data;
				last = first+block->len;
				cur = last-1;
			}
			return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const {
			return *cur;
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
			return cur;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
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
		friend deque;
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
			const T* first;
			const T* last;
			const T* cur;
			const node *block;
			const deque *container;
		public:
			const_iterator() {
				// TODO
				// nothing to do
			}
			const_iterator(const const_iterator &other) {
				// TODO
				first = other.first;
				last = other.last;
				cur = other.cur;
				block = other.block;
				container = other.container;
			}
			const_iterator(const iterator &other) {
				// TODO
				first = other.first;
				last = other.last;
				cur = other.cur;
				block = other.block;
				container = other.container;
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.

			const_iterator operator+(const int &n) const {
				const_iterator ret;
				ret.block = block;
				int idx = n+(cur-first);
				while(idx >= ret.block->len) {
					idx -= ret.block->len;
					block = ret.block->next;
				}
				ret.first = ret.block->data;
				ret.last = ret.first+ret.block->len;
				ret.cur = ret.first+idx;
				return ret;
			}
			const_iterator operator-(const int &n) const {
				const_iterator ret;
				ret.block = block;
				int idx = n+(cur-first);
				while(idx > ret.block->len) {
					idx -= ret.block->len;
					block = ret.block->prev;
				}
				ret.first = ret.block->data;
				ret.last = ret.first+ret.block->len;
				ret.cur = ret.last-idx;
				return ret;
			}
			int operator-(const const_iterator &rhs) const {
				if(container != rhs.container) throw invalid_iterator();
				int ret;
				if(block == rhs.block) ret = cur-rhs.cur;
				else {
					node *cur;
					ret = 0;
					for(cur = block->next; cur != rhs.block && cur != tail; cur = cur->next) ret += cur->len;
					if(cur == tail) {
						ret = 0;
						for(cur = block->tail; cur != rhs.block && cur != head; cur = cur->prev) ret += cur->len;
						ret += (cur-first+1) + (rhs.last-rhs.cur);
					}
					else ret += (last-cur) + (rhs.cur-rhs.first+1);
				}
				return ret;
			}
			const_iterator operator+=(const int &n) {
				int idx = n+(cur-first);
				while(idx >= block->len) {
					idx -= block->len;
					block = block->next;
				}
				first = block->data;
				last = first+block->len;
				cur = first+idx;
				return *this;
			}
			const_iterator operator-=(const int &n) {
				int idx = n+(last-cur);
				while(idx > block->len) {
					idx -= block->len;
					block = block->prev;
				}
				first = block->data;
				last = first+block->len;
				cur = last-idx;
				return *this;
			}
			const_iterator operator++(int) {
				const_iterator ret = *this;
				if(++cur == last) {
					block = block->next;
					first = block->data;
					last = first+block->len;
					cur = first;
				}
				return ret;
			}
			const_iterator& operator++() {
				if(++cur == last) {
					block = block->next;
					first = block->data;
					last = first+block->len;
					cur = first;
				}
				return *this;
			}
			const_iterator operator--(int) {
				iterator ret = *this;
				if(cur-- == first) {
					block = block->prev;
					first = block->data;
					last = first+block->len;
					cur = last-1;
				}
				return ret;
			}
			const_iterator& operator--() {
				if(cur-- == first) {
					block = block->prev;
					first = block->data;
					last = first+block->len;
					cur = last-1;
				}
				return *this;
			}
			const T & operator*() const {
				return *cur;
			}
			const T * operator->() const noexcept {
				return cur;
			}
			bool operator==(const const_iterator &rhs) const {
				return cur == rhs.cur;
			}
			bool operator==(const iterator &rhs) const {
				return cur == rhs.cur;
			}
			bool operator!=(const const_iterator &rhs) const {
				return cur != rhs.cur;
			}
			bool operator!=(const iterator &rhs) const {
				return cur != rhs.cur;
			}
			friend deque;
	};
	/**
	 * TODO Constructors
	 */
	deque() {
		node_init();
	}
	deque(const deque &other) {
		node_init();
		node *block;
		for(node *cur = other.head->next; cur != other.tail; cur = cur->next)
		{
			block = node_before(tail);
			for(int i = 0; i < cur->size; ++i)
				block->data[i] = cur->data[i];
			block->len = cur->len;
		}
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() {
		node *p = head, *q;
		while(p)
		{
			q = p->next;
			delete[] p->data;
			delete p;
			p = q;
		}
	}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {  // just do what deque*(const deque &other) do
		node_init();
		node *block;
		for(node *cur = other.head->next; cur != other.tail; cur = cur->next)
		{
			block = node_before(tail);
			for(int i = 0; i < cur->size; ++i)
				block->data[i] = cur->data[i];
			block->len = cur->len;
		}
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
		if(pos >= len)
			throw index_out_of_bound();

		size_t idx;
		node *cur;

		if(pos < (len>>1))  // serach from head
		{
			idx = 0;
			for(cur = head->next; idx <= pos; cur = cur->next)
				idx += cur->len;
			cur = cur->prev;  // back to the prev one
			return cur->data[pos - (idx - cur->len)];
		}
		else  // search from tail
		{
			idx = len;
			for(cur = tail->prev; idx > pos; cur = cur->prev)
				idx -= cur->len;
			cur = cur->next;  // back to the next one
			return cur->data[pos - idx];
		}
	}
	const T & at(const size_t &pos) const {  // just do what T &at() do
		if(pos >= len) throw index_out_of_bound();
		size_t idx;
		node *cur;
		if(pos < (len>>1)) {
			idx = 0;
			for(cur = head->next; idx <= pos; cur = cur->next) idx += cur->len;
			cur = cur->prev;
			return cur->data[pos - (idx - cur->len)];
		}
		else {
			idx = len;
			for(cur = tail->prev; idx > pos; cur = cur->prev) idx -= cur->len;
			cur = cur->next;
			return cur->data[pos - idx];
		}
	}
	T & operator[](const size_t &pos) {  // just do what T &at() do
		if(pos >= len) throw index_out_of_bound();
		size_t idx;
		node *cur;
		if(pos < (len>>1)) {
			idx = 0;
			for(cur = head->next; idx <= pos; cur = cur->next) idx += cur->len;
			cur = cur->prev;
			return cur->data[pos - (idx - cur->len)];
		}
		else {
			idx = len;
			for(cur = tail->prev; idx > pos; cur = cur->prev) idx -= cur->len;
			cur = cur->next;
			return cur->data[pos - idx];
		}
	}
	const T & operator[](const size_t &pos) const {  // just do what T &at() do
		if(pos >= len) throw index_out_of_bound();
		size_t idx;
		node *cur;
		if(pos < (len>>1)) {
			idx = 0;
			for(cur = head->next; idx <= pos; cur = cur->next) idx += cur->len;
			cur = cur->prev;
			return cur->data[pos - (idx - cur->len)];
		}
		else {
			idx = len;
			for(cur = tail->prev; idx > pos; cur = cur->prev) idx -= cur->len;
			cur = cur->next;
			return cur->data[pos - idx];
		}
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		return head->next->data[0];
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		return tail->prev->data[tail->prev->len-1];
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		iterator iter;
		iter.set_block(head->next);
		iter.cur = iter.first;
		iter.container = this;
		return iter;
	}
	const_iterator cbegin() const {
		const_iterator iter;
		iter.set_block(head->next);
		iter.cur = iter.first;
		iter.container = this;
		return iter;
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		iterator iter;
		iter.set_block(tail);
		iter.cur = iter.first;
		iter.container = this;
		return iter;
	}
	const_iterator cend() const {
		const_iterator iter;
		iter.set_block(tail);
		iter.cur = iter.first;
		iter.container = this;
		return iter;
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
		return len == 0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return len;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		node *p = head->next, *q;
		while(p != tail)
		{
			q = p->next;
			delete[] p->data;
			delete p;
			p = q;
		}

		len = 0;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
		if(pos.container != this || pos.block == head)
			throw invalid_iterator();  // ! it's still possible that block is a wild pointer

		node *block = pos.block;
		if(block->len == block->size)  // block is full
		{
			// block splits into 2 blocks from the insert point
			// ... <-> [1 2 3 4 5 6 7 8] <-> ... insert an x before 4 =>
			// ... <-> [1 2 3 x o o o o] <-> [4 5 6 7 8 o o o] <-> ...
			node *ins = new node(block, block->next);
			block->next = ins;
			ins->next->prev = ins;

			int i, j, idx = pos.cur-pos.first;
			for(i = idx, j = 0; i < block->size; ++i, ++j)
			{
				ins->data[j] = block->data[i];
			}
			block->data[idx] = value;
			block->len = idx+1;
			ins->len = j;
		}
		else  // block is not full
		{
			// directly insert the element just as vector do
			int idx = pos.cur-pos.first;
			for(int i = block->len; i > idx; ++i)
			{
				block->data[i] = block->data[i-1];
			}
			block->data[idx] = value;
			++block->len;
		}

		++len;

		return pos;
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
		if(empty() || pos.container != this || pos.block == tail)
			throw invalid_iterator();  // ! it's still possible that block is a wild pointer
		
		node *block = pos.block;
		// directly erase the element just as vector do
		for(int i = pos.cur-pos.first; i < block->len-1; ++i)
		{
			block->data[i] = block->data[i+1];
		}
		--block->len;
		if(block->len == 0)  // block is empty now
		{
			pos.set_block(block->next);
			pos.cur = pos.first;
			node_remove(block);
		}
		else
		{
			++pos;  // iterator points to the following element
		}

		--len;

		return pos;
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
		node *block = tail->prev;
		if(block->len == block->size)  // prev of tail is full
		{
			// add a new block
			block = node_before(tail);
			block->data[0] = value;
			block->len = 1;
		}
		else  // not full
		{
			block->data[block->len] = value;
			++block->len;
		}

		++len;
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
		if(empty())
			throw container_is_empty();

		node *block = tail->prev;
		--block->len;
		if(block->len == 0)  // prev of tail is empty now
		{
			// remove this block
			node_remove(block);
		}

		--len;
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
		node *block = head->next;
		if(block->len == block->size)  // next of head is full
		{
			// add a new block
			block = node_after(head);
			block->data[0] = value;
			block->len = 1;
		}
		else  // not full
		{
			for(int i = block->len; i > 0; --i)
			{
				block->data[i] = block->data[i-1];
			}
			block->data[0] = value;
			++block->len;
		}

		++len;
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
		if(empty())
			throw container_is_empty();

		node *block = head->next;
		for(int i = 0; i < block->len-1; ++i)
		{
			block->data[i] = block->data[i+1];
		}
		--block->len;
		if(block->len == 0)  // next of head is empty now
		{
			// remove this block
			node_remove(block);
		}

		--len;
	}
};

}

#endif
