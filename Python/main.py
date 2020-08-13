import numpy as np
def main():
    f = open("1.ppm", "wt")   # write file 1.ppm 

    nx = 200  # width 200
    ny = 100  # height 100

    f.write("P3\n")
    f.write("{} {}\n".format(nx, ny))
    f.write("255\n")

    for y in range(0, ny):
        for x in range(0, nx):
            r = x / nx  # red increase across x axis, from 0 to 1
            g = y / ny  # green increase across y axis, from 0 to 1
            b = 0.2     # fix b 0.2
            ir = int(r * 255) # scale to 0 - 255
            ig = int(g * 255) # scale to 0 - 255
            ib = int(b * 255) # scale to 0 - 255
            f.write("{} {} {}\n".format(ir, ig, ib))
    f.close()

# if __name__ == '__main__':
    # main()