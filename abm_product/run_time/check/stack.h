/*
* Copyright (c) 2011,公司名称
* All rights reserved.
* 
* 文件名称：stack.h 
* 摘    要：文件定义了stack 模板
* 
*
* 取代版本：1.0 
* 原作者  ：邱重阳
* 完成日期：2011年11月10日
*/
#ifndef __STACK_H_
#define __STACK_H_

namespace __HSS_Stack {

	template<typename T>
	class Stack {
	public:
		Stack(unsigned int uiSize = 64) {
			size = uiSize;
			data = new T[size];

			itop = 0;
		}
		virtual ~Stack();

		int push(T& node);

		T& pop();

		T& top();
		
		int empty();

	protected:
		T*	data;
		unsigned int itop;
		unsigned int size;
	};

	template<typename T>
	Stack<T>::~Stack() {
		size = 0;

		if (data) {
		   delete data;
		}
		data = NULL;
		itop	= 0;
	}


	template<typename T>
	int Stack<T>::push(T& node) {
		if (itop >= size) {
			return -1;
		}

		data[itop] = node;
		itop++;

		return 0;
	}

	template<typename T>
	T& Stack<T>::pop() {
		if (itop <= 0) {
			return data[0];
		}

		return data[--itop];
	}

	template<typename T>
	T& Stack<T>::top() {
		if (top <= 0) {
			return data[0];
		}

		return data[itop - 1];
	}

	template<typename T>
	int Stack<T>::empty() {
		return itop ? 0 : 1;
	}

}

using namespace __HSS_Stack;
#endif

