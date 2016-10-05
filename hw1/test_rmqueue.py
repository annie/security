import os
import sys
import subprocess

def main():
    os.system("make clean")
    os.system("make")
    os.system("./addqueue test.txt test2.txt test3.txt")
    stdout, stderr = subprocess.Popen(["./showqueue"], stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()

    rmqueue = ["./rmqueue"]
    for line in stdout.split("\n"):
        line = line.split("\t")
        if len(line) > 1:
            rmqueue.append(line[3])

    print

    pid = os.fork()
    if pid == 0:
        try:
            print "switching to user 100"
            os.setuid(100)
            print "user 100 does not own files in the queue, rmqueue should fail"
            stdout, stderr = subprocess.Popen(rmqueue, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
            print stderr
        finally:
            os._exit(0)

    os.waitpid(pid, 0)

    print "switching back to root user"
    print "root user owns files in the queue, rmqueue should complete successfully"
    stdout, stderr = subprocess.Popen(rmqueue, stdout=subprocess.PIPE, stderr=subprocess.PIPE).communicate()
    print stdout

main()
