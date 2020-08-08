import numpy as np
def main():
    nx = 200
    ny = 100
    print("P3")
    print(nx, ny)
    print(255)
    for y in range(0, ny):    
        for x in range(0, nx):
            r = x / nx
            g = y / ny
            b = 0.2
            ir = int(255 * r)
            ig = int(255 * g)
            ib = int(255 * b)
            print(ir, ig, ib)

if __name__ == '__main__':
    main()