#pragma once

#include "exceptions.hpp"
#include "array.hpp"

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* nextNode;
        Node() : nextNode(nullptr) {}
        Node(const T& dataItem, Node* next = nullptr) : data(dataItem), nextNode(next) {}
        Node(T&& dataItem, Node* next = nullptr) : data(std::move(dataItem)), nextNode(next) {}
    };

    Node* head = nullptr;

public:
    Stack() = default;

    Stack(const Stack& other) {
        if (!other.head) {
            head = nullptr;
            return;
        }

        head = new Node(other.head->data);
        
        Node* currentOld = other.head->nextNode;
        Node* tail = head;

        while (currentOld) {
            tail->nextNode = new Node(currentOld->data);
            tail = tail->nextNode;
            currentOld = currentOld->nextNode;
        }
    }
        
    ~Stack() {
        if (!head) return;
        Node* previousNode = head;
        Node* currentNode = previousNode->nextNode;
        while (currentNode) {
            delete previousNode;
            previousNode = currentNode;
            currentNode = currentNode->nextNode;
        }
        delete previousNode;
    }

    void push(const T& element) {
        Node* newNode = new Node(element);
        if (!head) {
            head = newNode;
            return;
        }
        newNode->nextNode = head;
        head = newNode;
    }

    void push(T&& element) {
        Node* newNode = new Node(std::move(element));
        if (!head) {
            head = newNode;
            return;
        }
        newNode->nextNode = head;
        head = newNode;
    }

    void pop() {
        if (!head) throw LogicErrorException("Стек пуст");
        Node* secondNode = head->nextNode;
        delete head;
        head = secondNode;
    }

    size_t getSize() const {
        size_t sizeCount = 0;
        Node* currentNode = head;
        while (currentNode) {
            ++sizeCount;
            currentNode = currentNode->nextNode;
        }
        return sizeCount;
    }

    T peek() const {
        if (!head) {
            throw OutOfRangeException("Стек пуст.");
        }
        return head->data;
    }

    bool isEmpty() const {
        return head == nullptr;
    }

    Stack& operator=(const Stack& other) {
        if (this == &other) {
            return *this;
        }

        Node* currentNode = head;
        while (currentNode) {
            Node* nextNodeToKeep = currentNode->nextNode;
            delete currentNode;
            currentNode = nextNodeToKeep;
        }
        head = nullptr;

        if (!other.head) {
            return *this;
        }

        head = new Node(other.head->data);
        
        Node* currentOld = other.head->nextNode;
        Node* tail = head;

        while (currentOld) {
            tail->nextNode = new Node(currentOld->data);
            tail = tail->nextNode;
            currentOld = currentOld->nextNode;
        }

        return *this;
    }
};