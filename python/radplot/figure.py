
from . import _librp

import ctypes
import numpy as np

class PoseArgs(ctypes.Structure):
    _fields_ = [('Location', ctypes.POINTER(ctypes.c_double))]

class Figure():
    def __init__(self):
        _librp.CreateFigure.restype = ctypes.c_void_p
        self._pfigure = _librp.CreateFigure()
    
    def wait_for_close(self):
        _librp.FigureWaitForClose.argtypes = [ctypes.c_void_p]
        _librp.FigureWaitForClose(self._pfigure)
    
    def draw_quad(self, loc):
        assert type(loc) == np.ndarray
        assert loc.shape == (3,)

        # ND_PTR = np.ctypeslib.ndpointer(dtype=np.float64, ndim=1, flags='C')

        pose = PoseArgs(np.ctypeslib.as_ctypes(loc))

        _librp.FigureDrawQuad.restype = None
        _librp.FigureDrawQuad.argtypes = [ctypes.c_void_p, PoseArgs]
        _librp.FigureDrawQuad(self._pfigure, pose)

        # librp.PrintArray.restype = None
        # librp.PrintArray.argtypes = [ND_PTR, ctypes.c_size_t]
        # librp.PrintArray(a, a.size)
