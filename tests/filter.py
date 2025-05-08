import sys
import shutil
import os
import subprocess

def run(filter, path_source, path_output):
    if shutil.which("ffmpeg") is None:
        print("Error: ffmpeg executable is not Found")
        return -1

    subprocess.call(["ffmpeg", "-y", "-i", path_source, "-vf", filter, path_output], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("`path_source` and `path_output` arguments expected")
        exit(1)

#    run("dctdnoiz=s=10", sys.argv[1], "dctdnoiz.png")
#    run("unsharp=3:3:-0.5:3:3:-0.5", sys.argv[1], "unsharp.png")
#    run("colorlevels=rimin=0.039:gimin=0.039:bimin=0.039:rimax=0.96:gimax=0.96:bimax=0.96", sys.argv[1], sys.argv[2])
#    run("colorlevels=rimin=0.039:gimin=0.039:bimin=0.039:rimax=0.96:gimax=0.96:bimax=0.96", sys.argv[1], "darker.png")
#    run("colorlevels=rimin=0.058:gimin=0.058:bimin=0.058", sys.argv[1], "contrast.png")

#    run("colorlevels=rimax=0.902:gimax=0.902:bimax=0.902", sys.argv[1], "lighter.png")
#    run("colorlevels=romin=0.05:gomin=0.05:bomin=0.05", sys.argv[1], "brightness.png")
#    run("noise=alls=25:allf=u", sys.argv[1], "noise.png")
#    run("nlmeans=s=10", sys.argv[1], "nlmeans.png")
#    run("pixelize=w=3:h=3", sys.argv[1], "pixelize.png")
    run("fftdnoiz=sigma=15", sys.argv[1], "fftdnoiz.png")
