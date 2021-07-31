#pragma once

template <typename T>
class Node
{
public:
    Node()
    {
        this->previous = NULL;
        this->next = NULL;
    };

    Node(T value) : Node()
    {
        this->value = value;
    }

    T GetValue() { return this->value; }
    void SetValue(T value) { this->value = value; }

    Node *GetPrevious() { return this->previous; }
    void SetPrevious(Node *node) { this->previous = node; }
    bool HasPrevious() { return this->previous != NULL; }

    Node *GetNext() { return this->next; }
    void SetNext(Node *node) { this->next = node; }
    bool HasNext() { return this->next != NULL; }

private:
    T value;
    Node *previous, *next;
};