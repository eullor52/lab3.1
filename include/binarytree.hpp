#pragma once

#include "stack.hpp"
#include <stack>
#include "exceptions.hpp"
#include <functional>
#include "pair.hpp"
#include <memory>

template <typename TKey, typename TValue>
class BinaryTree {
public:
    class Node {
    private:
        Pair<TKey, TValue> data;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        unsigned short height = 1;

        friend class BinaryTree<TKey, TValue>;
    public:
        Node() = default;
        Node(TKey key, TValue value, std::shared_ptr<Node> leftNode = nullptr, std::shared_ptr<Node> rightNode = nullptr) 
            : data(key, value), left(std::move(leftNode)), right(std::move(rightNode)), height(1) {}
        ~Node() = default;

        const TValue& getValue() const {
            return data.second;
        }

        const TKey& getKey() const {
            return data.first;
        }

        std::shared_ptr<Node> getLeft() const {
            return left;
        }

        std::shared_ptr<Node> getRight() const {
            return right;
        }
    };
    
    using Starter = std::function<std::shared_ptr<Node>(std::shared_ptr<Node>, Stack<std::shared_ptr<Node>>&)>;
    using Incrementor = std::function<std::shared_ptr<Node>(std::shared_ptr<Node>, Stack<std::shared_ptr<Node>>&)>;

private:
    std::shared_ptr<Node> root;

    std::shared_ptr<Node> copyNodes(const std::shared_ptr<const Node>& node) {
        if (!node) return nullptr;
        auto newNode = std::make_shared<Node>(node->getKey(), node->getValue());
        newNode->height = node->height;
        newNode->left = copyNodes(node->getLeft());
        newNode->right = copyNodes(node->getRight());
        return newNode;
    }

    unsigned short getNodeHeight(const std::shared_ptr<const Node>& node) const { 
        return node ? node->height : 0; 
    }

    void updateHeight(const std::shared_ptr<Node>& node) {
        unsigned short leftHeight = getNodeHeight(node->getLeft());
        unsigned short rightHeight = getNodeHeight(node->getRight());
        node->height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    }

    short getBalanceFactor(const std::shared_ptr<const Node>& node) const {
        return static_cast<short>(getNodeHeight(node->getLeft())) - static_cast<short>(getNodeHeight(node->getRight()));
    }

    std::shared_ptr<Node> rotateRight(std::shared_ptr<Node> yNode) {
        std::shared_ptr<Node> xNode = std::move(yNode->left);
        yNode->left = std::move(xNode->right);
        updateHeight(yNode);
        xNode->right = std::move(yNode);
        updateHeight(xNode);
        return xNode;
    }

    std::shared_ptr<Node> rotateLeft(std::shared_ptr<Node> xNode) {
        std::shared_ptr<Node> yNode = std::move(xNode->right);
        xNode->right = std::move(yNode->left);
        updateHeight(xNode);
        yNode->left = std::move(xNode);
        updateHeight(yNode);
        return yNode;
    }

    std::shared_ptr<Node> balanceNode(std::shared_ptr<Node> node) {
        updateHeight(node);
        short balanceFactor = getBalanceFactor(node);

        if (balanceFactor > 1) {
            if (getBalanceFactor(node->getLeft()) < 0)
                node->left = rotateLeft(std::move(node->left));
            return rotateRight(std::move(node));
        }
        if (balanceFactor < -1) {
            if (getBalanceFactor(node->getRight()) > 0)
                node->right = rotateRight(std::move(node->right));
            return rotateLeft(std::move(node));
        }
        return node;
    }

    std::shared_ptr<Node> insertNode(std::shared_ptr<Node> node, const TKey& key, const TValue& value) {
        if (!node) return std::make_shared<Node>(key, value);
        if (key < node->data.first) {
            node->left = insertNode(std::move(node->left), key, value);
        } else if (key > node->data.first) {
            node->right = insertNode(std::move(node->right), key, value);
        } else {
            throw InvalidArgumentException("Добавляемый элемент уже есть в дереве.");
        }
        return balanceNode(std::move(node));
    }

    std::shared_ptr<Node> removeMin(std::shared_ptr<Node> node, std::shared_ptr<Node>& minNode) {
        if (!node->left) {
            minNode = std::move(node);
            return std::move(minNode->right);
        }
        node->left = removeMin(std::move(node->left), minNode);
        return balanceNode(std::move(node));
    }

