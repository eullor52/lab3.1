#pragma once

#include "stack.hpp"
#include <stack>
#include "exceptions.hpp"
#include <functional>
#include "pair.hpp"

template <typename TKey, typename TValue>
class BinaryTree {
public:
    class Node {
    private:
        Pair<TKey, TValue> data;
        Node* left = nullptr;
        Node* right = nullptr;
        int height = 1;

        friend class BinaryTree<TKey, TValue>;
    public:
        Node() = default;
        Node(TKey k, TValue v, Node *l = nullptr, Node *r = nullptr) 
            : data(k, v), left(l), right(r), height(1) {}
        ~Node() = default;

        const TValue& GetValue() const {
            return data.second;
        }

        const TKey& GetKey() const {
            return data.first;
        }

        Node* GetLeft() const {
            return left;
        }

        Node* GetRight() const {
            return right;
        }
    };
    
    typedef std::function<void(const std::function<void(const TValue&)>&, Node*)> Traverser;

private:
    Node *Root = nullptr;

    void DeleteNodes(Node *&TempNode) {
        if (TempNode == nullptr) return;
        DeleteNodes(TempNode->right);
        DeleteNodes(TempNode->left);
        delete TempNode;
        TempNode = nullptr;
    }

    Node* CopyNodes(Node* node) {
        if (!node) return nullptr;
        Node* newNode = new Node(node->GetKey(), node->GetValue());
        newNode->height = node->height;
        newNode->left = CopyNodes(node->left);
        newNode->right = CopyNodes(node->right);
        return newNode;
    }

    int GetNodeHeight(const Node* node) const { 
        return node ? node->height : 0; 
    }

    void UpdateHeight(Node* node) {
        int hl = GetNodeHeight(node->left);
        int hr = GetNodeHeight(node->right);
        node->height = (hl > hr ? hl : hr) + 1;
    }

    int BalanceFactor(const Node* node) const {
        return GetNodeHeight(node->left) - GetNodeHeight(node->right);
    }

    Node* RotateRight(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        x->right = y;
        UpdateHeight(y);
        UpdateHeight(x);
        return x;
    }

    Node* RotateLeft(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        y->left = x;
        UpdateHeight(x);
        UpdateHeight(y);
        return y;
    }

    Node* Balance(Node* node) {
        UpdateHeight(node);
        int bf = BalanceFactor(node);

        if (bf > 1) {
            if (BalanceFactor(node->left) < 0)
                node->left = RotateLeft(node->left);
            return RotateRight(node);
        }
        if (bf < -1) {
            if (BalanceFactor(node->right) > 0)
                node->right = RotateRight(node->right);
            return RotateLeft(node);
        }
        return node;
    }

    Node* InsertNode(Node* node, const TKey& k, const TValue& v) {
        if (!node) return new Node(k, v);
        if (k < node->data.first) {
            node->left = InsertNode(node->left, k, v);
        } else if (k > node->data.first) {
            node->right = InsertNode(node->right, k, v);
        } else {
            throw InvalidArgumentException("Добавляемый элемент уже есть в дереве.");
        }
        return Balance(node);
    }

    Node* RemoveMin(Node* node, Node*& min) {
        if (!node->left) {
            min = node;
            return node->right;
        }
        node->left = RemoveMin(node->left, min);
        return Balance(node);
    }

    Node* RemoveNode(Node* node, const TKey& k) {
        if (!node) throw InvalidArgumentException("В дереве нет удаляемого элемента.");
        if (k < node->data.first) {
            node->left = RemoveNode(node->left, k);
            return Balance(node);
        }
        if (k > node->data.first) {
            node->right = RemoveNode(node->right, k);
            return Balance(node);
        }
        Node* left = node->left;
        Node* right = node->right;
        delete node;
        if (!right) return left;
        Node* min = nullptr;
        right = RemoveMin(right, min);
        min->left = left;
        min->right = right;
        return Balance(min);
    }

public:
    class TreeEnumerator {
    private:
        Node* current = nullptr;
        Stack<Node*> path;
        friend class BinaryTree<TKey, TValue>;

    public:
        TreeEnumerator() = default;

        const TValue& operator*() const {
            if (!current) {
                throw NullPtrException("Попытка разыменования пустого итератора.");
            }
            return current->data.second;
        }

        TreeEnumerator& operator++() {
            if (!current) {
                return *this;
            }
            if (current->right) {
                Node* temp = current->right;
                while (temp) {
                    path.push(temp);
                    temp = temp->left;
                }
            }
            if (!path.empty()) {
                current = path.top();
                path.pop();
            } else {
                current = nullptr;
            }
            return *this;
        }

