from api.matrix_api import LEDMatrix

import time

if __name__ == "__main__":
    matrix = LEDMatrix()

    matrix.pattern(4, 1, """
        10010111
        10010001
        10010111
        10010001
        11110111
    """)
    matrix.hline(7)

    for i in range(5):
        matrix.number(4, 9, i)
        matrix.show()
        time.sleep(1)

    matrix.clear()