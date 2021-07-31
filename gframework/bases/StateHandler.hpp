#pragma once
#include "defines.h"
#include "EventHandler.hpp"

template <typename T, typename TValue>
using StateGetter = EventHandler<T, TValue>;

template <typename T, typename TValue>
using StateChanged = EventHandler<T, void, TValue, TValue>;

template <typename T, typename TValue>
class StateHandler
{
public:
    StateHandler(String id, StateGetter<T, TValue> *getter, StateChanged<T, TValue> *changed)
    {
        this->id = id;
        this->getter = getter;
        this->changed = changed;
    }

    String GetId() { return id; }
    TValue GetValue() { return value; }
    StateGetter<T, TValue> *GetGetterHandler() { return getter; }
    StateChanged<T, TValue> *GetChangedHandler() { return changed; }

    void Update()
    {
        if (getter != NULL && this->changed != NULL)
        {
            TValue value = this->getter->Call();

            if (value != this->value)
            {
                changed->Call(this->value, value);
                this->value = value;
            }
        }
    }

private:
    String id;
    TValue value;
    StateGetter<T, TValue> *getter;
    StateChanged<T, TValue> *changed;
};