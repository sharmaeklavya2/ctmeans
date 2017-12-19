import sys
import timeit
from contextlib import contextmanager

timer_level = 0
@contextmanager
def Timer(task_name, fobj=sys.stdout):
    start_time = timeit.default_timer()
    global timer_level
    timer_level += 1
    yield
    timer_level -= 1
    elapsed = timeit.default_timer() - start_time
    if fobj is not None:
        s = '{}Time to {}: {}'.format('  ' * timer_level, task_name, elapsed)
        print(s, file=fobj)
