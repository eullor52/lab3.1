#pragma once

#include "exceptions.hpp"
#include "array.hpp"

template <typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node() : next(nullptr) {}
        Node(const T& d, Node* n = nullptr) : data(d), next(n) {}
        Node(T&& d, Node* n = nullptr) : data(d), next(n) {}
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
        
        Node* currentOld = other.head->next;
        Node* tail = head;

        while (currentOld) {
            tail->next = new Node(currentOld->data);
            tail = tail->next;
            currentOld = currentOld->next;
        }
    }
        

    ~Stack() {
        if (!head) return;
        Node* prev = head;
        Node* current = prev->next;
        while (current) {
            delete prev;
            prev = current;
            current = current->next;
        }
        delete prev;
    }

    void push(const T& elem) {
        Node* NewNode = new Node(elem);
        if (!head) {
            head = NewNode;
            return;
        }
        NewNode->next = head;
        head = NewNode;
    }

    void push(T&& elem) {
        Node* NewNode = new Node(std::move(elem));
        if (!head) {
            head = NewNode;
            return;
        }
        NewNode->next = head;
        head = NewNode;
    }

    void pop() {
        if (!head) throw LogicErrorException("Стек пуст");
        Node* second = head->next;
        delete head;
        head = second;
    }

    size_t size() const {
        size_t size = 0;
        Node* current = head;
        while (current) {
            ++size;
            current = current->next;
        }
        return size;
    }

    T top() const {
        if (!head) {
            throw OutOfRangeException("Стек пуст.");
        }
        return head->data;
    }

    bool empty() const {
        return head == nullptr;
    }

    Stack& operator=(const Stack& other) {
        if (this == &other) {
            return *this;
        }

        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;

        if (!other.head) {
            return *this;
        }

        head = new Node(other.head->data);
        
        Node* currentOld = other.head->next;
        Node* tail = head;

        while (currentOld) {
            tail->next = new Node(currentOld->data);
            tail = tail->next;
            currentOld = currentOld->next;
        }

        return *this;
    }
};