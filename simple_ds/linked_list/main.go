package LinkedList

import (
	"errors"
)

type Node[T any] struct {
	val  T
	next *Node[T]
}

type LinkedList[T any] struct {
	head *Node[T]
	size uint32
}

func (l *LinkedList[T]) insertHead(data T) {
	new := &Node[T]{data, nil}

	if l.head == nil {
		l.head = new
	} else {
		tmp := l.head
		l.head = new
		l.head.next = tmp
	}
	l.size++
}

func (l *LinkedList[T]) insertBack(data T) {
	new := &Node[T]{data, nil}

	if l.head == nil {
		l.head = new
	} else {
		cur := l.head
		for cur.next != nil {
			cur = cur.next
		}
		cur.next = new
	}
	l.size++
}

func (l *LinkedList[T]) insert(pos uint32, data T) error {
	if pos > l.size {
		return errors.New("pos error")
	}
	if pos == 0 {
		l.insertHead(data)
		return nil
	} else if pos == (l.size) {
		l.insertBack(data)
		return nil
	}

	new := &Node[T]{data, nil}
	cur := l.head
	var ix uint32 = 0

	for ix = 0; ix < pos; ix++ {
		cur = cur.next
	}

	new.next = cur.next
	cur.next = new

	l.size++

	return nil
}

func (l *LinkedList[T]) popHead() (T, error) {
	var zero T
	if l.head == nil {
		return zero, errors.New("popHead: list is empty")
	}
	res := l.head.val
	tmp := l.head
	l.head = tmp.next
	l.size--
	return res, nil
}

func (l *LinkedList[T]) popBack() (T, error) {
	var zero T
	if l.head == nil {
		return zero, errors.New("popBack: list is empty")
	}
	if l.head.next == nil {
		res := l.head.val
		l.head = nil
		l.size--
		return res, nil
	}
	cur := l.head
	var pred *Node[T]
	for cur.next != nil {
		pred = cur
		cur = cur.next
	}
	res := cur.val
	pred.next = nil
	l.size--
	return res, nil
}

func (l *LinkedList[T]) delete(pos uint32) (T, error) {
	if pos > l.size {
		var tmp T
		return tmp, errors.New("pos error")
	}
	if pos == 0 {
		return l.popHead()
	} else if pos == l.size-1 {
		return l.popBack()
	}
	cur := l.head
	var ix uint32
	for ix = 0; ix < pos-1; ix++ {
		cur = cur.next
	}
	to_del := cur.next
	cur.next = to_del.next
	res := to_del.val
	l.size--
	return res, nil
}

func (l *LinkedList[T]) toSlice() ([]T, error) {
	res := make([]T, 0, l.size)

	cur := l.head

	for cur != nil {
		res = append(res, cur.val)
		cur = cur.next
	}

	return res, nil
}

func (l *LinkedList[T]) forEach(callback func(T) error) error {
	cur := l.head
	for cur != nil {
		if err := callback(cur.val); err != nil {
			return err
		}
		cur = cur.next
	}
	return nil
}
