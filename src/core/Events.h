#pragma once

#include <list>
#include <map>

struct InputEvent {

};

enum class EventType {
    Input,
    Window,
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

template<typename T>
class MultiEventSubject {
public:
    virtual ~MultiEventSubject() {}

    virtual void listen(EventObserver<T>* observer, EventType type) {

        if (!m_observers.count(type)) {
            m_observers[type] = std::list<EventObserver<T>*>();
            m_observers[type].push_back(observer);
            return;
        }

        m_observers[type].push_back(observer);
    }
    virtual void unlisten(EventObserver<T>* observer, EventType type) {
        if (!m_observers.count(type)) {
            m_observers[type].remove(observer);
        }
    }

    virtual void emit_event(const T &event, EventType type) {
        if (!m_observers.count(type)) {
            return;
        }

        for (EventObserver<T>* observer : m_observers[type]) {
            observer->on_event(event);
        }
    }
protected:
    MultiEventSubject() {}
private:
    std::map<EventType, std::list<EventObserver<T>*>> m_observers;
};



