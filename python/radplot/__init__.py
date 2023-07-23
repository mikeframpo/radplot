
def _init_lib():
    import ctypes
    global _librp
    
    # TODO: should default to the system install path or similar
    _librp = ctypes.cdll.LoadLibrary('build/python/libpyradplot.so')

_librp = None
_init_lib()

from .figure import Figure
