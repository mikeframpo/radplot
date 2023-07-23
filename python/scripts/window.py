
import radplot
import numpy as np

figure = radplot.Figure()

# TODO: need a lock on Window/Renderer creation otherwise draw call gets a seg fault
import time
time.sleep(1.0)

figure.draw_quad(np.array([0,0,0], dtype=np.float64))
figure.draw_quad(np.array([2,2,0], dtype=np.float64))

figure.wait_for_close()