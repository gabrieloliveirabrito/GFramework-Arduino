#pragma once

template <typename T, typename... TArgs>
using FunctionCallback = T (*)(TArgs...);

template <typename S, typename T, typename... TArgs>
using MemberCallback = T (S::*)(TArgs...);

template <typename TInstance, typename TReturn, typename... TArgs>
class EventHandler
{
public:
    EventHandler(TInstance *instance, MemberCallback<TInstance, TReturn, TArgs...> member)
    {
        this->instance = instance;
        this->member = member;
    }

    TReturn Call(TArgs... args)
    {
        if (instance == NULL || member == NULL)
            return TReturn();
        else
            return ((*instance).*member)(args...);
    }

private:
    TInstance *instance;
    MemberCallback<TInstance, TReturn, TArgs...> member;
};

template <typename TReturn, typename... TArgs>
class FunctionHandler : public EventHandler<FunctionHandler<TReturn, TArgs...>, TReturn, TArgs...>
{
public:
    FunctionHandler(FunctionCallback<TReturn, TArgs...> member) : EventHandler<FunctionHandler<TReturn, TArgs...>, TReturn, TArgs...>(this, &InternalCall)
    {
        this->member = member;
    }

    TReturn Call(TArgs... args)
    {
        return EventHandler<FunctionHandler<TReturn, TArgs...>, TReturn, TArgs...>::Call(args...);
    }

    TReturn InternalCall(TArgs... args)
    {
        if (member == NULL)
            return TReturn();
        else
            return (*member)(args...);
    }

private:
    FunctionCallback<TReturn, TArgs...> member;
};