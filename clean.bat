@echo off
for %%f in (engine editor winmaped gfx_dd7 gfx_soft gfx_soft16 gfx_opengl sfx_mikmod) do deltree %%f\release
for %%f in (engine editor winmaped gfx_dd7 gfx_soft gfx_soft16 gfx_opengl sfx_mikmod) do deltree %%f\debug
