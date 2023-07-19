
import numpy as np
import ctypes
import time

librp = ctypes.cdll.LoadLibrary('build/python/libpyradplot.so')

# a = np.linspace(0, 10, 11)

# ND_PTR = np.ctypeslib.ndpointer(dtype=np.float64, ndim=1, flags='C')

# librp.PrintArray.restype = None
# librp.PrintArray.argtypes = [ND_PTR, ctypes.c_size_t]
# librp.PrintArray(a, a.size)

librp.CreateFigure.restype = ctypes.c_int
librp.CreateFigure()

# required when running this as a script
# while True:
#     time.sleep(1)

# from repo root, run
# RADPLOT_SHADER_PATH=./libradplot/shaders/ python python/radplot/radplot.py