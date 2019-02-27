#include "event.h"
#include <string.h>
#include <pthread.h>

void EventInit(Event * event, const char * name, uint32_t size)
{
  pthread_mutex_init(&(event->mutex), NULL);
  strcpy(event->name, name);
  event->dataSize = size;
  event->numListeners = 0;
}

void EventAddListener(Event * event, const Listener * listener)
{
  pthread_mutex_lock(&(event->mutex));
  memcpy(event->listeners + event->numListeners++, listener, sizeof(Listener));
  pthread_mutex_unlock(&(event->mutex));
}
void EventTrigger(Event * event, const void * data)
{
  pthread_mutex_lock(&(event->mutex));
  for (uint32_t i = 0; i < event->numListeners; ++i) 
    (*(event->listeners[i].callback))(event->listeners[i].data, data, event->dataSize);
  pthread_mutex_unlock(&(event->mutex));
}
