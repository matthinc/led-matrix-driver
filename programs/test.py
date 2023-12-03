from api.matrix_api import LEDMatrix

if __name__ == "__main__":
    matrix = LEDMatrix()
    matrix.draw_scrolling_text("DIES IST EIN TEST 123")

