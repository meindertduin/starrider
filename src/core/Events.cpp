#include "Events.h"

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


