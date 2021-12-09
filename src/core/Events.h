#pragma once

#include <list>

struct InputEvent {

};

template<typename T>
class EventSubject;

template<typename T>
class EventObserver {
public:
    virtual ~EventObserver() {}
    virtual void on_event(const T &event) = 0;

protected:
    EventObserver() {}
};

template<typename T>
class EventSubject {
public:
    virtual ~EventSubject() {}
    virtual void listen(EventObserver<T>* observer) {
        m_observers.push_back(observer);
    }

    virtual void unlisten(EventObserver<T>* observer) {
        m_observers.remove(observer);
    }

    virtual void emit_event(const T &event) {
        for (EventObserver<T>* observer : m_observers) {
            observer->on_event(event);
        }
    }
protected:
    EventSubject() {}

private:
    std::list<EventObserver<T>*> m_observers;
};
