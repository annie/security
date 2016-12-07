import os
import subprocess

def main():
    os.system("make")

    proc = subprocess.Popen(["sudo", "./hwcopy", "-c", "config.txt"], bufsize=0, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT, shell=False)
    (stdout, stderr) = proc.communicate("inputTest.txt testOutput1.txt\ninputAlias/outerTest.txt ../testOutput2.txt\nrandomFile.txt testOutput3.txt")

    print
    print("attempting to copy valid file in document root...")
    print("contents of copied file:")
    proc_docRoot = subprocess.Popen(["cat", "test/outputRoot/output/testOutput1.txt"], bufsize=0, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT, shell=False)
    print(proc_docRoot.communicate()[0])

    print
    print("attempting to copy valid aliased file...")
    print("contents of copied file:")
    proc_alias = subprocess.Popen(["cat", "test/outputRoot/output/testOutput2.txt"], bufsize=0, stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT, shell=False)
    print(proc_alias.communicate()[0])

    print
    print("attempting to copy file that doesn't exist in document root...")
    print(stdout)

    proc.stdin.close()

main()