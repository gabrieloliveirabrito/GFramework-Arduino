#pragma once

template <typename T>
class List
{
public:
    List()
    {
        this->first = NULL;
        this->last = NULL;
        this->count = 0;
    }

    void Add(T value)
    {
        Node<T> *node = new Node<T>(value);

        if (this->first == NULL)
        {
            this->first = node;
            this->last = node;
        }
        else
        {
            this->last->SetNext(node);
            this->last = node;
        }

        this->count++;
    }

    T GetItem(int index)
    {
        if (index < 0 || index >= this->count)
            return T();
        else
        {
            Node<T> *C = this->first;
            for (int i = 0; i < index; i++)
                C = C->GetNext();
            return C->GetValue();
        }
    }

    bool RemoveAt(int index)
    {
        if (index < 0 || index >= this->count)
            return false;
        else
        {
            Node<T> *C = this->first;
            for (int i = 0; i < index; i++)
                C = C->GetNext();

            C->GetPrevious()->SetNext(C->GetNext());

            if (C->HasNext())
                C->GetNext()->SetPrevious(C->GetPrevious());
            this->count--;

            return true;
        }
    }

    bool RemoveItem(T value)
    {
        Node<T> *current = this->First;
        while (current != nullptr)
        {
            if (current->GetValue() == value)
            {
                current->GetPrevious()->SetNext(current->GetNext());

                if (current->HasNext())
                    current->GetNext()->SetPrevious(current->GetPrevious());
                this->count--;

                return true;
            }

            current = current->GetNext();
        }
        return false;
    }

    int Count() { return this->count; }

private:
    int count;
    Node<T> *first, *last;
};