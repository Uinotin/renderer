#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "vulkancontext.h"
#include "event.h"

#define MAXSHADERFILESIZE 2047 


typedef struct Window
{
  VulkanContext context;
  GLFWwindow * window;
  Event events[4];
  uint32_t numEvents;
} Window;

typedef struct WindowSize
{
  int width, height;
} WindowSize;


void InitWindowEvents(Window * window);
void UpdateWindow(Window * window);
int ShouldClose(Window * window);
int CreateWindow(Window * window);
void DestroyWindow(Window * window);

#endif
