#pragma once

#include <list>

class InputEventManager {

};

template<typename T>
class EventSubject;

template<typename T>
class EventObserver {
public:
    virtual ~EventObserver();
    virtual void on_event(const T &event) = 0;

protected:
    EventObserver();
};

template<typename T>
class EventSubject {
public:
    virtual ~EventSubject();
    virtual void listen(EventObserver<T>* observer);
    virtual void unlisten(EventObserver<T>* observer);
    virtual void emit_event(const T &event);

protected:
    EventSubject();

private:
    std::list<EventObserver<T>*> m_observers;
};
