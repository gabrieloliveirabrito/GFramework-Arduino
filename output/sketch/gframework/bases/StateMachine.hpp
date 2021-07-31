#line 1 "c:\\Users\\gabri\\Projetos\\Arduino\\GFramework\\gframework\\bases\\StateMachine.hpp"
#pragma once
#include "../collections/includes.h"
#include "defines.h"
#include "StateHandler.hpp"

template <typename T, typename TValue>
class StateMachine
{
public:
    using Handler = StateHandler<T, TValue>;
    using Getter = StateGetter<T, TValue>;
    using Changed = StateChanged<T, TValue>;

    StateMachine()
    {
        handlers = new List<Handler *>();
    }

    void AddHandler(String id, Getter *getter, Changed *changed)
    {
        for (int i = 0, n = handlers->Count(); i < n; i++)
        {
            Handler *handler = handlers->GetItem(i);

            if (handler->GetId() == id)
                return;
        }

        handlers->Add(new Handler(id, getter, changed));
    }

    void Update()
    {
        for (int i = 0, n = handlers->Count(); i < n; i++)
        {
            handlers->GetItem(i)->Update();
        }
    }

private:
    List<Handler *> *handlers;
};