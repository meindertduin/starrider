#pragma once

#include <list>
#include <map>
#include <stdint.h>

enum WindowEventType {
    WinNone = 1,
    KeyUp = 1 << 1,
    KeyDown = 1 << 2,
    Mouse = 1 << 3,
    MouseButton = 1 << 4,
    WinExpose = 1 << 5,
    MouseMotion = 1 << 6,

    num_values = 7,
};

struct WindowEvent {
    WindowEvent() {
        event_type = WindowEventType::WinNone;
    }

    union {
        struct  {
            uint32_t x_pos : 16;
            uint32_t y_pos : 16;
            int d_xpos : 16;
            int d_ypos : 16;
        } mouse_event;

        struct {
            uint32_t width : 16;
            uint32_t height : 16;
            uint32_t value : 32; // TODO filler value needs to be filled in later
        } expose_event;

        struct {
            uint32_t keysym;
            uint32_t mask;
        } keyboard_event;
    } body;
    WindowEventType event_type;
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

    virtual void listen(EventObserver<T>* observer, WindowEventType type) {
        if (!m_observers.count(type)) {
            m_observers[type] = std::list<EventObserver<T>*>();
            m_observers[type].push_back(observer);
            return;
        }

        m_observers[type].push_back(observer);
    }

    virtual void listen_multiple(EventObserver<T>* observer, uint32_t types_mask) {
        for (int i = 0; i < WindowEventType::num_values; i++) {
            if (types_mask & (1 << i)) {
                listen(observer, static_cast<WindowEventType>(1 << i));
            }
        }
    }

    virtual void unlisten(EventObserver<T>* observer, WindowEventType type) {
        if (!m_observers.count(type)) {
            m_observers[type].remove(observer);
        }
    }

    virtual void emit_event(const T &event, WindowEventType type) {
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
    std::map<WindowEventType, std::list<EventObserver<T>*>> m_observers;
};



