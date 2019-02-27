add_library(renderer graphics.c nodetree.c node.c vulkanswapchain.c programs.c event.c)
target_link_libraries(renderer glfw)