        TreeEnumerator operator++(int) {
            TreeEnumerator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator!=(const TreeEnumerator& other) const {
            return current != other.current;
        }

        bool operator==(const TreeEnumerator& other) const {
            return current == other.current;
        }
    };

    BinaryTree() = default;

    BinaryTree(const BinaryTree& other) {
        Root = CopyNodes(other.Root);
    }

    BinaryTree& operator=(const BinaryTree& other) {
        if (this == &other) return *this;
        DeleteNodes(Root);
        Root = CopyNodes(other.Root);
        return *this;
    }

    BinaryTree(BinaryTree&& other) noexcept : Root(other.Root) {
        other.Root = nullptr;
    }

    BinaryTree& operator=(BinaryTree&& other) noexcept {
        if (this == &other) return *this;
        DeleteNodes(Root);
        Root = other.Root;
        other.Root = nullptr;
        return *this;
    }

    ~BinaryTree() {
        DeleteNodes(Root);
    }

    void Insert(const TKey& k, const TValue& Val) {
        Root = InsertNode(Root, k, Val);
    }

    bool Search(const TKey& k) const {
        if (!Root) return false;
        Node* current = Root;
        while (current) {
            if (k < current->data.first) {
                current = current->left;
            } else if (k > current->data.first) {
                current = current->right;
            } else {
                return true;
            }
        }
        return false;
    }

    void Remove(const TKey& k) {
        Root = RemoveNode(Root, k);
    }

    void Traverse(const Traverser& usersFunc, const std::function<void(const TValue&)>& action) const {
        usersFunc(action, Root);
    }

    void Traverse(const std::function<void(const TValue&)>& action) const {
        for (auto it = begin(); it != end(); ++it) {
            action(*it);
        }
    }

    TreeEnumerator begin() const {
        TreeEnumerator beg;
        Node* temp = Root;
        while (temp) {
            beg.path.push(temp);
            temp = temp->left;
        }
        if (!beg.path.empty()) {
            beg.current = beg.path.top();
            beg.path.pop();
        }
        return beg;
    }

    TreeEnumerator end() const {
        return TreeEnumerator();
    }
};

template <typename TKey, typename TValue>
void klpRec(const std::function<void(const TValue&)>& action,
             typename BinaryTree<TKey, TValue>::Node* n) {
    if (!n) return;
    action(n->GetValue());
    klpRec<TKey, TValue>(action, n->GetLeft());
    klpRec<TKey, TValue>(action, n->GetRight());
}

template <typename TKey, typename TValue>
void kplRec(const std::function<void(const TValue&)>& action,
             typename BinaryTree<TKey, TValue>::Node* n) {
    if (!n) return;
    action(n->GetValue());
    kplRec<TKey, TValue>(action, n->GetRight());
    kplRec<TKey, TValue>(action, n->GetLeft());
}

template <typename TKey, typename TValue>
void lpkRec(const std::function<void(const TValue&)>& action,
             typename BinaryTree<TKey, TValue>::Node* n) {
    if (!n) return;
    lpkRec<TKey, TValue>(action, n->GetLeft());
    lpkRec<TKey, TValue>(action, n->GetRight());
    action(n->GetValue());
}

template <typename TKey, typename TValue>
void lkpRec(const std::function<void(const TValue&)>& action,
             typename BinaryTree<TKey, TValue>::Node* n) {
    if (!n) return;
    lkpRec<TKey, TValue>(action, n->GetLeft());
    action(n->GetValue());
    lkpRec<TKey, TValue>(action, n->GetRight());
}

template <typename TKey, typename TValue>
void plkRec(const std::function<void(const TValue&)>& action,
             typename BinaryTree<TKey, TValue>::Node* n) {
    if (!n) return;
    plkRec<TKey, TValue>(action, n->GetRight());
    plkRec<TKey, TValue>(action, n->GetLeft());
    action(n->GetValue());
}

template <typename TKey, typename TValue>
void pklRec(const std::function<void(const TValue&)>& action,
             typename BinaryTree<TKey, TValue>::Node* n) {
    if (!n) return;
    pklRec<TKey, TValue>(action, n->GetRight());
    action(n->GetValue());
    pklRec<TKey, TValue>(action, n->GetLeft());
}

template <typename TKey, typename TValue>
void lkpIter(const std::function<void(const TValue&)>& action,
              typename BinaryTree<TKey, TValue>::Node* n) {
    Stack<typename BinaryTree<TKey, TValue>::Node*> stack;
    typename BinaryTree<TKey, TValue>::Node* current = n;

    while (current != nullptr || !stack.empty()) {
        while (current != nullptr) {
            stack.push(current);
            current = current->GetLeft();
        }
        current = stack.top();
        stack.pop();
        action(current->GetValue());
        current = current->GetRight();
    }
}