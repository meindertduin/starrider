#pragma once

#include <list>

struct InputEvent {

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

template<typename T>
EventObserver<T>::EventObserver() {}

template<typename T>
EventObserver<T>::~EventObserver() {}

template<typename T>
EventSubject<T>::EventSubject() {}

template<typename T>
EventSubject<T>::~EventSubject() {}

template<typename T>
void EventSubject<T>::listen(EventObserver<T>* observer) {
    m_observers.push_back(observer);
}

template<typename T>
void EventSubject<T>::unlisten(EventObserver<T>* observer) {
    m_observers.remove(observer);
}

template<typename T>
void EventSubject<T>::emit_event(const T &event) {
    for (EventObserver<T>* observer : m_observers) {
        observer->on_event(event);
    }
}