    std::shared_ptr<Node> removeNode(std::shared_ptr<Node> node, const TKey& key) {
        if (!node) throw InvalidArgumentException("В дереве нет удаляемого элемента.");
        if (key < node->data.first) {
            node->left = removeNode(std::move(node->left), key);
            return balanceNode(std::move(node));
        }
        if (key > node->data.first) {
            node->right = removeNode(std::move(node->right), key);
            return balanceNode(std::move(node));
        }
        
        std::shared_ptr<Node> leftNode = std::move(node->left);
        std::shared_ptr<Node> rightNode = std::move(node->right);
        node.reset();
        
        if (!rightNode) return leftNode;
        
        std::shared_ptr<Node> minNode;
        rightNode = removeMin(std::move(rightNode), minNode);
        minNode->left = std::move(leftNode);
        minNode->right = std::move(rightNode);
        return balanceNode(std::move(minNode));
    }

public:
    class TreeEnumerator {
    private:
        std::weak_ptr<Node> current;
        Stack<std::shared_ptr<Node>> path;
        Incrementor incrementor;

        friend class BinaryTree<TKey, TValue>;

    public:
        TreeEnumerator() = default;

        const TValue& operator*() const {
            auto sharedCurrent = current.lock();
            if (!sharedCurrent) {
                throw NullPointerException("Попытка разыменования пустого итератора.");
            }
            return sharedCurrent->data.second;
        }

        TreeEnumerator& operator++() {
            current = incrementor(current.lock(), path);
            return *this;
        }

        TreeEnumerator operator++(int) {
            TreeEnumerator temporaryIterator = *this;
            ++(*this);
            return temporaryIterator;
        }
        
        bool operator!=(const TreeEnumerator& other) const {
            return current.lock() != other.current.lock();
        }

        bool operator==(const TreeEnumerator& other) const {
            return current.lock() == other.current.lock();
        }
    };

    BinaryTree() = default;

    BinaryTree(const BinaryTree& other) {
        root = copyNodes(other.root);
    }

    BinaryTree& operator=(const BinaryTree& other) {
        if (this == &other) return *this;
        root = copyNodes(other.root);
        return *this;
    }

    BinaryTree(BinaryTree&& other) noexcept : root(std::move(other.root)) {}

    BinaryTree& operator=(BinaryTree&& other) noexcept {
        if (this == &other) return *this;
        root = std::move(other.root);
        return *this;
    }

    ~BinaryTree() = default;

    void insert(const TKey& key, const TValue& value) {
        root = insertNode(std::move(root), key, value);
    }

    bool contains(const TKey& key) const {
        if (!root) return false;
        std::shared_ptr<Node> current = root;
        while (current) {
            if (key < current->data.first) {
                current = current->getLeft();
            } else if (key > current->data.first) {
                current = current->getRight();
            } else {
                return true;
            }
        }
        return false;
    }

    void remove(const TKey& key) {
        root = removeNode(std::move(root), key);
    }

    void traverse(const std::function<void(const TValue&)>& action,
                    const Incrementor& incrementFunction, const Starter& startFunction) const {
        for (auto iterator = begin(startFunction, incrementFunction); iterator != end(); ++iterator) {
            action(*iterator);
        }
    }

    TreeEnumerator begin(const Starter& startFinder, const Incrementor& increaser) const {
        TreeEnumerator beginIterator;
        beginIterator.current = startFinder(root, beginIterator.path);
        beginIterator.incrementor = increaser;
        return beginIterator;
    }

    TreeEnumerator begin() const {
        return begin(findLkpStart<TKey, TValue>, incrementLkp<TKey, TValue>);
    }

    TreeEnumerator end() const {
        return TreeEnumerator();
    }
};

template <typename TKey, typename TValue>
std::shared_ptr<typename BinaryTree<TKey, TValue>::Node> 
        findLkpStart(std::shared_ptr<typename BinaryTree<TKey, TValue>::Node> root,
                        Stack<std::shared_ptr<typename BinaryTree<TKey, TValue>::Node>>& path) {
    if (!root) return nullptr;
    std::shared_ptr<typename BinaryTree<TKey, TValue>::Node> current = root;
    while (current->getLeft()) {
        path.push(current);
        current = current->getLeft();
    }
    return current;
}

template <typename TKey, typename TValue>
std::shared_ptr<typename BinaryTree<TKey, TValue>::Node> 
        incrementLkp(std::shared_ptr<typename BinaryTree<TKey, TValue>::Node> current,
                       Stack<std::shared_ptr<typename BinaryTree<TKey, TValue>::Node>>& path) {
    if (!current) {
        return current;
    }
    if (current->getRight()) {
        std::shared_ptr<typename BinaryTree<TKey, TValue>::Node> tempNode = current->getRight();
        path.push(tempNode);
        while (tempNode->getLeft()) {
            tempNode = tempNode->getLeft();
            path.push(tempNode);
        }
    }
    if (!path.isEmpty()) {
        current = path.peek();
        path.pop();
    } else {
        current = nullptr;
    }
    return current;
}