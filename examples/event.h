#ifndef EVENT_H
#define EVENT_H

#include <pthread.h>
#include <stdint.h>

#define MAXLISTENERS 32
#define MAXNAMELENTH 16
#define MAXEVENTDATASIZE 256

typedef void (*EventCallback)(void * listener, const void * eventData, uint32_t dataSize);

typedef struct Listener
{
  EventCallback callback;
  void * data;
} Listener;

typedef struct Event
{
  char name[MAXNAMELENTH];
  Listener listeners[MAXLISTENERS];
  uint32_t numListeners;
  uint32_t dataSize;
  pthread_mutex_t mutex;
} Event;

void EventInit(Event * event, const char * name, uint32_t size);
void EventAddListener(Event * event, Listener listener);
void EventTrigger(Event * event, const void * data);

#endif
